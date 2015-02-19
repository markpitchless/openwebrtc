
#include "cocoon_utils.h"

char* prompt_line(const char *prompt) {
  g_print("%s", prompt);

  int size = 64; // initial str size to store input
  char* str = malloc(size*sizeof(char));
  if (str == NULL) {
    return NULL; // out of memory
  }
  char c = '\0';
  int i = 0;
  do {
    c = getchar();
    if (c == '\r' || c == '\n' || c == EOF) {
        c = '\0'; // end str if user hits <enter>
    }
    if (i == size) {
        size *= 2; // duplicate str size
        str = realloc(str, size*sizeof(char));; // and reallocate it
        if (str == NULL) {
          free(str); // the memory block is not deallocated by realloc
          return NULL; // out of memory
        }
    }
    str[i++] = c;
  } while (c != '\0');
  str = realloc(str, i); // trim memory to the str content size
  return str;
}

void g_print_candidate(OwrCandidate *candidate)
{
    // Unpack the candidate data
    gchar *ice_ufrag, *ice_password;
    OwrCandidateType candidate_type;
    OwrComponentType component_type;
    OwrTransportType transport_type;
    gchar *foundation, *address, *related_address;
    gint port, priority, related_port;
    gchar *candidate_type_name, *component_type_name, *transport_type_name;
    g_object_get(candidate,
            "ufrag", &ice_ufrag,
            "password", &ice_password,
            "type", &candidate_type,
            "component-type", &component_type,
            "foundation", &foundation,
            "transport-type", &transport_type,
            "address", &address,
            "port", &port,
            "priority", &priority,
            "base-address", &related_address,
            "base-port", &related_port, NULL);

    // Convert the numbered types to strings
    switch ( candidate_type )
    {
        case OWR_CANDIDATE_TYPE_HOST             : candidate_type_name = "host"; break;
        case OWR_CANDIDATE_TYPE_SERVER_REFLEXIVE : candidate_type_name = "srflx"; break;
        case OWR_CANDIDATE_TYPE_PEER_REFLEXIVE   : candidate_type_name = "prflx"; break;
        case OWR_CANDIDATE_TYPE_RELAY            : candidate_type_name = "relay"; break;
        default: candidate_type_name = "unknown"; break;
    }
    switch ( component_type )
    {
        case OWR_COMPONENT_TYPE_RTP  : component_type_name = "rtp"; break;
        case OWR_COMPONENT_TYPE_RTCP : component_type_name = "rtcp"; break;
        default: component_type_name = "unknown"; break;
    }
    gchar *transport = "tcp";
    switch ( transport_type )
    {
        case OWR_TRANSPORT_TYPE_UDP:
            transport = "udp";
            transport_type_name = "udp";
            break;
        case OWR_TRANSPORT_TYPE_TCP_ACTIVE   : transport_type_name = "active"; break;
        case OWR_TRANSPORT_TYPE_TCP_PASSIVE  : transport_type_name = "passive"; break;
        case OWR_TRANSPORT_TYPE_TCP_SO       : transport_type_name = "so"; break;
        default: transport_type_name = "unknown"; break;
    }
    //g_print("candidate: type:%s ufrag:%s icepassword:%s component-type:%s foundation:%s transport_type:%i"
    //        " address:%s port:%i priority:%i related_address:%s related_port:%i\n",
    //        candidate_type_name, ice_ufrag, ice_password, component_type_name, foundation, transport_type,
    //        address, port, priority, related_address, related_port);

    // Output SDP ICE candidate line.
    g_print("candidate:%s %i %s %i %s %i typ %s",
            foundation, component_type, transport, priority, address, port, candidate_type_name);
    if (transport_type != OWR_TRANSPORT_TYPE_UDP) { // tcp
        g_print(" tcptype %s", transport_type_name);
    }
    if (candidate_type == OWR_CANDIDATE_TYPE_SERVER_REFLEXIVE) {
        g_print(" raddr %s rport %i", related_address, related_port);
    }
    g_print(" generation 0\n");
}

OwrCandidate* sdp_2_candidate(gchar *line)
{
    OwrCandidate *remote_candidate;
    OwrCandidateType candidate_type;
    OwrComponentType component_type;
    OwrTransportType transport_type;
    gchar cand_type[100], foundation[100], transport[100], address[100],
          tcp_type[100], candidate_type_name[100], r_address[100];
    gint priority, port, r_port;

    sscanf( line, "candidate:%s %u %s %u %s %u typ %s",
            foundation, &component_type, transport, &priority, address, &port, candidate_type_name);

    if ( strcmp(candidate_type_name, "host") == 0 )  candidate_type = OWR_CANDIDATE_TYPE_HOST;
    if ( strcmp(candidate_type_name, "srflx") == 0 ) candidate_type = OWR_CANDIDATE_TYPE_SERVER_REFLEXIVE;
    if ( strcmp(candidate_type_name, "prflx") == 0 ) candidate_type = OWR_CANDIDATE_TYPE_PEER_REFLEXIVE;
    if ( strcmp(candidate_type_name, "relay") == 0 ) candidate_type = OWR_CANDIDATE_TYPE_RELAY;

    if ( strcmp(transport, "udp") == 0 )
        transport_type = OWR_TRANSPORT_TYPE_UDP;
    else
        transport_type = OWR_TRANSPORT_TYPE_TCP_ACTIVE;
    if ( transport_type != OWR_TRANSPORT_TYPE_UDP ) { // tcp
        sscanf( line, "candidate:* * * * * * typ * tcptype %s", tcp_type);
        if (!g_strcmp0(tcp_type, "active"))
            transport_type = OWR_TRANSPORT_TYPE_TCP_ACTIVE;
        else if (!g_strcmp0(tcp_type, "passive"))
            transport_type = OWR_TRANSPORT_TYPE_TCP_PASSIVE;
        else
            transport_type = OWR_TRANSPORT_TYPE_TCP_SO;
    }
    if (candidate_type == OWR_CANDIDATE_TYPE_SERVER_REFLEXIVE) {
        sscanf( line, "candidate:* * * * * * typ * raddr %s rport %u", r_address, &r_port);
    }

    remote_candidate = owr_candidate_new(candidate_type, component_type);
    g_object_set(remote_candidate, "foundation", foundation, NULL);
    g_object_set(remote_candidate, "transport-type", transport_type, NULL);
    g_object_set(remote_candidate, "address", address, NULL);
    g_object_set(remote_candidate, "port", port, NULL);
    g_object_set(remote_candidate, "priority", priority, NULL);

    g_print("CANDIDATE: foundation:%s component-type:%i transport:%s priority:%i address:%s port:%i typ:%s %i\n", foundation, component_type, transport, priority, address, port, candidate_type_name, candidate_type);

    return remote_candidate;
}
