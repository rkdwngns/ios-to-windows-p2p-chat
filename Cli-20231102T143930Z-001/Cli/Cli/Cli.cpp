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
		// 상대 클라이언트로부터 메시지 수신 및 출력
		int bytesReceived = recvfrom(clientSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&remoteAddr, &remoteAddrLen);
		if (bytesReceived  > 0) {
			buffer[bytesReceived] = '\0';
			std::cout << "상대 클라이언트로부터 받은 내용: " << buffer << std::endl;
		}
		
	}

}

int main() {
	// 콘솔 타이틀 설정
	SetConsoleTitleA("클라이언트A");

	// WinSock 초기화
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup 실패" << std::endl;
		return 1;
	}

	// UDP 클라이언트 소켓 생성
	clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (clientSocket == INVALID_SOCKET) {
		HandleError("클라이언트 소켓 생성 실패");
	}

	// 클라이언트 소켓 주소 설정
	sockaddr_in clientAddr;
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(55895); // 클라이언트가 사용할 로컬 포트 번호
	clientAddr.sin_addr.s_addr = INADDR_ANY;

	// 클라이언트 소켓 주소에 바인딩
	if (bind(clientSocket, (struct sockaddr*)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR) {
		HandleError("클라이언트 소켓 바인딩 실패");
	}
	/*
	
	*/
	
	// 서버 주소 설정
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(25592); // 서버 포트 번호
	serverAddr.sin_addr.s_addr = inet_addr("10.11.110.40"); // 서버 IP 주소

	// 서버에 주소 요청 메시지 전송
	if (sendto(clientSocket, "RequestAddress", sizeof("RequestAddress"), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		HandleError("요청 전송 실패");
	}

	int serverAddrLen = sizeof(serverAddr);
	char buffer[BUFFER_SIZE];


	// 서버로부터 주소 정보 수신
	int bytesReceived = recvfrom(clientSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&serverAddr, &serverAddrLen);
	if (bytesReceived == SOCKET_ERROR) {
		HandleError("주소 정보 수신 실패");
		return -1;
	}
	buffer[bytesReceived] = '\0';

	// 받은 주소 정보를 문자열로 변환
	std::string addressInfo(buffer);

	// 서버에서 받은 주소 정보 처리
	//if (addressInfo.substr(0, 12) == "PeerAddress:") {
		std::string peerAddressData = addressInfo.substr(12);
		size_t colonPos = peerAddressData.find(':');
		if (colonPos != std::string::npos) {
			// 상대 클라이언트의 IP 주소와 포트 번호 추출
			std::string peerIP = peerAddressData.substr(0, colonPos);
			int peerPort = std::stoi(peerAddressData.substr(colonPos + 1));

		

			std::cout << "Received address from server: " << peerIP << ":" << peerPort << std::endl;
			

			// 상대 클라이언트의 주소 설정
			sockaddr_in peerAddr;
			peerAddr.sin_family = AF_INET;
			peerAddr.sin_port = htons(peerPort);
			peerAddr.sin_addr.s_addr = inet_addr(peerIP.c_str());


			std::thread t1(Receive);
			t1.detach();


			while (true) {
				std::string message;
				std::cout << "메시지를 입력하세요: ";
				std::getline(std::cin, message);
				sendto(clientSocket, message.c_str(), message.length(), 0, (sockaddr*)&peerAddr, sizeof(peerAddr));
				std::cout << std::endl;
			}
		}
	//}

	// 클라이언트 소켓 닫기 및 WinSock 정리
	closesocket(clientSocket);
	WSACleanup();
	return 0;
}
