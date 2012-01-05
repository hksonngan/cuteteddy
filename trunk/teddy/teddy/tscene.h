#ifndef TSCENE_H
#define TSCENE_H

#include <QMatrix4x4>
#include <QPolygonF>
#include "talgorithm.h"

class TScene : public QObject
{
public:
	TScene(QObject *parent = 0);

	inline void rotate(double angle, const QVector3D& v)
	{QMatrix4x4 rm; rm.rotate(angle, v); m_mat = rm * m_mat;}
	inline void translate(const QVector3D& t) 
	{QMatrix4x4 tm; tm.translate(t); m_mat = tm * m_mat;}
	
	void camMoveView(const QVector3D& trans);
	void camMoveCenter(const QVector3D& trans);
	inline void camZoom(double d){camMoveView(QVector3D(0, 0, d));}
	
	bool open(const QString& filename);
	bool save(const QString& filename);
	
	void paint();
	void paintMarkers();
	void setupViewport(int w, int h);

	// seeds polygons' edge are equal length, seeds.last() != seeds.first()
	bool build(const QPolygonF& seeds);

	QList<QVector3D> mapToZPlane(const QVector<QPointF>& screenPs, double z = 1.0);
	void mapToZPlane(TriMesh& mesh, double z = 1.0);

private:
	TriMesh m_mesh;
	QMatrix4x4 m_mat;
	QMatrix4x4 m_cam;
	QMatrix4x4 m_proj;
	int m_canvasWidth, m_canvasHeight;
	
	QVector3D m_cam_eye, m_cam_center, m_cam_up;

	QList<QVector3D> m_seeds;
};


#endif // TSCENE_H
