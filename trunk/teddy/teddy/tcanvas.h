#ifndef TCANVAS_H
#define TCANVAS_H

#include <QGLWidget>

class TCanvas : public QGLWidget
{
	Q_OBJECT

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
	
};

#endif // TCANVAS_H
