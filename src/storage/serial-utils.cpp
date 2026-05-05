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
  task.id = obj["id"].toInt();
  task.name = obj["name"].toString();
  task.description = obj["description"].toString();
  task.discipline = obj["discipline"].toString();
  task.deadline = QDateTime::fromString(obj["deadline"].toString(), Qt::ISODate);
  task.priority = static_cast< Priority >(obj["priority"].toInt());
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
    loaded.append(taskFromJson(v.toObject()));
  }

  tasks  = loaded;
  nextId = root["nextId"].toInt();
  return true;
}
