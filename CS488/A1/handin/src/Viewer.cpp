#include <QtWidgets>
#include <QtOpenGL>
#include <QVector3D>
#include "Viewer.hpp"
#include <iostream>
#include <math.h>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

Viewer::Viewer(Game *game, const QGLFormat& format, QWidget *parent)
    : QGLWidget(format, parent)
    , mGame(game)
    , mCubeDataBuffer(QOpenGLBuffer::VertexBuffer)
    , mCubeIndexBuffer(QOpenGLBuffer::IndexBuffer)
    , mVertexArrayObject(this)
    , mCurrentDrawMode(Viewer::FACE_FILL)
    , mRotationCoasting(false)
    , mWasZooming(false)
    , mTotalScale(1.0f)
    , mRotationAxis(QVector3D(1.0f, 0.0f, 0.0f))
{
    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(update()));
    mTimer->start(1000/30);

    resetView();

    mBlockColours[0] = QColor(200, 67, 0, 255);
    mBlockColours[1] = QColor(0, 200, 23, 255);
    mBlockColours[2] = QColor(70, 40, 200, 255);
    mBlockColours[3] = QColor(200, 180, 45, 255);
    mBlockColours[4] = QColor(0, 230, 100, 255);
    mBlockColours[5] = QColor(200, 0, 56, 255);
    mBlockColours[6] = QColor(34, 125, 78, 255);

    buildWellMatrices();
}

void Viewer::buildWellMatrices() {
    //bottom
    for(int col = -1; col < 11; col++) {
        QMatrix4x4 mat;
        mat.translate(boardToWorldCoords(-1, col));
        mWellMatrices.push_back(mat);
    }

    //sides
    for(int row = 0; row < 21; row++) {
        QMatrix4x4 mat;
        mat.translate(boardToWorldCoords(row, -1));
        mWellMatrices.push_back(mat);
        mat.setToIdentity();
        mat.translate(boardToWorldCoords(row, 10));
        mWellMatrices.push_back(mat);
    }
}

QVector3D Viewer::boardToWorldCoords(const int r, const int c) {
    float x = -4.5f + (float)c;
    float y = -8.5f + (float)r;

    return QVector3D(x, y, 0.0f);
}

Viewer::~Viewer() {

}

QSize Viewer::minimumSizeHint() const {
    return QSize(50, 50);
}

QSize Viewer::sizeHint() const {
    return QSize(300, 600);
}

void Viewer::updateView() {
    update();
}

void Viewer::initializeGL() {
    QGLFormat glFormat = QGLWidget::format();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    if (!glFormat.sampleBuffers()) {
        std::cerr << "Could not enable sample buffers." << std::endl;
        return;
    }

    glClearColor(0.2, 0.3, 0.7, 0.0);

    //Check src directory if shader can't be found
    if (!mProgram.addShaderFromSourceFile(QGLShader::Vertex, "shader.vert")) {
    	if (!mProgram.addShaderFromSourceFile(QGLShader::Vertex, "src/shader.vert")) {
	        std::cerr << "Cannot load vertex shader." << std::endl;
	        return;
       	}
    }

    if (!mProgram.addShaderFromSourceFile(QGLShader::Fragment, "shader.frag")) {
    	if (!mProgram.addShaderFromSourceFile(QGLShader::Fragment, "src/shader.frag")) {
	        std::cerr << "Cannot load fragment shader." << std::endl;
	        return;
	    }
    }

    if ( !mProgram.link() ) {
        std::cerr << "Cannot link shaders." << std::endl;
        return;
    }

    float sideLength = 0.5f;
    //Original cube data modified from http://qt-project.org/doc/qt-4.8/opengl-cube.html
    QVector3D cubeData[] = {
         // Vertex data for face 0
         QVector3D(-sideLength, -sideLength,  sideLength),  // v0
         QVector3D( sideLength, -sideLength,  sideLength), // v1
         QVector3D(-sideLength,  sideLength,  sideLength),  // v2
         QVector3D( sideLength,  sideLength,  sideLength), // v3

         // Vertex data for face 1
         QVector3D( sideLength, -sideLength,  sideLength), // v4
         QVector3D( sideLength, -sideLength, -sideLength), // v5
         QVector3D( sideLength,  sideLength,  sideLength),  // v6
         QVector3D( sideLength,  sideLength, -sideLength), // v7

         // Vertex data for face 2
         QVector3D( sideLength, -sideLength, -sideLength), // v8
         QVector3D(-sideLength, -sideLength, -sideLength),  // v9
         QVector3D( sideLength,  sideLength, -sideLength), // v10
         QVector3D(-sideLength,  sideLength, -sideLength),  // v11

         // Vertex data for face 3
         QVector3D(-sideLength, -sideLength, -sideLength), // v12
         QVector3D(-sideLength, -sideLength,  sideLength),  // v13
         QVector3D(-sideLength,  sideLength, -sideLength), // v14
         QVector3D(-sideLength,  sideLength,  sideLength),  // v15

         // Vertex data for face 4
         QVector3D(-sideLength, -sideLength, -sideLength), // v16
         QVector3D( sideLength, -sideLength, -sideLength), // v17
         QVector3D(-sideLength, -sideLength,  sideLength), // v18
         QVector3D( sideLength, -sideLength,  sideLength), // v19

         // Vertex data for face 5
         QVector3D(-sideLength,  sideLength,  sideLength), // v20
         QVector3D( sideLength,  sideLength,  sideLength), // v21
         QVector3D(-sideLength,  sideLength, -sideLength), // v22
         QVector3D( sideLength,  sideLength, -sideLength),  // v23

         //Colours
         QVector3D( 0.3f,  0.4f,  0.8f),
         QVector3D( 0.3f,  0.4f,  0.8f),
         QVector3D( 0.3f,  0.4f,  0.8f),
         QVector3D( 0.3f,  0.4f,  0.8f),

         QVector3D( 0.0f,  1.0f,  1.0f),
         QVector3D( 0.0f,  1.0f,  1.0f),
         QVector3D( 0.0f,  1.0f,  1.0f),
         QVector3D( 0.0f,  1.0f,  1.0f),

         QVector3D( 0.0f,  0.5f,  1.0f),
         QVector3D( 0.0f,  0.5f,  1.0f),
         QVector3D( 0.0f,  0.5f,  1.0f),
         QVector3D( 0.0f,  0.5f,  1.0f),

         QVector3D( 0.0f,  1.0f,  0.0f),
         QVector3D( 0.0f,  1.0f,  0.0f),
         QVector3D( 0.0f,  1.0f,  0.0f),
         QVector3D( 0.0f,  1.0f,  0.0f),

         QVector3D( 1.0f,  1.0f,  0.0f),
         QVector3D( 1.0f,  1.0f,  0.0f),
         QVector3D( 1.0f,  1.0f,  0.0f),
         QVector3D( 1.0f,  1.0f,  0.0f),

         QVector3D( 1.0f,  0.0f,  1.0f),
         QVector3D( 1.0f,  0.0f,  1.0f),
         QVector3D( 1.0f,  0.0f,  1.0f),
         QVector3D( 1.0f,  0.0f,  1.0f),
     };

     GLushort indices[] = {
          0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
          4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
          8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
         12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
         16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
         20, 20, 21, 22, 23      // Face 5 - triangle strip (v20, v21, v22, v23)
     };

    mVertexArrayObject.create();
    mVertexArrayObject.bind();

    mCubeDataBuffer.create();
    mCubeDataBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
	mCubeDataBuffer.bind();
	mCubeDataBuffer.allocate(cubeData, 48 * sizeof(QVector3D));

	mCubeIndexBuffer.create();
    mCubeIndexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
	mCubeIndexBuffer.bind();
	mCubeIndexBuffer.allocate(indices, 34 * sizeof(GLushort));

    mProgram.bind();
    mProgram.enableAttributeArray("vert");
    mProgram.setAttributeBuffer("vert", GL_FLOAT, 0, 3);

    mProgram.enableAttributeArray("colour");
    mProgram.setAttributeBuffer("colour", GL_FLOAT, 24 * sizeof(QVector3D), 3);

    mMvpMatrixLocation = mProgram.uniformLocation("mvpMatrix");
    mSolidColorLocation = mProgram.uniformLocation("solidColor");
    mUseSolidColorLocation = mProgram.uniformLocation("useSolidColor");

    mProgram.setUniformValue(mUseSolidColorLocation, true);
}

void Viewer::paintGL() {
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    doPersistentRotation();

    drawBorderBlocks();
    drawGameBlocks();
}

void Viewer::drawBlock() {
	glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);
}

void Viewer::drawBorderBlocks() {
    QColor color(200, 200, 200, 255);

    mProgram.setUniformValue(mSolidColorLocation, color);
    for (int i = 0; i < (int)mWellMatrices.size(); i++) {
        mProgram.setUniformValue(mMvpMatrixLocation, getCameraMatrix() * mWellMatrices[i]);
        drawBlock();
    }
}

void Viewer::drawGameBlocks() {
    int width = mGame->getWidth();
    int height = mGame->getHeight();

    for(int r = 0; r < height + 4; r++) {
        for(int c = 0; c < width; c++) {
            int status = mGame->get(r,c);

            if(status != -1) {
                QMatrix4x4 mat;
                mat.translate(boardToWorldCoords(r, c));

                mProgram.setUniformValue(mMvpMatrixLocation, getCameraMatrix() * mat);
                mProgram.setUniformValue(mSolidColorLocation, mBlockColours[status]);
                drawBlock();
            }
        }
    }
}

void Viewer::resizeGL(int width, int height) {
    if (height == 0) {
        height = 1;
    }

    mPerspMatrix.setToIdentity();
    mPerspMatrix.perspective(60.0, (float) width / (float) height, 0.001, 1000);

    glViewport(0, 0, width, height);
}

void Viewer::mousePressEvent ( QMouseEvent * event ) {
    mPrevMousePos = event->pos();
    mRotationCoasting = false;
    mMouseDeltaX = 0;
}

void Viewer::mouseReleaseEvent ( QMouseEvent * event ) {
    //Check that the mouse was moving and that we weren't zooming
    if(abs(mMouseDeltaX) > 1 && !mWasZooming) {
        mRotationCoasting = true;
    }
}

void Viewer::mouseMoveEvent ( QMouseEvent * event ) {
    float scalingFactor = 0.5f;
    Qt::MouseButtons buttons = event->buttons();

    QPoint newMousePos = event->pos();
    QPoint deltaPos = newMousePos - mPrevMousePos;
    mMouseDeltaX = deltaPos.x();

    //Scaling
    if(QApplication::keyboardModifiers() == Qt::ShiftModifier){  
    	float amount = 1.0f + mMouseDeltaX * 0.001;

    	//bounds
    	if((amount < 1.0f && mTotalScale > 0.33f) || (amount > 1.0f && mTotalScale < 2.5f)) {
	        mTotalScale *= amount;
	        scaleWorld(amount, amount, amount);

	        mWasZooming = true;
    	}  
    } else {
        //Rotation
        mRotationAngle = mMouseDeltaX * scalingFactor;

        mRotationAxis =  QVector3D(0.0f, 0.0f, 0.0f);
        if(buttons & Qt::LeftButton) {    
            mRotationAxis += QVector3D(1.0f, 0.0f, 0.0f);
        }
        if(buttons & Qt::RightButton) {    
            mRotationAxis += QVector3D(0.0f, 0.0f, -1.0f); 
        }
        if(buttons & Qt::MidButton) {    
            mRotationAxis += QVector3D(0.0f, 1.0f, 0.0f);  
        }

        rotateWorld(mRotationAngle, mRotationAxis.x(), mRotationAxis.y(), mRotationAxis.z());  
        mWasZooming = false;
    }
    
    mPrevMousePos = event->pos();
}

QMatrix4x4 Viewer::getCameraMatrix() {
    QMatrix4x4 vMatrix;

    QMatrix4x4 cameraTransformation;
    QVector3D cameraPosition = cameraTransformation * QVector3D(0, 0, 20.0);
    QVector3D cameraUpDirection = cameraTransformation * QVector3D(0, 1, 0);

    vMatrix.lookAt(cameraPosition, QVector3D(0, 0, 0), cameraUpDirection);

    return mPerspMatrix * vMatrix * mTransformMatrix;
}

void Viewer::translateWorld(float x, float y, float z) {
    mTransformMatrix.translate(x, y, z);
}

void Viewer::rotateWorld(float angle, float x, float y, float z) {
    mTransformMatrix.rotate(angle, x, y, z);
}

void Viewer::scaleWorld(float x, float y, float z) {
    mTransformMatrix.scale(x, y, z);
}

void Viewer::doPersistentRotation() {
    if(mRotationCoasting) {
        rotateWorld(mRotationAngle, mRotationAxis.x(), mRotationAxis.y(), mRotationAxis.z());  
    }
}

void Viewer::setDrawWireframe() {
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
}

void Viewer::setDrawFaceFill() {
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    mProgram.setUniformValue(mUseSolidColorLocation, true);
}

void Viewer::setDrawMultiColour() {
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    mProgram.setUniformValue(mUseSolidColorLocation, false);
}

void Viewer::resetView() {
    float initial_scale = 1.0f;
    mTransformMatrix.setToIdentity();
    scaleWorld(initial_scale, initial_scale, initial_scale);
    mRotationCoasting = false;
    mTotalScale = 1.0f;
    update();
}