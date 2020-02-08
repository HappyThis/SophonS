#pragma once

#include "../BasisFunction/Base.h"

#include<QProcess>


class RemoteCommand :public Base
{
	Q_OBJECT
public:

	RemoteCommand();

	~RemoteCommand();

	virtual void DisconnectEx();

	virtual void ConnectEx();

protected slots:

	virtual void AnalyzeAndProcess(Token, QByteArray);

	virtual void NotActive();

private:

	QProcess* cmdProc;

private slots:
	void NewStdInfo();


};

