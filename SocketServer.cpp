#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#include "SocketServer.h"
#pragma comment(lib, "ws2_32.lib") //Winsock library

WSADATA wsa;
SOCKET lis_sock;
SOCKET acc_sock;
struct sockaddr_in server, client;
int c, nb_sock;
WSAPOLLFD fds[2];
u_long ul = 1;

int main(int argc, char* argv[]) {
	//1. Initialize Winsock
	std::cout << std::endl << "Initialising Server...";
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("\nServer init failed. Error: %d ", WSAGetLastError());
		return 1;
	}
	std::cout << "\nServer init complete." << std::endl;

	//2. Create socket
	if ((lis_sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
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
	if (bind(lis_sock, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed. Error: %d ", WSAGetLastError());
		ExitProcess(EXIT_FAILURE);
	}
	puts("Binding done");

	//5. Listen
	listen(lis_sock, 5);

	//set nb here
	turn_socket_into_nonblocking();

	//6. Accept the incoming connections - put into stream
	puts("Waiting for incoming connections");

	c = sizeof(struct sockaddr_in);
	acc_sock = accept(lis_sock, (struct sockaddr*)&client, &c);
	if (acc_sock == INVALID_SOCKET) {
		printf("Accept failed. Error: %d\n", WSAGetLastError());
	}
	else {
		puts("Connection accepted");
		//7. handshake to client
		int scheck = send(acc_sock, "Hello client", 13, 0);
		if (scheck == SOCKET_ERROR) {
			printf("Send failed. Error: %d\n", WSAGetLastError());
		}
	}

	closesocket(lis_sock);
	closesocket(acc_sock);
	WSACleanup();
	return 0;
}

int turn_socket_into_nonblocking() {
	//4b. Turn the socket into non-blocking
	nb_sock = ioctlsocket(lis_sock, FIONBIO, &ul);
	if (nb_sock == SOCKET_ERROR) {
		printf("ioctlsocket() failed with error %d\n", WSAGetLastError());
		return 1;
	}
	else
		printf("ioctlsocket() is OK!\n");

	//4c. define pollfd sock and events and start polling
	fds[0].fd = lis_sock;
	fds[0].events = POLLIN;
	int results = WSAPoll(fds, 1, 10000); //10s
	if (results > 0) {
		if (fds[0].revents & POLLIN) //able to read
			printf("data read\n");
	}
	else if (results == 0) {
		printf("Poll timed out\n");
	}
	else {
		printf("Poll error\n");
	}
	return 0;
}
