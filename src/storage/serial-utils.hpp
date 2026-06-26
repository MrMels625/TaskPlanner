#ifndef SERIAL_UTILS_HPP
#define SERIAL_UTILS_HPP
#include <QJsonObject>
#include <QString>
#include "task.hpp"

namespace storage
{
  namespace serial
  {
    inline constexpr auto k_fileName = "tasks.json";
    inline constexpr auto k_fileNameBackup = "tasks.backup.json";

    void setBaseDirOverride(const QString &path);

    QString filePath(const QString &name);
    QJsonObject taskToJson(const Task &task);
    Task taskFromJson(const QJsonObject &obj);
    bool tryLoad(const QString &path, QList<Task> &tasks, int &nextId);
  }
}

#endif
