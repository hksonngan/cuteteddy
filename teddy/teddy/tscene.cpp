#include "tscene.h"

#include <QString>
#include <QtOpenGL/QtOpenGL>

inline void qglVertex3d(const QVector3D& v){glVertex3d(v.x(), v.y(), v.z());}
inline void qglNormal3d(const QVector3D& v){glNormal3d(v.x(), v.y(), v.z());}

TScene::TScene(QObject *parent /* = 0 */)
	: QObject(parent)
{
	m_mesh.request_vertex_normals();
	Q_ASSERT(m_mesh.has_vertex_normals());

	m_cam_eye = QVector3D(0, 0, 2.0);
	m_cam_center = QVector3D(0, 0, 0);
	m_cam_up = QVector3D(0, 1, 0);

	m_cam.lookAt(m_cam_eye, m_cam_center, m_cam_up);
	m_proj.perspective(45, 1.0, 0.001, 5000);
}

bool TScene::open(const QString& filename)
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

bool TScene::save(const QString& filename)
{
	if(!OpenMesh::IO::write_mesh(m_mesh, filename.toStdString()))
		return false;
	return true;
}

void TScene::paint()
{
	glLoadIdentity();
	glMultMatrixd((m_mat * m_cam).constData());

	for(TriMesh::FaceIter fit = m_mesh.faces_begin(); 
		fit != m_mesh.faces_end();++ fit)
	{		
		glBegin(GL_TRIANGLE_FAN);
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
	for(int i = 0; i < seeds.size(); i++)
		glVertex3d(seeds[i][0], seeds[i][1], seeds[i][2]);
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


QList<TScene::TriMesh::Point> TScene::mapToZPlane( const QVector<QPointF>& screenPs, double z /*= 1.0*/ )
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

	QList<TriMesh::Point> results;

	foreach(QPointF screenP, screenPs){
		winX = screenP.x();
		winY = m_canvasHeight - screenP.y();
		glReadPixels(screenP.x(), int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

		if(GL_TRUE == gluUnProject( winX, winY, 1, (m_mat * m_cam).constData(), 
			m_proj.constData(), 
			viewport, &posX, &posY, &posZ))
		{
			QVector3D p(posX, posY, posZ);
			p = m_cam_eye + (p - m_cam_eye).normalized() * z;
			results.append(TriMesh::Point(p.x(), p.y(), p.z()));
		}
	}
	return results;
}

void TScene::camMoveView(const QVector3D& tt)
{
	// like Google earth
	QVector3D xv = QVector3D::crossProduct((m_cam_center - m_cam_eye), m_cam_up).normalized();
	QVector3D yv = QVector3D::crossProduct(xv, (m_cam_center - m_cam_eye)).normalized();
	QVector3D xyTrans = xv * tt.x() + yv * tt.y();
	double r = (m_cam_eye - m_cam_center).length() * (1.0 - 0.1 * tt.z()) / 
		(m_cam_eye + xyTrans - m_cam_center).length();
	m_cam_eye = (m_cam_eye + xyTrans - m_cam_center) * r + m_cam_center;
	m_cam_up = yv.normalized();

	m_cam.setToIdentity();
	m_cam.lookAt(m_cam_eye, m_cam_center, m_cam_up);
}

void TScene::camMoveCenter(const QVector3D& tt)
{
	QVector3D xv = QVector3D::crossProduct((m_cam_center - m_cam_eye), m_cam_up).normalized();
	QVector3D yv = QVector3D::crossProduct(xv, (m_cam_center - m_cam_eye)).normalized();
	QVector3D zv = (m_cam_center  - m_cam_eye).normalized();
	QVector3D trans = xv * tt.x() + yv * tt.y() + zv * tt.z();
	m_cam_eye += trans;
	m_cam_center += trans;

	m_cam.setToIdentity();
	m_cam.lookAt(m_cam_eye, m_cam_center, m_cam_up);
}


bool TScene::build( const QPolygonF& xyseeds )
{
	QList<TriMesh::Point> ponts = mapToZPlane(xyseeds, (m_cam_eye - m_cam_center).length());
	
	seeds.clear();
	seeds << ponts;
	
	
	return true;
}
