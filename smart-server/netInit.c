#include "netInit.h"

void init_server(network_t* netInfo)
{
   netInfo->serverTCP.sin_addr.s_addr = htonl(INADDR_ANY);
   netInfo->serverTCP.sin_port = htons(TCP_PORT);
   netInfo->serverTCP.sin_family = AF_INET;
 
   netInfo->sSocketTCP = xsocket(AF_INET, SOCK_STREAM, 0);
   xbind(netInfo->sSocketTCP, (const struct sockaddr *)&netInfo->serverTCP, sizeof(netInfo->serverTCP));
   xlisten(netInfo->sSocketTCP, MAX_CLIENT);

   netInfo->sSocketUDP = xsocket(AF_INET, SOCK_DGRAM, 0);
   uint32_t broadcast = 1;
   if (setsockopt(netInfo->sSocketUDP, SOL_SOCKET, SO_BROADCAST, (void*)&broadcast, sizeof(broadcast)) < 0)
   {
      RARCH_ERR("Smart-Server: setsockopt error");
      exit(EXIT_FAILURE);
   }

   netInfo->serverUDP.sin_addr.s_addr = htonl(INADDR_BROADCAST);
   netInfo->serverUDP.sin_port = htons(UDP_PORT);
   netInfo->serverUDP.sin_family = AF_INET;

   netInfo->nbClients = 0;

   RARCH_LOG("Smart-Server: Server is listenning on TCP port %d and UDP port %d\n", htons(netInfo->serverTCP.sin_port), htons(netInfo->serverUDP.sin_port));
}

int32_t find_free_idx(network_t* netInfo)
{
   uint8_t i;
   for (i = 0; i < MAX_CLIENT; i++)
   {
      if (netInfo->clients[i] == NULL)
         return i;
   }
   return -1;
}

uint32_t new_client(network_t* netInfo, uint8_t* maxsocket)
{
   struct sockaddr_in csin;
   size_t csinsize = sizeof(csin);

   uint32_t csock = accept(netInfo->sSocketTCP, (struct sockaddr *)&csin, (socklen_t *)&csinsize);

   // setup new client
   client_t* client = malloc(sizeof(*client));
   client->socket = csock;
   client->type = 0; 
   strcpy(client->name, "undefined");
   strcpy(client->ip, inet_ntoa(csin.sin_addr));

   RARCH_LOG("Smart-Server: New client (%s) connected.\n", client->ip);

   int32_t idx = find_free_idx(netInfo);
   if (idx != -1)
      netInfo->clients[idx] = client;
   else
      netInfo->clients[netInfo->nbClients] = client;
   netInfo->nbClients++;

   *maxsocket = client->socket > *maxsocket ? client->socket : *maxsocket;

   return (client->socket);
}