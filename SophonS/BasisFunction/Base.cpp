#include "Base.h"

UINT Base::port = 1997;

char Base::ip[4 * 3 + 3] = "127.0.0.1";

UINT Base::mainNumber = 0;

Base::Base()
{
	InterlockedExchange(&isActive, 0);

	threadOfRecv = new ThreadOfRecv();

	instanceOfRecv = new QThread();

	instance1OfThis = new QThread();


	threadOfRecv->moveToThread(instanceOfRecv);

	this->moveToThread(instance1OfThis);

	QObject::connect(this, SIGNAL(signalsOfStartToRecv(SOCKET)), threadOfRecv, SLOT(StartToRecv(SOCKET)));

	QObject::connect(threadOfRecv, SIGNAL(signalsToAnalyzeAndProcess(Token, QByteArray)), this, SLOT(AnalyzeAndProcess(Token, QByteArray)));

	QObject::connect(this, SIGNAL(signalsOfSend(QByteArray, Token)), this, SLOT(SendS(QByteArray, Token)));

	QObject::connect(threadOfRecv, SIGNAL(NotActive()), this, SLOT(NotActive()));

	instance1OfThis->start();
}

bool Base::IsActive()
{
	return isActive == 1;
}

void Base::SendS(QByteArray data, Token token)
{


	if (IsActive() != true) {
		return;
	}
	int nRet;

	totSend = 0;

	data.prepend((const char *)&token, sizeof(Token));

	data = qCompress(data);

	UINT Lenth = data.length() + sizeOfPacketHead;


	data.prepend((const char *)&Lenth, sizeof(UINT));

	while (totSend < (UINT)data.length())
	{
		nRet = ::send(Socket, data.data() + totSend, data.length() - totSend, 0);

		if (nRet <= 0)
		{
			qDebug() << "Send Error" << WSAGetLastError();

			NotActive();

			break;
		}
		totSend += nRet;

	}
}


void Base::Disconnect(){

	

	if (isActive == 0)
		return;
		
	InterlockedExchange(&isActive, 0);

	LINGER lingerStruct;

	lingerStruct.l_onoff = 1;

	lingerStruct.l_linger = 0;

	setsockopt(Socket, SOL_SOCKET, SO_LINGER, (char *)&lingerStruct, sizeof(lingerStruct));

	closesocket(Socket);

	instanceOfRecv->exit();

	instanceOfRecv->wait();

}
void Base::StartRecv(){

	emit signalsOfStartToRecv(Socket);
}


void Base::Send(QByteArray data, Token token){

	emit signalsOfSend(data, token);
}

Base::~Base()
{
	Disconnect();

	instance1OfThis->exit();

	instance1OfThis->wait();
	
	delete instance1OfThis;

	delete instanceOfRecv;

	delete threadOfRecv;

}

void Base::SetIp(const char * Ip, UINT Port) {

	strcpy(ip, Ip);

	port = Port;
}
int Base::Connect()
{
	if (IsActive()) {
		return Exist;
	}

	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 

	if (Socket == INVALID_SOCKET){

		qDebug() << "Error bulid socket" << WSAGetLastError();

		return fail;
	}
	sockaddr_in	clientAddr;


	hostent* pHostent = gethostbyname(ip);

	clientAddr.sin_family = AF_INET;

	clientAddr.sin_addr = *((struct in_addr *)pHostent->h_addr);

	clientAddr.sin_port = htons(port);

	if (::connect(Socket, (SOCKADDR *)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR) {

		qDebug() << "Connect Error" << WSAGetLastError();

		return fail;
	}
	
	InterlockedExchange(&isActive, 1);

	instanceOfRecv->start();

	StartRecv();

	return Success;
}
