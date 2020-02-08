#pragma once

#include<QObject.h>

#include"../Function_Analysis/IpContainerUnit.h"

#include<Qdebug>

#include"../Function_Analysis/Header.h"

#include<QHash>

#include"../BasisFunction/Token.h"

#include<Qvector>

#include<QPair>

#include<QMutex>

#define RUNNING 1

#define NOTRUNNING 0


class ThreadOfNetWork :public QObject
{

	Q_OBJECT

public:

	ThreadOfNetWork();

	~ThreadOfNetWork();

	void Stop();

	void CmdGetIp();

	void CmdGetIo();

	void CmdGetProtocol();

signals:

	void Send(QByteArray, Token);

public slots:

	void StartCapture(char *ip);

private:

	UINT isActive;

	QMutex Mutex;

	char ipV4[4 * 3 + 3];

	char ipV6[4 * 8 + 7];

	IpContainerUnit* ICU;

	void ip4tos(u_long in, char* addressV4);

	void ip6tos(struct sockaddr *sockaddr, char *addressV6, int addrlen);

	/*IP occupation ratio*/
	unsigned int Protocol[MaxProtocol];

	/*Protocol statistical information*/

	QHash<QString,IpContainerUnit*>ipContainer;

	/*IO statistical information*/

	QList<QPair<unsigned int, unsigned int>>ioInfoContainer;

	void AnalyzeEth(const u_char* pkt);

	void AnalyzeArp(const u_char* pkt);

	void AnalyzeIpV6(const u_char* pkt);

	void AnalyzeIpV4(const u_char* pkt);

	void AnalyzeTcp(const u_char* pkt);

	void AnalyzeUdp(const u_char* pkt);

	void TryToInsert(int type);

	void Lock();

	void UnLock();


};

