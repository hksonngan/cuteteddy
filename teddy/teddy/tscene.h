#ifndef TSCENE_H
#define TSCENE_H

#include <QMatrix4x4>
#include <QPolygonF>

//#include <CGAL/Simple_cartesian.h>
//#include <CGAL/Polyhedron_3.h>
//#include <CGAL/Polyhedron_traits_with_normals_3.h>

//typedef CGAL::Simple_cartesian<double> Kernel;
//typedef Kernel::Point_3 Point_3;
//typedef Kernel::Vector_3 Vector_3;
//typedef Kernel::Direction_3 Direction_3;
//
//typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
//
//typedef Polyhedron::Halfedge_handle Halfedge_handle;
//typedef Polyhedron::Facet_handle Facet_handle;
//typedef Polyhedron::Vertex_handle Vertex_handle;
//typedef Polyhedron::Halfedge_around_facet_circulator HF_circulator;
//typedef Polyhedron::Halfedge_around_facet_const_circulator HF_const_circulator;
//typedef Polyhedron::Halfedge_around_vertex_circulator VF_circulator;
//typedef Polyhedron::Halfedge_around_vertex_const_circulator VF_const_circulator;

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

struct TMeshTraits : public OpenMesh::DefaultTraits
{
	VertexAttributes( OpenMesh::Attributes::Normal |  OpenMesh::Attributes::Status);
	FaceAttributes( OpenMesh::Attributes::Normal | OpenMesh::Attributes::Status | OpenMesh::Attributes::Color);
	HalfedgeAttributes( OpenMesh::Attributes::Status);
	EdgeAttributes(OpenMesh::Attributes::Status);
};

typedef OpenMesh::TriMesh_ArrayKernelT<TMeshTraits> TriMesh;

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

	//QList<QVector3D> mapToZPlane(const QVector<QPointF>& screenPs, double z = 1.0);
	void mapToZPlane(double z = 1.0);

private:
	//Polyhedron m_mesh;
	TriMesh m_mesh;
	OpenMesh::FPropHandleT<int> m_ftype;

	QMatrix4x4 m_mat;
	QMatrix4x4 m_cam;
	QMatrix4x4 m_proj;
	int m_canvasWidth, m_canvasHeight;
	
	QVector3D m_cam_eye, m_cam_center, m_cam_up;

	QList<QVector3D> m_seeds;
};


#endif // TSCENE_H
