#ifndef IVIEW_HPP
#define IVIEW_HPP

#include <QObject>
#include "../storage/task.hpp"

namespace view
{
  class IView: public QObject
  {
    Q_OBJECT

  public:
    virtual ~IView() = default;
    virtual void showTaskList(const QList< storage::Task > &tasks) = 0;
    virtual void showTasksForDate(const QDate &date, const QList< storage:: Task > &tasks) = 0;
    virtual void showTaskCreationForm() = 0;
    virtual void showErrorMessage(const QString &message) = 0;

  signals:
    void taskAddedRequest(const storage::Task &task);
    void taskDeletedRequest(int taskId);
    void dateSelected(const QDate &date);
    void viewReady();
  };
}

#endif
