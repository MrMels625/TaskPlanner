#ifndef TASK_HPP
#define TASK_HPP
#include <QDate>
#include <QList>
#include <QString>
#include <QDateTime>

namespace storage
{
  enum class Priority;

  struct Task
  {
    int id;
    QString name;
    QString description;
    QString discipline;
    QDateTime deadline;
    Priority priority;
    bool completed;
    QList< QString > tags;

    Task() = default;

    bool operator<(const Task &other) const;
    bool operator==(const Task &other) const;
  };
}

#endif
