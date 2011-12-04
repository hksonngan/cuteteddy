#include "tmainwind.h"
#include <QtGui/QApplication>

#include "stylesheet.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setStyleSheet(_ss_);
	TMainWind w;
	w.show();
	return a.exec();
}
