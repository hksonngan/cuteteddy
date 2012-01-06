#include "tscene.h"

#include <QString>
#include <QtOpenGL/QtOpenGL>

#include <fstream>

#include <CGAL/IO/Polyhedron_iostream.h>
#include <CGAL/IO/Polyhedron_inventor_ostream.h>
#include <CGAL/IO/Polyhedron_VRML_1_ostream.h>
#include <CGAL/IO/Polyhedron_VRML_2_ostream.h>
#include <CGAL/IO/Polyhedron_geomview_ostream.h>

#include <list>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Projection_traits_xy_3.h>
#include <CGAL/Delaunay_triangulation_2.h>

// delaunay triangulation defines
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K> Triangulation;

inline void qglVertex3d(const QVector3D& v){glVertex3d(v.x(), v.y(), v.z());}
inline void cgglVertex3d(const Point_3& p){glVertex3d(p.x(), p.y(), p.z());}
inline void qglNormal3d(const QVector3D& v){glNormal3d(v.x(), v.y(), v.z());}
inline void cgglNormal3d(const Vector_3& d){glNormal3d(d.x(), d.y(), d.z());}

TScene::TScene(QObject *parent /* = 0 */)
	: QObject(parent)
{
	m_cam_eye = QVector3D(0, 0, 2.0);
	m_cam_center = QVector3D(0, 0, 0);
	m_cam_up = QVector3D(0, 1, 0);

	m_cam.lookAt(m_cam_eye, m_cam_center, m_cam_up);
	m_proj.perspective(45, 1.0, 0.001, 5000);
}

bool TScene::open(const QString& filename)
{
	m_mesh.clear();

	wchar_t wstr[256];
	int length = filename.toWCharArray(wstr);
	wstr[length] = 0;
	
	std::ifstream is;
	is.open(wstr);
	is >> m_mesh;
	return true;
}

bool TScene::save(const QString& filename)
{
	wchar_t wstr[256];
	int length = filename.toWCharArray(wstr);
	wstr[length] = 0;
	
	std::ofstream os;
	os.open(wstr);
	os << m_mesh;
	return true;
}

void TScene::paint()
{
	glLoadIdentity();
	glMultMatrixd((m_mat * m_cam).constData());

	for(Polyhedron::Face_iterator fit = m_mesh.facets_begin();
		fit != m_mesh.facets_end();
		++ fit)
	{
		glBegin(GL_LINE_LOOP);
		int order = 0;
		HF_circulator h = fit->facet_begin();
		do {
			Point_3 p0 = h->prev()->vertex()->point();
			Point_3 p1 = h->vertex()->point();
			Point_3 p2 = h->next()->vertex()->point();
			cgglNormal3d(CGAL::cross_product(p2 - p1, p1 - p0));
			cgglVertex3d(p1);
			++ order;
		} while ( ++h != fit->facet_begin());
		CGAL_assertion( order >= 3); // guaranteed by definition of
		glEnd();
	}
}

void TScene::paintMarkers()
{
	glLoadIdentity();
	glMultMatrixd((m_mat * m_cam).constData());

	glBegin(GL_LINE_LOOP);
	for(int i = 0; i < m_seeds.size(); i++)
		qglVertex3d(m_seeds[i]);
	glEnd();
}

void TScene::setupViewport( int w, int h )
{
	m_canvasHeight = h;
	m_canvasWidth = w;

	int side = qMax(w, h);
	glViewport((w - side) / 2, (h - side) / 2, side, side);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	glMultMatrixd(m_proj.constData());
	glMatrixMode(GL_MODELVIEW);
}


QList<QVector3D> TScene::mapToZPlane( const QVector<QPointF>& screenPs, double z /*= 1.0*/ )
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLdouble winX, winY, winZ;
	GLdouble posX, posY, posZ;

	int side = qMax(m_canvasWidth, m_canvasHeight);
	viewport[0] = (m_canvasWidth - side) / 2;
	viewport[1] = (m_canvasHeight - side) / 2;
	viewport[2] = viewport[3] = side;

	QList<QVector3D> results;

	foreach(QPointF screenP, screenPs){
		winX = screenP.x();
		winY = m_canvasHeight - screenP.y();
		glReadPixels(screenP.x(), int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

		if(GL_TRUE == gluUnProject( winX, winY, 1, (m_mat * m_cam).constData(), 
			m_proj.constData(), 
			viewport, &posX, &posY, &posZ))
		{
			QVector3D p(posX, posY, posZ);
			results.append(m_cam_eye + (p - m_cam_eye).normalized() * z);
		}
	}
	return results;
}

//void TScene::mapToZPlane(TriMesh& mesh, double z /* = 1.0 */)
//{
//
//}

void TScene::camMoveView(const QVector3D& t)
{
	// like Google earth
	QVector3D tt = t * (m_cam_eye - m_cam_center).length() / 4.0;

	QVector3D xv = QVector3D::crossProduct((m_cam_center - m_cam_eye), m_cam_up).normalized();
	QVector3D yv = QVector3D::crossProduct(xv, (m_cam_center - m_cam_eye)).normalized();
	QVector3D xyTrans = xv * tt.x() + yv * tt.y();
	double r = ((m_cam_eye - m_cam_center).length() - tt.z()) / 
		(m_cam_eye + xyTrans - m_cam_center).length();
	m_cam_eye = (m_cam_eye + xyTrans - m_cam_center) * r + m_cam_center;
	m_cam_up = yv.normalized();

	m_cam.setToIdentity();
	m_cam.lookAt(m_cam_eye, m_cam_center, m_cam_up);
}

void TScene::camMoveCenter(const QVector3D& t)
{
	QVector3D tt = t * (m_cam_eye - m_cam_center).length() / 8.0;

	QVector3D xv = QVector3D::crossProduct((m_cam_center - m_cam_eye), m_cam_up).normalized();
	QVector3D yv = QVector3D::crossProduct(xv, (m_cam_center - m_cam_eye)).normalized();
	QVector3D zv = (m_cam_center  - m_cam_eye).normalized();
	QVector3D trans = xv * tt.x() + yv * tt.y() + zv * tt.z();
	m_cam_eye += trans;
	m_cam_center += trans;

	m_cam.setToIdentity();
	m_cam.lookAt(m_cam_eye, m_cam_center, m_cam_up);
}


bool TScene::build( const QPolygonF& xyseeds )
{
	// triangulate
	std::list<Triangulation::Point> points;
	for(int i = 0; i < xyseeds.size(); i++)
		points.push_back(Triangulation::Point(xyseeds[i].x(), xyseeds[i].y()));

	Triangulation t;
	t.insert(points.begin(), points.end());
	qDebug() << t.number_of_vertices();
	qDebug() << t.number_of_faces();
	
	std::list<Triangulation::Point> axis;
	
	for(Triangulation::Edge_iterator ei = t.edges_begin();
		ei != t.edges_end();
		++ ei)
	{
		//if(t.degree(ei->first()->vertex()) == 2 && t.is_edge(ei->first()->next())){}
	}


	QList<QVector3D> ponts 
		= mapToZPlane(xyseeds, (m_cam_eye - m_cam_center).length());
	
	m_seeds.clear();
	m_seeds << ponts;
	
	return true;
}
