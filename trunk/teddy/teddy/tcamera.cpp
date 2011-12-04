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
	reset(eye, center, nearDepth, farDepth, width, height, up, true);
}

TCamera::~TCamera()
{

}
