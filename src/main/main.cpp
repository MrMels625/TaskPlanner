#include <QApplication>
#include "taskplanner.hpp"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  TaskPlanner w;
  w.show();
  return QCoreApplication::exec();
}
