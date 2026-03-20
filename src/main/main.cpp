#include "taskplanner.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TaskPlanner w;
    w.show();
    return QCoreApplication::exec();
}
