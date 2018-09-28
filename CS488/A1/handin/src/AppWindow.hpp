#ifndef APPWINDOW_HPP
#define APPWINDOW_HPP

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QTimer>
#include <vector>

#include "Viewer.hpp"
#include "game.hpp"

class AppWindow : public QMainWindow
{
    Q_OBJECT

public:
    AppWindow();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    void createActions();
    void createMenu();

    // Each menu itself
    QMenu* m_menu_app;
    QMenu* m_menu_draw_mode;
    QMenu* m_menu_speed;

    std::vector<QAction*> m_menu_actions;
    std::vector<QAction*> m_menu_draw_mode_actions;
    std::vector<QAction*> m_menu_speed_actions;
    Viewer* m_viewer;
    Game* m_game;

    QTimer m_game_timer;

private slots:
	void tickGame();
	void newGame();

	void setSlowSpeed();
	void setMediumSpeed();
	void setFastSpeed();
};

#endif