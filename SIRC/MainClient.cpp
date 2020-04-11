#include "NetworkComponents.h"
#include "RSA.h"
using namespace std;

int main(int argc, char** argv) {
	/*if (argc < 2) {
		cout << "Please select port!" << endl;
		return 0;
	}
	long long p = genPrime();
	long long q = genPrime();
	while (p == q) {
		q = genPrime();
	}
	long long n = p * q;
	long long t = (p - 1) * (q - 1);
	long long e = calculateE(t);
	long long d = calculateD(e, t);*/
	system("chcp 65001");
	SOCKET ListenSocket = initConnection(argv[1]);
	cout << "SERVER SUCCESFULLY STARTED ON PORT " << argv[1] << ". WAITING FOR CLIENT TO CONNECT" << endl;
	waitForClients(ListenSocket);
	int iResult = 0;
	iResult = shutdown(ListenSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	closesocket(ListenSocket);
	WSACleanup();
	return 0;
}