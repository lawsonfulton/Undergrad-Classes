#include <QtWidgets>
#include <iostream>
#include "paintwindow.hpp"

PaintWindow::PaintWindow() {
    setWindowTitle("488 Paint");

    QVBoxLayout *layout = new QVBoxLayout;

    m_canvas = new PaintCanvas(this);
    layout->addWidget(m_canvas);

    QPushButton *quitButton = new QPushButton("Quit");
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));
    layout->addWidget(quitButton);

    setCentralWidget(new QWidget);
    centralWidget()->setLayout(layout);

    createMenu();
}

void PaintWindow::createMenu() {
    // Adding the drop down menu to the menubar
    m_menu_app = menuBar()->addMenu(tr("&Application"));
    m_menu_tools = menuBar()->addMenu(tr("&Tools"));
    m_menu_colour = menuBar()->addMenu(tr("&Colour"));
    m_menu_help = menuBar()->addMenu(tr("&Help"));

    // Adding the menu items for each drop down menu
    QAction* quitAct = new QAction(tr("&Quit"), this);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Exits the program"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
    m_menu_app->addAction(quitAct);

    QAction* clearAct = new QAction(tr("&Clear"), this);
    clearAct->setShortcut(QKeySequence(Qt::Key_C));
    clearAct->setStatusTip(tr("Clears the screen"));
    connect(clearAct, SIGNAL(triggered()), this, SLOT(clear()));
    m_menu_app->addAction(clearAct);

    //Tools
    QRadioButton *radioButton = new QRadioButton(m_menu_tools);
    radioButton ->setText("Line");

    QWidgetAction* drawLineAct = new QWidgetAction(this);
    drawLineAct->setShortcut(QKeySequence(Qt::Key_L));
    drawLineAct->setDefaultWidget(radioButton);
    drawLineAct->setStatusTip(tr("Draws a line"));
    connect(drawLineAct, SIGNAL(triggered()), this, SLOT(set_line()));

    QAction* drawOvalAct = new QAction(tr("&Oval"), this);
    drawOvalAct->setShortcut(QKeySequence(Qt::Key_O));
    drawOvalAct->setStatusTip(tr("Draws an Oval"));
    connect(drawOvalAct, SIGNAL(triggered()), this, SLOT(set_oval()));

    QAction* drawRectangleAct = new QAction(tr("&Rectangle"), this);
    drawRectangleAct->setShortcut(QKeySequence(Qt::Key_R));
    drawRectangleAct->setStatusTip(tr("Draws a rectangle"));
    connect(drawRectangleAct, SIGNAL(triggered()), this, SLOT(set_rect()));

    drawLineAct->setCheckable(true);
    drawOvalAct->setCheckable(true);
    drawRectangleAct->setCheckable(true);  

    QActionGroup* group = new QActionGroup(this);
    drawLineAct->setActionGroup(group);
    drawOvalAct->setActionGroup(group);
    drawRectangleAct->setActionGroup(group);
    drawLineAct->toggle();

    m_menu_tools->addAction(drawLineAct);
    m_menu_tools->addAction(drawOvalAct);
    m_menu_tools->addAction(drawRectangleAct);

    //Colours
    QAction* drawBlack = new QAction(tr("&Black"), this);
    drawBlack->setStatusTip(tr("Selects the colour Black"));
    connect(drawBlack, SIGNAL(triggered()), this, SLOT(set_black()));

    QAction* drawRed = new QAction(tr("&Red"), this);
    drawRed->setStatusTip(tr("Selects the colour Red"));
    connect(drawRed, SIGNAL(triggered()), this, SLOT(set_red()));

    QAction* drawGreen = new QAction(tr("&Green"), this);
    drawGreen->setStatusTip(tr("Selects the colour Green"));
    connect(drawGreen, SIGNAL(triggered()), this, SLOT(set_green()));

    QAction* drawBlue = new QAction(tr("&Blue"), this);
    drawBlue->setStatusTip(tr("Selects the colour Blue"));
    connect(drawBlue, SIGNAL(triggered()), this, SLOT(set_blue()));

    drawBlack->setCheckable(true);
    drawRed->setCheckable(true);
    drawGreen->setCheckable(true);  
    drawBlue->setCheckable(true);  

    QActionGroup* colourGroup = new QActionGroup(this);
    drawBlack->setActionGroup(colourGroup);
    drawRed->setActionGroup(colourGroup);
    drawGreen->setActionGroup(colourGroup);
    drawBlue->setActionGroup(colourGroup);
    drawBlack->toggle();

    m_menu_colour->addAction(drawBlack);
    m_menu_colour->addAction(drawRed);
    m_menu_colour->addAction(drawGreen);
    m_menu_colour->addAction(drawBlue);

    //Help
    QAction* helpLineAct = new QAction(tr("&Line Help"), this);
    helpLineAct->setStatusTip(tr("Help Instructions"));
    connect(helpLineAct, SIGNAL(triggered()), this, SLOT(help_line()));
    m_menu_help->addAction(helpLineAct);

    QAction* helpRectangleAct = new QAction(tr("&Rectangle Help"), this);
    helpRectangleAct->setStatusTip(tr("Help Instructions"));
    connect(helpRectangleAct, SIGNAL(triggered()), this, SLOT(help_rectangle()));
    m_menu_help->addAction(helpRectangleAct);

    QAction* helpOvalAct = new QAction(tr("&Oval Help"), this);
    helpOvalAct->setStatusTip(tr("Help Instructions"));
    connect(helpOvalAct, SIGNAL(triggered()), this, SLOT(help_oval()));
    m_menu_help->addAction(helpOvalAct);
}

void PaintWindow::help_line() {
    const char* message =
    "Drawing a Line\n"
    "\n"
    "To draw a line, press the left mouse button to mark the beginning of the line.  Drag the mouse to the end of the line and release the button.";

    QMessageBox msgBox;
    msgBox.setText(QString(message));
    msgBox.exec();
}

void PaintWindow::help_rectangle() {
    const char* message =
    "Drawing a Rectangle\n"
    "\n"
    "To draw a line, press the left mouse button to mark the first corner of the rectangle.  Drag the mouse to the the opposite corner of the rectangle and release the button.";

    QMessageBox msgBox;
    msgBox.setText(QString(message));
    msgBox.exec();
}

void PaintWindow::help_oval() {
    const char* message =
    "Drawing an Oval\n"
    "\n"
    "To draw an oval, press the left mouse button to mark the first corner of the bounding box.  Drag the mouse to the opposite corner of the bounding box and release the button. The oval will be drawn as the maximal oval that fits within the bounding box.";

    QMessageBox msgBox;
    msgBox.setText(QString(message));
    msgBox.exec();
}

void PaintWindow::set_line() {
    m_canvas->set_mode(PaintCanvas::DRAW_LINE);
}

void PaintWindow::set_oval() {
    m_canvas->set_mode(PaintCanvas::DRAW_OVAL);
}

void PaintWindow::set_rect() {
    m_canvas->set_mode(PaintCanvas::DRAW_RECTANGLE);
}

void PaintWindow::set_black() {
    m_canvas->set_colour(PaintCanvas::BLACK);
}

void PaintWindow::set_red() {
    m_canvas->set_colour(PaintCanvas::RED);
}

void PaintWindow::set_green() {
    m_canvas->set_colour(PaintCanvas::GREEN);
}

void PaintWindow::set_blue() {
    m_canvas->set_colour(PaintCanvas::BLUE);
}


void PaintWindow::clear() {
    m_canvas->clear();
}

