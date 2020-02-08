#include "ThreadOfConnect.h"



ThreadOfConnect::ThreadOfConnect()
{
}


ThreadOfConnect::~ThreadOfConnect()
{
}

void ThreadOfConnect::CloseConnect(){

	LINGER lingerStruct;

	lingerStruct.l_onoff = 1;

	lingerStruct.l_linger = 0;

	setsockopt(Socket, SOL_SOCKET, SO_LINGER, (char *)&lingerStruct, sizeof(lingerStruct));

	closesocket(Socket);

}

void ThreadOfConnect::StartToConnect(
	UINT Port, 
	const char *ip,
    QString *filePath,
	QByteArray *fileData
) {


	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 

	if (Socket == INVALID_SOCKET) {

		qDebug() << "Error bulid socket" << WSAGetLastError();

		return;
	}
	sockaddr_in	clientAddr;

	hostent* pHostent = gethostbyname(ip);

	clientAddr.sin_family = AF_INET;

	clientAddr.sin_addr = *((struct in_addr *)pHostent->h_addr);

	clientAddr.sin_port = htons(Port);

	if (::connect(Socket, (SOCKADDR *)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR) {

		qDebug() << "Connect Error" << WSAGetLastError();

		return;
	}

	BYTE tempBufferOfRecv[2 << 15];

	QRingBuffer recvBuffer;

	UINT totSend = 0;

	bool ok = false;

	while (true){

		int nRet = recv(Socket, (char*)tempBufferOfRecv, 2 << 15, 0);

		if (nRet <= 0)
		{
			qDebug() << "Recv Error!" << WSAGetLastError();

			break;
		}

		recvBuffer.append((const char*)tempBufferOfRecv, nRet);



		if (!ok || recvBuffer.getBufferSize() > sizeof(UINT)) {

			recvBuffer.read(fileData, sizeof(UINT));

			CopyMemory(&totSend, fileData->data(), sizeof(UINT));

			totSend -= sizeof(UINT);

			ok = true;

		}

		if (recvBuffer.getBufferSize() >= totSend) {

			ok = false;

			recvBuffer.read(fileData, totSend);

			QFile write(*filePath);

			write.open(QIODevice::WriteOnly);

			write.write(qUncompress(*fileData));

			emit Send(QByteArray(), TokenFinish);

			break;
		}
	}

	CloseConnect();


}
