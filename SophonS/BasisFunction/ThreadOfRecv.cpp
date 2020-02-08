

#include "ThreadOfRecv.h"

#include<QThread>

ThreadOfRecv::ThreadOfRecv()
{

	totRecv = 0;

	recvTest = false;

}

void ThreadOfRecv::StartToRecv(SOCKET Socket)
{
	int nRet;

	qDebug() << "OK! I'M Recving............!";

	while (true)
	{
		nRet = recv(Socket, (char*)tempBufferOfRecv, MaxBufferLenth, 0);

		if (nRet <= 0)
		{
			qDebug() << "Recv Error!"<<WSAGetLastError();

			emit NotActive();

			break;
		}

		recvBuffer.append((const char*)tempBufferOfRecv, nRet);

		while (recvTest || recvBuffer.getBufferSize() >= sizeOfPacketHead)
		{
			if (!recvTest){

				recvBuffer.read(&bufferOfComplete, sizeOfPacketHead);

				CopyMemory(&totSizeOfRecvedData, bufferOfComplete.data(), sizeOfPacketHead);

				totSizeOfRecvedData -= sizeOfPacketHead;

				recvTest = true;

			}

			if (recvBuffer.getBufferSize() >= totSizeOfRecvedData){

				recvTest = false;

				recvBuffer.read(&bufferOfComplete, totSizeOfRecvedData);

				bufferOfComplete = qUncompress(bufferOfComplete);

				CopyMemory(&token, bufferOfComplete.data(), sizeof(Token));

				bufferOfComplete.remove(0, sizeof(Token));

				emit signalsToAnalyzeAndProcess(token, bufferOfComplete);

			}
			else {
				break;
			}

		}
	}


}

