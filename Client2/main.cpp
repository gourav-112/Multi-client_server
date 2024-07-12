#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<thread>
#include<string>
using namespace std;
#pragma comment(lib, "ws2_32.lib")
/*
	Key Steps:

	- Initialize winsock

	- Create socket

	- Connect to the server

	- Send/receive

	- Close the socket

*/

bool initialize() {
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void SendMsg(SOCKET s) {

	cout << "Enter your chat name : \n";
	string name;
	getline(cin, name);

	string message;
	while (true) {
		getline(cin, message);
		string msg = name + " : " + message;
		int bytessent = send(s, msg.c_str(), msg.length(), 0);

		if (bytessent == SOCKET_ERROR) {
			cout << "Error_sending_message\n";
			break;
		}

		if (message == "quit") {
			cout << "Stopping the Application\n";
			break;
		}

	}

	closesocket(s);
	WSACleanup();

}

void ReceiveMsg(SOCKET s) {
	char buffer[4096];
	int receivelength;
	string msg;
	while (true) {
		receivelength = recv(s, buffer, sizeof(buffer), 0);
		if (receivelength <= 0) {
			cout << "Disconnected from server\n";
			break;
		}
		else {
			msg = string(buffer, receivelength);
			cout << msg << "\n";
		}
	}

	closesocket(s);
	WSACleanup();

}

int main() {

	if (!initialize()) {
		cout << "Initialization failed\n";
		return 1;
	}

	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, 0);

	if (s == INVALID_SOCKET) {
		cout << "Invalid socket created\n";
		return 1;
	}
	int port = 123;
	string serveraddress = "127.0.0.1";
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	inet_pton(AF_INET, serveraddress.c_str(), &(serveraddr.sin_addr));


	if (connect(s, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
		cout << "Not able to connect to server\n";
		closesocket(s);
		WSACleanup();
		return 1;
	}

	cout << "Successfully connected to the server\n";

	thread senderthread(SendMsg, s);
	thread receiverthread(ReceiveMsg, s);

	senderthread.join();
	receiverthread.join();

	return 0;
}