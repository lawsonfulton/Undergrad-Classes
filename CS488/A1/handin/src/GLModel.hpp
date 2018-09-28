#ifndef GLOBJECT_H
#define GLOBJECT_H

#include <QtWidgets>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#else
#include <QGLBuffer>
#endif


class GLModel {
public:
	GLModel();
	~GLModel();

	void initialize();
	void allocate(const void *data, int count);
	void bind();
	void drawFill();


private:
	int nVerts;
	
	#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    QOpenGLBuffer mVertexBufferObject;
    QOpenGLVertexArrayObject mVertexArrayObject;
	#else 
    QGLBuffer mVertexBufferObject;
	#endif

};

#endif
