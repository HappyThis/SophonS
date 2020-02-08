#pragma once

#include "Base.h"

#include"../Function_Analysis/NetWork.h"

#include"../Function_File/Document.h"

#include"../Function_Cmd/RemoteCommand.h"


#include <QSysInfo>
#include "QSettings"
#include "QDebug"
#include <QDesktopWidget>
#include <QFileInfoList>
#include <QDir>
#include <QLibrary>
#include <QTimer>
#include<QApplication>

#include <QTimer>

#include<QSettings>

#define GB 1073741824

#define MaxFunction 10

#define FNetWork 0

#define FRemoteCmd 1

#define FDocument 2

class MainManager :
	public Base
{
	Q_OBJECT
public:

	MainManager();

	~MainManager();

	virtual void DisconnectEx();

protected slots:

	virtual void AnalyzeAndProcess(Token, QByteArray);

	virtual void NotActive();
	
	virtual void ConnectEx();

	void Timer();

    void HeartBeat();

	void SendOnlineInfo(QString info);

private:

	char localIp[4 * 3 + 3];

	Base* function[MaxFunction];

	QSettings * set;

	QTimer *connectTimer;

signals:

	void CreateTimer();

	void GetOnlineInfo();

	
};

