#include "memorystorage.hpp"
#include <algorithm>

namespace
{
  struct TaskSorter
  {
    storage::Criterion criterion;

    explicit TaskSorter(storage::Criterion crit):
      criterion(crit)
    {}

    bool operator()(const storage::Task &a, const storage::Task &b) const
    {
      switch (criterion)
      {
      case storage::Criterion::Date:
      {
        return a.deadline < b.deadline;
      }
      case storage::Criterion::Priority:
      {
        return a.priority > b.priority;
      }
      case storage::Criterion::Completed:
      {
        return a.completed < b.completed;
      }
      default:
      {
        return false;
      }
      }
    }
  };

  struct TaskFilter
  {
    QString searchText;
    bool today;
    bool overdue;
    storage::Priority priority;
    QDateTime now;
    QDate currentDate;

    TaskFilter(const QString &search, bool t, bool o, storage::Priority p):
      searchText(search),
      today(t),
      overdue(o),
      priority(p),
      now(QDateTime::currentDateTime()),
      currentDate(QDate::currentDate())
    {}

    bool operator()(const storage::Task &task) const
    {
      if (!searchText.isEmpty() && !task.name.contains(searchText, Qt::CaseInsensitive) && !task.description.contains(searchText, Qt::CaseInsensitive))
      {
        return false;
      }

      if (today && task.deadline.date() != currentDate)
      {
        return false;
      }

      if (overdue && (task.completed || task.deadline >= now))
      {
        return false;
      }

      if (priority != storage::Priority::All && task.priority != priority)
      {
        return false;
      }

      return true;
    }
  };
}

storage::MemoryStorage::MemoryStorage():
  nextId_(1)
{}

void storage::MemoryStorage::addTask(const Task &task)
{
  Task newTask = task;
  newTask.id = nextId_++;
  tasks_.append(newTask);
}

void storage::MemoryStorage::removeTask(int id)
{
  for (size_t i = 0; i < tasks_.size(); ++i)
  {
    if (tasks_[i].id == id)
    {
      tasks_.removeAt(i);
      return;
    }
  }
}

void storage::MemoryStorage::updateTask(const Task &task)
{
  for (auto &t: tasks_)
  {
    if (t.id == task.id)
    {
      t = task;
      return;
    }
  }
}

QList< storage::Task > storage::MemoryStorage::getAllTasks() const noexcept
{
  return tasks_;
}

QList< storage::Task > storage::MemoryStorage::getTasksForDate(const QDate &date) const
{
  QList< Task > result;
  for (const auto &task: tasks_)
  {
    if (task.deadline.date() == date)
    {
     result.append(task);
    }
  }
  return result;
}

QList< storage::Task > storage::MemoryStorage::getTasksForToday() const
{
  return getTasksForDate(QDate::currentDate());
}

QList< storage::Task > storage::MemoryStorage::getOverdueTasks() const
{
  QList< Task > result;
  QDateTime now = QDateTime::currentDateTime();
  for (const auto &task: tasks_)
  {
    if (!task.completed && task.deadline < now)
    {
      result.append(task);
    }
  }
  return result;
}

QList< storage::Task > storage::MemoryStorage::getTasksFiltered(const QString &searchText, bool today, bool overdue, Priority priority) const
{
  QList< Task > result;
  std::copy_if(tasks_.begin(), tasks_.end(), std::back_inserter(result), TaskFilter(searchText, today, overdue, priority));
  return result;
}

QList< storage::Task > storage::MemoryStorage::getSortedTasks(const QList< Task > &tasks, Criterion criterion) const noexcept
{
  QList< Task > result(tasks);
  std::stable_sort(result.begin(), result.end(), TaskSorter(criterion));
  return result;
}

void storage::MemoryStorage::saveToFile() noexcept
{
}

void storage::MemoryStorage::loadFromFile() noexcept
{
}
