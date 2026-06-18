#ifndef IGAMIFICATION_STORAGE_HPP
#define IGAMIFICATION_STORAGE_HPP

#include <QDate>
#include <QString>
#include <QList>
#include <QJsonObject>

namespace storage
{
  struct Achievement
  {
    QString id;
    QString name;
    QString description;
    QString iconPath;
    bool isUnlocked;
    QDate unlockedDate;
    int xpReward;
  };

  struct UserProgress
  {
    int currentLevel;
    int currentXP;
    int xpToNextLevel;
    int streakDays;
    QDate lastActivityDate;
    QString currentTitle;
    QList<QString> unlockedAchievements;
    QList<QString> unlockedLocations;
  };

  class IGamificationStorage
  {
  public:
    virtual ~IGamificationStorage() = default;

    virtual UserProgress getUserProgress() const = 0;
    virtual void updateUserProgress(const UserProgress &progress) = 0;
    virtual void addXP(int amount) = 0;
    virtual void updateStreak(const QDate &currentDate) = 0;

    virtual QList<Achievement> getAllAchievements() const = 0;
    virtual void unlockAchievement(const QString &achievementId) = 0;
    virtual bool isAchievementUnlocked(const QString &achievementId) const = 0;

    virtual QList<QString> getUnlockedLocations() const = 0;
    virtual void unlockLocation(const QString &locationId) = 0;
    virtual bool isLocationUnlocked(const QString &locationId) const = 0;

    virtual void saveToFile() noexcept = 0;
    virtual void loadFromFile() noexcept = 0;
    virtual void serializeProgress() const = 0;
    virtual void deserializeProgress(const QJsonObject &obj) = 0;
  };

}

#endif
