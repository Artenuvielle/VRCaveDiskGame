#include "Network.h"

#include <iostream>

Client::Client() {
	_enetHost = enet_host_create (NULL, 1, 2, 57600 / 8, 14400 / 8);
	if (_enetHost == NULL)
	{
		std::cerr << "An error occurred while trying to create an ENet client host" << std::endl;
		exit (EXIT_FAILURE);
	}
	_packetHandler = nullptr;
}

Client::~Client() {
	enet_host_destroy(_enetHost);
}

void Client::setPacketHandler(SToCPacketHandler* handler) {
	_packetHandler = handler;
}

SToCPacketHandler* Client::getPacketHandler() {
	return _packetHandler;
}

bool Client::connect(const char* hostAdress, short port) {
	if (_peer != nullptr) {
		disconnect();
	}
	keepConnection = true;

	ENetAddress enetAddress;
	enet_address_set_host(&enetAddress, hostAdress);
	enetAddress.port = port;
	_peer = enet_host_connect(_enetHost, &enetAddress, 2, 0); 
	if (_peer == NULL)
	{
		std::cerr << "No available peers for initiating an ENet connection.\n";
		_peer = nullptr;
		return false;
	}
	ENetEvent event;
	if (enet_host_service(_enetHost, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
		if (_packetHandler != nullptr) {
			_packetHandler->handleConnect(event.peer->incomingPeerID);
		}
		std::cout << "Connection to server succeeded\n";
	} else {
		enet_peer_reset (_peer);
		_peer = nullptr;
		std::cout << "Connection to server failed\n";
		return false;
	}
	return true;
}

void Client::disconnect() {
	if(isConnected()) {
		keepConnection = false;
		ENetEvent event;

		enet_peer_disconnect(_peer, 0);
		while (enet_host_service (_enetHost, &event, 3000) > 0)
		{
			if (event.type == ENET_EVENT_TYPE_RECEIVE) {
				enet_packet_destroy(event.packet);
			} else if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
				std::cout << "disconnect acknowledged by server\n";
				break;
			}
		}
		if (_packetHandler != nullptr) {
			_packetHandler->handleDisconnect(event.peer->incomingPeerID);
		}
		enet_peer_reset (_peer);
		_peer = nullptr;
		std::cout << "Disconnection succeeded\n";
		Sleep(10);
	}
}

void Client::networkLoop() {
	if (isConnected()) {
		ENetEvent event;
		SToCPacketType* header;
		void* actualData;
		while (keepConnection && enet_host_service (_enetHost, &event, 0) > 0) {
			switch (event.type) {
			case ENET_EVENT_TYPE_RECEIVE:
				header = reinterpret_cast<SToCPacketType*>(event.packet->data);
				actualData = reinterpret_cast<void*>(header + 1);
				if (_packetHandler != nullptr) {
					_packetHandler->handleSToCPacket(event.peer->incomingPeerID, header, actualData, event.packet->dataLength - sizeof(SToCPacketType));
				}
				enet_packet_destroy(event.packet);
				break;
       
			case ENET_EVENT_TYPE_DISCONNECT:
				if (_packetHandler != nullptr) {
					_packetHandler->handleDisconnect(event.peer->incomingPeerID);
				}
				std::cout << "Server closed connection\n";
				return;
			}
		}
	}
}

bool Client::isConnected() {
	return _peer != nullptr;
}

void Client::sendPacket(CToSPacketType header, void* data, int size, bool reliable) {
	void* packetData = malloc(size + sizeof(CToSPacketType));
	memcpy(packetData, &header, sizeof(CToSPacketType));
	memcpy((reinterpret_cast<unsigned char *>(packetData) + sizeof(CToSPacketType)), data, size);
	ENetPacket* packet = enet_packet_create(packetData, size + sizeof(CToSPacketType), reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
	enet_peer_send(_peer, 1, packet);
}

void networkLoopOnClient(void* client) {
	reinterpret_cast<Client*>(client)->networkLoop();
}