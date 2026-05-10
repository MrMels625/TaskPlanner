#include <QApplication>

#include "../controller/controller.hpp"
#include "../storage/memorystorage.hpp"
#include "../view/iview.hpp"
#include "../view/taskplannerview.hpp"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  storage::MemoryStorage storage;
  controller::Controller controller;
  view::TaskPlannerView view;

  controller.setStorage(&storage);
  controller.setView(&view);

  view.show();
  controller.start();

  return QApplication::exec();
}
