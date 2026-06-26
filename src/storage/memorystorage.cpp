#include "memorystorage.hpp"
#include <algorithm>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include "serial-utils.hpp"

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
{
  initDefaultAchievements();
  load();
}

void storage::MemoryStorage::addTask(const Task &task)
{
  Task newTask = task;
  newTask.id = nextId_++;
  tasks_.append(newTask);
  saveToFile();
}

void storage::MemoryStorage::removeTask(int id)
{
  for (int i = 0; i < tasks_.size(); ++i)
  {
    if (tasks_[i].id == id)
    {
      tasks_.removeAt(i);
      saveToFile();
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
      saveToFile();
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

QList< storage::Task > storage::MemoryStorage::getSortedTasks(const QList<Task> &tasks, Criterion criterion) const noexcept
{
  QList< Task > result(tasks);
  std::stable_sort(result.begin(), result.end(), TaskSorter(criterion));
  return result;
}

storage::UserProgress storage::MemoryStorage::getUserProgress() const
{
  return progress_;
}

void storage::MemoryStorage::updateUserProgress(const UserProgress &progress)
{
  progress_ = progress;
  saveGamificationData();
}

void storage::MemoryStorage::addXP(int amount, const QString &reason)
{
  Q_UNUSED(reason)
  progress_.currentXP += amount;
  checkLevelUp();
  saveGamificationData();
}

void storage::MemoryStorage::updateStreak(const QDate &currentDate)
{
  if (!progress_.lastActivityDate.isValid())
  {
    progress_.streakDays = 1;
    progress_.lastActivityDate = currentDate;
    saveGamificationData();
    return;
  }

  const int daysDiff = progress_.lastActivityDate.daysTo(currentDate);

  if (daysDiff == 0)
  {
    return;
  }

  if (daysDiff == 1)
  {
    ++progress_.streakDays;
  }
  else
  {
    progress_.streakDays = 1;
  }

  progress_.lastActivityDate = currentDate;
  saveGamificationData();
}

int storage::MemoryStorage::getCurrentLevel() const
{
  return progress_.currentLevel;
}

int storage::MemoryStorage::getTotalXP() const
{
  return progress_.currentXP;
}

int storage::MemoryStorage::getStreakDays() const
{
  return progress_.streakDays;
}

QList< storage::Achievement > storage::MemoryStorage::getAllAchievements() const
{
  return achievements_;
}

void storage::MemoryStorage::unlockAchievement(const QString &achievementId)
{
  if (isAchievementUnlocked(achievementId))
  {
    return;
  }

  for (auto &achievement: achievements_)
  {
    if (achievement.id == achievementId)
    {
      achievement.isUnlocked = true;
      achievement.unlockedDate = QDate::currentDate();
      progress_.unlockedAchievements.append(achievementId);
      addXP(achievement.xpReward, achievementId);
      return;
    }
  }
}

bool storage::MemoryStorage::isAchievementUnlocked(const QString &achievementId) const
{
  return progress_.unlockedAchievements.contains(achievementId);
}

storage::Achievement storage::MemoryStorage::getAchievementById(const QString &id) const
{
  for (const auto &achievement: achievements_)
  {
    if (achievement.id == id)
    {
      return achievement;
    }
  }
  return {};
}

QList< QString > storage::MemoryStorage::getUnlockedLocations() const
{
  return progress_.unlockedLocations;
}

void storage::MemoryStorage::unlockLocation(const QString &locationId)
{
  if (!progress_.unlockedLocations.contains(locationId))
  {
    progress_.unlockedLocations.append(locationId);
    saveGamificationData();
  }
}

bool storage::MemoryStorage::isLocationUnlocked(const QString &locationId) const
{
  return progress_.unlockedLocations.contains(locationId);
}

int storage::MemoryStorage::xpForLevel(int level) const noexcept
{
  return level * 100;
}

void storage::MemoryStorage::checkLevelUp() noexcept
{
  while (progress_.currentXP >= progress_.xpToNextLevel)
  {
    progress_.currentXP -= progress_.xpToNextLevel;
    ++progress_.currentLevel;
    progress_.xpToNextLevel = xpForLevel(progress_.currentLevel);
  }
}

void storage::MemoryStorage::initDefaultAchievements() noexcept
{
  achievements_ = {
    {"first_task", "First Step", "Complete your first task", "", false, {}, 50},
    {"streak_3", "On a Roll", "Maintain a 3-day streak", "", false, {}, 75},
    {"streak_7", "Week Warrior", "Maintain a 7-day streak", "", false, {}, 150},
    {"tasks_10", "Productive", "Complete 10 tasks", "", false, {}, 100},
    {"tasks_50", "Overachiever", "Complete 50 tasks", "", false, {}, 300},
    {"level_5", "Rising Star", "Reach level 5", "", false, {}, 200},
    {"level_10", "Veteran", "Reach level 10", "", false, {}, 500},
  };
}

void storage::MemoryStorage::saveToFile() noexcept
{
  QJsonArray array;
  for (const Task &t: tasks_)
  {
    array.append(serial::taskToJson(t));
  }

  QJsonObject root;
  root["nextId"] = nextId_;
  root["tasks"] = array;

  const QByteArray data = QJsonDocument(root).toJson(QJsonDocument::Indented);
  const QString main = serial::filePath(serial::k_fileName);
  const QString backup = serial::filePath(serial::k_fileNameBackup);

  QFile f(main);
  if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    return;
  }

  f.write(data);
  f.close();

  const QString temp = backup + ".tmp";
  QFile::remove(temp);
  if (QFile::copy(main, temp))
  {
    QFile::remove(backup);
    QFile::rename(temp, backup);
  }
}

void storage::MemoryStorage::saveGamificationData() noexcept
{
  QJsonArray achievementsArray;
  for (const Achievement &a: achievements_)
  {
    achievementsArray.append(serial::achievementToJson(a));
  }

  QJsonObject root;
  root["progress"] = serial::progressToJson(progress_);
  root["achievements"] = achievementsArray;

  const QByteArray data = QJsonDocument(root).toJson(QJsonDocument::Indented);
  const QString main = serial::filePath(serial::k_gamificationFileName);
  const QString backup = serial::filePath(serial::k_gamificationFileNameBackup);

  QFile f(main);
  if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    return;
  }

  f.write(data);
  f.close();

  const QString temp = backup + ".tmp";
  QFile::remove(temp);
  if (QFile::copy(main, temp))
  {
    QFile::remove(backup);
    QFile::rename(temp, backup);
  }
}

void storage::MemoryStorage::load() noexcept
{
  if (!serial::tryLoad(serial::filePath(serial::k_fileName), tasks_, nextId_))
  {
    serial::tryLoad(serial::filePath(serial::k_fileNameBackup), tasks_, nextId_);
  }
  loadGamificationData();
}

void storage::MemoryStorage::loadFromFile() noexcept
{
  load();
}

void storage::MemoryStorage::loadGamificationData() noexcept
{
  UserProgress loadedProgress;
  QList< Achievement > loadedAchievements;

  if (!serial::tryLoadGamification(serial::filePath(serial::k_gamificationFileName), loadedProgress, loadedAchievements))
  {
    if (!serial::tryLoadGamification(serial::filePath(serial::k_gamificationFileNameBackup), loadedProgress, loadedAchievements))
    {
      return;
    }
  }

  progress_ = loadedProgress;

  for (const Achievement &loaded: loadedAchievements)
  {
    for (auto &existing: achievements_)
    {
      if (existing.id == loaded.id)
      {
        existing.isUnlocked = loaded.isUnlocked;
        existing.unlockedDate = loaded.unlockedDate;
        break;
      }
    }
  }
}

void storage::MemoryStorage::serializeProgress() const
{
  QJsonArray achievementsArray;
  for (const Achievement &a: achievements_)
  {
    achievementsArray.append(serial::achievementToJson(a));
  }

  QJsonObject root;
  root["progress"] = serial::progressToJson(progress_);
  root["achievements"] = achievementsArray;

  const QByteArray data = QJsonDocument(root).toJson(QJsonDocument::Indented);
  const QString main = serial::filePath(serial::k_gamificationFileName);

  QFile f(main);
  if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    return;
  }

  f.write(data);
  f.close();
}

void storage::MemoryStorage::deserializeProgress(const QJsonObject &obj)
{
  if (obj.contains("progress"))
  {
    progress_ = serial::progressFromJson(obj["progress"].toObject());
  }

  if (obj.contains("achievements"))
  {
    for (const QJsonValue &v: obj["achievements"].toArray())
    {
      if (!v.isObject())
      {
        continue;
      }
      const Achievement loaded = serial::achievementFromJson(v.toObject());
      for (auto &existing: achievements_)
      {
        if (existing.id == loaded.id)
        {
          existing.isUnlocked = loaded.isUnlocked;
          existing.unlockedDate = loaded.unlockedDate;
          break;
        }
      }
    }
  }
}
