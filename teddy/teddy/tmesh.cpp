#include "tmesh.h"

#include <QtOpenGL/QtOpenGL>

TMesh::TMesh(QObject *parent /* = 0 */)
	: TEntity(parent)
{

}

bool TMesh::open(const QString& filename)
{
	return true;
}

bool TMesh::save(const QString& filename)
{
	return true;
}

void TMesh::paintEntity()
{
	
}
