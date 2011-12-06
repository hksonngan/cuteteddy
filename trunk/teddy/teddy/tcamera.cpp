#include "tcamera.h"

TCamera::TCamera(QObject* parent /* = 0 */)
	: QObject(parent)
{}
TCamera::TCamera(const QVector3D& eye, 
	const QVector3D& center,
	double nearDepth, double farDepth, 
	double width, double height, 
	const QVector3D& up,
	bool perspective, QObject *parent)
	: QObject(parent)
{
	m_projectMat.setToIdentity();
	if(perspective)
		m_projectMat.frustum(-width/2.0, width/2.0, -height/2.0, height/2.0, nearDepth, farDepth);
	else
		m_projectMat.ortho(-width/2.0, width/2.0, -height/2.0, height/2.0, nearDepth, farDepth);
	m_lookMat.setToIdentity();
	m_lookMat.lookAt(eye, center, up);
}

TCamera::~TCamera()
{

}
