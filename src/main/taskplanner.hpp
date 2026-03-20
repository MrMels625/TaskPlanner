#ifndef TASKPLANNER_H
#define TASKPLANNER_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
  class TaskPlanner;
}
QT_END_NAMESPACE

class TaskPlanner: public QMainWindow
{
  Q_OBJECT

public:
  TaskPlanner(QWidget *parent = nullptr);
  ~TaskPlanner();

private:
  Ui::TaskPlanner *ui;
};
#endif
