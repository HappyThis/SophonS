#include "SophonS.h"

#include <QtWidgets/QApplication>

#include "./BasisFunction/MainManager.h"

#include"./Function_File/Document.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	a.setQuitOnLastWindowClosed(false);

	SophonS w;

	return a.exec();

}
