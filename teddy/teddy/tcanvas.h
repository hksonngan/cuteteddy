#ifndef TCANVAS_H
#define TCANVAS_H

#include <QGLWidget>
#include <QMatrix4x4>

#include "tmesh.h"

class TCanvas : public QGLWidget
{
public:
	TCanvas(QWidget *parent = 0);
	~TCanvas();

	//inline TMesh* object() const {return m_object;}
	inline void setFileInfo(const QFileInfo& fi) {m_fileInfo = fi;}
	inline QFileInfo fileInfo() const {return m_fileInfo;}
	inline bool fileExists() const {return m_fileInfo.exists();}

	inline bool open();
	inline bool save();

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int w, int h);

	void camMoveView(const QVector3D& trans);
	void camMoveCenter(const QVector3D& trans);
	inline void camZoom(double d){camMoveView(QVector3D(0, 0, d));}

	void mousePressEvent(QMouseEvent * e);
	void mouseMoveEvent(QMouseEvent * e);
	void mouseReleaseEvent(QMouseEvent * e);
	void wheelEvent(QWheelEvent * e);

private:
	TMesh* m_object;

	QFileInfo m_fileInfo;

	QMatrix4x4 m_modelmat;
	QMatrix4x4 m_cammat;
	QMatrix4x4 m_projmat;
	QVector3D m_cam_center, m_cam_eye, m_cam_up;

	QPointF m_mouseLastPos;
};

#endif // TCANVAS_H
