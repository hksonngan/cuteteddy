#include "talgorithm.h"

void tTriangulate(const QList<QVector3D>& points, TriMesh& mesh)
{
	if(points.size() < 3)
		return;

	qreal maxx = -DBL_MAX, minx = DBL_MAX;
	qreal maxy = -DBL_MAX, miny = DBL_MAX;
	qreal maxz = -DBL_MAX, minz = DBL_MAX;

	foreach(QVector3D p, points){
		maxx = maxx < p.x() ? p.x() : maxx;
		minx = minx > p.x() ? p.x() : minx;
		maxy = maxy < p.y() ? p.y() : maxy;
		miny = miny > p.y() ? p.y() : miny;
		maxz = maxz < p.z() ? p.z() : maxz;
		minz = minz > p.z() ? p.z() : minz;
	}


}

void tReTriangulate(TriMesh& triMesh)
{

}