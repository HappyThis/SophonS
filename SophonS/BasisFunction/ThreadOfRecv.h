

#ifndef THREADOFRECV_H
#define THREADOFRECV_H

#include"RingBuffer.h"


#define MaxBufferLenth 65536

#define sizeOfPacketHead sizeof(UINT)

#include <QObject>

#include "Token.h"



class ThreadOfRecv :public QObject
{

	Q_OBJECT

public:

	ThreadOfRecv();

private:



	QRingBuffer recvBuffer;


	PBYTE tempBufferOfRecv[MaxBufferLenth];


	QByteArray bufferOfComplete;



	UINT totRecv;


	bool recvTest;



	UINT totSizeOfRecvedData;


	Token token;

signals:




	void  signalsToAnalyzeAndProcess(Token, QByteArray);



	void NotActive();



public slots:

	void StartToRecv(SOCKET Socket);

};

#endif // THREADOFRECV_H
