#ifndef IGAMIFICATION_CONTROLLER_HPP
#define IGAMIFICATION_CONTROLLER_HPP

#include <QObject>
#include <QDate>
#include "../storage/igamification_storage.hpp"
#include "../view/igamification_view.hpp"

namespace controller
{
  class IGamificationController: public QObject
  {
    Q_OBJECT

  public:
    explicit IGamificationController(QObject *parent = nullptr):
      QObject(parent)
    {}

    virtual ~IGamificationController() = default;

    virtual void setStorage(storage::IGamificationStorage *storage) = 0;
    virtual void setView(view::IGamificationView *view) = 0;
    virtual void initialize() = 0;

  public slots:
    virtual void onTaskCompleted() = 0;
    virtual void onDailyTasksCompleted() = 0;

    virtual void onAchievementsRequested() = 0;
    virtual void onMapRequested() = 0;

    virtual void onNewDay(const QDate &date) = 0;
  };

}

#endif
