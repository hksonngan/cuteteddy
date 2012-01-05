#include "talgorithm.h"

void tTrianglulateAddPoint(TriMesh& mesh, QPointF& p)
{

}

void tTriangulate(const QVector<QPointF>& points, TriMesh& mesh)
{
	mesh.clear();
	if(points.size() < 3)
		return;

	qreal maxx = -DBL_MAX, minx = DBL_MAX;
	qreal maxy = -DBL_MAX, miny = DBL_MAX;

	foreach(QPointF p, points){
		maxx = maxx < p.x() ? p.x() : maxx;
		minx = minx > p.x() ? p.x() : minx;
		maxy = maxy < p.y() ? p.y() : maxy;
		miny = miny > p.y() ? p.y() : miny;
	}

	minx -= 10000*(maxx - minx); miny -= 10000 * (maxy - miny);
	maxx += 10000*(maxx - minx); maxy += 10000 * (maxy - miny);

	QPointF a1((maxx + minx)/2, (3*maxy - miny)/2), 
		a2(2*minx - maxx, miny), 
		a3(2*maxx - minx, miny);


}

void tReTriangulate(TriMesh& triMesh)
{

}