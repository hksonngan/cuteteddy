#ifndef TALGORITHM_H
#define TALGORITHM_H

#include <QVector3D>
#include <QPointF>
#include <QPolygonF>
#include <QList>

// need to define _USE_MATH_DEFINES and NOMINMAX in preprocess tab
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

typedef OpenMesh::TriMesh_ArrayKernelT<> TriMesh;

void tTriangulate(const QVector<QPointF>& points, TriMesh& mesh);

void tReTriangulate(TriMesh& triMesh);

#endif // TALGORITHM_H
