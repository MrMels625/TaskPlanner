#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_taskplanner.h"

namespace Ui {
class TaskPlanner;
}

class TaskPlanner : public QMainWindow
{
    Q_OBJECT

public:
    explicit TaskPlanner(QWidget *parent = nullptr);
    ~TaskPlanner();

private:
    Ui::TaskPlanner *ui;
};

#endif