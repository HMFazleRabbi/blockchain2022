// SocketClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// socketProgramming.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <winsock.h>

// Library effective with Windows
#include <windows.h>
#define LOG(X) std::cout<<X<<std::endl;
#define PORT 9909
int nRet = 0;

void InitializeSocket(int& nSocket, struct  sockaddr_in &srv) {
	WSADATA ws;
	if (WSAStartup(MAKEWORD(2, 2), &ws)) { std::cout << std::endl << "WSA failed"; }
	else { LOG("Created WSA successfully."); }

	//Open a socket - listener
	nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (nSocket < 0) { LOG("Failed to Initialize Socket!"); }
	else { LOG("Initialize Socket successfully."); }


	//sockaddr structure

	srv.sin_family = AF_INET;
	srv.sin_addr.s_addr = inet_addr("192.168.50.8"); //INADDR_ANY only for server, for client use other methor like address inet.addr("192.168.50.8")
	srv.sin_port = htons(PORT);
	memset(&(srv.sin_zero), 0, 8);

	//Setsockopt
	int nOptVal = 0;
	int nOptLen = sizeof(nOptVal);
	nRet = setsockopt(nSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&nOptVal, nOptLen);
	if (!nRet) {
		LOG("setsockopt successful!")
	}
	else {
		LOG("Failed setsockopt")
			WSACleanup(); exit(EXIT_FAILURE);
	}

	//Blocking vs unblocking
	u_long optval = 0;
	nRet = ioctlsocket(nSocket, FIONBIO, &optval);
	if (nRet != 0) {
		LOG("Failed ioctlsocket")
			WSACleanup(); exit(EXIT_FAILURE);
	}
	else {
		LOG("ioctlsocket successful!")
	}

}

void ConnectToServer(int& nSocket, struct  sockaddr_in &srv) {

	for (size_t i = 0; i < 3; i++)
	{
		nRet = connect(nSocket, (sockaddr*)&srv, sizeof(srv));
		if (nRet >= 0)
			break;
		LOG("Attempt(" << i + 1 << "/3): Cannot connect to server...");
		Sleep(5000);
	}

	if (nRet < 0) {
		LOG("Error: Cannot connect to server");
		WSACleanup(); exit(EXIT_FAILURE);
	}
	else {
		char buf[1024] = { 0 };
		recv(nSocket, buf, 1024, 0);
		LOG(buf);

	}

}


int main()
{
	//Initialize Socket
	int nClientSocket;
	struct  sockaddr_in srv;
	InitializeSocket(nClientSocket, srv);

	//Connection
	ConnectToServer(nClientSocket, srv);
	send(nClientSocket, "HM Fazle Rabbi has entered the chat", 37, 0);
	char buf[1024] = { 0 };

	//Message
	while (true)
	{
		memset(buf, 0, 1024);
		std::cout << "HM Fazle Rabbi:\t";
		fgets(buf, 1024, stdin);
		send(nClientSocket, buf, 1024, 0);
		std::cout << "Reply:\t";
		recv(nClientSocket, buf, 1024, 0); 
		std::cout << buf << "\n";
		Sleep(500);

	}

}
