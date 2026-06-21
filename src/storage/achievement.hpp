#ifndef ACHIEVEMENT_HPP
#define ACHIEVEMENT_HPP

#include <QString>

namespace storage
{
  // TODO: временная заглушка. Заменить на реальную структуру,
  // когда модуль storage предоставит окончательную версию achievement.hpp.
  struct Achievement
  {
    int id;
    QString name;
    QString description;
    bool unlocked;

    Achievement() = default;
  };
}

#endif
