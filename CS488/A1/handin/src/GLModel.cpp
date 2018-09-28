#include <iostream>
#include "GLModel.hpp"


GLModel::GLModel()
#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
	: mVertexBufferObject(QOpenGLBuffer::VertexBuffer)
	, mVertexArrayObject(0) //TODO Make this VieweR?
#else
    : mVertexBufferObject(QGLBuffer::VertexBuffer)
#endif
    , nVerts(0)
{

}

GLModel::~GLModel() {

}

void GLModel::initialize() {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    mVertexArrayObject.create();
    mVertexArrayObject.bind();

    mVertexBufferObject.create();
    mVertexBufferObject.setUsagePattern(QOpenGLBuffer::StaticDraw);
#else
    /*
     * if qt version is less than 5.1, use the following commented code
     * instead of QOpenGLVertexVufferObject. Also use QGLBuffer instead of
     * QOpenGLBuffer.
     */
    uint vao;

    typedef void (APIENTRY *_glGenVertexArrays) (GLsizei, GLuint*);
    typedef void (APIENTRY *_glBindVertexArray) (GLuint);

    _glGenVertexArrays glGenVertexArrays;
    _glBindVertexArray glBindVertexArray;

    glGenVertexArrays = (_glGenVertexArrays) QGLWidget::context()->getProcAddress("glGenVertexArrays");
    glBindVertexArray = (_glBindVertexArray) QGLWidget::context()->getProcAddress("glBindVertexArray");

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    mVertexBufferObject.create();
    mVertexBufferObject.setUsagePattern(QGLBuffer::StaticDraw);
#endif

    if (!mVertexBufferObject.bind()) {
        std::cerr << "could not bind vertex buffer to the context." << std::endl;
        return;
    }
}

void GLModel::allocate(const void *data, int count) {
	mVertexBufferObject.allocate(data, count);
	nVerts = count;
}

void GLModel::bind() {
    mVertexBufferObject.bind();
}

void GLModel::drawFill() {
	glDrawArrays(GL_TRIANGLES, 0, nVerts);
}
