#ifndef TASK_HPP
#define TASK_HPP
#include <QDate>
#include <QList>
#include <QString>
#include <QDateTime>

namespace storage
{
  struct Task
  {
    enum class Priority { Low, Medium, Hard };

    int id;
    QString name;
    QString description;
    QString discipline;
    QDateTime deadline;
    Priority priority;
    bool completed;
    QList< QString > tags;

    Task();
    bool operator<(const Task &other) const;
  };
}

#endif
