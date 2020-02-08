#pragma once

#include <QtWidgets/QMainWindow>

#include "ui_SophonS.h"

#include"../BasisFunction/MainManager.h"

#include<QProcess>

#include<QFileInfo>

#include<QSystemTrayIcon>

#include <QInputDialog>


#pragma execution_character_set("utf-8")

class SophonS : public QMainWindow
{
	Q_OBJECT

public:

	SophonS(QWidget *parent = Q_NULLPTR);

	~SophonS();

private:

	Ui::SophonSClass ui;

	MainManager* mainManager;

	QTimer *connectTimer;

	QSystemTrayIcon* systemTrayIcon;

signals:
	void SendOnlineInfo(QString);

private slots:

	void PopInputBox();

};
