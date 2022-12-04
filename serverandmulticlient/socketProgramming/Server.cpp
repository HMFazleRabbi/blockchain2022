// socketProgramming.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <winsock.h>

// Library effective with Windows
#include <windows.h>
#define LOG(X) std::cout<<X<<std::endl;


static fd_set fr, fw, fe;
#define PORT 9909
#define MaxClient 5
int nRet = 0;
int nArrClient[MaxClient] = { 0 };

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

void ProcessMessage(int& nClientSocket, struct  sockaddr_in &srv) {
	LOG("Process the new message for client socket: " << nClientSocket);
	char buff[1024] = { 0 };
	nRet = recv(nClientSocket, buff, 1024, 0);
	if (nRet < 0) {
		closesocket(nClientSocket);
		for (size_t i = 0; i < MaxClient; i++)
		{
			nArrClient[i] = nArrClient[i] == nClientSocket ? 0 : nArrClient[i];
		}
		LOG("Error: Something s wrong, closing connection!");
	}
	else {
		LOG("Message:\t" << buff);
		send(nClientSocket, "Processed your request", 23, 0);
		LOG("******************************");
	}
}

void ProcessTheNewRequest(int& nSocket, struct  sockaddr_in &srv) 
{
	if (FD_ISSET(nSocket, &fr)) {
		int nLen = sizeof(struct  sockaddr);
		int nClientSocket = accept(nSocket, NULL, &nLen);
	
		if (nClientSocket > 0) {
			LOG("Client connection at port "<< nSocket <<" accepted.");
			int index;
			for (index = 0; index < MaxClient; index++)
			{
				nArrClient[index] = nClientSocket;
				send(nClientSocket, "Connection to server successful.", 33, 0);
				break;
			}
			if (index == 5) {
				LOG("No space available.");
			}
			ProcessMessage(nClientSocket, srv);
		}
	}
	else {
		int index;
		for (index = 0; index < MaxClient; index++)
		{
			if (nArrClient[index]) {
				ProcessMessage(nArrClient[index], srv);
			}
		}
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
	InitListener(nSocket, MaxClient);



	//Message
	timeval tval;
	tval.tv_sec = 2;
	tval.tv_usec = 0;
	int nMax = nSocket;
	while (1) {
		//Reset and add to queue of socket that needs to be 
		//checked for connection/request
		FD_ZERO(&fr); // fr.fd_count is 0
		FD_ZERO(&fw); // fw.fd_count is 0
		FD_ZERO(&fe); // fe.fd_count is 0


		FD_SET(nSocket, &fr); // fr.fd_count set to 1
		FD_SET(nSocket, &fw); // fw.fd_count set to 1
		for (size_t i = 0; i < MaxClient; i++)
		{
			if (nArrClient[i] != 0) {
				FD_SET(nArrClient[i], &fr);
				FD_SET(nArrClient[i], &fe);

			}
		}


		nRet = select(nMax + 1, &fr, &fw, &fe, &tval);
		if (nRet > 0) {
			LOG("Data on port, porcessing now....");
			ProcessTheNewRequest(nSocket, srv);
		}
		else if (nRet == 0) {
			LOG("Nothing on PORT: " << PORT);
		}
		else {
			LOG("Error waiting for connection...")
				WSACleanup();exit(EXIT_FAILURE);
		}

		Sleep(500);
	}
	

	
	


}
