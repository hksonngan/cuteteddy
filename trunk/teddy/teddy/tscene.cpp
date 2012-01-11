#include "tscene.h"

#include <vector>

#include <QQueue>
#include <QMap>
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

#define FT_JOINT	0
#define FT_SLEEVE	1
#define FT_TERMINAL 2
#define FT_ISOLATED 3
#define FT_NEW		5

#define FT_JOINT_COLOR		TriMesh::Color(0, 0, 1)
#define FT_SLEEVE_COLOR		TriMesh::Color(0, 1, 0)
#define FT_TERMINAL_COLOR	TriMesh::Color(1, 0, 0)

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

		glBegin(GL_LINE_LOOP);
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
			mesh.add_face(vhs[is[0]], vhs[is[1]], vhs[is[2]]);
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

void tDelaunay(TriMesh& mesh)
{
	//// delaunay
	for(TriMesh::VertexIter vi = mesh.vertices_begin();
		vi != mesh.vertices_end();
		++ vi)
	{
		for(TriMesh::VertexOHalfedgeIter oh = mesh.voh_begin(vi.handle());
			oh != mesh.voh_end(vi.handle());
			++ oh)
		{
			TriMesh::HalfedgeHandle testh = mesh.next_halfedge_handle(oh.handle());
			tLegalize(mesh, vi.handle(), testh);
		}
	}

	mesh.delete_isolated_vertices();
	mesh.garbage_collection();
}

inline double tSqDist(const TriMesh::Point& p1, const TriMesh::Point& p2)
{
	TriMesh::Point sub = p1 - p2;
	return sub[0] * sub[0] + sub[1] * sub[1] + sub[2] * sub[2];
}

inline int tFaceType(TriMesh& mesh, const TriMesh::FaceHandle& curfh)
{
	int t = 0;
	for(TriMesh::FaceHalfedgeIter fei = mesh.fh_begin(curfh);
		fei != mesh.fh_end(curfh);
		++ fei)
		if(mesh.is_boundary(fei.handle()))
			t ++;
	return t;
}

template <typename T>
inline void tFillFacesProperty(TriMesh& mesh, OpenMesh::FPropHandleT<T> prop, T v)
{
	for(TriMesh::FaceIter fi = mesh.faces_begin();
		fi != mesh.faces_end();
		++ fi)
	{
		mesh.property(prop, fi.handle()) = v;
	}
}

void tClassifyFaces(TriMesh& mesh, OpenMesh::FPropHandleT<int> faceType)
{
	tFillFacesProperty<int>(mesh, faceType, 0);
	for(TriMesh::EdgeIter ei = mesh.edges_begin();
		ei != mesh.edges_end();
		++ ei)
	{
		TriMesh::EdgeHandle eh = ei.handle();
		TriMesh::FaceHandle fh1 = mesh.face_handle(mesh.halfedge_handle(eh, 0));
		TriMesh::FaceHandle fh2 = mesh.face_handle(mesh.halfedge_handle(eh, 1));

		if(!mesh.is_valid_handle(fh1)){
			mesh.property(faceType, fh2) = mesh.property(faceType, fh2) + 1;
		}
		if(!mesh.is_valid_handle(fh2)){
			mesh.property(faceType, fh1) = mesh.property(faceType, fh1) + 1;
		}
	}
}

inline void tAdjustAllVertices(TriMesh& mesh)
{
	for(TriMesh::VertexIter vi = mesh.vertices_begin();
		vi != mesh.vertices_end();
		++ vi)
		mesh.adjust_outgoing_halfedge(vi.handle());
}

QList<TriMesh::FaceHandle> 
tSplitJointFaces(TriMesh& mesh, OpenMesh::FPropHandleT<int> faceType)
{
	QList<TriMesh::FaceHandle> terminalFaces;
	for(TriMesh::FaceIter fi = mesh.faces_begin();
		fi != mesh.faces_end();
		++ fi)
	{
		TriMesh::FaceHandle fh = fi.handle();
		int t = mesh.property(faceType, fh);
		if(t == FT_TERMINAL){
			//mesh.set_color(fh, FT_TERMINAL_COLOR);
			terminalFaces.push_back(fh);
		}else if(t == FT_SLEEVE){
			//mesh.set_color(fh, FT_SLEEVE_COLOR);
		}else if(t == FT_JOINT){
			//mesh.set_color(fh, FT_JOINT_COLOR);
			// split joint faces
			TriMesh::Point c(0, 0, 0);
			for(TriMesh::FaceVertexIter fvi = mesh.fv_begin(fh);
				fvi != mesh.fv_end(fh);
				++ fvi)
				c += mesh.point(fvi.handle());
			c /= 3.0;

			TriMesh::VertexHandle nvh = mesh.add_vertex(c);
			mesh.split(fh, nvh);
			for(TriMesh::VertexFaceIter vfi = mesh.vf_begin(nvh);
				vfi != mesh.vf_end(nvh);
				++ vfi)
				mesh.property(faceType, vfi.handle()) = FT_NEW;
		}
	}

	return terminalFaces;
}

void tReTopo(TriMesh& mesh)
{
	// mark all faces
	QList<TriMesh::FaceHandle> terminalFaces;

	OpenMesh::FPropHandleT<int> faceType;
	OpenMesh::FPropHandleT<bool> faceChecked;
	mesh.add_property(faceType, "face-type");
	mesh.add_property(faceChecked, "face-checked");

	tFillFacesProperty(mesh, faceChecked, false);

	// classify faces
	tClassifyFaces(mesh, faceType);
	terminalFaces = tSplitJointFaces(mesh, faceType);
	tClassifyFaces(mesh, faceType);
	tAdjustAllVertices(mesh);

	// remesh terminal faces and sleeve faces
	for(int i = 0; i < terminalFaces.size(); i++)
	{	
		// old record may change
		if(mesh.property(faceType, terminalFaces[i]) != FT_TERMINAL)
			continue;
		if(mesh.property(faceChecked, terminalFaces[i]))
			continue;

		QList<TriMesh::VertexHandle> collectedVs; // vertices recorded in clockwise
		QList<TriMesh::FaceHandle> collectedFs; // faces recorded, will be deleted for remesh		

		// insert the boundary point
		for(TriMesh::FaceVertexIter fvi = mesh.fv_begin(terminalFaces[i]);
			fvi != mesh.fv_end(terminalFaces[i]);
			++ fvi)
		{
			if(mesh.valence(fvi.handle()) == 2){
				collectedVs.push_back(fvi.handle());
				break;
			}
		}
		Q_ASSERT(collectedVs.size() == 1);		

		forever {
			bool mustStop = false;

			// get curfh
			if(collectedFs.empty()){
				collectedFs.push_back(terminalFaces[i]);
			}else{
				TriMesh::FaceHandle nextFh;
				for(TriMesh::FaceFaceIter ffi = mesh.ff_begin(collectedFs.last());
					ffi != mesh.ff_end(collectedFs.last());
					++ ffi)
				{
					if(!collectedFs.contains(ffi.handle())
						&& mesh.is_valid_handle(ffi.handle())
						&& mesh.property(faceType, ffi.handle()) != FT_NEW )
					{
						nextFh = ffi.handle();
						break;
					}
				}
				if(mesh.is_valid_handle(nextFh))
					collectedFs.push_back(nextFh);
				else
					mustStop = true;
			}
			TriMesh::FaceHandle curfh = collectedFs.last();
			mesh.property(faceChecked, curfh) = true;

			int t = mesh.property(faceType, curfh);			

			if(t == FT_SLEEVE || t == FT_TERMINAL){ // sleeve face or terminal face
				// get next hh2check
				// this edge should not be a bound
				TriMesh::HalfedgeHandle hh2check;
				for(TriMesh::FaceHalfedgeIter fhi = mesh.fh_begin(curfh);
					fhi != mesh.fh_end(curfh);
					++ fhi)
				{
					TriMesh::FaceHandle oppof = mesh.opposite_face_handle(fhi.handle());
					if(mesh.is_valid_handle(oppof)
						&& !collectedFs.contains(oppof))
					{
						hh2check = fhi.handle();
						break;
					}
				}
				Q_ASSERT(mesh.is_valid_handle(hh2check) && !mesh.is_boundary(hh2check));

				// add new vertices
				TriMesh::VertexHandle right, left; 
				right = mesh.from_vertex_handle(hh2check); 
				left = mesh.to_vertex_handle(hh2check);
				Q_ASSERT(mesh.is_valid_handle(left) && mesh.is_valid_handle(right));
				if(!collectedVs.contains(left) && mesh.is_valid_handle(left))
					collectedVs.push_front(left);
				if(!collectedVs.contains(right) && mesh.is_valid_handle(right))
					collectedVs.push_back(right);

				// circle check
				bool inCircle = true;
				TriMesh::Point center = (mesh.point(right) + mesh.point(left)) / 2.0;
				double diamSq = tSqDist(mesh.point(right), mesh.point(left));
				for(int i = 1; i < collectedVs.size()-1; i++)
				{
					TriMesh::VertexHandle vh = collectedVs[i];
					if(tSqDist(mesh.point(vh), center) * 4 > diamSq){
						inCircle = false;
						break;
					}
				}
				if(inCircle && !mustStop){	
					continue;
				}else{
					// delete all collected faces
					foreach(TriMesh::FaceHandle fh, collectedFs)
						mesh.delete_face(fh, false); // don't delete vertices!!!
					collectedFs.clear();
					for(int i = 0; i < collectedVs.size(); i++)
						mesh.adjust_outgoing_halfedge(collectedVs[i]);

					// add a new vertex on the middle point of hh2check
					TriMesh::VertexHandle finalvh = mesh.add_vertex(center);
					mesh.split(mesh.edge_handle(hh2check), finalvh);
					
					// rebuild faces
					for(int i = 1; i < collectedVs.size(); i++)
						mesh.add_face(finalvh, collectedVs[i-1], collectedVs[i]);

					mesh.adjust_outgoing_halfedge(finalvh);
					for(int i = 0; i < collectedVs.size(); i++)
						mesh.adjust_outgoing_halfedge(collectedVs[i]);

					// update type
					for(TriMesh::VertexFaceIter vfi = mesh.vf_begin(finalvh);
						vfi != mesh.vf_end(finalvh);
						++ vfi)
					{
						mesh.property(faceChecked, vfi.handle()) = true;
						mesh.property(faceType, vfi.handle()) = FT_NEW;
					}
					break;
				}
			}	
			
			if(t == FT_JOINT){ // joint face
				// if meets split joint face, remesh and break immediately
				// the final vertex
				TriMesh::VertexHandle finalvh;
				for(TriMesh::FaceVertexIter fvi = mesh.fv_begin(curfh);
					fvi != mesh.fv_end(curfh);
					++ fvi)
				{
					if(!mesh.is_boundary(fvi.handle())){
						finalvh = fvi.handle();
						break;
					}
				}
				Q_ASSERT(mesh.is_valid_handle(finalvh));

				// now remesh
				foreach(TriMesh::FaceHandle fh, collectedFs)
					mesh.delete_face(fh, false);
				collectedFs.clear();
				mesh.adjust_outgoing_halfedge(finalvh);
				for(int i = 0; i < collectedVs.size(); i++)
					mesh.adjust_outgoing_halfedge(collectedVs[i]);

				for(int i = 1; i < collectedVs.size(); i++)
					mesh.add_face(finalvh, collectedVs[i-1], collectedVs[i]);

				mesh.adjust_outgoing_halfedge(finalvh);
				for(int i = 0; i < collectedVs.size(); i++)
					mesh.adjust_outgoing_halfedge(collectedVs[i]);
				
				// update type
				for(TriMesh::VertexFaceIter vfi = mesh.vf_begin(finalvh);
					vfi != mesh.vf_end(finalvh);
					++ vfi)
				{
					mesh.property(faceChecked, vfi.handle()) = true;
					mesh.property(faceType, vfi.handle()) = FT_NEW;
				}
				break;
			}
		}		
	}	

	mesh.remove_property(faceType);
	mesh.remove_property(faceChecked);
	mesh.garbage_collection();
}



bool TScene::build( const QPolygonF& xyseeds )
{
	m_mesh.clear();
	if(xyseeds.empty())
		return false;
	
	std::vector<TriMesh::VertexHandle> vhs;
	for(int i = 0; i < xyseeds.size(); i++)
		vhs.push_back(m_mesh.add_vertex(TriMesh::Point(xyseeds[i].x(), xyseeds[i].y(), 0)));
	
	tPartition(m_mesh, vhs);
	tDelaunay(m_mesh);
	tReTopo(m_mesh);

	


	mapToZPlane((m_cam_eye - m_cam_center).length());
	m_mesh.garbage_collection();
	
	return false;
}
