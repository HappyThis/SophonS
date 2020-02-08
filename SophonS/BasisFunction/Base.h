#ifndef BASE_H
#define BASE_H

#include <QDir>


#include <QThread>

#include "ThreadOfRecv.h"

#include <QTimer.h>

#pragma comment(lib,"WS2_32.lib")

#pragma execution_character_set("utf-8")

#define Exist 1
#define Success 0
#define fail -1

class Base :public QObject
{
	Q_OBJECT

public:

	Base();

	void Send(QByteArray, Token);

	virtual ~Base();

	static void SetIp(const char* Ip,UINT Port);

	bool IsActive();

	virtual void DisconnectEx() = 0;

	virtual void ConnectEx() = 0;

protected:

	ThreadOfRecv * threadOfRecv;

	QThread* instanceOfRecv;

	QThread* instance1OfThis;

	UINT isActive;

	static UINT mainNumber;

	void Disconnect();

	int Connect();

private:

	void StartRecv();

	SOCKET Socket;

	UINT totSend;

	UINT totSizeOfRecvedData;

	static char ip[4 * 3 + 3];

	static UINT port;

protected slots:

	virtual void AnalyzeAndProcess(Token, QByteArray) = 0;

	virtual void NotActive()=0;

private slots:

	void SendS(QByteArray, Token);

signals:

	void signalsOfSend(QByteArray, Token);

	void signalsOfStartToRecv(SOCKET);

};
#endif // BASE_H
