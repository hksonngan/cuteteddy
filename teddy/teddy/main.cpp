#include "tmainwind.h"
#include <QtGui/QApplication>
#include <QSettings>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	TMainWind w;
	w.show();
	return a.exec();
}
