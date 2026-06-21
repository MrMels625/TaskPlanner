#ifndef MOCKSTORAGE_HPP
#define MOCKSTORAGE_HPP

#include "istorage.hpp"
#include <QList>
#include <QString>
#include <QVector>

namespace test
{
class MockStorage: public storage::IStorage
{
  public:
    MockStorage() = default;
    ~MockStorage() override = default;

    void addTask(const storage::Task &task) override
    {
      addTaskCallCount++;
      lastAddedTask = task;
      m_tasks.append(task);
    }

    void removeTask(int id) override
    {
      removeTaskCallCount++;
      lastRemovedId = id;
      for (int i = 0; i < m_tasks.size(); ++i)
      {
        if (m_tasks[i].id == id)
        {
          m_tasks.removeAt(i);
          return;
        }
      }
    }

    void updateTask(const storage::Task &task) override
    {
      updateTaskCallCount++;
      lastUpdatedTask = task;
      for (auto &existing : m_tasks)
      {
        if (existing.id == task.id)
        {
          existing = task;
          return;
        }
      }
    }

    QList< storage::Task > getAllTasks() const override
    {
      getAllTasksCallCount++;
      return m_tasks;
    }

    QList< storage::Task > getTasksForDate(const QDate &date) const override
    {
      getTasksForDateCallCount++;
      lastQueriedDate = date;
      QList< storage::Task > result;
      for (const auto &task : m_tasks)
      {
        if (task.deadline.date() == date)
        {
          result.append(task);
        }
      }
      return result;
    }

    QList< storage::Task > getTasksForToday() const override
    {
      getTasksForTodayCallCount++;
      return tasksForTodayToReturn;
    }

    QList< storage::Task > getOverdueTasks() const override
    {
      getOverdueTasksCallCount++;
      return overdueTasksToReturn;
    }

    QList< storage::Task > getTasksFiltered(
      const QString &searchText,
      bool today,
      bool overdue,
      storage::Priority priority) const override
    {
      getTasksFilteredCallCount++;
      lastSearchText = searchText;
      lastTodayFlag = today;
      lastOverdueFlag = overdue;
      lastPriorityArg = priority;
      return filteredTasksToReturn;
    }

    QList< storage::Task > getSortedTasks(
      const QList< storage::Task > &tasks,
      storage::Criterion criterion) const override
    {
      getSortedTasksCallCount++;
      lastSortedInput = tasks;
      lastCriterion = criterion;
      return tasks;
    }

    void saveToFile(const QString &path) override
    {
      Q_UNUSED(path);
    }

    void loadFromFile(const QString &path) override
    {
      Q_UNUSED(path);
    }

    void setTasks(const QList< storage::Task > &tasks)
    {
      m_tasks = tasks;
    }

    mutable int addTaskCallCount = 0;
    mutable int removeTaskCallCount = 0;
    mutable int updateTaskCallCount = 0;
    mutable int getAllTasksCallCount = 0;
    mutable int getTasksForDateCallCount = 0;
    mutable int getTasksForTodayCallCount = 0;
    mutable int getOverdueTasksCallCount = 0;
    mutable int getTasksFilteredCallCount = 0;
    mutable int getSortedTasksCallCount = 0;

    storage::Task lastAddedTask;
    storage::Task lastUpdatedTask;
    int lastRemovedId = -1;
    mutable QDate lastQueriedDate;
    mutable QString lastSearchText;
    mutable bool lastTodayFlag = false;
    mutable bool lastOverdueFlag = false;
    mutable storage::Priority lastPriorityArg = storage::Priority::All;
    mutable QList< storage::Task > lastSortedInput;
    mutable storage::Criterion lastCriterion = storage::Criterion::Date;

    QList< storage::Task > tasksForTodayToReturn;
    QList< storage::Task > overdueTasksToReturn;
    QList< storage::Task > filteredTasksToReturn;

  private:
    QList< storage::Task > m_tasks;
  };
}

#endif
