#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<tchar.h>
#include<thread>
#include<vector>
using namespace std;

#pragma comment(lib, "ws2_32.lib")
/*
	Key Steps:

	- Initialize winsock library

	- Get IP and port

	- Bind the IP/Port with the socket

	- Listen on the socket

	- Accept

	- Recieve and send

	- Close the socket

	- Clean the winsock

*/

bool initialize() {
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}


void InteractWithClient(SOCKET clientSocket, vector<SOCKET>&clients) {
	// sending/receiving will be done here

	cout << "Client connected\n";

	char buffer[4096];
	
	while (true) {
		int bytesreceived = recv(clientSocket, buffer, sizeof(buffer), 0);

		if (bytesreceived <= 0) {
			cout << "Client disconnected\n";
			break;
		}
		string message(buffer, bytesreceived);
		cout << "Message from client : " << message << "\n";

		for (auto client : clients) {
			if (client != clientSocket) {
				send(client, message.c_str(), message.length(), 0);
			}
		}
	}

	auto it = find(clients.begin(), clients.end(), clientSocket);
	if (it != clients.end()) {
		clients.erase(it);
	}

	closesocket(clientSocket);

}


int main() {
	if (!initialize()) {
		cout << "Winsock initialization failed\n";
	}

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (listenSocket == INVALID_SOCKET) {
		cout << "Socket creation failed\n";
		return 1;
	}


	// Creating address structure
	int port = 123;
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);


	// Converting the IP address (0.0.0.0) and putting it in the sin_family in binary format
	if (InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1) {
		cout << "Setting address structure failed\n";
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// binding the IP to the socket 
	if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
		cout << "Binding failed\n";
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// listening on the socket
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		cout << "Listening failed\n";
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	cout << "Server is listening on port : " << port << "\n";

	vector<SOCKET>clients;
	
	while (true) {
		// Accepting the connection from client
		SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
		if (clientSocket == INVALID_SOCKET) {
			cout << "Invalid client socket\n";
		}

		clients.push_back(clientSocket);

		thread t1(InteractWithClient, clientSocket, std::ref(clients));
		t1.detach();
	}
	
	
	closesocket(listenSocket);

	WSACleanup();
	return 0;
}