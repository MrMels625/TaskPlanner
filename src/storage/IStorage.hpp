#ifndef ISTORAGE_H
#define ISTORAGE_H

#include "task.hpp"

namespace storage
{
  class IStorage
  {
    virtual ~IStorage();
    virtual void addTask(const Task &task) = 0;
    virtual void removeTask(int id) = 0;
    virtual void updateTask(const Task &task) = 0;
    virtual QList< Task > getAllTasks() const = 0;
    virtual QList< Task > getTasksForDate(const QDate &date) const = 0;
    virtual QList< Task > getSortedTasks() const = 0;
    virtual void saveToFile(const QString &path) = 0;
    virtual void loadFromFile(const QString &path) = 0;
  };
}

#endif
