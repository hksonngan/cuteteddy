#include "tmainwind.h"

#include <QFileDialog>
#include <QMessageBox>

#include "tcanvas.h"

TMainWind::TMainWind(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	setCentralWidget(m_canvas = new TCanvas);
}

TMainWind::~TMainWind()
{

}

void TMainWind::on_actionNew_triggered()
{
	
}

void TMainWind::on_actionOpen_triggered()
{
	QString filePath = QFileDialog::getOpenFileName(this, tr("Select file..."), QString(), QString());
}

void TMainWind::on_actionSave_triggered()
{

}

void TMainWind::on_actionUndo_triggered()
{

}

void TMainWind::on_actionRedo_triggered()
{

}

void TMainWind::on_actionInfo_triggered()
{
	QMessageBox::about(this, tr("Application Information"), 
		tr("A Sketching Interface for 3D Freeform Design \n"
		"based on a paper by Takeo Igarashi, etc. (1999).\n\n"
		"Developer: YANG Hao <yangh2007@gmail.com>"));
}