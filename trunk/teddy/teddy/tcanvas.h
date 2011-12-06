#ifndef TCANVAS_H
#define TCANVAS_H

#include <QGLWidget>
#include <QMatrix4x4>

#include "tcamera.h"
#include "tentity.h"
#include "tmesh.h"

class TCanvas : public QGLWidget
{
public:
	TCanvas(QWidget *parent = 0);
	~TCanvas();

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int w, int h);

	void mousePressEvent(QMouseEvent * e);
	void mouseMoveEvent(QMouseEvent * e);
	void mouseReleaseEvent(QMouseEvent * e);
	void wheelEvent(QWheelEvent * e);

private:
	TEntity* m_ent;
	TCamera* m_cam;
	QPointF m_mouseLastPos;
};

#endif // TCANVAS_H
