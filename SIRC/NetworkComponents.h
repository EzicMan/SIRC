#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <map>
#include <thread>
#include <vector>
#include <mutex>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512

extern std::map<SOCKET,std::string> gUsers;
extern std::vector<SOCKET> gSockets;
extern bool doExit;
extern std::vector<std::thread> gThreads;

SOCKET initConnection(char* port);
void receiveData(SOCKET ClientSocket);
void sendData(SOCKET& ClientSocket, std::string);
void authUser(SOCKET ClientSocket);
void waitForClients(SOCKET& ListenSocket);