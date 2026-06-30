#include "serial-utils.hpp"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

QString storage::serial::filePath(const QString &name)
{
  return QCoreApplication::applicationDirPath() + QDir::separator() + name;
}

QJsonObject storage::serial::taskToJson(const Task &task)
{
  QJsonArray tagsArray;
  for (const QString &tag: task.tags)
  {
    tagsArray.append(tag);
  }

  QJsonObject obj;
  obj["id"] = task.id;
  obj["name"] = task.name;
  obj["description"] = task.description;
  obj["discipline"] = task.discipline;
  obj["deadline"] = task.deadline.toString(Qt::ISODate);
  obj["priority"] = static_cast< int >(task.priority);
  obj["completed"] = task.completed;
  obj["tags"] = tagsArray;
  return obj;
}

storage::Task storage::serial::taskFromJson(const QJsonObject &obj)
{
  Task task;

  const int id = obj["id"].toInt();
  if (id <= 0)
  {
    return {};
  }
  task.id = id;

  const QString name = obj["name"].toString();
  if (name.isEmpty())
  {
    return {};
  }
  task.name = name;

  task.description = obj["description"].toString();
  task.discipline = obj["discipline"].toString();

  const QDateTime deadline = QDateTime::fromString(obj["deadline"].toString(), Qt::ISODate);
  if (!deadline.isValid())
  {
    return {};
  }
  task.deadline = deadline;

  const int priorityInt = obj["priority"].toInt(-1);
  if (priorityInt < static_cast< int >(Priority::All) || priorityInt > static_cast< int >(Priority::Hard))
  {
    return {};
  }
  task.priority = static_cast< Priority >(priorityInt);

  task.completed = obj["completed"].toBool();

  for (const QJsonValue &v: obj["tags"].toArray())
  {
    task.tags.append(v.toString());
  }

  return task;
}

bool storage::serial::tryLoad(const QString &path, QList< Task > &tasks, int &nextId)
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
  if (!root.contains("tasks") || !root.contains("nextId"))
  {
    return false;
  }

  QList< Task > loaded;
  for (const QJsonValue &v: root["tasks"].toArray())
  {
    if (!v.isObject())
    {
      return false;
    }
    const Task task = taskFromJson(v.toObject());
    if (task.id <= 0)
    {
      return false;
    }
    loaded.append(task);
  }

  tasks = loaded;
  nextId = root["nextId"].toInt();
  return true;
}

QJsonObject storage::serial::progressToJson(const UserProgress &progress)
{
  QJsonArray unlockedAchievements;
  for (const QString &id: progress.unlockedAchievementIds)
  {
    unlockedAchievements.append(id);
  }

  QJsonArray unlockedLocations;
  for (const QString &id: progress.unlockedLocations)
  {
    unlockedLocations.append(id);
  }

  QJsonObject obj;
  obj["currentLevel"] = progress.currentLevel;
  obj["currentXP"] = progress.currentXP;
  obj["xpToNextLevel"] = progress.xpToNextLevel;
  obj["streakDays"] = progress.streakDays;
  obj["lastActivityDate"] = progress.lastActivityDate.toString(Qt::ISODate);
  obj["currentTitle"] = progress.currentTitle;
  obj["unlockedAchievements"] = unlockedAchievements;
  obj["unlockedLocations"] = unlockedLocations;
  return obj;
}

storage::UserProgress storage::serial::progressFromJson(const QJsonObject &obj)
{
  UserProgress progress;
  progress.currentLevel = obj["currentLevel"].toInt(1);
  progress.currentXP = obj["currentXP"].toInt(0);
  progress.xpToNextLevel = obj["xpToNextLevel"].toInt(100);
  progress.streakDays = obj["streakDays"].toInt(0);
  progress.lastActivityDate = QDate::fromString(obj["lastActivityDate"].toString(), Qt::ISODate);
  progress.currentTitle = obj["currentTitle"].toString();

  for (const QJsonValue &v: obj["unlockedAchievements"].toArray())
  {
    progress.unlockedAchievementIds.append(v.toString());
  }

  for (const QJsonValue &v: obj["unlockedLocations"].toArray())
  {
    progress.unlockedLocations.append(v.toString());
  }

  return progress;
}

QJsonObject storage::serial::achievementToJson(const Achievement &achievement)
{
  QJsonObject obj;
  obj["id"] = achievement.id;
  obj["name"] = achievement.name;
  obj["description"] = achievement.description;
  obj["iconPath"] = achievement.iconPath;
  obj["xpReward"] = achievement.xpReward;
  return obj;
}

storage::Achievement storage::serial::achievementFromJson(const QJsonObject &obj)
{
  Achievement achievement;
  achievement.id = obj["id"].toString();
  achievement.name = obj["name"].toString();
  achievement.description = obj["description"].toString();
  achievement.iconPath = obj["iconPath"].toString();
  achievement.xpReward = obj["xpReward"].toInt(0);
  return achievement;
}

bool storage::serial::tryLoadGamification(const QString &path, UserProgress &progress, QList<Achievement> &achievements)
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
  if (!root.contains("progress") || !root.contains("achievements"))
  {
    return false;
  }

  progress = progressFromJson(root["progress"].toObject());

  QList< Achievement > loaded;
  for (const QJsonValue &v: root["achievements"].toArray())
  {
    if (!v.isObject())
    {
      return false;
    }
    loaded.append(achievementFromJson(v.toObject()));
  }

  achievements = loaded;
  return true;
}
