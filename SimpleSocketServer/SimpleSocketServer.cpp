// SimpleSocketServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#define WIN32_LEAN_AND_MEAN

#pragma comment (lib, "Ws2_32.lib")

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <thread>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "4101"
#include <random>
#include <functional>

std::uniform_int_distribution<int> dice_distribution(1, 6);
std::mt19937 random_number_engine; // pseudorandom number generator
auto dice_roller = std::bind(dice_distribution, random_number_engine);

void enable_keepalive(int sock) {

    int i = 1;
	setsockopt( sock, IPPROTO_TCP, TCP_NODELAY, (char *)&i, sizeof(i));
	
    int zero = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&zero, sizeof(int));

	zero = 0;
    setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&zero, sizeof(int));
}

void respond(int ClientSocket)
{
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

	char buf[1000];
	time_t now = time(NULL);
	struct tm tm;
	gmtime_s(&tm,&now);
	strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);

	std::string strDate(buf);

	std::string response = "HTTP/1.1 200 OK\nAccess-Control-Allow-Origin: *\nConnection: keep-alive\nDate:"+strDate+"\nTransfer-Encoding: chunked\n\n4\ntrue\n0\n\n\n\n";

	//send( ClientSocket, response.c_str(), response.length(), 0 );

	

			//string response = "HTTP/1.1 200 OK\nAccess-Control-Allow-Origin: *\nAccess-Control-Allow-Methods: POST\nAccess-Control-Allow-Credentials: false\nAccess-Control-Max-Age: 86400\nAccess-Control-Allow-Headers: access-control-allow-headers,access-control-allow-origin,content-type\nConnection: keep-alive\nTransfer-Encoding: chunked\n\n28\n{\"id\":14,\"jsonrpc\":\"2.0\",\"result\":false}\n0\n\n";
	std::string responseT = "HTTP/1.1 200 OK\nAccess-Control-Allow-Origin: *\nConnection: keep-alive\nDate:"+strDate+"\nTransfer-Encoding: chunked\n\n4\ntrue\n0\n\n\n\n";
	std::string responseF = "HTTP/1.1 200 OK\nAccess-Control-Allow-Origin: *\nConnection: keep-alive\nDate:"+strDate+"\nTransfer-Encoding: chunked\n\n5\nfalse\n0\n\n\n\n";

	int iSendResult;
	int iResult = 0;
        // Echo the buffer back to the sender
	
	int random_roll = dice_roller();  // Generate one of the integers 1,2,3,4,5,6.
	if( random_roll >= 4 )
	{
	    send( ClientSocket, responseT.c_str(), responseT.length(), 0 );
	}
	else
	{
	    send( ClientSocket, responseF.c_str(), responseF.length(), 0 );
	}

	//printf("Bytes sent: %d\n", response.length());
	shutdown(ClientSocket,SD_SEND);
	closesocket( ClientSocket);
	/*
	do {
        int iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);

			//string response = "HTTP/1.1 200 OK\nAccess-Control-Allow-Origin: *\nAccess-Control-Allow-Methods: POST\nAccess-Control-Allow-Credentials: false\nAccess-Control-Max-Age: 86400\nAccess-Control-Allow-Headers: access-control-allow-headers,access-control-allow-origin,content-type\nConnection: keep-alive\nTransfer-Encoding: chunked\n\n28\n{\"id\":14,\"jsonrpc\":\"2.0\",\"result\":false}\n0\n\n";
			std::string response = "HTTP/1.1 200 OK\nAccess-Control-Allow-Origin: *\nAccess-Control-Allow-Methods: POST\nAccess-Control-Allow-Credentials: false\nAccess-Control-Max-Age: 86400\nAccess-Control-Allow-Headers: access-control-allow-headers,access-control-allow-origin,content-type\nConnection: keep-alive\nTransfer-Encoding: chunked\n\n4\ntrue\n0\n\n";

        // Echo the buffer back to the sender
            int iSendResult = send( ClientSocket, response.c_str(), iResult, 0 );
            printf("Bytes sent: %d\n", iSendResult);
			closesocket(ClientSocket);
			break;
        }
        else if (iResult == 0)
		{
            printf("Connection closing...\n");
			break;
		}
        else  
		{
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
			break;
        }

    } while (true); */
}

int main()
{
    WSADATA wsaData;
    int iResult;


    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_IP;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }
	
    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

	
		enable_keepalive(ListenSocket);

		bool bOptVal = true;
		int bOptLen = sizeof (BOOL);

		iResult = setsockopt(ListenSocket, SOL_SOCKET, SO_KEEPALIVE, (char *) &bOptVal, bOptLen);
	reaccept:
    // Accept a client socket
		
	while( true )
	{
	    ClientSocket = accept(ListenSocket, NULL, NULL);
		enable_keepalive(ClientSocket);
		
		//printf("got new client...");
		std::thread* clientThread = new std::thread(respond,ClientSocket);
	}

    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    // Receive until the peer shuts down the connection
    do {
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);

			//string response = "HTTP/1.1 200 OK\nAccess-Control-Allow-Origin: *\nAccess-Control-Allow-Methods: POST\nAccess-Control-Allow-Credentials: false\nAccess-Control-Max-Age: 86400\nAccess-Control-Allow-Headers: access-control-allow-headers,access-control-allow-origin,content-type\nConnection: keep-alive\nTransfer-Encoding: chunked\n\n28\n{\"id\":14,\"jsonrpc\":\"2.0\",\"result\":false}\n0\n\n";
			std::string responseT = "HTTP/1.1 200 OK\nAccess-Control-Allow-Origin: *\nAccess-Control-Allow-Methods: POST\nAccess-Control-Allow-Credentials: false\nAccess-Control-Max-Age: 86400\nAccess-Control-Allow-Headers: access-control-allow-headers,access-control-allow-origin,content-type\nConnection: keep-alive\nTransfer-Encoding: chunked\n\n4\ntrue\n0\n\n";
			std::string responseF = "HTTP/1.1 200 OK\nAccess-Control-Allow-Origin: *\nAccess-Control-Allow-Methods: POST\nAccess-Control-Allow-Credentials: false\nAccess-Control-Max-Age: 86400\nAccess-Control-Allow-Headers: access-control-allow-headers,access-control-allow-origin,content-type\nConnection: keep-alive\nTransfer-Encoding: chunked\n\n5\nfalse\n0\n\n";

        // Echo the buffer back to the sender
			if( (rand()%10 + 1) > 5 )
			{
	            iSendResult = send( ClientSocket, responseT.c_str(), iResult, 0 );
			}
			else
			{
	            iSendResult = send( ClientSocket, responseF.c_str(), iResult, 0 );
			}
            printf("Bytes sent: %d\n", iSendResult);
			closesocket(ClientSocket);
			break;
        }
        else if (iResult == 0)
		{
            printf("Connection closing...\n");
			break;
		}
        else  
		{
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
			break;
        }

    } while (true);
	goto reaccept;
	

    // No longer need server socket
    closesocket(ListenSocket);

    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
