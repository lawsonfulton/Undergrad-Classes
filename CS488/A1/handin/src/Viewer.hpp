#ifndef CS488_VIEWER_HPP
#define CS488_VIEWER_HPP

#include <QGLWidget>
#include <QGLShaderProgram>
#include <QMatrix4x4>
#include <QtGlobal>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <vector>

#include "game.hpp"

class Viewer : public QGLWidget {
    
    Q_OBJECT

public:
    enum DRAW_MODE
    {
        WIRE_FRAME,
        FACE_FILL,
        MULTI_COLOUR
    };

    Viewer(Game *game, const QGLFormat& format, QWidget *parent = 0);
    virtual ~Viewer();
    
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    // force a redraw
    void updateView();
  
protected:

    //Builds the matrices to move bricks for well outline
    void buildWellMatrices();

    QVector3D boardToWorldCoords(const int r, const int c);

    // Called when GL is first initialized
    virtual void initializeGL();
    // Called when our window needs to be redrawn
    virtual void paintGL();
    //Draws the blocks comprsing the border of the game
    void drawBorderBlocks();
    //Draws the blocks active in the game
    void drawGameBlocks();
    // Called when the window is resized (formerly on_configure_event)
    virtual void resizeGL(int width, int height);
    // Called when a mouse button is pressed
    virtual void mousePressEvent ( QMouseEvent * event );
    // Called when a mouse button is released
    virtual void mouseReleaseEvent ( QMouseEvent * event );
    // Called when the mouse moves
    virtual void mouseMoveEvent ( QMouseEvent * event );

public Q_SLOTS:
    void setDrawWireframe();
    void setDrawFaceFill();
    void setDrawMultiColour();
    void resetView();

private:

    QMatrix4x4 getCameraMatrix();
    void translateWorld(float x, float y, float z);
    void rotateWorld(float angle, float x, float y, float z);
    void scaleWorld(float x, float y, float z);
    void doPersistentRotation();
    void drawBlock();

    Game *mGame;

    QOpenGLBuffer mCubeDataBuffer;
    QOpenGLBuffer mCubeIndexBuffer;
    QOpenGLVertexArrayObject mVertexArrayObject;
    int mVertDataSize;

    QColor mBlockColours[7];

    int mVertexLocation;
    int mMvpMatrixLocation;
    int mSolidColorLocation;
    int mUseSolidColorLocation;

    QMatrix4x4 mPerspMatrix;
    QMatrix4x4 mTransformMatrix;
    std::vector<QMatrix4x4> mWellMatrices;
    
    QTimer* mTimer;
    
    QGLShaderProgram mProgram;
    DRAW_MODE mCurrentDrawMode;

    QPoint mPrevMousePos;
    int mMouseDeltaX;
    bool mRotationCoasting;
    bool mWasZooming;
    float mRotationAngle;
    float mTotalScale;
    QVector3D mRotationAxis;
    
};

#endif
