#ifndef IGAMIFICATION_VIEW_HPP
#define IGAMIFICATION_VIEW_HPP

#include <QString>
#include <QList>
#include "../storage/igamification_storage.hpp"

namespace view
{
  class IGamificationView
  {
  public:
    virtual ~IGamificationView() = default;

    virtual void showUserLevel(int level, int currentXP, int xpToNext) = 0;
    virtual void showStreak(int days) = 0;
    virtual void showUserTitle(const QString &title) = 0;
    virtual void showXPNotification(int amount, const QString &reason) = 0;

    virtual void showAchievementUnlocked(const storage::Achievement &achievement) = 0;
    virtual void showAchievementsList(const QList<storage::Achievement> &achievements) = 0;

    virtual void showCampusMap(const QList<QString> &unlockedLocations) = 0;
    virtual void showLocationUnlocked(const QString &locationName) = 0;

    virtual void showLevelUpAnimation(int newLevel, const QString &newTitle) = 0;

  signals:
    void gamificationViewReady();
    void achievementsRequested();
    void mapRequested();
  };

}

#endif
