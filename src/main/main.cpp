#include "taskplanner.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TaskPlanner w;
    w.show();
    return a.exec();
}
