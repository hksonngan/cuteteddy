#include "tcanvas.h"

#include <QMouseEvent>
#include <QVector3D>
#include <QVector2D>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

TCanvas::TCanvas(QWidget *parent)
	: QGLWidget(parent),
	m_fileInfo(tr(UNTITLED)), 
	m_penCursor(QPixmap(":/TMainWind/Resources/other/black/pencil_icon&16.png"), 0, 16)
{
	m_scene = new TScene(this);

	setAutoFillBackground(false);
	setCursor(m_penCursor);

	m_stepLength = m_stepLengthRemained = 1.0;
}

TCanvas::~TCanvas()
{

}

void TCanvas::initializeGL()
{
	glEnable(GL_MULTISAMPLE);
}

void TCanvas::resizeGL(int w, int h)
{
	m_scene->setupViewport(w, h);
}




void TCanvas::paintEvent(QPaintEvent* e)
{
	makeCurrent();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	qglClearColor(Qt::gray);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_POLYGON_SMOOTH);
	static GLfloat lightPosition[4] = { 6.5, 10.0, 14.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	m_scene->setupViewport(width(), height());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	m_scene->paint();

	glShadeModel(GL_FLAT);
	//glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_POLYGON_SMOOTH);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();


	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(QPen(Qt::yellow, 2.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	painter.drawPolyline(m_sketch);
	painter.end();
}

void TCanvas::mousePressEvent(QMouseEvent * e)
{
	m_mouseLastPos = e->posF();
	if(e->buttons() & Qt::RightButton)
		setCursor(Qt::OpenHandCursor);
	else if(e->buttons() & Qt::MidButton)
		setCursor(Qt::SizeAllCursor);
	else{// left button
		m_sketch.clear();
		m_sketch.append(m_mouseLastPos);
		setCursor(m_penCursor);
	}

	m_scene->mapToZPlane(e->posF());
}

void TCanvas::mouseMoveEvent(QMouseEvent * e)
{
	QVector3D t(e->posF() - m_mouseLastPos);
	double side = qMax(width(), height());
	t.setX( - t.x());
	t /= side / 20.0;
	if(e->buttons() & Qt::RightButton){
		m_scene->rotate( - t.x() * 5, QVector3D(0, 1, 0));
		m_scene->rotate(t.y() * 5, QVector3D(1, 0, 0));
		setCursor(Qt::ClosedHandCursor);
		update();
	}else if(e->buttons() & Qt::MidButton){
		m_scene->translate(- t);
		update();
	}else{
		if(m_sketch.empty()){
			m_sketch.append(e->posF());
			m_stepLengthRemained = 0;
		}else{
			QVector2D newV(e->posF() - m_sketch.last());
			double newDist = newV.length();
			m_stepLengthRemained += newDist;
			while(m_stepLengthRemained > m_stepLength){
				m_stepLengthRemained -= m_stepLength;
				m_sketch.append(e->posF() - newV.normalized().toPointF() * m_stepLengthRemained);
			}		
		}		
		update();
	}
	m_mouseLastPos = e->posF();
}

void TCanvas::mouseReleaseEvent(QMouseEvent * e)
{
	setCursor(m_penCursor);
	if(m_mode == Creation){
		if(m_scene->build(m_sketch)){
			emit creationFinished();
			m_sketch.clear();
		}
	}else if(m_mode == Painting){

	}else if(m_mode == Extrusion){

	}else if(m_mode == Bending){

	}
	update();
}

void TCanvas::wheelEvent(QWheelEvent * e)
{
	m_scene->camZoom(e->delta() / 800.0);
	update();
}

void TCanvas::neww()
{
	m_scene->deleteLater();
	m_scene = new TScene(this);
	update();
}

void TCanvas::open( const QString& filePath /*= tr("untitled.off")*/ )
{
	if(m_scene->open(filePath)){
		m_fileInfo = QFileInfo(filePath);
		update();
	}
}

void TCanvas::save( const QString& filePath /*= QString()*/ )
{
	if(filePath == QString()){
		m_scene->save(m_fileInfo.absolutePath());
	}else if(m_scene->save(filePath)){
		m_fileInfo = QFileInfo(filePath);
		update();
	}		
}

void TCanvas::setMode( TMode m )
{
	m_mode = m;
}
