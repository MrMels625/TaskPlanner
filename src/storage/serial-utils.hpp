#ifndef STORAGE_SERIALIZATION_HPP
#define STORAGE_SERIALIZATION_HPP
#include <QJsonObject>
#include <QString>
#include "task.hpp"

namespace storage
{
  namespace serial
  {
    inline constexpr auto k_fileName = "tasks.json";
    inline constexpr auto k_fileNameBackup = "tasks.backup.json";

    QString filePath(const QString &name);
    QJsonObject taskToJson(const Task &task);
    Task taskFromJson(const QJsonObject &obj);
    bool tryLoad(const QString &path, QList< Task > &tasks, int &nextId);
  }
}

#endif
