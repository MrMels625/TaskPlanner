#ifndef MOCKVIEW_HPP
#define MOCKVIEW_HPP

#include "iview.hpp"
#include "achievement.hpp"

namespace test
{
  class MockView : public view::IView
  {
  public:
    MockView() = default;
    ~MockView() override = default;

    void showTaskList(const QList<storage::Task> &tasks) override
    {
      showTaskListCallCount++;
      lastShownTasks = tasks;
    }

    void showTasksForDate(const QDate &date, const QList<storage::Task> &tasks) override
    {
      showTasksForDateCallCount++;
      lastDateArg = date;
      lastShownTasks = tasks;
    }

    void setTaskListTitle(const QString &title) override
    {
      setTaskListTitleCallCount++;
      lastTitle = title;
    }

    void showTaskCreationForm() override
    {
      showTaskCreationFormCallCount++;
      lastFormHadTask = false;
    }

    void showTaskCreationForm(const storage::Task &task) override
    {
      showTaskCreationFormCallCount++;
      lastFormHadTask = true;
      lastFormTask = task;
    }

    void closeTaskCreationForm() override
    {
      closeTaskCreationFormCallCount++;
    }

    void showErrorMessage(const QString &message) override
    {
      showErrorMessageCallCount++;
      lastErrorMessage = message;
    }

    void showInfoMessage(const QString &message) override
    {
      showInfoMessageCallCount++;
      lastInfoMessage = message;
    }

    void updateStats(int total, int completed, int today) override
    {
      updateStatsCallCount++;
      lastStatsTotal = total;
      lastStatsCompleted = completed;
      lastStatsToday = today;
    }

    void showUserLevel(int level, int currentXP, int xpToNext) override
    {
      Q_UNUSED(level);
      Q_UNUSED(currentXP);
      Q_UNUSED(xpToNext);
    }

    void showStreak(int days) override
    {
      Q_UNUSED(days);
    }

    void showUserTitle(const QString &title) override
    {
      Q_UNUSED(title);
    }

    void showXPNotification(int amount, const QString &reason) override
    {
      Q_UNUSED(amount);
      Q_UNUSED(reason);
    }

    void showAchievementUnlocked(const storage::Achievement &achievement) override
    {
      Q_UNUSED(achievement);
    }

    void showAchievementsList(const QList<storage::Achievement> &achievements) override
    {
      Q_UNUSED(achievements);
    }

    void showCampusMap(const QList<QString> &unlockedLocations) override
    {
      Q_UNUSED(unlockedLocations);
    }

    void showLocationUnlocked(const QString &locationName) override
    {
      Q_UNUSED(locationName);
    }

    void showLevelUpAnimation(int newLevel, const QString &newTitle) override
    {
      Q_UNUSED(newLevel);
      Q_UNUSED(newTitle);
    }

    void updateGamificationPanel() override
    {}

    int showTaskListCallCount = 0;
    int showTasksForDateCallCount = 0;
    int setTaskListTitleCallCount = 0;
    int showTaskCreationFormCallCount = 0;
    int closeTaskCreationFormCallCount = 0;
    int showErrorMessageCallCount = 0;
    int showInfoMessageCallCount = 0;
    int updateStatsCallCount = 0;

    QList<storage::Task> lastShownTasks;
    QDate lastDateArg;
    QString lastTitle;
    bool lastFormHadTask = false;
    storage::Task lastFormTask;
    QString lastErrorMessage;
    QString lastInfoMessage;
    int lastStatsTotal = 0;
    int lastStatsCompleted = 0;
    int lastStatsToday = 0;
  };
}

#endif
