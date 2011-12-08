#include "tcanvas.h"

#include <QMouseEvent>
#include <QVector3D>

#include "tmesh.h"

TCanvas::TCanvas(QWidget *parent)
	: QGLWidget(parent)
{
	m_object = new TMesh(this);

	m_cam_eye = QVector3D(0, 1.0, 2.0);
	m_cam_center = QVector3D(0, 0, 0);
	m_cam_up = QVector3D(0, 1, 0);

	m_cammat.lookAt(m_cam_eye, m_cam_center, m_cam_up);
	m_projmat.perspective(45, 1.0, 0.001, 5000);
}

TCanvas::~TCanvas()
{

}

void TCanvas::initializeGL()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
 
	glEnable(GL_BLEND); 

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}

void TCanvas::paintGL()
{
	static GLfloat mat_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
	static GLfloat mat_ambient[] = {0.8f, 0.8f, 0.8f, 0.8f};
	static GLfloat mat_specular[] = {0.8f, 0.8f, 0.8f, 0.8f};
	static GLfloat mat_shininess = 1.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

	static GLfloat light_position[] = {-3.0f, 3.0f, 3.0f, 1.0f};
	static GLfloat light_ambient[] = {1.0f, 0.0f, 1.0f, 1.0f};
	static GLfloat light_diffuse[] = {1.0f, 0.0f, 1.0f, 1.0f};
	static GLfloat light_specular[] = {0.5f, 0.5f, 0.5f, 0.2f};

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	qglClearColor(Qt::gray);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glLoadIdentity();

	glMultMatrixd(m_cammat.constData());
	glMultMatrixd(m_object->matrixData());
	m_object->paint();

	glPopMatrix();
}

void TCanvas::resizeGL(int w, int h)
{
	int side = qMax(w, h);
	glViewport((w - side) / 2, (h - side) / 2, side, side);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	glMultMatrixd(m_projmat.constData());
	glMatrixMode(GL_MODELVIEW);
}

void TCanvas::camMoveView(const QVector3D& tt)
{
	// like Google earth
	QVector3D xv = QVector3D::crossProduct((m_cam_center - m_cam_eye), m_cam_up).normalized();
	QVector3D yv = QVector3D::crossProduct(xv, (m_cam_center - m_cam_eye)).normalized();
	QVector3D xyTrans = xv * tt.x() + yv * tt.y();
	double r = (m_cam_eye - m_cam_center).length() * (1 - 0.1 * tt.z()) / 
		(m_cam_eye + xyTrans - m_cam_center).length();
	m_cam_eye = (m_cam_eye + xyTrans - m_cam_center) * r + m_cam_center;
	m_cam_up = yv.normalized();

	m_cammat.setToIdentity();
	m_cammat.lookAt(m_cam_eye, m_cam_center, m_cam_up);
}

void TCanvas::camMoveCenter(const QVector3D& tt)
{
	QVector3D xv = QVector3D::crossProduct((m_cam_center - m_cam_eye), m_cam_up).normalized();
	QVector3D yv = QVector3D::crossProduct(xv, (m_cam_center - m_cam_eye)).normalized();
	QVector3D zv = (m_cam_center  - m_cam_eye).normalized();
	QVector3D trans = xv * tt.x() + yv * tt.y() + zv * tt.z();
	m_cam_eye += trans;
	m_cam_center += trans;

	m_cammat.setToIdentity();
	m_cammat.lookAt(m_cam_eye, m_cam_center, m_cam_up);
}

void TCanvas::mousePressEvent(QMouseEvent * e)
{
	m_mouseLastPos = e->posF();
	if(e->buttons() & Qt::LeftButton)
		setCursor(Qt::OpenHandCursor);
	else if(e->buttons() & Qt::MidButton)
		setCursor(Qt::SizeAllCursor);
	else
		setCursor(Qt::ArrowCursor);
}

void TCanvas::mouseMoveEvent(QMouseEvent * e)
{
	QVector3D t(e->posF() - m_mouseLastPos);
	double side = qMax(width(), height());
	t.setX( - t.x());
	t /= side / 3.0;
	if(e->buttons() & Qt::LeftButton){
		camMoveView(t);
		setCursor(Qt::ClosedHandCursor);
		update();
	}else if(e->buttons() & Qt::MidButton){
		camMoveCenter(t);
		update();
	}
	m_mouseLastPos = e->posF();
}

void TCanvas::mouseReleaseEvent(QMouseEvent * e)
{
	setCursor(Qt::ArrowCursor);
}

void TCanvas::wheelEvent(QWheelEvent * e)
{
	camZoom(e->delta() / 800.0);
	update();
}