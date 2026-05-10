#include <QApplication>
#include "../controller/controller.hpp"
#include "../storage/memorystorage.hpp"
//#include "../view/taskplannerview.hpp"
#include "taskplanner.hpp"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
//  storage::MemoryStorage storage;
//  controller::Controller controller;
 // view::TaskPlannerView view;

 // controller.setStorage(&storage);
 // controller.setView(&view);
  //view.show();
  //controller.start();
  TaskPlanner w;
  w.show();
  return QCoreApplication::exec();
}
