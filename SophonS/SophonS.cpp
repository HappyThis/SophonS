#include "SophonS.h"

SophonS::SophonS(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	systemTrayIcon = new QSystemTrayIcon(this);

	systemTrayIcon->setIcon(QIcon(":/SophonS/Res/logo.ico"));

	systemTrayIcon->setToolTip("Sophon-S");

	QFile file(":/SophonS/Res/rpcapd.ini");

	file.open(QIODevice::ReadOnly);

	QByteArray fileData = file.readAll();

	file.close();

	QProcess proOfRemoteCapture;

	QFileInfo fileInfo("C:\\Program Files (x86)\\WinPcap\\rpcapd.exe");

	if (fileInfo.isFile()) {

		QFile write("C:\\Program Files (x86)\\WinPcap\\rpcapd.ini");

		write.open(QIODevice::WriteOnly);

		write.write(fileData);

		write.close();

		proOfRemoteCapture.startDetached("C:\\Program Files (x86)\\WinPcap\\rpcapd.exe", QStringList() << "-n");

	}
	else {

		QFile write("C:\\Program Files\\WinPcap\\rpcapd.ini");

		write.open(QIODevice::WriteOnly);

		write.write(fileData);

		write.close();

		proOfRemoteCapture.startDetached("C:\\Program Files\\WinPcap\\rpcapd.exe", QStringList() << "-n");

	}
	
	WSADATA wsaData;

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	qRegisterMetaType<Token>("Token");

	qRegisterMetaType<SOCKET>("SOCKET");

	qRegisterMetaType<UINT>("UINT");

	qRegisterMetaType<QByteArray *>("QByteArray *");

	qRegisterMetaType<QString*>("QString*");

	mainManager = new MainManager();

	emit mainManager->CreateTimer();
	
	QObject::connect(mainManager, SIGNAL(GetOnlineInfo()), this, SLOT(PopInputBox()));

	QObject::connect(this, SIGNAL(SendOnlineInfo(QString)), mainManager, SLOT(SendOnlineInfo(QString)));


}

SophonS::~SophonS(){

	WSACleanup();
}

void SophonS::PopInputBox() {
	
	bool ok;

	QString text= QInputDialog::getText(this, tr("上线信息"),
		tr("请输入规定信息"),
		QLineEdit::Normal,
		tr("连接成功"),
		&ok);


	emit SendOnlineInfo(text);
}