#ifndef TENTITY_H
#define TENTITY_H

#include <QObject>
#include <QMatrix4x4>

class TEntity : public QObject
{
public:
	TEntity(QObject *parent);
	~TEntity();

	inline QMatrix4x4 matrix() const {return m_mat;}
	inline const qreal* matrixData() const {return m_mat.constData();}
	inline void rotate(double angle, double x, double y, double z = 0) 
		{m_mat.rotate(angle, x, y, z);}
	inline void translate(double x, double y, double z) 
		{m_mat.translate(x, y, z);}

	virtual void paint3D(){}

private:
	QMatrix4x4 m_mat;
};

#endif // TENTITY_H
