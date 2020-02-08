#pragma once

#include<QObject>

#include"../BasisFunction/RingBuffer.h"

#include<QFile>

#include"../BasisFunction/Token.h"

class ThreadOfConnect:public QObject
{
	Q_OBJECT
public:
	ThreadOfConnect();

	~ThreadOfConnect();	

	void CloseConnect();

private:

	SOCKET Socket;

private slots:

	void StartToConnect(
		 UINT,
		const char *,
		 QString *,
		QByteArray *
	);

signals:

	void Send(QByteArray data, Token token);
};

