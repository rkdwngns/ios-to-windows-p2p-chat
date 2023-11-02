#include "stdafx.h"
#include <iostream>
#include <map>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

static int PORT = 25592;

SOCKET serverSocket;
std::map<int, sockaddr_in> clientAddresses;

bool InitializeServerSocket();
void CleanUp();

int main() {
	SetConsoleTitleA("홀 펀칭 중개 서버");

	if (!InitializeServerSocket()) {
		return 1;
	}

	char buffer[1024];
	while (true) {
		sockaddr_in clientAddr{};
		int clientAddrLen = sizeof(clientAddr);
		int bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer), 0,
			(struct sockaddr*)&clientAddr, &clientAddrLen);
		buffer[bytesReceived] = '\0';

		if (bytesReceived > 0 && std::string(buffer) == "RequestAddress") {
			int clientId = clientAddresses.size() + 1;
			clientAddresses[clientId] = clientAddr;

			// 클라이언트의 주소 정보 출력
			char clientIP[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
			int clientPort = ntohs(clientAddr.sin_port);
			std::cout << "클라이언트 ID " << clientId << "의 주소: " << clientIP << ":" << clientPort << std::endl;

			// 다른 클라이언트들에게 클라이언트의 주소 정보 전달
			for (const auto& clientInfo : clientAddresses) {
				int otherClientId = clientInfo.first;
				sockaddr_in otherAddr = clientInfo.second;

				if (otherClientId != clientId) { // 자기 자신 제외
					char peerAddress[INET_ADDRSTRLEN];
					inet_ntop(AF_INET, &otherAddr.sin_addr, peerAddress, INET_ADDRSTRLEN);
					int peerPort = ntohs(otherAddr.sin_port);
					std::string addressInfo = "PeerAddress:" + std::string(peerAddress) + ":" + std::to_string(peerPort);
					sendto(serverSocket, addressInfo.c_str(), addressInfo.length(), 0,
						(struct sockaddr*)&clientAddr, sizeof(clientAddr));

					inet_ntop(AF_INET, &clientAddr.sin_addr, peerAddress, INET_ADDRSTRLEN);
					peerPort = ntohs(clientAddr.sin_port);
					 addressInfo = "PeerAddress:" + std::string(peerAddress) + ":" + std::to_string(peerPort);
					sendto(serverSocket, addressInfo.c_str(), addressInfo.length(), 0,
						(struct sockaddr*)&otherAddr, sizeof(otherAddr));
				}
			
			}

			std::cout << "Client with ID " << clientId << " registered and address info sent." << std::endl;
		}
	}

	CleanUp();
	return 0;
}

bool InitializeServerSocket() {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup failed" << std::endl;
		return false;
	}

	serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (serverSocket == INVALID_SOCKET) {
		std::cerr << "Server socket creation failed" << std::endl;
		WSACleanup();
		return false;
	}

	sockaddr_in serverAddr{};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cerr << "bind error" << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		return false;
	}

	std::cout << "서버가 시작되었습니다." << std::endl;

	return true;
}

void CleanUp() {
	closesocket(serverSocket);
	WSACleanup();
}
