// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <enet/enet.h>

struct Vector3 {
	float x;
	float y;
	float z;
};

struct ClientData {
	int packetType = 0;
	int clientIndex;
};

struct ClientPacket {
	int clientIndex;
	Vector3 position;
	int objectIndex;
};

struct PhysicsData{
	int packetType = 1;
	Vector3 position;
	int index;
};

int main()
{
	bool programRunning = true;

	int clientCount = 0;

	if (enet_initialize() != 0) {
		std::cout << "Enet initialisation failed." << std::endl;
	}

	ENetAddress address;
	ENetHost* server;
	ENetEvent enetEvent;
	ENetPacket* dataPacket;

	address.host = ENET_HOST_ANY;
	address.port = 140110;
	server = enet_host_create(&address, 32, 2, 0, 0);
	if (server == NULL) {
		std::cout << "Server initialisation failed." << std::endl;
	}

	ClientData* data = new ClientData;
	ClientPacket* clientPacket = new ClientPacket;
	PhysicsData* receivedData = new PhysicsData;

	while (programRunning) {
		while (enet_host_service(server, &enetEvent, 0) > 0) {
			switch (enetEvent.type) {
			case ENET_EVENT_TYPE_CONNECT:
				std::cout << "A client connected from address " << enetEvent.peer->address.host << ":" << enetEvent.peer->address.port << std::endl;
				data->clientIndex = clientCount;

				dataPacket = enet_packet_create(data, sizeof(ClientData), ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(enetEvent.peer, 0, dataPacket);

				enetEvent.peer->data = (void*)"This is a client";
				clientCount++;

				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				std::cout << "The client from address " << enetEvent.peer->address.host << ":" << enetEvent.peer->address.port << " disconnected" << std::endl;

				enetEvent.peer->data = NULL;

				clientCount--;
				if (clientCount == 0) {
					programRunning = false;
				}

				break;
			case ENET_EVENT_TYPE_RECEIVE:
				memcpy(clientPacket, enetEvent.packet->data, enetEvent.packet->dataLength);
				int currentClient = clientPacket->clientIndex;
				receivedData->position = clientPacket->position;
				receivedData->index = clientPacket->objectIndex;
				if (clientCount >= 2 && receivedData->index != 0 && receivedData->index != 14) {
					std::cout << "Should send data." << std::endl;
					dataPacket = enet_packet_create(receivedData, sizeof(PhysicsData), ENET_PACKET_FLAG_RELIABLE);
					enet_host_broadcast(server, 0, dataPacket);
				}
			}
		}
	}

	enet_host_destroy(server);
	enet_deinitialize();
}

