#ifndef IVIEW_HPP
#define IVIEW_HPP

#include <QDate>
#include <QList>
#include <QObject>
#include "../storage/istorage.hpp"
#include "../storage/task.hpp"

namespace view
{
  class IView: public QObject
  {
    Q_OBJECT

  public:
    virtual ~IView() = default;
    virtual void showTaskList(const QList< storage::Task > &tasks) = 0;
    virtual void showTasksForDate(const QDate &date, const QList< storage::Task > &tasks) = 0;
    virtual void showTaskCreationForm() = 0;
    virtual void showErrorMessage(const QString &message) = 0;
    virtual void showInfoMessage(const QString &message) = 0;

  signals:
    void viewReady();
    void taskAddRequested(const storage::Task &task);
    void taskEditRequested(int taskID);
    void taskUpdateRequested(const storage::Task &task);
    void taskDeleteRequested(int taskId);
    void taskCompleteRequested(int taskId);
    void dateSelected(const QDate &date);
    void sortRequested(storage::Criterion criterion);
    void filterChanged(storage::Filter filter, const QVariant &value);
  };
}

#endif
