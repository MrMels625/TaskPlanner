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

const QList< storage::Achievement > storage::MemoryStorage::k_allAchievements = {
  achievements::LEVEL_1,
  achievements::LEVEL_5,
  achievements::LEVEL_10,
  achievements::LEVEL_20,
  achievements::LEVEL_30,
  achievements::LEVEL_50,
  achievements::TASKS_10,
  achievements::TASKS_50,
  achievements::TASKS_100,
  achievements::TASKS_250,
  achievements::ON_TIME_5,
  achievements::ON_TIME_20,
  achievements::ON_TIME_50,
  achievements::HARD_10,
  achievements::HARD_50,
  achievements::MEDIUM_30,
  achievements::LOW_50,
  achievements::BALANCED_ALL,
  achievements::PERFECT_DAY_1,
  achievements::PERFECT_DAY_7,
  achievements::PERFECT_DAY_30,
  achievements::STREAK_7,
  achievements::STREAK_30,
  achievements::STREAK_100,
  achievements::COMBO_NIGHTMARE,
  achievements::COMBO_MARATHON,
  achievements::LOCATION_5,
  achievements::LOCATION_10,
  achievements::LOCATION_ALL,
  achievements::DELETE_5,
  achievements::DELETE_20,
  achievements::DELETE_100,
};

storage::MemoryStorage::MemoryStorage():
  nextId_(1),
  perfectDaysCount_(0),
  maxTasksCompletedInOneDay_(0),
  maxHardTasksCompletedInOneDay_(0)
{
  progress_.currentLevel = 1;
  progress_.currentXP = 0;
  progress_.xpToNextLevel = calculateXPForLevel(1);
  progress_.streakDays = 0;
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
  for (qsizetype i = 0; i < tasks_.size(); ++i)
  {
    if (tasks_[i].id == id)
    {
      ++progress_.deletedTasksCount;
      tasks_.removeAt(i);
      saveToFile();
      return;
    }
  }
}

void storage::MemoryStorage::updateTask(Task &task)
{
  for (auto &t: tasks_)
  {
    if (t.id == task.id)
    {
      task.completedAt = QDateTime::currentDateTime();
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

QList< storage::Task > storage::MemoryStorage::getSortedTasks(const QList< Task > &tasks, Criterion criterion) const noexcept
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
  return k_allAchievements;
}

void storage::MemoryStorage::unlockAchievement(const QString &achievementId)
{
  if (isAchievementUnlocked(achievementId))
  {
    return;
  }
  progress_.unlockedAchievementIds.append(achievementId);
  saveGamificationData();
}

bool storage::MemoryStorage::isAchievementUnlocked(const QString &achievementId) const
{
  return progress_.unlockedAchievementIds.contains(achievementId);
}

storage::Achievement storage::MemoryStorage::getAchievementById(const QString &id) const
{
  for (const auto &achievement: k_allAchievements)
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

int storage::MemoryStorage::getCompletedTasksCount() const
{
  int count = 0;
  for (const auto &task: tasks_)
  {
    if (task.completed)
    {
      ++count;
    }
  }
  return count;
}

int storage::MemoryStorage::getOnTimeCompletedCount() const
{
  int count = 0;
  for (const auto &task: tasks_)
  {
    if (task.completed && task.completedAt <= task.deadline)
    {
      ++count;
    }
  }
  return count;
}

int storage::MemoryStorage::getCompletedCountByPriority(Priority priority) const
{
  int count = 0;
  for (const auto &task: tasks_)
  {
    if (task.completed && task.priority == priority)
    {
      ++count;
    }
  }
  return count;
}

int storage::MemoryStorage::getDeletedTasksCount() const
{
  return progress_.deletedTasksCount;
}

int storage::MemoryStorage::getPerfectDaysCount() const
{
  return perfectDaysCount_;
}

int storage::MemoryStorage::getTotalLocationsCount() const
{
  return 0;
}

int storage::MemoryStorage::getMaxTasksCompletedInOneDay() const
{
  return maxTasksCompletedInOneDay_;
}

int storage::MemoryStorage::getMaxHardTasksCompletedInOneDay() const
{
  return maxHardTasksCompletedInOneDay_;
}

void storage::MemoryStorage::checkLevelUp() noexcept
{
  while (progress_.currentLevel < xp::MAX_LEVEL && progress_.currentXP >= progress_.xpToNextLevel)
  {
    progress_.currentXP -= progress_.xpToNextLevel;
    ++progress_.currentLevel;
    progress_.xpToNextLevel = calculateXPForLevel(progress_.currentLevel);
  }
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
  QJsonObject root;
  root["progress"] = serial::progressToJson(progress_);
  root["perfectDaysCount"] = perfectDaysCount_;
  root["maxTasksCompletedInOneDay"] = maxTasksCompletedInOneDay_;
  root["maxHardTasksCompletedInOneDay"] = maxHardTasksCompletedInOneDay_;

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

  const auto tryLoad = [&](const QString &path) -> bool
  {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
    {
      return false;
    }

    const QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();

    if (doc.isNull() || !doc.isObject())
    {
      return false;
    }

    const QJsonObject root = doc.object();

    if (!root.contains("progress"))
    {
      return false;
    }

    loadedProgress = serial::progressFromJson(root["progress"].toObject());
    perfectDaysCount_ = root["perfectDaysCount"].toInt(0);
    maxTasksCompletedInOneDay_ = root["maxTasksCompletedInOneDay"].toInt(0);
    maxHardTasksCompletedInOneDay_ = root["maxHardTasksCompletedInOneDay"].toInt(0);
    return true;
  };

  if (tryLoad(serial::filePath(serial::k_gamificationFileName)) || tryLoad(serial::filePath(serial::k_gamificationFileNameBackup)))
  {
    progress_ = loadedProgress;
  }
}
