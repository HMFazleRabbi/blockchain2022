// socketProgramming.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <winsock.h>

// Library effective with Windows
#include <windows.h>
#define LOG(X) std::cout<<X<<std::endl;


#define PORT 19909
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
	srv.sin_addr.s_addr = INADDR_ANY; //INADDR_ANY only for server, for client use other methor like address inet.addr("127.0.0.1")
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

void BindSocket(int& nSocket, struct  sockaddr_in &srv) {
	//Bind address to local port
	//Only for listener
	nRet = bind(nSocket, (sockaddr*)&srv, sizeof(sockaddr));
	if (nRet < 0) {
		LOG("Failed to Bind address to local port")
			WSACleanup(); exit(EXIT_FAILURE);
	}
	else {
		LOG("Bind address to local port successful!")
	}
}

void InitListener(int& nSocket, int nMaxClient) {
	nRet = listen(nSocket, nMaxClient);
	if (nRet < 0) {
		LOG("Failed to Listen the request from  client")
			WSACleanup(); exit(EXIT_FAILURE);
	}
	else {
		LOG("Listening for request from  client...")
	}

}
int main()
{
	//Initialize Socket
	int nSocket;
	struct  sockaddr_in srv;
	InitializeSocket(nSocket, srv);

	//Bind
	BindSocket(nSocket, srv);


	//Listen the request from  client (queue the request)
	InitListener(nSocket, 5);

	//Wait for connection
	LOG("Waiting for client connection....");
	int nLen = sizeof(struct  sockaddr);
	int nClientSocket = accept(nSocket, (sockaddr*)&srv, &nLen);

	//Receive message
	char sBuff[1024] = { 0 };
	while (true)
	{
		memset(sBuff, 0, 1024);
		nRet = recv(nClientSocket, sBuff, 1024, 0);
		if (nRet < 0) {
			LOG("ERROR: Cannot receive message!")
			WSACleanup(); exit(EXIT_FAILURE);

		}
		if(sBuff[0]!='0')
			LOG("Received Message:\t" << sBuff);
		LOG("...")
		Sleep(5000);

	}

}
