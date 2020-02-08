#pragma once
#include "../BasisFunction/Base.h"

#include"../Function_Analysis/ThreadOfNetwork.h"
class NetWork :
	public Base
{
	Q_OBJECT

public:

	NetWork();

	~NetWork();

	virtual void DisconnectEx();

	virtual void ConnectEx();

	void StartCap(char* ip);

protected slots:

	virtual void AnalyzeAndProcess(Token, QByteArray);

	virtual void NotActive();

private:

	ThreadOfNetWork *threadOfNetWork;

	QThread * instanceOfNetWork;

signals:

	void StartCapture(char*);


};

