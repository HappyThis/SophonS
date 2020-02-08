#include "Document.h"



Document::Document() :Base() {


	instanceOfConnect = new QThread();

	instanceOfListen = new QThread();

	threadOfConnect = new ThreadOfConnect();

	threadOfListen = new ThreadOfListen();

	threadOfConnect->moveToThread(instanceOfConnect);

	threadOfListen->moveToThread(instanceOfListen);

	QObject::connect(threadOfConnect, SIGNAL(Send(QByteArray, Token)), this, SLOT(SendS(QByteArray, Token)));
	QObject::connect(this, SIGNAL(Listen(UINT, QByteArray*)), threadOfListen, SLOT(StartToListen(UINT, QByteArray* )));
	QObject::connect(this,
		SIGNAL(ConnectS(
			const UINT,
			const char *,
			QString *,
			QByteArray*)
		),
		threadOfConnect,
		SLOT(StartToConnect(
			UINT,
			const char *,
			QString *,
			QByteArray *
		)));
}


Document::~Document() {

	if(IsActive())
	DisconnectEx();
}

void Document::DisconnectEx() {


	threadOfConnect->CloseConnect();

	threadOfListen->CloseListen();

	instanceOfConnect->exit();

	instanceOfListen->exit();

	instanceOfConnect->wait();

	instanceOfConnect->wait();

	delete instanceOfConnect;

	delete instanceOfListen;

	delete threadOfListen;

	delete threadOfConnect;
}

void Document::ConnectEx() {


	int nRet = Connect();

	if (nRet == Exist || nRet == Success) {

		instanceOfConnect->start();

		instanceOfListen->start();

		Send(QByteArray().append((const char*)&mainNumber, sizeof(UINT)), CommandOpenDoucmentFuntion);

	}
}

void Document::NotActive() {

	Disconnect();
}

void Document::StartListen(UINT  Port, QByteArray * FileData) {

	emit Listen(Port, FileData);
}

void Document::StartConnect(UINT  Port, const char * Ip, QString * FilePath, QByteArray* FileData) {

	emit ConnectS(Port, Ip, FilePath, FileData);
}

void Document::AnalyzeAndProcess(Token token, QByteArray data) {

	switch (token) {

	case CommandNewFile: {
		

		fileData.clear();

		fileData.append(data);

		QFile file(filePath);

		file.open(QIODevice::WriteOnly);

		file.write(fileData);

		file.close();

		Send(QByteArray(), TokenFinish);

		break;
	}
	case CommandNewFilePath: {


		filePath.clear();

		filePath.append(data);

		break;
	}
	case CommandFathersIp: {

		CopyMemory(fathersIp, data.data(), sizeof(fathersIp));

		CopyMemory(&port, data.data() + sizeof(fathersIp), sizeof(UINT));

		qDebug() << fathersIp << port;

		StartConnect(port, fathersIp,& filePath, &fileData);

		break;
	}
	case CommandReadyToListen: {


		StartListen(port, &fileData);

		break;
	}

	case CommandReadyToConnect: {

		StartConnect(port, fathersIp, &filePath,&fileData);

		break;
	}
	case CommandCloseDoucmentFuntion:{

		DisconnectEx();

		Disconnect();

		qDebug() << "END";

		break;
	}


	}

}