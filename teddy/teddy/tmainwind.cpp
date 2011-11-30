#include "tmainwind.h"

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

}