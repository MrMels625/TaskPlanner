#ifndef SERIAL_UTILS_HPP
#define SERIAL_UTILS_HPP
#include <QJsonObject>
#include <QString>
#include "task.hpp"
#include "istorage.hpp"

namespace storage
{
  namespace serial
  {
    inline constexpr auto k_fileName = "tasks.json";
    inline constexpr auto k_fileNameBackup = "tasks.backup.json";
    inline constexpr auto k_gamificationFileName = "gamification.json";
    inline constexpr auto k_gamificationFileNameBackup = "gamification.backup.json";

    QString filePath(const QString &name);

    QJsonObject taskToJson(const Task &task);
    Task taskFromJson(const QJsonObject &obj);
    bool tryLoad(const QString &path, QList< Task > &tasks, int &nextId);

    QJsonObject progressToJson(const UserProgress &progress);
    UserProgress progressFromJson(const QJsonObject &obj);

    QJsonObject achievementToJson(const Achievement &achievement);
    Achievement achievementFromJson(const QJsonObject &obj);

    bool tryLoadGamification(const QString &path, UserProgress &progress, QList< Achievement > &achievements);
  }
}

#endif
