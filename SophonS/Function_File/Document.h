#pragma once

#include "../BasisFunction/Base.h"

#include <QDir>

#include"../Function_File/ThreadOfConnect.h"

#include"../Function_File/ThreadOfListen.h"

class Document :
	public Base
{
	Q_OBJECT
public:
	Document();

	~Document();

	virtual void DisconnectEx();

	virtual void ConnectEx();


protected slots:
	 
	void AnalyzeAndProcess(Token, QByteArray);

	virtual void NotActive();

private:

	void StartListen(UINT Port, QByteArray* FileData);

	void StartConnect(
		UINT Port,
		const char *Ip,
		QString *FilePath,
		QByteArray* FileData
	);

	QString filePath;

	UINT port;

	QByteArray fileData;

	char fathersIp[4 * 3 + 3];

	ThreadOfConnect* threadOfConnect;

	ThreadOfListen* threadOfListen;

	QThread *instanceOfListen;

	QThread *instanceOfConnect;

signals:
	void Listen(UINT, QByteArray*);

	void ConnectS(
		UINT,
		const char *,
		QString *,
		QByteArray*
	);
};

