
#define _GNU_SOURCE 1
#include <string.h>

#include "owr.h"
#include "owr_local.h"
#include "owr_media_source.h"
#include "owr_media_renderer.h"
#include "owr_audio_renderer.h"
#include "owr_video_renderer.h"
#include "owr_payload.h"
#include "owr_audio_payload.h"
#include "owr_video_payload.h"
#include "owr_session.h"
#include "owr_media_session.h"
#include "owr_transport_agent.h"
#include "test_utils.h"
#include "cocoon_utils.h"

static OwrTransportAgent *recv_transport_agent = NULL;
static OwrMediaSession *recv_session_audio = NULL;
static OwrMediaSession *recv_session_video = NULL;
static OwrTransportAgent *send_transport_agent = NULL;
static OwrMediaSession *send_session_audio = NULL;
static OwrMediaSession *send_session_video = NULL;

/*
static void got_remote_source(OwrMediaSession *session, OwrMediaSource *source, gpointer user_data)
{
    gchar *name = NULL;
    OwrMediaRenderer *owr_renderer = NULL;
    OwrMediaType media_type;

    g_assert(!user_data);

    g_object_get(source, "media-type", &media_type, "name", &name, NULL);

    g_print("Got remote source: %s\n", name);

    if (media_type == OWR_MEDIA_TYPE_VIDEO) {
        OwrVideoRenderer *renderer;

        g_print("Creating video renderer\n");
        renderer = owr_video_renderer_new(NULL);
        g_assert(renderer);

        g_print("Connecting source to video renderer\n");
        owr_media_renderer_set_source(OWR_MEDIA_RENDERER(renderer), source);
        owr_renderer = OWR_MEDIA_RENDERER(renderer);
    } else if (media_type == OWR_MEDIA_TYPE_AUDIO) {
        OwrAudioRenderer *renderer;

        g_print("Creating audio renderer\n");
        renderer = owr_audio_renderer_new();
        g_assert(renderer);

        g_print("Connecting source to audio renderer\n");
        owr_media_renderer_set_source(OWR_MEDIA_RENDERER(renderer), source);
        owr_renderer = OWR_MEDIA_RENDERER(renderer);
    }

    g_free(name);

    if (media_type == OWR_MEDIA_TYPE_VIDEO) {
        write_dot_file("test_receive-got_remote_source-video-source", owr_media_source_get_dot_data(source), TRUE);
        write_dot_file("test_receive-got_remote_source-video-renderer", owr_media_renderer_get_dot_data(owr_renderer), TRUE);
    } else {
        write_dot_file("test_receive-got_remote_source-audio-source", owr_media_source_get_dot_data(source), TRUE);
        write_dot_file("test_receive-got_remote_source-audio-renderer", owr_media_renderer_get_dot_data(owr_renderer), TRUE);
    }
}
*/

static void got_candidate(OwrMediaSession *session_a, OwrCandidate *candidate, OwrMediaSession *session_b)
{
    //g_print("got_candidate:");
    g_print_candidate(candidate);
    //owr_session_add_remote_candidate(OWR_SESSION(session_b), candidate);
}

static void got_sources(GList *sources, gpointer user_data)
{
    OwrMediaSource *source = NULL;
    static gboolean have_video = FALSE, have_audio = FALSE;
    OwrMediaRenderer *video_renderer = NULL;
    OwrMediaSource *audio_source = NULL, *video_source = NULL;

    g_assert(sources);

    while(sources && (source = sources->data)) {
        OwrMediaType media_type;
        OwrSourceType source_type;

        g_assert(OWR_IS_MEDIA_SOURCE(source));

        g_object_get(source, "type", &source_type, "media-type", &media_type, NULL);

        if (!have_video && media_type == OWR_MEDIA_TYPE_VIDEO && source_type == OWR_SOURCE_TYPE_CAPTURE) {
            OwrVideoRenderer *renderer;
            OwrPayload *payload;

            have_video = TRUE;

            payload = owr_video_payload_new(OWR_CODEC_TYPE_VP8, 103, 90000, TRUE, FALSE);
            g_object_set(payload, "width", 1280, "height", 720, "framerate", 30.0, NULL);
            g_object_set(payload, "rtx-payload-type", 123, NULL);

            owr_media_session_set_send_payload(send_session_video, payload);

            owr_media_session_set_send_source(send_session_video, source);

            owr_transport_agent_add_session(send_transport_agent, OWR_SESSION(send_session_video));

            g_print("Displaying self-view\n");

            renderer = owr_video_renderer_new(NULL);
            g_assert(renderer);
            g_object_set(renderer, "width", 1280, "height", 720, "max-framerate", 30.0, NULL);
            owr_media_renderer_set_source(OWR_MEDIA_RENDERER(renderer), source);
            video_renderer = OWR_MEDIA_RENDERER(renderer);
            video_source = source;
        } else if (!have_audio && media_type == OWR_MEDIA_TYPE_AUDIO && source_type == OWR_SOURCE_TYPE_CAPTURE) {
            OwrPayload *payload;

            have_audio = TRUE;

            payload = owr_audio_payload_new(OWR_CODEC_TYPE_OPUS, 100, 48000, 1);
            owr_media_session_set_send_payload(send_session_audio, payload);

            owr_media_session_set_send_source(send_session_audio, source);

            owr_transport_agent_add_session(send_transport_agent, OWR_SESSION(send_session_audio));
            audio_source = source;
        }

        if (have_video && have_audio)
            break;

        sources = sources->next;
    }

    if (audio_source)
        write_dot_file("test_send-got_source-audio-source", owr_media_source_get_dot_data(audio_source), TRUE);
    if (video_source)
        write_dot_file("test_send-got_source-video-source", owr_media_source_get_dot_data(video_source), TRUE);
    if (video_renderer)
        write_dot_file("test_send-got_source-video-renderer", owr_media_renderer_get_dot_data(video_renderer), TRUE);
}

static gboolean get_candidates_cb(gpointer *user_data)
{
    // Aks for candidates
    char* line = prompt_line("Enter candidate:\n");

    OwrCandidate *remote_candidate;
    remote_candidate = sdp_2_candidate(line);

//    owr_session_add_remote_candidate(OWR_SESSION(send_session_audio), remote_candidate);
    owr_session_add_remote_candidate(OWR_SESSION(send_session_video), remote_candidate);

    return TRUE; // keep asking
}

static void candidate_gathering_done(GObject *media_session, gpointer user_data)
{
    g_print("candidate_gathering_done:\n");
    g_return_if_fail(!user_data);
    g_object_set_data(media_session, "gathering-done", GUINT_TO_POINTER(1));
    //if (can_send_answer())
    //    send_answer();
}

int main() {
    GMainContext *ctx = g_main_context_default();
    GMainLoop *loop = g_main_loop_new(ctx, FALSE);

    /* PREPARE FOR RECEIVING */

    //OwrPayload *receive_payload;

    owr_init_with_main_context(ctx);

    //recv_transport_agent = owr_transport_agent_new(FALSE);
    //g_assert(OWR_IS_TRANSPORT_AGENT(recv_transport_agent));

    //owr_transport_agent_set_local_port_range(recv_transport_agent, 5000, 5999);
    //owr_transport_agent_add_local_address(recv_transport_agent, "127.0.0.1");

    // SEND
    send_transport_agent = owr_transport_agent_new(TRUE);
    g_assert(OWR_IS_TRANSPORT_AGENT(send_transport_agent));

    owr_transport_agent_set_local_port_range(send_transport_agent, 5000, 5999);
    owr_transport_agent_add_local_address(send_transport_agent, "127.0.0.1");

    //recv_session_audio = owr_media_session_new(FALSE);
    //recv_session_video = owr_media_session_new(FALSE);
//    send_session_audio = owr_media_session_new(TRUE);
    send_session_video = owr_media_session_new(TRUE);

    // XXX
    //g_signal_connect(recv_session_audio, "on-new-candidate", G_CALLBACK(got_candidate), send_session_audio);
    //g_signal_connect(recv_session_video, "on-new-candidate", G_CALLBACK(got_candidate), send_session_video);
    //g_signal_connect(send_session_audio, "on-new-candidate", G_CALLBACK(got_candidate), recv_session_audio);
    //g_signal_connect(send_session_video, "on-new-candidate", G_CALLBACK(got_candidate), recv_session_video);

    // This signal fires when we generate local candidates that should sent to
    // the other end via the signal channel.
    g_signal_connect(send_session_video, "on-new-candidate", G_CALLBACK(got_candidate), NULL);
    g_signal_connect(send_session_video, "on-candidate-gathering-done", G_CALLBACK(candidate_gathering_done), NULL);
//    g_signal_connect(send_session_audio, "on-new-candidate", G_CALLBACK(got_candidate), NULL);

    // VIDEO
    //g_signal_connect(recv_session_video, "on-incoming-source", G_CALLBACK(got_remote_source), NULL);

    //receive_payload = owr_video_payload_new(OWR_CODEC_TYPE_VP8, 103, 90000, TRUE, FALSE);
    //g_object_set(receive_payload, "rtx-payload-type", 123, NULL);

    //owr_media_session_add_receive_payload(recv_session_video, receive_payload);

    //owr_transport_agent_add_session(recv_transport_agent, OWR_SESSION(recv_session_video));


    // AUDIO
    //g_signal_connect(recv_session_audio, "on-incoming-source", G_CALLBACK(got_remote_source), NULL);

    //receive_payload = owr_audio_payload_new(OWR_CODEC_TYPE_OPUS, 100, 48000, 1);
    //owr_media_session_add_receive_payload(recv_session_audio, receive_payload);

    //owr_transport_agent_add_session(recv_transport_agent, OWR_SESSION(recv_session_audio));


    /* PREPARE FOR SENDING */

//    owr_get_capture_sources(OWR_MEDIA_TYPE_AUDIO|OWR_MEDIA_TYPE_VIDEO, got_sources, NULL);
    owr_get_capture_sources(OWR_MEDIA_TYPE_VIDEO, got_sources, NULL);

    g_timeout_add_seconds(1, (GSourceFunc)get_candidates_cb, NULL);

    g_main_loop_run(loop);

    return 0;
}