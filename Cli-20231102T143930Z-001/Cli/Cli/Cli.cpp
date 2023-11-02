#include "stdafx.h"
#include <iostream>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <string>
#include <thread>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable : 4996)

const int BUFFER_SIZE = 1024;

SOCKET clientSocket;

void HandleError(const std::string& errorMessage) {
	std::cerr << errorMessage << " (Error code: " << WSAGetLastError() << ")" << std::endl;
	closesocket(clientSocket);
	WSACleanup();
	exit(1);
}

void Receive() {
	while (true) {
		SOCKADDR_IN remoteAddr;
		int	remoteAddrLen = sizeof(remoteAddr);
		char buffer[BUFFER_SIZE];
		// ��� Ŭ���̾�Ʈ�κ��� �޽��� ���� �� ���
		int bytesReceived = recvfrom(clientSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&remoteAddr, &remoteAddrLen);
		if (bytesReceived  > 0) {
			buffer[bytesReceived] = '\0';
			std::cout << "��� Ŭ���̾�Ʈ�κ��� ���� ����: " << buffer << std::endl;
		}
		
	}

}

int main() {
	// �ܼ� Ÿ��Ʋ ����
	SetConsoleTitleA("Ŭ���̾�ƮA");

	// WinSock �ʱ�ȭ
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup ����" << std::endl;
		return 1;
	}

	// UDP Ŭ���̾�Ʈ ���� ����
	clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (clientSocket == INVALID_SOCKET) {
		HandleError("Ŭ���̾�Ʈ ���� ���� ����");
	}

	// Ŭ���̾�Ʈ ���� �ּ� ����
	sockaddr_in clientAddr;
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(55895); // Ŭ���̾�Ʈ�� ����� ���� ��Ʈ ��ȣ
	clientAddr.sin_addr.s_addr = INADDR_ANY;

	// Ŭ���̾�Ʈ ���� �ּҿ� ���ε�
	if (bind(clientSocket, (struct sockaddr*)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR) {
		HandleError("Ŭ���̾�Ʈ ���� ���ε� ����");
	}
	/*
	
	*/
	
	// ���� �ּ� ����
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(25592); // ���� ��Ʈ ��ȣ
	serverAddr.sin_addr.s_addr = inet_addr("10.11.110.40"); // ���� IP �ּ�

	// ������ �ּ� ��û �޽��� ����
	if (sendto(clientSocket, "RequestAddress", sizeof("RequestAddress"), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		HandleError("��û ���� ����");
	}

	int serverAddrLen = sizeof(serverAddr);
	char buffer[BUFFER_SIZE];


	// �����κ��� �ּ� ���� ����
	int bytesReceived = recvfrom(clientSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&serverAddr, &serverAddrLen);
	if (bytesReceived == SOCKET_ERROR) {
		HandleError("�ּ� ���� ���� ����");
		return -1;
	}
	buffer[bytesReceived] = '\0';

	// ���� �ּ� ������ ���ڿ��� ��ȯ
	std::string addressInfo(buffer);

	// �������� ���� �ּ� ���� ó��
	//if (addressInfo.substr(0, 12) == "PeerAddress:") {
		std::string peerAddressData = addressInfo.substr(12);
		size_t colonPos = peerAddressData.find(':');
		if (colonPos != std::string::npos) {
			// ��� Ŭ���̾�Ʈ�� IP �ּҿ� ��Ʈ ��ȣ ����
			std::string peerIP = peerAddressData.substr(0, colonPos);
			int peerPort = std::stoi(peerAddressData.substr(colonPos + 1));

		

			std::cout << "Received address from server: " << peerIP << ":" << peerPort << std::endl;
			

			// ��� Ŭ���̾�Ʈ�� �ּ� ����
			sockaddr_in peerAddr;
			peerAddr.sin_family = AF_INET;
			peerAddr.sin_port = htons(peerPort);
			peerAddr.sin_addr.s_addr = inet_addr(peerIP.c_str());


			std::thread t1(Receive);
			t1.detach();


			while (true) {
				std::string message;
				std::cout << "�޽����� �Է��ϼ���: ";
				std::getline(std::cin, message);
				sendto(clientSocket, message.c_str(), message.length(), 0, (sockaddr*)&peerAddr, sizeof(peerAddr));
				std::cout << std::endl;
			}
		}
	//}

	// Ŭ���̾�Ʈ ���� �ݱ� �� WinSock ����
	closesocket(clientSocket);
	WSACleanup();
	return 0;
}
