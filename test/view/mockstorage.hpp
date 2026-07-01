#ifndef MOCKSTORAGE_HPP
#define MOCKSTORAGE_HPP

#include "istorage.hpp"
#include <QList>

namespace test
{
  class MockStorage : public storage::IStorage
  {
  public:
    MockStorage() = default;
    ~MockStorage() override = default;

    void addTask(const storage::Task &task) override
    {
      add_task_call_count++;
      last_added_task = task;
      m_tasks.append(task);
    }

    void removeTask(int id) override
    {
      remove_task_call_count++;
      last_removed_id = id;
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
      update_task_call_count++;
      last_updated_task = task;
      for (auto &existing: m_tasks)
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
      get_all_tasks_call_count++;
      return m_tasks;
    }

    QList< storage::Task > getTasksForDate(const QDate &date) const override
    {
      get_tasks_for_date_call_count++;
      last_queried_date = date;
      QList< storage::Task > result;
      for (const auto &task: m_tasks)
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
      get_tasks_for_today_call_count++;
      return tasks_for_today_to_return;
    }

    QList< storage::Task > getOverdueTasks() const override
    {
      get_overdue_tasks_call_count++;
      return overdue_tasks_to_return;
    }

    QList< storage::Task > getTasksFiltered(const QString &search_text, bool today, bool overdue, storage::Priority priority) const override
    {
      get_tasks_filtered_call_count++;
      last_search_text = search_text;
      last_today_flag = today;
      last_overdue_flag = overdue;
      last_priority_arg = priority;
      return filtered_tasks_to_return;
    }

    QList< storage::Task > getSortedTasks(const QList< storage::Task > &tasks, storage::Criterion criterion) const override
    {
      get_sorted_tasks_call_count++;
      last_sorted_input = tasks;
      last_criterion = criterion;
      return tasks;
    }

    void saveToFile() noexcept override
    {
    }

    void loadFromFile() noexcept override
    {
    }

    void setTasks(const QList< storage::Task > &tasks)
    {
      m_tasks = tasks;
    }

    storage::UserProgress getUserProgress() const override
    {
      return {};
    }

    void updateUserProgress(const storage::UserProgress &progress) override
    {
      Q_UNUSED(progress);
    }

    void addXP(int amount, const QString &reason) override
    {
      Q_UNUSED(amount);
      Q_UNUSED(reason);
    }

    void updateStreak(const QDate &current_date) override
    {
      Q_UNUSED(current_date);
    }

    int getCurrentLevel() const override
    {
      return 1;
    }

    int getTotalXP() const override
    {
      return 0;
    }

    int getStreakDays() const override
    {
      return 0;
    }

    QList< storage::Achievement > getAllAchievements() const override
    {
      return {};
    }

    void unlockAchievement(const QString &achievement_id) override
    {
      Q_UNUSED(achievement_id);
    }

    bool isAchievementUnlocked(const QString &achievement_id) const override
    {
      Q_UNUSED(achievement_id);
      return false;
    }

    storage::Achievement getAchievementById(const QString &id) const override
    {
      Q_UNUSED(id);
      return {};
    }

    QList< QString > getUnlockedLocations() const override
    {
      return {};
    }

    void unlockLocation(const QString &location_id) override
    {
      Q_UNUSED(location_id);
    }

    bool isLocationUnlocked(const QString &location_id) const override
    {
      Q_UNUSED(location_id);
      return false;
    }

    void saveGamificationData() noexcept override
    {
    }

    void loadGamificationData() noexcept override
    {
    }

    mutable int add_task_call_count = 0;
    mutable int remove_task_call_count = 0;
    mutable int update_task_call_count = 0;
    mutable int get_all_tasks_call_count = 0;
    mutable int get_tasks_for_date_call_count = 0;
    mutable int get_tasks_for_today_call_count = 0;
    mutable int get_overdue_tasks_call_count = 0;
    mutable int get_tasks_filtered_call_count = 0;
    mutable int get_sorted_tasks_call_count = 0;

    storage::Task last_added_task;
    storage::Task last_updated_task;
    int last_removed_id = -1;
    mutable QDate last_queried_date;
    mutable QString last_search_text;
    mutable bool last_today_flag = false;
    mutable bool last_overdue_flag = false;
    mutable storage::Priority last_priority_arg = storage::Priority::All;
    mutable QList< storage::Task > last_sorted_input;
    mutable storage::Criterion last_criterion = storage::Criterion::Date;

    QList< storage::Task > tasks_for_today_to_return;
    QList< storage::Task > overdue_tasks_to_return;
    QList< storage::Task > filtered_tasks_to_return;

  private:
    QList< storage::Task > m_tasks;
  };
}

#endif
