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

    UserProgress getUserProgress() const override;
    void updateUserProgress(const UserProgress &progress) override;
    void addXP(int amount, const QString &reason) override;
    void updateStreak(const QDate &currentDate) override;
    int getCurrentLevel() const override;
    int getTotalXP() const override;
    int getStreakDays() const override;

    QList< Achievement > getAllAchievements() const override;
    void unlockAchievement(const QString &achievementId) override;
    bool isAchievementUnlocked(const QString &achievementId) const override;
    Achievement getAchievementById(const QString &id) const override;

    QList< QString > getUnlockedLocations() const override;
    void unlockLocation(const QString &locationId) override;
    bool isLocationUnlocked(const QString &locationId) const override;

  private:
    QList< Task > tasks_;
    int nextId_;

    void saveToFile() noexcept override;
    void loadFromFile() noexcept override;
    void load() noexcept;

    void saveGamificationData() noexcept override;
    void loadGamificationData() noexcept override;
  };
}

#endif

