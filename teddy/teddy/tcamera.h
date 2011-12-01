#ifndef TCAMERA_H
#define TCAMERA_H

#include <QObject>
#include <QMatrix4x4>

class TCamera : public QObject
{
public:
	TCamera(QObject *parent);
	~TCamera();

private:
	QMatrix4x4 m_mat;
};

#endif // TCAMERA_H
