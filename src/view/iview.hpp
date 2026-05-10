#ifndef IVIEW_HPP
#define IVIEW_HPP

#include <QDate>
#include <QList>
#include <QString>
#include <QVariant>

#include "../storage/istorage.hpp"
#include "../storage/task.hpp"

namespace view
{
  class IView
  {
  public:
    virtual ~IView() = default;

    virtual void showTaskList(const QList< storage::Task > &tasks) = 0;
    virtual void showTasksForDate(const QDate &date, const QList< storage::Task > &tasks) = 0;
    virtual void showTaskCreationForm() = 0;
    virtual void showTaskCreationForm(const storage::Task &task) = 0;
    virtual void closeTaskCreationForm() = 0;
    virtual void showErrorMessage(const QString &message) = 0;
    virtual void showInfoMessage(const QString &message) = 0;
    virtual void updateStats(int total, int completed, int today) = 0;
  };
}

#endif
