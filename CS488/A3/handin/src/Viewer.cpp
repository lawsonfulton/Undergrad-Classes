//Name: Lawson Fulton
//UserID: ljfulton
//Student#: 20381453

#include <QtWidgets>
#include <QtOpenGL>
#include "Viewer.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include "scene_lua.hpp"

#ifdef __APPLE__
#include <OpenGL/glu.h>
//#include <OpenGL/gl.h>
#else
//#include <GL/gl.h>
#include <GL/glu.h>
#endif

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

//TODO
//walk_gl
//render simple scene

Viewer::Viewer(const QGLFormat& format, QWidget *parent, SceneNode *root) 
    : QGLWidget(format, parent) 
#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    , mVertexBufferObject(QOpenGLBuffer::VertexBuffer)
    , mVertexArrayObject(this)
#else 
    , mVertexBufferObject(QGLBuffer::VertexBuffer)
#endif
    , mSceneRoot(root)
{

    mCameraPosition = QVector3D(0.0, 0.0, 6.0);
    mLightPosition = QVector3D(3.0, 3.0, 2.0);

    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(update()));
    mTimer->start(1000/30);

    circleOn = false;
    z_buffer = false;
    front_cull = false;
    back_cull = false;
    mPosMode = true;
}

Viewer::~Viewer() {
    // Nothing to do here right now.
}

QSize Viewer::minimumSizeHint() const {
    return QSize(50, 50);
}

QSize Viewer::sizeHint() const {
    return QSize(300, 300);
}

void Viewer::initializeGL() {
    QGLFormat glFormat = QGLWidget::format();
    if (!glFormat.sampleBuffers()) {
        std::cerr << "Could not enable sample buffers." << std::endl;
        return;
    }

    
    
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



    //Build the sphere
    vector<QVector3D> sphereVerts;

    int vRes = 80;
    int hRes = 80;
    float sphereRad = 1.0f;
    float halfPi = M_PI / 2.0f;

    sphereVerts.push_back(QVector3D(0.0, sphereRad, 0.0)); //top point

    for(int i = 1; i < vRes; i++) {
        float theta = halfPi + (M_PI/vRes) * i;
        float y = sphereRad * sin(theta);

        for(int j = 0; j < hRes; j++) {
            float alpha = (2 * M_PI)/hRes * j;
            float h = sphereRad - abs(y);
            float a =  sqrt(h * (2.0f * sphereRad - h));
            float x = a * cos(alpha);
            float z = a * sin(alpha);

            sphereVerts.push_back(QVector3D(x,y,z));
        }
    }

    sphereVerts.push_back(QVector3D(0.0, -sphereRad, 0.0)); //bottom point
    
    //Now build the tris
    vector<QVector3D> vertData;

    //cap
    for(int i = 0; i < hRes; i++) {
        vertData.push_back(sphereVerts[0]);
        vertData.push_back(sphereVerts[1 + i]);
        vertData.push_back(sphereVerts[1 + (i + 1) % hRes]);
    }

    //layers
    for(int i = 0; i < vRes - 2; i++) {
        for(int j = 0; j < hRes; j++) {
            int offset = 1 + (i * hRes);
            vertData.push_back(sphereVerts[offset + j]);
            vertData.push_back(sphereVerts[offset + hRes + (j + 1) % hRes]);
            vertData.push_back(sphereVerts[offset + (j + 1) % hRes]);

            vertData.push_back(sphereVerts[offset + j]);
            vertData.push_back(sphereVerts[offset + hRes + j]);
            vertData.push_back(sphereVerts[offset + hRes + (j + 1) % hRes]);
        }
    }

    //bottom
    int last = (int)sphereVerts.size() - 1;
    int start = last - hRes;
    for(int i = 0; i < hRes; i++) {
        vertData.push_back(sphereVerts[last]);
        vertData.push_back(sphereVerts[start + (i + 1)%hRes]);
        vertData.push_back(sphereVerts[start + i]);
    }

    mSphereTris = (int)vertData.size()/3;

    //circle
    double radius = width() < height() ? 
        (float)width() * 0.25 : (float)height() * 0.25;
        
    for(size_t i=0; i<40; ++i) {
        float x = radius * cos(i*2*M_PI/40);
        float y = radius * sin(i*2*M_PI/40);
        float z = 0.0;
        vertData.push_back(QVector3D(x,y,z));
    };

    mCircleVerts = (int)vertData.size();

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

    mVertexBufferObject.allocate(&vertData[0], (int)vertData.size() * sizeof(QVector3D));

    mProgram.bind();

    mProgram.enableAttributeArray("vert");
    mProgram.setAttributeBuffer("vert", GL_FLOAT, 0, 3);

    mProjMatrixLocation = mProgram.uniformLocation("projMatrix");
    mViewMatrixLocation = mProgram.uniformLocation("viewMatrix");
    mTransRotMatrixLocation = mProgram.uniformLocation("transRotMatrix");
    mScaleMatrixLocation = mProgram.uniformLocation("scaleMatrix");
    mItScaleMatrixLocation = mProgram.uniformLocation("itScaleMatrix");

    mDiffuseColorLocation = mProgram.uniformLocation("diffuse_color");
    mSpecularColorLocation = mProgram.uniformLocation("specular_color");
    mAmbientColorLocation = mProgram.uniformLocation("ambient_color");
    mPhongCoefLocation = mProgram.uniformLocation("phong_coef");

    mLightLocLocation = mProgram.uniformLocation("light_loc");
    mCameraLocLocation = mProgram.uniformLocation("camera_loc");

    mOrigWidth = width();
    mOrigHeight = height();
}

void Viewer::paintGL() {
    // Clear framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw stuff
    mSceneRoot->walk_gl(this, QMatrix4x4());

    if(circleOn) {
        draw_trackball_circle();
    }
}

void Viewer::resizeGL(int width, int height) {
    if (height == 0) {
        height = 1;
    }

    mPerspMatrix.setToIdentity();
    mPerspMatrix.perspective(30.0, (float) width / (float) height, 0.001, 1000);

    glViewport(0, 0, width, height);
}

void Viewer::mousePressEvent ( QMouseEvent * event ) {

    prevMousePos = event->pos();
    mouseDeltaX = 0;
}

void Viewer::mouseReleaseEvent ( QMouseEvent * event ) {
}

void Viewer::mouseMoveEvent ( QMouseEvent * event ) {
    Qt::MouseButtons buttons = event->buttons();
    QPoint newMousePos = event->pos();
    QPoint deltaPos = newMousePos - prevMousePos;

    float scalingFactor = 0.03f;


    if(mPosMode) {    
        if(buttons & Qt::LeftButton) {
            scalingFactor *= mOrigHeight / (float)height();
            QVector3D trans(deltaPos.x() * scalingFactor, -deltaPos.y()*scalingFactor, 0.0f);
            mSceneRoot->translate(trans);
        }
        if(buttons & Qt::MiddleButton) {
            QVector3D trans(0, 0, deltaPos.y() * scalingFactor);
            mSceneRoot->translate(trans);
        }
        if(buttons & Qt::RightButton) {
            QPointF newPoint(newMousePos);
            QPointF oldPoint(prevMousePos);
            QPointF offset(width()/2, height()/2);

            float diameter = 0.5 * min(width(), height());

            QVector3D rotVec =  vCalcRotVec(newPoint - offset, oldPoint - offset, diameter);
            rotVec *= QVector3D(1,-1,1);//flip vertical direction

            QMatrix4x4 rotMat = vAxisRotMatrix(rotVec);
            mSceneRoot->getNode("rotateNode")->apply_transform(rotMat);
        }
    } 
    else { //joint mode
        vector<SceneNode*> joints = mSceneRoot->getJointNodes();

        if(buttons & Qt::LeftButton) {
            for(int i = 0; i < (int)joints.size(); i++) {
                if(joints[i]->isSelected()) {
                    float angle = deltaPos.x() * scalingFactor * 2;
                    if(joints[i]->getName() == "upperNeckJoint") joints[i]->rotate('y', angle);
                }
            }
        }
        if(buttons & Qt::MiddleButton) {
            for(int i = 0; i < (int)joints.size(); i++) {
                if(joints[i]->isSelected()) {
                    float angle = deltaPos.y() * scalingFactor * 2;
                    joints[i]->rotate('x', angle);
                }
            }
        }
    }

    prevMousePos = event->pos(); 
}

QMatrix4x4 Viewer::getViewMatrix() {
    // Todo: Ask if we want to keep this.
    QMatrix4x4 vMatrix;
    QVector3D cameraUpDirection = QVector3D(0, 1, 0);
    vMatrix.lookAt(mCameraPosition, QVector3D(0, 0, 0), cameraUpDirection);

    return vMatrix;
}

QMatrix4x4 Viewer::getCameraMatrix() {
    return mPerspMatrix * getViewMatrix();
}

void Viewer::translateWorld(float x, float y, float z) {
    // Todo: Ask if we want to keep this.
    mTransformMatrix.translate(x, y, z);
}

void Viewer::rotateWorld(float x, float y, float z) {
    // Todo: Ask if we want to keep this.
    mTransformMatrix.rotate(x, y, z);
}

void Viewer::scaleWorld(float x, float y, float z) {
    // Todo: Ask if we want to keep this.
    mTransformMatrix.scale(x, y, z);
}

void Viewer::set_colour(const QColor& diffuse_col, const QColor& specular_col, const QColor& ambient_col, float phong_coef)
{
  mProgram.setUniformValue(mDiffuseColorLocation, diffuse_col);
  mProgram.setUniformValue(mSpecularColorLocation, specular_col);
  mProgram.setUniformValue(mAmbientColorLocation, ambient_col);
  mProgram.setUniformValue(mPhongCoefLocation, phong_coef);
}

void Viewer::set_material(Material *material) {
    PhongMaterial *mat = (PhongMaterial*)material;
    set_colour(mat->m_kd.q_col(), mat->m_ks.q_col(), QColor(50,50,50), mat->m_shininess);
}

void Viewer::draw_trackball_circle()
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    int current_width = width();
    int current_height = height();

    // Set up for orthogonal drawing to draw a circle on screen.
    // You'll want to make the rest of the function conditional on
    // whether or not we want to draw the circle this time around.

    set_colour(QColor(0,0,0), QColor(0,0,0), QColor(0,0,0), 0.0);

    // Set orthographic Matrix
    QMatrix4x4 orthoMatrix;
    QMatrix4x4 ident;

    orthoMatrix.ortho(0.0, (float)current_width, 
           0.0, (float)current_height, -0.1, 0.1);

    // Translate the view to the middle
    QMatrix4x4 transformMatrix;
    transformMatrix.translate(width()/2.0, height()/2.0, 0.0);
    float xFact = mOrigWidth / (float)width();
    float yFact = mOrigHeight / (float)height();
    float fact = min(1.0/xFact, 1.0/yFact);
    transformMatrix.scale(QVector3D(fact,fact,1));

    // Bind buffer object
    mVertexBufferObject.bind();
    mProgram.setUniformValue(mProjMatrixLocation, orthoMatrix * transformMatrix);

    mProgram.setUniformValue(mViewMatrixLocation, ident);
    mProgram.setUniformValue(mTransRotMatrixLocation, ident);
    mProgram.setUniformValue(mScaleMatrixLocation, ident);
    mProgram.setUniformValue(mItScaleMatrixLocation, ident);//scaleMatrix.inverted().transposed());

    // mProgram.setUniformValue(mLightLocLocation,QVector4D(mLightPosition, 1.0));
    // mProgram.setUniformValue(mCameraLocLocation,  QVector4D(mCameraPosition, 1.0));

    // Draw buffer
    glDrawArrays(GL_LINE_LOOP, mSphereTris * 3, 40);    
}

void Viewer::draw_sphere(const QMatrix4x4 &transformMatrix)
{
    glShadeModel(GL_SMOOTH);
    glClearColor( 0.4, 0.4, 0.4, 0.0 );

    if(z_buffer) glEnable(GL_DEPTH_TEST);
    if(!z_buffer) glDisable(GL_DEPTH_TEST);
    if(back_cull || front_cull) glEnable(GL_CULL_FACE);
    if(!back_cull && !front_cull) glDisable(GL_CULL_FACE);
    if(back_cull) glCullFace(GL_FRONT);
    if(front_cull) glCullFace(GL_BACK);
    if(front_cull && back_cull) glCullFace(GL_FRONT_AND_BACK);


    QMatrix4x4 normalMatrix = QMatrix4x4(transformMatrix.normalMatrix());
    QMatrix4x4 ident;
    // Bind buffer object
    mVertexBufferObject.bind();
    mProgram.setUniformValue(mProjMatrixLocation, getCameraMatrix());
    mProgram.setUniformValue(mViewMatrixLocation, getViewMatrix());
    mProgram.setUniformValue(mTransRotMatrixLocation, transformMatrix);
    mProgram.setUniformValue(mScaleMatrixLocation, ident);
    mProgram.setUniformValue(mItScaleMatrixLocation, normalMatrix);//scaleMatrix.inverted().transposed());

    mProgram.setUniformValue(mLightLocLocation,QVector4D(mLightPosition, 1.0));
    mProgram.setUniformValue(mCameraLocLocation,  QVector4D(mCameraPosition, 1.0));


    // Draw buffer
    glDrawArrays(GL_TRIANGLES, 0, mSphereTris * 3);    
}



/*******************************************************
 * 
 * void vCalcRotVec(float fNewX, float fNewY, 
 *                  float fOldX, float fOldY,
 *                  float fDiameter,
 *                  float *fVecX, float *fVecY, float *fVecZ);
 *
 *    Calculates a rotation vector based on mouse motion over
 *    a virtual trackball.
 *
 *    The fNew and fOld mouse positions
 *    should be in 'trackball' space. That is, they have been
 *    transformed into a coordinate system centered at the middle
 *    of the trackball. fNew, fOld, and fDiameter must all be specified
 *    in the same units (pixels for example).
 *
 * Parameters: fNewX, fNewY - New mouse position in trackball space.
 *                            This is the second point along direction
 *                            of rotation.
 *             fOldX, fOldY - Old mouse position in trackball space.
 *                            This is the first point along direction
 *                            of rotation.
 *             fDiameter - Diameter of the trackball. This should
 *                         be specified in the same units as fNew and fOld.
 *                         (ie, usually pixels if fNew and fOld are transformed
 *                         mouse positions)
 *             fVec - The output rotation vector. The length of the vector
 *                    is proportional to the angle of rotation.
 *
 *******************************************************/
QVector3D Viewer::vCalcRotVec(QPointF newPoint, QPointF oldPoint, float fDiameter) {
   long  nXOrigin, nYOrigin;
   float fNewVecX, fNewVecY, fNewVecZ,        /* Vector corresponding to new mouse location */
         fOldVecX, fOldVecY, fOldVecZ,        /* Vector corresponding to old mouse location */
         fLength;

   /* Vector pointing from center of virtual trackball to
    * new mouse position
    */
   fNewVecX    = newPoint.x() * 2.0 / fDiameter;
   fNewVecY    = newPoint.y() * 2.0 / fDiameter;
   fNewVecZ    = (1.0 - fNewVecX * fNewVecX - fNewVecY * fNewVecY);

   /* If the Z component is less than 0, the mouse point
    * falls outside of the trackball which is interpreted
    * as rotation about the Z axis.
    */
   if (fNewVecZ < 0.0) {
      fLength = sqrt(1.0 - fNewVecZ);
      fNewVecZ  = 0.0;
      fNewVecX /= fLength;
      fNewVecY /= fLength;
   } else {
      fNewVecZ = sqrt(fNewVecZ);
   }

   /* Vector pointing from center of virtual trackball to
    * old mouse position
    */
   fOldVecX    = oldPoint.x() * 2.0 / fDiameter;
   fOldVecY    = oldPoint.y() * 2.0 / fDiameter;
   fOldVecZ    = (1.0 - fOldVecX * fOldVecX - fOldVecY * fOldVecY);
 
   /* If the Z component is less than 0, the mouse point
    * falls outside of the trackball which is interpreted
    * as rotation about the Z axis.
    */
   if (fOldVecZ < 0.0) {
      fLength = sqrt(1.0 - fOldVecZ);
      fOldVecZ  = 0.0;
      fOldVecX /= fLength;
      fOldVecY /= fLength;
   } else {
      fOldVecZ = sqrt(fOldVecZ);
   }

   /* Generate rotation vector by calculating cross product:
    * 
    * fOldVec x fNewVec.
    * 
    * The rotation vector is the axis of rotation
    * and is non-unit length since the length of a crossproduct
    * is related to the angle between fOldVec and fNewVec which we need
    * in order to perform the rotation.
    */
   float fVecX = fOldVecY * fNewVecZ - fNewVecY * fOldVecZ;
   float fVecY = fOldVecZ * fNewVecX - fNewVecZ * fOldVecX;
   float fVecZ = fOldVecX * fNewVecY - fNewVecX * fOldVecY;

   return QVector3D(fVecX, fVecY, fVecZ);
}

/*******************************************************
 * void vAxisRotMatrix(float fVecX, float fVecY, float fVecZ, Matrix mNewMat)
 *    
 *    Calculate the rotation matrix for rotation about an arbitrary axis.
 *    
 *    The axis of rotation is specified by (fVecX,fVecY,fVecZ). The length
 *    of the vector is the amount to rotate by.
 *
 * Parameters: fVecX,fVecY,fVecZ - Axis of rotation
 *             mNewMat - Output matrix of rotation in column-major format
 *                       (ie, translation components are in column 3 on rows
 *                       0,1, and 2).
 *
 *******************************************************/
QMatrix4x4 Viewer::vAxisRotMatrix(QVector3D fVec) {
    float fRadians, fInvLength, fNewVecX, fNewVecY, fNewVecZ;
    QMatrix4x4 mNewMat;
    /* Find the length of the vector which is the angle of rotation
     * (in radians)
     */
    fRadians = sqrt(fVec.x() * fVec.x() + fVec.y() * fVec.y() + fVec.z() * fVec.z());

    /* If the vector has zero length - return the identity matrix */
    if (fRadians > -0.000001 && fRadians < 0.000001) {
        return mNewMat;
    }

    /* Normalize the rotation vector now in preparation for making
     * rotation matrix. 
     */
    fInvLength = 1 / fRadians;
    fNewVecX   = fVec.x() * fInvLength;
    fNewVecY   = fVec.y() * fInvLength;
    fNewVecZ   = fVec.z() * fInvLength;

    /* Create the arbitrary axis rotation matrix */
    double dSinAlpha = sin(fRadians);
    double dCosAlpha = cos(fRadians);
    double dT = 1 - dCosAlpha;

    mNewMat(0,0) = dCosAlpha + fNewVecX*fNewVecX*dT;
    mNewMat(0,1) = fNewVecX*fNewVecY*dT + fNewVecZ*dSinAlpha;
    mNewMat(0,2) = fNewVecX*fNewVecZ*dT - fNewVecY*dSinAlpha;
    mNewMat(0,3) = 0;

    mNewMat(1,0) = fNewVecX*fNewVecY*dT - dSinAlpha*fNewVecZ;
    mNewMat(1,1) = dCosAlpha + fNewVecY*fNewVecY*dT;
    mNewMat(1,2) = fNewVecY*fNewVecZ*dT + dSinAlpha*fNewVecX;
    mNewMat(1,3) = 0;

    mNewMat(2,0) = fNewVecZ*fNewVecX*dT + dSinAlpha*fNewVecY;
    mNewMat(2,1) = fNewVecZ*fNewVecY*dT - dSinAlpha*fNewVecX;
    mNewMat(2,2) = dCosAlpha + fNewVecZ*fNewVecZ*dT;
    mNewMat(2,3) = 0;

    mNewMat(3,0) = 0;
    mNewMat(3,1) = 0;
    mNewMat(3,2) = 0;
    mNewMat(3,3) = 1;

    return mNewMat;
}

void Viewer::toggle_circle() {
    circleOn = !circleOn;
}

void Viewer::toggle_z_buffer() {
    z_buffer = !z_buffer;
}

void Viewer::toggle_front_cull() {
    front_cull = !front_cull;
}

void Viewer::toggle_back_cull() {
    back_cull = !back_cull;
}

void Viewer::enable_postion_mode() {
    mPosMode = true;
    
}

void Viewer::enable_joint_mode() {
    mPosMode = false;

}
