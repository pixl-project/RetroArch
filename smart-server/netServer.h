#ifndef __NETSERVER_H__
# define __NETSERVER_H__

#include "netSharedDefines.h"
#include "netInit.h"
#include "netPackets.h"
#include "netPacketsDef.h"

network_t netInfo;

void *launch_smartserver(void*);
void send_broadcast_packet(void);
void disconnect_client(client_t*);
void test_still_connected(void);

#endif
