#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <winsock2.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib") //Winsock library

int main(int argc, char* argv[]) {
	WSADATA wsa;
	SOCKET sock, new_sock;
	struct sockaddr_in server, client;
	int c;

	//1. Initialize Winsock
	std::cout << std::endl << "Initialising Winsock...";
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("\nWinsock init failed. Error: %d ", WSAGetLastError());
		return 1;
	}
	std::cout << "\nWinsock init complete." << std::endl;

	//2. Create socket
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket. Error: %d ", WSAGetLastError());
		ExitProcess(EXIT_FAILURE);
	}
	std::cout << "Socket created." << std::endl;

	//3. Define the socket structure
	ZeroMemory(&server, sizeof(server));
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(80);

	//4. Bind
	if (bind(sock, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed. Error: %d ", WSAGetLastError());
		ExitProcess(EXIT_FAILURE);
	}
	puts("Binding done");

	//5. Listen
	listen(sock, 5);

	//6. Accept the incoming connections
	puts("Waiting for incoming connections");

	c = sizeof(struct sockaddr_in);
	new_sock = accept(sock, (struct sockaddr*)&client, &c);
	if (new_sock == INVALID_SOCKET) {
		printf("Accept failed. Error: %d", WSAGetLastError());
	}
	puts("Connection accepted");

	closesocket(sock);
	WSACleanup();
	return 0;
}

