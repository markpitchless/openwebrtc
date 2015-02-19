#ifndef __COCOON_UTILS_H__
#define __COCOON_UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdlib.h>
#include <glib.h>
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

/**
 * Ask the user on stdout to enter a line of text (on stdin), reads chars until
 * user enters a newline or EOF, returns pointer (char*) to entered text.
 * Handles allocating memory for the input (hopefully safely).
 * Returns NULL on out of memory.
 */
// http://stackoverflow.com/questions/25233730/dynamically-prompt-for-string-without-knowing-string-size
char* prompt_line(const char *prompt);

/**
 * Parse an SDP candidate, single line string, into an OwrCandidate*
 */
OwrCandidate* sdp_2_candidate(gchar *line);

/**
 * Print the given OWR ICE candidate as a sdp line.
 */
void g_print_candidate(OwrCandidate *candidate);

#endif /* __COCOON_UTILS_H__ */
