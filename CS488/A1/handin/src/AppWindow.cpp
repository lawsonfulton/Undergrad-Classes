#include <QtWidgets>
#include <QGLFormat>
#include <iostream>
#include "AppWindow.hpp"

AppWindow::AppWindow() {
    setWindowTitle("488 Tetrominoes on the Wall");

    QGLFormat glFormat;
    glFormat.setVersion(3,3);
    glFormat.setProfile(QGLFormat::CoreProfile);
    glFormat.setSampleBuffers(true);

    QVBoxLayout *layout = new QVBoxLayout;

    m_game = new Game(10, 20);
    m_viewer = new Viewer(m_game, glFormat, this);
    layout->addWidget(m_viewer);
    setCentralWidget(new QWidget);
    centralWidget()->setLayout(layout);
    m_viewer->show();

    createActions();
    createMenu();

    
    connect(&m_game_timer, SIGNAL(timeout()), this, SLOT(tickGame()));
    setSlowSpeed();
    m_game_timer.start();
}

void AppWindow::keyPressEvent(QKeyEvent *event) {
    int key = event->key();

    switch(key) {
        case Qt::Key_Left:
            m_game->moveLeft();
            break;
        case Qt::Key_Right:
            m_game->moveRight();
            break;
        case Qt::Key_Up:
            m_game->rotateCCW();
            break;
        case Qt::Key_Down:
            m_game->rotateCW();
            break;
        case Qt::Key_Space:
            m_game->drop();
            break;
        case Qt::Key_Escape:
            QCoreApplication::instance()->quit();
            break;

        //Had to put all shortcuts in here too to make sure it works with Unity
		case Qt::Key_N:
		    newGame();
		    break;
		case Qt::Key_R:
		    m_viewer->resetView();
		    break;
		case Qt::Key_Q:
		    close();
		    break;
		case Qt::Key_W:
		    m_viewer->setDrawWireframe();
		    break;
		case Qt::Key_F:
		    m_viewer->setDrawFaceFill();
		    break;
		case Qt::Key_M:
		    m_viewer->setDrawMultiColour();
		    break;
	    case Qt::Key_1:
	    	setSlowSpeed();
	    	break;
	    case Qt::Key_2:
	    	setMediumSpeed();
	    	break;
	    case Qt::Key_3:
	    	setFastSpeed();

        default:
            QWidget::keyPressEvent(event);
            break;
    }
}

void AppWindow::createActions() {
    /* Application Actions */
    QAction* newGameAct = new QAction(tr("&New Game"), this);
    m_menu_actions.push_back(newGameAct);
    newGameAct->setShortcut(QKeySequence(Qt::Key_N));
    newGameAct->setStatusTip(tr("Starts a new game"));
    connect(newGameAct, SIGNAL(triggered()), this, SLOT(newGame()));

    QAction* resetAct = new QAction(tr("&Reset"), this);
    m_menu_actions.push_back(resetAct);
    resetAct->setShortcut(QKeySequence(Qt::Key_R));
    resetAct->setStatusTip(tr("Reset the view of the game to its default"));
    connect(resetAct, SIGNAL(triggered()), m_viewer, SLOT(resetView()));

    QAction* quitAct = new QAction(tr("&Quit"), this);
    m_menu_actions.push_back(quitAct);
    quitAct->setShortcut(QKeySequence(Qt::Key_Q));
    quitAct->setStatusTip(tr("Exits the file"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    /* Draw Mode Actions */
    //Wireframe
    QAction* wireFramAct = new QAction(tr("&Wire-frame"), this);
    m_menu_draw_mode_actions.push_back(wireFramAct);
    wireFramAct->setShortcut(QKeySequence(Qt::Key_W));
    wireFramAct->setStatusTip(tr("Switches to wire-frame draw mode."));
    connect(wireFramAct, SIGNAL(triggered()), m_viewer, SLOT(setDrawWireframe()));

    //Face Fill
    QAction* faceAct = new QAction(tr("&Face"), this);
    m_menu_draw_mode_actions.push_back(faceAct);
    faceAct->setShortcut(QKeySequence(Qt::Key_F));
    faceAct->setStatusTip(tr("Switches to face fill draw mode."));
    connect(faceAct, SIGNAL(triggered()), m_viewer, SLOT(setDrawFaceFill()));

    //Multicoloured
    QAction* multicolouredAct = new QAction(tr("&Multicoloured"), this);
    m_menu_draw_mode_actions.push_back(multicolouredAct);
    multicolouredAct->setShortcut(QKeySequence(Qt::Key_M));
    multicolouredAct->setStatusTip(tr("Switches to Multicoloured draw mode."));
    connect(multicolouredAct, SIGNAL(triggered()), m_viewer, SLOT(setDrawMultiColour()));

    //Group Them
    wireFramAct->setCheckable(true);
    faceAct->setCheckable(true);
    multicolouredAct->setCheckable(true);  

    QActionGroup* drawGroup = new QActionGroup(this);
    wireFramAct->setActionGroup(drawGroup);
    faceAct->setActionGroup(drawGroup);
    multicolouredAct->setActionGroup(drawGroup);
    faceAct->toggle();


    /* Speed Mode Actions */
    //Slow
    QAction* slowAct = new QAction(tr("&Slow"), this);
    m_menu_speed_actions.push_back(slowAct);
    slowAct->setShortcut(QKeySequence(Qt::Key_1));
    slowAct->setStatusTip(tr("Switches to Slow game speed."));
    connect(slowAct, SIGNAL(triggered()), this, SLOT(setSlowSpeed()));

    //Medium
    QAction* mediumAct = new QAction(tr("&Medium"), this);
    m_menu_speed_actions.push_back(mediumAct);
    mediumAct->setShortcut(QKeySequence(Qt::Key_2));
    mediumAct->setStatusTip(tr("Switches to medium game speed."));
    connect(mediumAct, SIGNAL(triggered()), this, SLOT(setMediumSpeed()));

    //Fast
    QAction* fastAct = new QAction(tr("&Fast"), this);
    m_menu_speed_actions.push_back(fastAct);
    fastAct->setShortcut(QKeySequence(Qt::Key_3));
    fastAct->setStatusTip(tr("Switches to fast game speed."));
    connect(fastAct, SIGNAL(triggered()), this, SLOT(setFastSpeed()));

    //Group Them
    slowAct->setCheckable(true);
    mediumAct->setCheckable(true);
    fastAct->setCheckable(true);  
    
    QActionGroup* speedGroup = new QActionGroup(this);
    slowAct->setActionGroup(speedGroup);
    mediumAct->setActionGroup(speedGroup);
    fastAct->setActionGroup(speedGroup);
    slowAct->toggle();

}

void AppWindow::createMenu() {
    m_menu_app = menuBar()->addMenu(tr("&Application"));

    for (auto& action : m_menu_actions) {
        m_menu_app->addAction(action);
    }

    m_menu_draw_mode = menuBar()->addMenu(tr("&Draw Mode"));

    for (auto& action : m_menu_draw_mode_actions) {
        m_menu_draw_mode->addAction(action);
    }

    m_menu_speed = menuBar()->addMenu(tr("&Speed"));

    for (auto& action : m_menu_speed_actions) {
        m_menu_speed->addAction(action);
    }
}

void AppWindow::tickGame() {
    //TODO game over
    m_game->tick();
    m_viewer->updateView();
}

void AppWindow::newGame() {
    m_game->reset();
    m_viewer->updateView();
}


void AppWindow::setSlowSpeed() {
    m_game_timer.setInterval(500);
}

void AppWindow::setMediumSpeed() {
    m_game_timer.setInterval(300);
}

void AppWindow::setFastSpeed() {
    m_game_timer.setInterval(100);
}


