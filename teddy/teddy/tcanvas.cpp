#include "tcanvas.h"

#include "tentity.h"
#include "tmesh.h"

TCanvas::TCanvas(QWidget *parent)
	: QGLWidget(parent)
{
	m_ent = new TMesh(this);
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

	// 启动混合并设置混合因子 
	glEnable(GL_BLEND); 

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}

void TCanvas::paintGL()
{
	//材质
	static GLfloat mat_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};//漫反射光颜色
	static GLfloat mat_ambient[] = {0.8f, 0.8f, 0.8f, 0.8f};//环境光颜色	
	static GLfloat mat_specular[] = {0.8f, 0.8f, 0.8f, 0.8f};//镜面反射光颜色
	static GLfloat mat_shininess = 1.0f;//镜面指数

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

	// 开始绘制
	qglClearColor(Qt::gray);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glLoadIdentity();
	m_ent->paintAll();

	// 光照
	static GLfloat light_position[] = {-1.0f, 1.0f, 1.0f, 0.0f};
	static GLfloat light_ambient[] = {1.0f, 1.0f, 1.0f, 1.0f};
	static GLfloat light_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
	static GLfloat light_specular[] = {0.5f, 0.5f, 0.5f, 0.2f};

	glPushAttrib(GL_LIGHTING_BIT);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, light_diffuse);
	//drawLight(light_position);
	glPopAttrib();

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	
	// draw entity
	glMultMatrixd(m_cam->matrixData());
	glMultMatrixd(m_ent->matrixData());



	glPopMatrix();
}

void TCanvas::resizeGL(int w, int h)
{
	int side = qMax(w, h);
	glViewport((w - side) / 2, (h - side) / 2, side, side);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//scene->cameraProject();
	glMatrixMode(GL_MODELVIEW);
}

void TCanvas::mousePressEvent(QMouseEvent * e)
{

}

void TCanvas::mouseMoveEvent(QMouseEvent * e)
{

}

void TCanvas::mouseReleaseEvent(QMouseEvent * e)
{

}

void TCanvas::wheelEvent(QWheelEvent * e)
{

}