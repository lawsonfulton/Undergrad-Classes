#ifndef PAINTCANVAS_HPP
#define PAINTCANVAS_HPP

#include <list>
#include <QWidget>
#include <QPainter>
#include "algebra.hpp"

// An abstract base class that defines shapes we'll store and
// draw.  Each shape is currently defined from two corners m_from 
// and m_to, and a method that asks the shape to draw itself.
// 
class PaintShape
{
public:
    virtual ~PaintShape() {}

    virtual void draw( QPainter& painter ) = 0;

protected:
    PaintShape( const Point2D& from, const Point2D& to, const QColor& colour)
        : m_from( from )
        , m_to( to )
        , m_colour( colour )
    {}

protected:
    Point2D     m_from;
    Point2D     m_to;
    QColor      m_colour;
};

class PaintLine
    : public PaintShape
{
public:
    PaintLine( const Point2D& from, const Point2D& to, const QColor& colour )
        : PaintShape( from, to, colour )
    {}

    virtual void draw( QPainter& painter );
};

class PaintRect
    : public PaintShape
{
public:
    PaintRect( const Point2D& from, const Point2D& to, const QColor& colour )
        : PaintShape( from, to, colour )
    {}

    virtual void draw( QPainter& painter );
};

class PaintOval
    : public PaintShape
{
public:
    PaintOval( const Point2D& from, const Point2D& to, const QColor& colour )
        : PaintShape( from, to, colour )
    {}

    virtual void draw( QPainter& painter );
};

class PaintCanvas : public QWidget {

    Q_OBJECT

public:
    enum Mode {
    DRAW_LINE,
    DRAW_OVAL,
    DRAW_RECTANGLE
    };

    enum Colour {
    BLACK,
    RED,
    GREEN,
    BLUE
    };

    PaintCanvas(QWidget *parent = 0);
    virtual ~PaintCanvas();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    void set_mode(Mode mode) { m_mode = mode; }
    void set_colour(Colour colour);

    void clear();

protected:

    // Events we implement

    // Called when our window needs to be redrawn
    virtual void paintEvent( QPaintEvent * event );
    // Called when a mouse button is pressed
    virtual void mousePressEvent ( QMouseEvent * event );
    // Called when a mouse button is released
    virtual void mouseReleaseEvent ( QMouseEvent * event );


private:
    Mode m_mode; // what to do when a user clicks

    QColor m_colour;

    Point2D m_start_pos; // position where the user last clicked

    std::list<PaintShape*> m_shapes;
};

#endif
