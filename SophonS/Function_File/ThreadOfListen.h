#pragma once

#include<QObject>

#include"../BasisFunction/RingBuffer.h"

class ThreadOfListen:public QObject
{
	Q_OBJECT

public:
	ThreadOfListen();

	~ThreadOfListen();

	void CloseListen();

private:

	SOCKET listenSocket;

	SOCKET  Client;

private slots:

	void StartToListen(UINT Port, QByteArray* FileData);

};

