//Name: Lawson Fulton
//UserID: ljfulton
//Student#: 20381453

#include <QtWidgets>
#include <iostream>
#include "AppWindow.hpp"
#include "scene_lua.hpp"

AppWindow::AppWindow(SceneNode *root, std::string filename) {
    setWindowTitle("488 Assignment Two");

    mRoot = root;
    mFilename = filename;

    QGLFormat glFormat;
    glFormat.setVersion(3,3);
    glFormat.setProfile(QGLFormat::CoreProfile);
    glFormat.setSampleBuffers(true);

    QVBoxLayout *layout = new QVBoxLayout;
    m_viewer = new Viewer(glFormat, this, mRoot);
    layout->addWidget(m_viewer);
    setCentralWidget(new QWidget);
    centralWidget()->setLayout(layout);

    createActions();
    createMenu();

}

void AppWindow::createActions() {
    //Application Menu Actions
    QAction* resetPositionAct = new QAction(tr("&Reset Position"), this);
    m_menu_app_actions.push_back(resetPositionAct);
    resetPositionAct->setShortcut(QKeySequence(Qt::Key_I));
    resetPositionAct->setStatusTip(tr("resetPosition the view of the game to its default"));
    connect(resetPositionAct, SIGNAL(triggered()), this, SLOT(reset_position()));

    QAction* resetOrientationAct = new QAction(tr("&Reset Orientation"), this);
    m_menu_app_actions.push_back(resetOrientationAct);
    resetOrientationAct->setShortcut(QKeySequence(Qt::Key_O));
    resetOrientationAct->setStatusTip(tr("resetOrientation the view of the game to its default"));
    connect(resetOrientationAct, SIGNAL(triggered()), this, SLOT(reset_orientation()));

    QAction* resetAllAct = new QAction(tr("&Reset All"), this);
    m_menu_app_actions.push_back(resetAllAct);
    resetAllAct->setShortcut(QKeySequence(Qt::Key_A));
    resetAllAct->setStatusTip(tr("resetAll the view of the game to its default"));
    connect(resetAllAct, SIGNAL(triggered()), this, SLOT(reset_all()));

    QAction* quitAct = new QAction(tr("&Quit"), this);
    m_menu_app_actions.push_back(quitAct);
    quitAct->setShortcut(QKeySequence(Qt::Key_Q));
    quitAct->setStatusTip(tr("Exits the file"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    //Mode Menu Actions
    QAction* posModeAct = new QAction(tr("&Position/Orientation"), this);
    m_menu_mode_actions.push_back(posModeAct);
    posModeAct->setShortcut(QKeySequence(Qt::Key_P));
    posModeAct->setStatusTip(tr("toggle posMode"));
    connect(posModeAct, SIGNAL(triggered()), m_viewer, SLOT(enable_postion_mode()));

    QAction* jointModeAct = new QAction(tr("&Joints"), this);
    m_menu_mode_actions.push_back(jointModeAct);
    jointModeAct->setShortcut(QKeySequence(Qt::Key_J));
    jointModeAct->setStatusTip(tr("toggle jointMode"));
    connect(jointModeAct, SIGNAL(triggered()), m_viewer, SLOT(enable_joint_mode()));

    //Group Them
    posModeAct->setCheckable(true);
    jointModeAct->setCheckable(true);

    QActionGroup* drawGroup = new QActionGroup(this);
    posModeAct->setActionGroup(drawGroup);
    jointModeAct->setActionGroup(drawGroup);
    posModeAct->toggle();

    //Option Menu Actions
    QAction* circleAct = new QAction(tr("&Circle"), this);
    m_menu_options_actions.push_back(circleAct);
    circleAct->setShortcut(QKeySequence(Qt::Key_C));
    circleAct->setStatusTip(tr("toggle circle"));
    connect(circleAct, SIGNAL(triggered()), m_viewer, SLOT(toggle_circle()));

    QAction* zBufferAct = new QAction(tr("&zBuffer"), this);
    m_menu_options_actions.push_back(zBufferAct);
    zBufferAct->setShortcut(QKeySequence(Qt::Key_Z));
    zBufferAct->setStatusTip(tr("toggle zBuffer"));
    connect(zBufferAct, SIGNAL(triggered()), m_viewer, SLOT(toggle_z_buffer()));

    QAction* backCullAct = new QAction(tr("&Backface Cull"), this);
    m_menu_options_actions.push_back(backCullAct);
    backCullAct->setShortcut(QKeySequence(Qt::Key_B));
    backCullAct->setStatusTip(tr("toggle backCull"));
    connect(backCullAct, SIGNAL(triggered()), m_viewer, SLOT(toggle_back_cull()));

    QAction* frontCullAct = new QAction(tr("&Frontface Cull"), this);
    m_menu_options_actions.push_back(frontCullAct);
    frontCullAct->setShortcut(QKeySequence(Qt::Key_F));
    frontCullAct->setStatusTip(tr("toggle frontCull"));
    connect(frontCullAct, SIGNAL(triggered()), m_viewer, SLOT(toggle_front_cull()));

    //Picking Menu Actions
    std::vector<SceneNode*> joints = mRoot->getJointNodes();
    for(int i = 0; i < (int)joints.size(); i++) {
        QAction* selectAct = new QAction(tr(joints[i]->getName().c_str()), this);
        m_menu_picking_actions.push_back(selectAct);
        selectAct->setStatusTip(tr("toggle select"));
        connect(selectAct, SIGNAL(triggered()), this, SLOT(select_node()));
    }
}

void AppWindow::createMenu() {
    m_menu_app = menuBar()->addMenu(tr("&Application"));

    for (auto& action : m_menu_app_actions) {
        QWidget::addAction(action);
        m_menu_app->addAction(action);
    }

    m_menu_mode = menuBar()->addMenu(tr("&mode"));
    for (auto& action : m_menu_mode_actions) {
        QWidget::addAction(action);
        m_menu_mode->addAction(action);
    }

    m_menu_options = menuBar()->addMenu(tr("&Options"));
    for (auto& action : m_menu_options_actions) {
        QWidget::addAction(action);
        m_menu_options->addAction(action);
    }

    m_menu_picking = menuBar()->addMenu(tr("&Picking"));
    for (auto& action : m_menu_picking_actions) {
        QWidget::addAction(action);
        m_menu_picking->addAction(action);
    }
}

void AppWindow::reset_position() {
    QMatrix4x4 trans;
    trans.translate(QVector3D(0, 1, -11));
    mRoot->set_transform(trans);
}

void AppWindow::reset_orientation() {
    QMatrix4x4 trans;
    mRoot->getNode("rotateNode")->set_transform(trans);
}

void AppWindow::reset_all() {
    SceneNode * newroot = import_lua(mFilename);
    *mRoot = *newroot;

    reset_orientation();
    reset_position();
}


void AppWindow::select_node() {
    QAction* selectAct = (QAction*)sender();
    std::string name = selectAct->text().toStdString();

    SceneNode* node = mRoot->getNode(name);
    node->toggleSelected();
    node->toggleChildrenSelected();
}