#ifndef TALGORITHMS_H
#define TALGORITHMS_H

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

namespace TAlgorithms
{
	void tPartition(TriMesh& mesh, const std::vector<TriMesh::VertexHandle>& vhs);
	void tDelaunay(TriMesh& mesh);
	void tReTopo(TriMesh& mesh);
	void tSew(TriMesh& mesh, int sep);
	void tSmooth(TriMesh& mesh, int rep);
}

#endif // TALGORITHMS_H
