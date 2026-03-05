#include "taskplanner.hpp"
#include "ui_taskplanner.h"

TaskPlanner::TaskPlanner(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TaskPlanner)
{
    ui->setupUi(this);
}

TaskPlanner::~TaskPlanner()
{
    delete ui;
}
