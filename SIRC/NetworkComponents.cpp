#include "NetworkComponents.h"

std::map<SOCKET,std::string> gUsers;
std::vector<SOCKET> gSockets;
std::vector<std::thread> gThreads;
std::mutex sender;
std::mutex couter;
bool doExit = false;

SOCKET initConnection(char* port) {
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;

	PCSTR DEFAULT_PORT = port;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return INVALID_SOCKET;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return INVALID_SOCKET;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return INVALID_SOCKET;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return INVALID_SOCKET;
	}

	freeaddrinfo(result);

	return ListenSocket;
}

void receiveData(SOCKET ClientSocket) {
	int iResult;
	char recvbuf[DEFAULT_BUFLEN+1];
	int recvbuflen = DEFAULT_BUFLEN;
	std::string ans = "";
	// Receive until the peer shuts down the connection
	do {

		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			recvbuf[iResult] = '\0';
			for (int i = 0; i < iResult; i++) {
				if (recvbuf[i] != '\0') {
					ans += recvbuf[i];
				}
			}
			if (ans[ans.size() - 1] == '\n') {
				couter.lock();
				std::cout << gUsers[ClientSocket];
				std::cout << ":";
				std::cout << ans;
				couter.unlock();
				for (int i = 0; i < gSockets.size(); i++) {
					if (gSockets[i] != ClientSocket) {
						sendData(gSockets[i], gUsers[ClientSocket]);
						sendData(gSockets[i], ":");
						sendData(gSockets[i], ans);
						sendData(gSockets[i], "\r");
					}
				}
			}
		}
		else if(iResult != 0) {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return;
		}

	} while (iResult > 0);
	couter.lock();
	std::cout << "USER DISCONNECTED " << gUsers[ClientSocket] << std::endl;
	couter.unlock();
	for (int i = 0; i < gSockets.size(); i++) {
		if (gSockets[i] != ClientSocket) {
			sendData(gSockets[i], "sysmes");
			sendData(gSockets[i], "User disconnected ");
			sendData(gSockets[i], gUsers[ClientSocket] + "\r\n");
		}
	}
	gSockets.erase(std::find(gSockets.begin(), gSockets.end(), ClientSocket));
	gUsers.erase(ClientSocket);
	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return;
	}
	// cleanup
	closesocket(ClientSocket);
	return;
}

void sendData(SOCKET& ClientSocket, std::string message) {
	sender.lock();
	int iResult;
	int iSendResult;
	// Receive until the peer shuts down the connection
	if (ClientSocket != INVALID_SOCKET) {
		iSendResult = send(ClientSocket, message.c_str(), message.size(), 0);
		if (iSendResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return;
		}
	}
	sender.unlock();
}

void authUser(SOCKET ClientSocket) {
	int iResult;
	char recvbuf[DEFAULT_BUFLEN + 1];
	int recvbuflen = DEFAULT_BUFLEN;
	std::string ans = "";
	do {
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			recvbuf[iResult] = '\0';
			for (int i = 0; i < iResult; i++) {
				if (recvbuf[i] != '\0') {
					ans += recvbuf[i];
				}
			}
			if (ans[ans.size() - 1] == '\n') {
				gUsers[ClientSocket] = "";
				for (int i = 0; i < ans.size(); i++) {
					if (ans[i] > 31 && ans[i] != 127) {
						gUsers[ClientSocket] += ans[i];
					}
				}
				ans = "";
			}
		}
		else if(iResult != 0){
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return;
		}
	} while (ans != "");
}

void waitForClients(SOCKET& ListenSocket) {
	while (!doExit) {
		int iResult = listen(ListenSocket, SOMAXCONN);
		if (iResult == SOCKET_ERROR) {
			printf("listen failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return;
		}
		SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return;
		}
		couter.lock();
		std::cout << "CLIENT CONNECTED. WAITING FOR AUTHORISATION" << std::endl;
		couter.unlock();
		sendData(ClientSocket, "Please enter your nickname: ");
		authUser(ClientSocket);
		couter.lock();
		std::cout << "USER CONNECTED " << gUsers[ClientSocket] << std::endl;
		for (int i = 0; i < gSockets.size(); i++) {
			if (gSockets[i] != ClientSocket) {
				sendData(gSockets[i], "sysmes");
				sendData(gSockets[i], "User connected ");
				sendData(gSockets[i], gUsers[ClientSocket]+"\r\n");
			}
		}
		couter.unlock();
		gSockets.push_back(ClientSocket);
		gThreads.push_back(std::thread(receiveData, std::ref(ClientSocket)));
		gThreads[gThreads.size() - 1].detach();
	}
}