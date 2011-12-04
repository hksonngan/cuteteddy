#include "tentity.h"

#include <QtOpenGL>

TEntity::TEntity(QObject *parent)
	: QObject(parent)
{

}

TEntity::~TEntity()
{

}

void TEntity::paintAll()
{
	glMultMatrixd(m_mat.constData());
	paintEntity();
	foreach(QObject* child, children()){
		TEntity* childEnt = dynamic_cast<TEntity*>(child);
		if(childEnt)
			childEnt->paintAll();
	}
}
