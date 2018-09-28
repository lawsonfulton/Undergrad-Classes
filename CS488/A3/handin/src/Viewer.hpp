#ifndef CS488_VIEWER_HPP
#define CS488_VIEWER_HPP

//Name: Lawson Fulton
//UserID: ljfulton
//Student#: 20381453

#include <QGLWidget>
#include <QGLShaderProgram>
#include <QMatrix4x4>
#include <QtGlobal>


#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#else 
#include <QGLBuffer>
#endif

using namespace std;

class SceneNode;
class Material;

class Viewer : public QGLWidget {
    
    Q_OBJECT

public:
    Viewer(const QGLFormat& format, QWidget *parent = 0, SceneNode *root = 0);
    virtual ~Viewer();
    
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    // If you want to render a new frame, call do not call paintGL(),
    // instead, call update() to ensure that the view gets a paint 
    // event.
    void draw_sphere(const QMatrix4x4 &transformMatrix);
    void set_material(Material *material);
    void set_colour(const QColor& diffuse_col, const QColor& specular_col, const QColor& ambient_col, float phong_coef);

public Q_SLOTS:
    void toggle_circle();
    void toggle_z_buffer();
    void toggle_front_cull();
    void toggle_back_cull();
    void enable_postion_mode();
    void enable_joint_mode();

protected:

    // Events we implement

    // Called when GL is first initialized
    virtual void initializeGL();
    // Called when our window needs to be redrawn
    virtual void paintGL();
    // Called when the window is resized (formerly on_configure_event)
    virtual void resizeGL(int width, int height);
    // Called when a mouse button is pressed
    virtual void mousePressEvent ( QMouseEvent * event );
    // Called when a mouse button is released
    virtual void mouseReleaseEvent ( QMouseEvent * event );
    // Called when the mouse moves
    virtual void mouseMoveEvent ( QMouseEvent * event );
    
    // Draw a circle for the trackball, with OpenGL commands.
    // Assumes the context for the viewer is active.
    void draw_trackball_circle();
    QVector3D vCalcRotVec(QPointF newPoint, QPointF oldPoint, float fDiameter);
    QMatrix4x4 vAxisRotMatrix(QVector3D fVec);

private:

    QMatrix4x4 getCameraMatrix();
    QMatrix4x4 getViewMatrix();
    void translateWorld(float x, float y, float z);
    void rotateWorld(float x, float y, float z);
    void scaleWorld(float x, float y, float z);


#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    QOpenGLBuffer mVertexBufferObject;
    QOpenGLVertexArrayObject mVertexArrayObject;
#else 
    QGLBuffer mVertexBufferObject;
#endif
    
    int mProjMatrixLocation;
    int mViewMatrixLocation;
    int mTransRotMatrixLocation;
    int mScaleMatrixLocation;
    int mItScaleMatrixLocation;
    int mDiffuseColorLocation;
    int mSpecularColorLocation;
    int mAmbientColorLocation;
    int mPhongCoefLocation;
    int mLightLocLocation;
    int mCameraLocLocation;

    QVector3D mCameraPosition;
    QVector3D mLightPosition;

    QMatrix4x4 mPerspMatrix;
    QMatrix4x4 mTransRotMatrix;
    QMatrix4x4 mScaleMatrix;
    QMatrix4x4 mTransformMatrix;
    QMatrix4x4 vMatrix;
    QGLShaderProgram mProgram;

    int mSphereTris;
    int mCircleVerts;

    SceneNode *mSceneRoot;

    QTimer* mTimer;

    QPoint prevMousePos;
    int mouseDeltaX;

    bool circleOn;
    bool z_buffer;
    bool front_cull;
    bool back_cull;
    bool mPosMode;

    int mOrigWidth;
    int mOrigHeight;
};

#endif