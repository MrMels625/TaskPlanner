#ifndef ISTORAGE_HPP
#define ISTORAGE_HPP
#include <QMetaType>
#include "task.hpp"
#include "../utils/achievements.hpp"

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

  struct UserProgress
  {
    int currentLevel;
    int currentXP;
    int xpToNextLevel;
    int streakDays;
    QDate lastActivityDate;
    QString currentTitle;
    QList< QString > unlockedAchievementIds;
    QList< QString > unlockedLocations;
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

    virtual UserProgress getUserProgress() const = 0;
    virtual void updateUserProgress(const UserProgress &progress) = 0;
    virtual void addXP(int amount, const QString &reason) = 0;
    virtual void updateStreak(const QDate &currentDate) = 0;
    virtual int getCurrentLevel() const = 0;
    virtual int getTotalXP() const = 0;
    virtual int getStreakDays() const = 0;

    virtual QList< Achievement > getAllAchievements() const = 0;
    virtual void unlockAchievement(const QString &achievementId) = 0;
    virtual bool isAchievementUnlocked(const QString &achievementId) const = 0;
    virtual Achievement getAchievementById(const QString &id) const = 0;

    virtual QList< QString > getUnlockedLocations() const = 0;
    virtual void unlockLocation(const QString &locationId) = 0;
    virtual bool isLocationUnlocked(const QString &locationId) const = 0;

    virtual int getCompletedTasksCount() const = 0;
    virtual int getOnTimeCompletedCount() const = 0;
    virtual int getCompletedCountByPriority(Priority priority) const = 0;
    virtual int getDeletedTasksCount() const = 0;
    virtual int getPerfectDaysCount() const = 0;

    virtual int getTotalLocationsCount() const = 0;
    virtual int getMaxTasksCompletedInOneDay() const = 0;
    virtual int getMaxHardTasksCompletedInOneDay() const = 0;

  private:
    virtual void saveToFile() noexcept = 0;
    virtual void loadFromFile() noexcept = 0;
    virtual void saveGamificationData() noexcept = 0;
    virtual void loadGamificationData() noexcept = 0;
  };
}

Q_DECLARE_METATYPE(storage::Priority)
Q_DECLARE_METATYPE(storage::Criterion)
Q_DECLARE_METATYPE(storage::Filter)

#endif
