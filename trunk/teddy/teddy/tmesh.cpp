#include "tmesh.h"

#include <QString>
#include <QtOpenGL/QtOpenGL>
#include <gl/GLUT.H>

TMesh::TMesh(QObject *parent /* = 0 */)
	: TEntity(parent)
{
	m_mesh.request_vertex_normals();
	Q_ASSERT(m_mesh.has_vertex_normals());
}

bool TMesh::open(const QString& filename)
{
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

bool TMesh::save(const QString& filename)
{
	if(!OpenMesh::IO::write_mesh(m_mesh, filename.toStdString()))
		return false;
	return true;
}

void TMesh::paintEntity()
{
	/*TriMesh::FaceIter fit;
	for(fit = m_mesh.faces_begin(); 
		fit != m_mesh.faces_end(); 
		fit ++){
		
	}*/
	glutSolidTeapot(0.5);
}
