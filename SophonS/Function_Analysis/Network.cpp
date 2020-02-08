#include "NetWork.h"


NetWork::NetWork() :Base() {

	instanceOfNetWork = new QThread();

	instanceOfNetWork->start();

	threadOfNetWork = new ThreadOfNetWork();

	threadOfNetWork->moveToThread(instanceOfNetWork);

	QObject::connect(threadOfNetWork, SIGNAL(Send(QByteArray, Token)), this, SLOT(SendS(QByteArray, Token)));

	QObject::connect(this, SIGNAL(StartCapture(char*)), threadOfNetWork, SLOT(StartCapture(char*)));


}


NetWork::~NetWork() {

	DisconnectEx();

	threadOfNetWork->Stop();

	instanceOfNetWork->exit();

	instanceOfNetWork->wait();

	delete threadOfNetWork;

	delete instanceOfNetWork;
}

void NetWork::DisconnectEx() {

	Disconnect();

}

void NetWork::ConnectEx() {

	int nRet = Connect();

	if (nRet == Exist || nRet == Success) {

		Send(QByteArray().append((const char*)&mainNumber, sizeof(UINT)), CommandOpenNetworkServer);

	}
}

void NetWork::StartCap(char * ip)
{
	emit StartCapture(ip);
}

void NetWork::NotActive() {

	Disconnect();

}

void NetWork::AnalyzeAndProcess(Token token, QByteArray data) {

	switch (token)
	{
	case CommandGetIp: {

		threadOfNetWork->CmdGetIp();

		break;
	}

	case CommandGetIo: {

		threadOfNetWork->CmdGetIo();

		break;
	}

	case CommandGetProtocol: {

		threadOfNetWork->CmdGetProtocol();

		break;
	}
	case CommandCloseNetworkServer: {

		Disconnect();

		break;

	}

	default:
		break;
	}

}
