#ifndef APPWINDOW_HPP
#define APPWINDOW_HPP

//Name: Lawson Fulton
//UserID: ljfulton
//Student#: 20381453

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <vector>
#include "Viewer.hpp"
#include "scene_lua.hpp"

class AppWindow : public QMainWindow
{
    Q_OBJECT

public:
    AppWindow(SceneNode *root, std::string filename);

private:
    void createActions();
    void createMenu();

    // Each menu itself
    QMenu* m_menu_app;
    QMenu* m_menu_mode;
    QMenu* m_menu_options;
    QMenu* m_menu_picking;

    std::vector<QAction*> m_menu_app_actions;
    std::vector<QAction*> m_menu_mode_actions;
    std::vector<QAction*> m_menu_options_actions;
    std::vector<QAction*> m_menu_picking_actions;

    Viewer* m_viewer;
    SceneNode* mRoot;
    std::string mFilename;

public Q_SLOTS:
	void reset_position();
	void reset_orientation();
	void reset_all();
	void select_node();
};

#endif