#include "tmainwind.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QStateMachine>
#include <QLabel>

#include "tcanvas.h"

TMainWind::TMainWind(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	init();
}

TMainWind::~TMainWind()
{

}

void TMainWind::init()
{
	setCentralWidget(m_canvas = new TCanvas);

	QLabel* statusLabel = new QLabel;
	statusBar()->addPermanentWidget(statusLabel);

	QStateMachine* machine = new QStateMachine(this);
	QState* creation = new QState;
	QState* bending = new QState;
	QState* painting = new QState;
	QState* extrusion = new QState;

	creation->assignProperty(m_canvas, "mode", TCanvas::Creation);
	creation->assignProperty(statusLabel, "text", tr("Mode: Creation"));
	creation->addTransition(m_canvas, SIGNAL(creationFinished()), painting);
	
	bending->assignProperty(m_canvas, "mode", TCanvas::Bending);
	bending->assignProperty(statusLabel, "text", tr("Mode: Bending"));
	bending->addTransition(m_canvas, SIGNAL(bendingFinished()), painting);

	painting->assignProperty(m_canvas, "mode", TCanvas::Painting);
	painting->assignProperty(statusLabel, "text", tr("Mode: Painting"));
	painting->addTransition(m_canvas, SIGNAL(toEdit()), extrusion);

	extrusion->assignProperty(m_canvas, "mode", TCanvas::Extrusion);
	extrusion->assignProperty(statusLabel, "text", tr("Mode: Extrusion"));
	extrusion->addTransition(m_canvas, SIGNAL(extrusionFinished()), painting);

	bending->addTransition(m_canvas, SIGNAL(restart()), creation);
	painting->addTransition(m_canvas, SIGNAL(restart()), creation);
	extrusion->addTransition(m_canvas, SIGNAL(restart()), creation);
	

	machine->addState(creation);
	machine->addState(bending);
	machine->addState(painting);
	machine->addState(extrusion);
	machine->setInitialState(creation);
	machine->start();
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
		"based on Takeo Igarashi etc. 1999.\n\n"
		"by YANG Hao <yangh2007@gmail.com>"));
}