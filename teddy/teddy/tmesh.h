#ifndef TMESH_H
#define TMESH_H

#include "tentity.h"

// need to define _USE_MATH_DEFINES and NOMINMAX in preprocess tab
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

class TMesh : public TEntity
{
public:
	TMesh(QObject *parent = 0);
	void paintEntity();
private:
	OpenMesh::TriMesh_ArrayKernelT<> m_mesh;
};

#endif // TMESH_H
