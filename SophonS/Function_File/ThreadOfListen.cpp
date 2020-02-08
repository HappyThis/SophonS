#include "ThreadOfListen.h"



ThreadOfListen::ThreadOfListen()
{
}


ThreadOfListen::~ThreadOfListen()
{
}

void ThreadOfListen::CloseListen(){

	LINGER lingerStruct;

	lingerStruct.l_onoff = 1;

	lingerStruct.l_linger = 0;

	setsockopt(listenSocket, SOL_SOCKET, SO_LINGER, (char *)&lingerStruct, sizeof(lingerStruct));

	setsockopt(Client, SOL_SOCKET, SO_LINGER, (char *)&lingerStruct, sizeof(lingerStruct));

	closesocket(listenSocket);

	closesocket(Client);

	listenSocket = Client = INVALID_SOCKET;

}

void ThreadOfListen::StartToListen(UINT Port, QByteArray* FileData) {


     listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 

	if (listenSocket == INVALID_SOCKET) {

		qDebug() << "Error bulid socket" << WSAGetLastError();

		return;
	}


	SOCKADDR_IN serAddr;

	serAddr.sin_family = AF_INET;

	serAddr.sin_port = htons(Port);

	serAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	int retVal = bind(listenSocket, (LPSOCKADDR)&serAddr, sizeof(SOCKADDR_IN));

	if (SOCKET_ERROR == retVal) {

		qDebug() << "bind failed!" << WSAGetLastError();

		return;
	}

	retVal = listen(listenSocket, SOMAXCONN);

	if (SOCKET_ERROR == retVal) {

		qDebug() << "listen failed!" << WSAGetLastError();

		return;
	}

	qDebug() << "Listen......";

	sockaddr_in addrClient;

	int addrClientlen = sizeof(addrClient);

     Client = accept(listenSocket, (sockaddr FAR*)&addrClient, &addrClientlen);

	if (INVALID_SOCKET == Client) {

		qDebug() << "accept failed!" << WSAGetLastError();

		return;
	}


	qDebug() << "Accept Successful!";

	UINT totSend = 0;

	while (totSend < (UINT)FileData->length()){

		int nRet = ::send(Client, FileData->data() + totSend, FileData->length() - totSend, 0);

		if (nRet <= 0)
		{
			qDebug() << "Send Error" << WSAGetLastError();

			break;
		}
		totSend += nRet;
	}

	CloseListen();

}
