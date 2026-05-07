#ifndef ISTORAGE_H
#define ISTORAGE_H
#include "task.hpp"

namespace storage
{
  enum class Priority
  {
    All,
    Low,
    Medium,
    Hard
  };

  enum class Criterion
  {
    Date,
    Priority,
    Completed
  };

  enum class Filter
  {
    Search,
    ShowAll,
    ShowToday,
    ShowOverdue,
    Priority
  };

  class IStorage
  {
  public:
    virtual ~IStorage() = default;
    virtual void addTask(const Task &task) = 0;
    virtual void removeTask(int id) = 0;
    virtual void updateTask(const Task &task) = 0;
    virtual QList< Task > getAllTasks() const = 0;
    virtual QList< Task > getTasksForDate(const QDate &date) const = 0;
    virtual QList< Task > getTasksForToday() const = 0;
    virtual QList< Task > getOverdueTasks() const = 0;
    virtual QList< Task > getTasksFiltered(const QString &searchText, bool today, bool overdue, Priority priority) const = 0;
    virtual QList< Task > getSortedTasks(const QList< Task > &tasks, Criterion criterion) const = 0;

  private:
    virtual void saveToFile() noexcept = 0;
    virtual void loadFromFile() noexcept = 0;
  };
}

#endif
