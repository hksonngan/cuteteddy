#include "tmesh.h"

#include <QtOpenGL/QtOpenGL>

TMesh::TMesh(QObject *parent /* = 0 */)
	: TEntity(parent)
{

}

void TMesh::paintEntity()
{
	glBegin(GL_TRIANGLES);
		glNormal3d(1, 1, 1);
		glVertex3d(1, 0, 0);
		glVertex3d(0, 1, 0);
		glVertex3d(0, 0, 1);
	glEnd();
}
