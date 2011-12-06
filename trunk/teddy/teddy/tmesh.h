#ifndef TMESH_H
#define TMESH_H

#include "tentity.h"

// need to define _USE_MATH_DEFINES and NOMINMAX in preprocess tab
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

class TMesh : public TEntity
{
	typedef OpenMesh::TriMesh_ArrayKernelT<> TriMesh;
public:
	TMesh(QObject *parent = 0);
	
	bool open(const QString& filename);
	bool save(const QString& filename);
	
	void paintEntity();

private:
	TriMesh m_mesh;
};

#endif // TMESH_H
