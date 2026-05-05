#ifndef MEMORYSTORAGE_HPP
#define MEMORYSTORAGE_HPP
#include "istorage.hpp"
#include "task.hpp"

namespace storage
{
  class MemoryStorage: public IStorage
  {
  public:
    MemoryStorage();

    void addTask(const Task &task) override;
    void removeTask(int id) override;
    void updateTask(const Task &task) override;

    QList< Task > getAllTasks() const noexcept override;
    QList< Task > getTasksForDate(const QDate &date) const override;
    QList< Task > getTasksForToday() const override;
    QList< Task > getOverdueTasks() const override;
    QList< Task > getTasksFiltered(const QString &searchText, bool today, bool overdue, Priority priority) const override;
    QList< Task > getSortedTasks(const QList< Task > &tasks, Criterion criterion) const noexcept override;

  private:
    QList< Task > tasks_;
    int nextId_;

    void saveToFile() noexcept override;
    void loadFromFile() noexcept override;
  };
}

#endif
