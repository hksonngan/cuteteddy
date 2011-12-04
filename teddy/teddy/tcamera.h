#ifndef TCAMERA_H
#define TCAMERA_H

#include <QObject>
#include <QMatrix4x4>

class TCamera : public QObject
{
public:
	TCamera(QObject* parent = 0);
	TCamera(const QVector3D& eye = QVector3D(0, 0, 2), 
		const QVector3D& center = QVector3D(0, 0, 0),
		double nearDepth = 0.1, double farDepth = 30.0, 
		double width = 0.2, double height = 0.2, 
		const QVector3D& up = QVector3D(0, 1, 0),
		bool perspective = true, QObject *parent = 0);
	~TCamera();

	inline void reset(const QVector3D& eye = QVector3D(0, 0, 2), 
		const QVector3D& center = QVector3D(0, 0, 0),
		double nearDepth = 0.1, double farDepth = 30.0, 
		double width = 0.2, double height = 0.2, 
		const QVector3D& up = QVector3D(0, 1, 0),
		bool perspective = true)
	{
		m_projectMat.setToIdentity();
		m_projectMat.frustum(-width/2.0, width/2.0, -height/2.0, height/2.0, nearDepth, farDepth);
		m_lookMat.setToIdentity();
		m_lookMat.lookAt(eye, center, up);
	}

	inline const qreal* projectMatData() const
		{return m_projectMat.constData();}
	inline const qreal* lookMatData() const
		{return m_lookMat.constData();}

private:
	QMatrix4x4 m_projectMat, m_lookMat;
};

#endif // TCAMERA_H
