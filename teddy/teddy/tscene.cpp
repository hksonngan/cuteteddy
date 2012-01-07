#include "tscene.h"

#include <vector>

#include <QQueue>
#include <QList>
#include <QString>
#include <QMatrix3x3>
#include <QtOpenGL/QtOpenGL>

//#include <fstream>
//
//#include <CGAL/IO/Polyhedron_iostream.h>
//#include <CGAL/IO/Polyhedron_inventor_ostream.h>
//#include <CGAL/IO/Polyhedron_VRML_1_ostream.h>
//#include <CGAL/IO/Polyhedron_VRML_2_ostream.h>
//#include <CGAL/IO/Polyhedron_geomview_ostream.h>
//
//#include <list>
//
//#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
//#include <CGAL/Constrained_Delaunay_triangulation_2.h>
//#include <CGAL/Triangulation_conformer_2.h>
//
//#include <iostream>
//
//typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
//typedef CGAL::Constrained_Delaunay_triangulation_2<K> CDT;

inline void qglVertex3d(const QVector3D& v){glVertex3d(v.x(), v.y(), v.z());}
//inline void cgglVertex3d(const Point_3& p){glVertex3d(p.x(), p.y(), p.z());}
inline void qglNormal3d(const QVector3D& v){glNormal3d(v.x(), v.y(), v.z());}
//inline void cgglNormal3d(const Vector_3& d){glNormal3d(d.x(), d.y(), d.z());}

#define qRoundAt(index, c) \
	(index < 0 ? c[-((-index) % c.size()) + c.size()] : c[(index + c.size()) % c.size()])
#define qRoundNear(a, b, size) \
	(abs(a - b) <= 1 || a == 0 && b == (size)-1 || a == (size)-1 && b== 0)

#define FT_INIT_COLOR		TriMesh::Color(2, 0, 0)
#define FT_INCR_COLOR		TriMesh::Color(-2, 0, 2)

#define FT_JOINT_COLOR		FT_INIT_COLOR
#define FT_SLEEVE_COLOR		(FT_INIT_COLOR + FT_INCR_COLOR)
#define FT_TERMINAL_COLOR	(FT_INIT_COLOR + FT_INCR_COLOR + FT_INCR_COLOR)

#define ET_AXIS_COLOR		TriMesh::Color(1, 1, 1)
#define ET_NOAXIS_COLOR		TriMesh::Color(0, 0, 0)

#define VT_AXIS_COLOR		ET_AXIS_COLOR
#define VT_NOAXIS_COLOR		ET_NOAXIS_COLOR

TScene::TScene(QObject *parent /* = 0 */)
	: QObject(parent)
{
	m_mesh.request_vertex_normals();
	Q_ASSERT(m_mesh.has_vertex_normals());
	m_mesh.add_property(m_ftype, "face_type");

	m_cam_eye = QVector3D(0, 0, 2.0);
	m_cam_center = QVector3D(0, 0, 0);
	m_cam_up = QVector3D(0, 1, 0);

	m_cam.lookAt(m_cam_eye, m_cam_center, m_cam_up);
	m_proj.perspective(45, 1.0, 0.001, 5000);
}

bool TScene::open(const QString& filename)
{
	/*m_mesh.clear();

	wchar_t wstr[256];
	int length = filename.toWCharArray(wstr);
	wstr[length] = 0;
	
	std::ifstream is;
	is.open(wstr);
	is >> m_mesh;
	return true;*/

	OpenMesh::IO::Options opt;
	if(!OpenMesh::IO::read_mesh(m_mesh, filename.toStdString(), opt))
		return false;
	if(!opt.check(OpenMesh::IO::Options::VertexNormal)){
		m_mesh.request_face_normals();
		m_mesh.update_normals();
		m_mesh.release_face_normals();
	}
	return true;
}

bool TScene::save(const QString& filename)
{
	/*wchar_t wstr[256];
	int length = filename.toWCharArray(wstr);
	wstr[length] = 0;
	
	std::ofstream os;
	os.open(wstr);
	os << m_mesh;
	return true;*/


	if(!OpenMesh::IO::write_mesh(m_mesh, filename.toStdString()))
		return false;
	return true;
}

void TScene::paint()
{
	glLoadIdentity();
	glMultMatrixd((m_mat * m_cam).constData());

	//for(Polyhedron::Face_iterator fit = m_mesh.facets_begin();
	//	fit != m_mesh.facets_end();
	//	++ fit)
	//{
	//	glBegin(GL_LINE_LOOP);
	//	int order = 0;
	//	HF_circulator h, end;
	//	h = end = fit->facet_begin();
	//	CGAL_For_all(h, end){
	//		Point_3 p0 = h->prev()->vertex()->point();
	//		Point_3 p1 = h->vertex()->point();
	//		Point_3 p2 = h->next()->vertex()->point();
	//		cgglNormal3d(CGAL::cross_product(p2 - p1, p1 - p0));
	//		cgglVertex3d(p1);
	//		++ order;
	//	}
	//	CGAL_assertion( order >= 3); // guaranteed by definition of
	//	glEnd();
	//}

	for(TriMesh::FaceIter fit = m_mesh.faces_begin(); 
		fit != m_mesh.faces_end();++ fit)
	{  
		TriMesh::Color c = m_mesh.color(fit.handle());
		glColor3d(c[0], c[1], c[2]);

		glBegin(GL_TRIANGLE_FAN);
		for(TriMesh::FaceVertexIter fvit = m_mesh.fv_begin(fit);
			fvit != m_mesh.fv_end(fit); ++fvit)
		{
			TriMesh::Normal n = m_mesh.normal(fvit.handle());
			glNormal3d(n[0], n[1], n[2]);
			TriMesh::Point p = m_mesh.point(fvit.handle());
			glVertex3d(p[0], p[1], p[2]);
		}
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


//QList<QVector3D> TScene::mapToZPlane( const QVector<QPointF>& screenPs, double z /*= 1.0*/ )
//{
//	GLint viewport[4];
//	GLdouble modelview[16];
//	GLdouble projection[16];
//	GLdouble winX, winY, winZ;
//	GLdouble posX, posY, posZ;
//
//	int side = qMax(m_canvasWidth, m_canvasHeight);
//	viewport[0] = (m_canvasWidth - side) / 2;
//	viewport[1] = (m_canvasHeight - side) / 2;
//	viewport[2] = viewport[3] = side;
//
//	QList<QVector3D> results;
//
//	foreach(QPointF screenP, screenPs){
//		winX = screenP.x();
//		winY = m_canvasHeight - screenP.y();
//		glReadPixels(screenP.x(), int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
//
//		if(GL_TRUE == gluUnProject( winX, winY, 1, (m_mat * m_cam).constData(), 
//			m_proj.constData(), 
//			viewport, &posX, &posY, &posZ))
//		{
//			QVector3D p(posX, posY, posZ);
//			results.append(m_cam_eye + (p - m_cam_eye).normalized() * z);
//		}
//	}
//	return results;
//}

void TScene::mapToZPlane(double z /* = 1.0 */)
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

	for(TriMesh::VertexIter vi  = m_mesh.vertices_begin();
		vi != m_mesh.vertices_end();
		++ vi)
	{
		TriMesh::Point p = m_mesh.point(vi.handle());
		winX = p[0];
		winY = m_canvasHeight - p[1];
		glReadPixels(int(winX), int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

		if(GL_TRUE == gluUnProject( winX, winY, 1, (m_mat * m_cam).constData(), 
			m_proj.constData(), 
			viewport, &posX, &posY, &posZ))
		{
			QVector3D p(posX, posY, posZ);
			QVector3D r = m_cam_eye + (p - m_cam_eye).normalized() * z;
			m_mesh.set_point(vi.handle(), TriMesh::Point(r.x(), r.y(), r.z()));
		}
	}
}

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



inline double tDet(double* data)
{
	double tmp1 = data[0*3+0] * (data[1*3+1]*data[2*3+2] - data[1*3+2]*data[2*3+1]);
	double tmp2 = data[0*3+1] * (data[1*3+0]*data[2*3+2] - data[1*3+2]*data[2*3+0]);
	double tmp3 = data[0*3+2] * (data[1*3+0]*data[2*3+1] - data[1*3+1]*data[2*3+0]);
	return tmp1 - tmp2 + tmp3;
}

inline bool tLeft(const TriMesh::Point& p,
				  const TriMesh::Point& a, const TriMesh::Point& b)
{
	double data[9] = {a[0], a[1], 1, b[0], b[1], 1, p[0], p[1], 1};
	return tDet(data) > 0;
}

inline bool tInTriangle(const TriMesh::Point& p,
						const TriMesh::Point& a, const TriMesh::Point& b, const TriMesh::Point& c)
{
	bool lab = tLeft(p, a, b);
	bool lbc = tLeft(p, b, c);
	bool lca = tLeft(p, c, a);
	return lab == lbc && lbc == lca;
}

inline void tPartition(TriMesh& mesh, const std::vector<TriMesh::VertexHandle>& vhs)
{
	// use queue!!!
	QQueue<QList<int> > vhIndexGroupQ;
	QList<int> indexG;
	for(int i = 0; i < vhs.size(); i++)
		indexG.push_back(i);
	vhIndexGroupQ.push_back(indexG);

	while(!vhIndexGroupQ.empty())
	{
		QList<int> is = vhIndexGroupQ.first();
		vhIndexGroupQ.pop_front();

		Q_ASSERT(is.size() >= 3);
		if(is.size() <= 2)
			continue;

		if(is.size() == 3){
			TriMesh::FaceHandle fi = mesh.add_face(vhs[is[0]], vhs[is[1]], vhs[is[2]]);
			mesh.set_color(fi, FT_INIT_COLOR);
		}else{
			// leftmost
			int leftmostII = 0;
			TriMesh::Point leftmostP = mesh.point(vhs[is[leftmostII]]);
			for(int i = 0; i < is.size(); i++){
				TriMesh::Point p = mesh.point(vhs[is[i]]);
				if(p[0] < leftmostP[0]){
					leftmostII = i;
					leftmostP = p;
				}
			}

			int leftmostPrevII = (leftmostII + is.size() - 1) % is.size();
			int leftmostNextII = (leftmostII + 1) % is.size();
			TriMesh::Point a = mesh.point(vhs[is[leftmostPrevII]]);
			TriMesh::Point b = mesh.point(vhs[is[leftmostNextII]]);

			int innerLeftmostII = -1;
			TriMesh::Point innerLeftmostP;
			for(int i = 0; i < is.size(); i++){
				if(qRoundNear(i, leftmostII, is.size()))
					continue;
				TriMesh::Point p = mesh.point(vhs[is[i]]);
				if(tInTriangle(p, a, leftmostP, b))
				{
					if(innerLeftmostII == -1){
						innerLeftmostII = i;
						innerLeftmostP = p;
					}else if(p[0] < innerLeftmostP[0]){
						innerLeftmostII = i;
						innerLeftmostP = p;
					}
				}
			}

			int split1 = leftmostII;
			int split2 = innerLeftmostII;
			if(innerLeftmostII < 0){
				split1 = leftmostPrevII;
				split2 = leftmostNextII;
			}

			Q_ASSERT(split1 != split2);

			QList<int> part1, part2;

			for(int i = split1; i != split2; i = (i + 1) % is.size())
				part1.push_back(is[i]);
			part1.push_back(is[split2]);
			for(int i = split2; i != split1; i = (i + 1) % is.size())
				part2.push_back(is[i]);
			part2.push_back(is[split1]);

			Q_ASSERT(part1.size() >= 3);
			Q_ASSERT(part2.size() >= 3);

			is.clear();

			vhIndexGroupQ.push_back(part1);
			vhIndexGroupQ.push_back(part2);
		}
	}
}

inline bool tInCircle(const TriMesh::Point& p,
					  const TriMesh::Point& a, const TriMesh::Point& b, const TriMesh::Point& c)
{
	double data1[9] = { b[0], b[1], b[0]*b[0] + b[1]*b[1], 
		c[0], c[1], c[0]*c[0] + c[1]*c[1],
		p[0], p[1], p[0]*p[0] + p[1]*p[1]};
	double data2[9] = { a[0], a[1], a[0]*a[0] + a[1]*a[1], 
		c[0], c[1], c[0]*c[0] + c[1]*c[1],
		p[0], p[1], p[0]*p[0] + p[1]*p[1]};
	double data3[9] = { a[0], a[1], a[0]*a[0] + a[1]*a[1],
		b[0], b[1], b[0]*b[0] + b[1]*b[1], 					    
		p[0], p[1], p[0]*p[0] + p[1]*p[1]};
	double data4[9] = { a[0], a[1], a[0]*a[0] + a[1]*a[1],
		b[0], b[1], b[0]*b[0] + b[1]*b[1], 					    
		c[0], c[1], c[0]*c[0] + c[1]*c[1]};
	double result = - tDet(data1) + tDet(data2) - tDet(data3) + tDet(data4);
	return result > 0;
}

void tLegalize(TriMesh& mesh, const TriMesh::VertexHandle& vh, 
					  const TriMesh::HalfedgeHandle& testh)
{
	if(testh == TriMesh::InvalidHalfedgeHandle)
		return;

	TriMesh::HalfedgeHandle oppoh = mesh.opposite_halfedge_handle(testh);

	TriMesh::VertexHandle sidev1  = mesh.to_vertex_handle(testh);
	TriMesh::VertexHandle sidev2 = mesh.to_vertex_handle(oppoh);
	TriMesh::VertexHandle oppov = mesh.to_vertex_handle(mesh.next_halfedge_handle(oppoh));

	TriMesh::Point p = mesh.point(vh);
	TriMesh::Point sidep1 = mesh.point(sidev1);
	TriMesh::Point sidep2 = mesh.point(sidev2);
	TriMesh::Point oppop = mesh.point(oppov);

	if(tInCircle(p, sidep1, sidep2, oppop))
	{
		TriMesh::EdgeHandle eh = mesh.edge_handle(testh);
		if(mesh.is_flip_ok(eh)){
			TriMesh::HalfedgeHandle h1, h2;
			h1 = mesh.next_halfedge_handle(oppoh);
			h2 = mesh.next_halfedge_handle(h1);
			mesh.flip(eh);

			// more legalize
			tLegalize(mesh, vh, h1);
			tLegalize(mesh, vh, h2);
		}
	} 
}

void tReTriangulate(TriMesh& mesh)
{
	// mark all ear faces
	std::vector<TriMesh::FaceHandle> terminalFaces;
	for(TriMesh::EdgeIter ei = mesh.edges_begin();
		ei != mesh.edges_end();
		++ ei)
	{
		TriMesh::EdgeHandle eh = ei.handle();
		TriMesh::FaceHandle fh1 = mesh.face_handle(mesh.halfedge_handle(eh, 0));
		TriMesh::FaceHandle fh2 = mesh.face_handle(mesh.halfedge_handle(eh, 1));

		if(!mesh.is_valid_handle(fh1)){
			mesh.set_color(fh2, mesh.color(fh2) + FT_INCR_COLOR);
		}
		if(!mesh.is_valid_handle(fh2)){
			mesh.set_color(fh1, mesh.color(fh1) + FT_INCR_COLOR);
		}
	}

	for(TriMesh::FaceIter fi = mesh.faces_begin();
		fi != mesh.faces_end();
		++ fi)
	{
		TriMesh::FaceHandle fh = fi.handle();
		TriMesh::Color c = mesh.color(fh);
		if(c == FT_TERMINAL_COLOR)
			terminalFaces.push_back(fh);
	}

	
}

bool TScene::build( const QPolygonF& xyseeds )
{
	m_mesh.clear();
	if(xyseeds.empty())
		return false;
	//for(int i = 0; i < xyseeds.size(); i++){
	//	QLineF line1(xyseeds[i], qRoundAt(i+1, xyseeds));
	//	for(int j = i + 2; j < xyseeds.size(); j++){
	//		QLineF line2(qRoundAt(j, xyseeds), qRoundAt(j+1, xyseeds));
	//		if(line1.intersect(line2, 0) == QLineF::BoundedIntersection)
	//			return false;
	//	}
	//}
	// add type property for faces
	
	// partition
	std::vector<TriMesh::VertexHandle> vhs;
	for(int i = 0; i < xyseeds.size(); i++)
		vhs.push_back(m_mesh.add_vertex(TriMesh::Point(xyseeds[i].x(), xyseeds[i].y(), 0)));
	tPartition(m_mesh, vhs);

	qDebug() << m_mesh.n_vertices();
	qDebug() << m_mesh.n_faces();

	//// delaunay
	for(TriMesh::VertexIter vi = m_mesh.vertices_begin();
		vi != m_mesh.vertices_end();
		++ vi)
	{
		for(TriMesh::VertexOHalfedgeIter oh = m_mesh.voh_begin(vi.handle());
			oh != m_mesh.voh_end(vi.handle());
			++ oh)
		{
			TriMesh::HalfedgeHandle testh = m_mesh.next_halfedge_handle(oh.handle());
			tLegalize(m_mesh, vi.handle(), testh);
		}
	}

	/// retriangulate
	tReTriangulate(m_mesh);	

	mapToZPlane((m_cam_eye - m_cam_center).length());
	m_mesh.garbage_collection();

	//QList<QVector3D> ponts 
	//	= mapToZPlane(xyseeds, (m_cam_eye - m_cam_center).length());
	//
	//m_seeds.clear();
	//m_seeds << ponts;
	
	return false;
}
