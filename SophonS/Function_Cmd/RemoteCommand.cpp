#include "RemoteCommand.h"



RemoteCommand::RemoteCommand() :Base() {

	cmdProc = new QProcess();

	QObject::connect(cmdProc, SIGNAL(readyRead()), this, SLOT(NewStdInfo()));
}


RemoteCommand::~RemoteCommand() {
	DisconnectEx();
}

void RemoteCommand::DisconnectEx() {

	cmdProc->close();

	delete cmdProc;
}

void RemoteCommand::ConnectEx() {

	int nRet = Connect();

	if (nRet == Exist || nRet == Success) {

		cmdProc->start("cmd");

		cmdProc->waitForStarted();

		Send(QByteArray().append((const char*)&mainNumber, sizeof(UINT)), CommandOpenCmd);

	}
}

void RemoteCommand::NotActive() {

	Disconnect();

}

void RemoteCommand::AnalyzeAndProcess(Token token, QByteArray data) {

	switch (token)
	{
	case CommandNewStdInput: {

		cmdProc->write(data);

		break;
	}
	case CommandCloseCmd: {
		Disconnect();
		break;
	}

	default:
		break;
	}
}

void RemoteCommand::NewStdInfo() {

	qDebug() << "GET";
	Send(
		cmdProc->readAllStandardOutput(),
		TokenNewStdOutput
	);
	
}
