#ifndef ICONTROLLER_HPP
#define ICONTROLLER_HPP

#include <QObject>
#include "../view/iview.hpp"
#include "../storage/istorage.hpp"

namespace controller
{
  class IController: public QObject
  {
    Q_OBJECT

  public:
    explicit IController(QObject *parent = nullptr):
      QObject(parent)
    {}
    virtual ~IController() = default;
    virtual void setStorage(storage::IStorage *storage) = 0;
    virtual void setView(view::IView *view) = 0;
    virtual void start() = 0;

  public slots:
    virtual void onViewReady() = 0;
    virtual void onTaskAddRequested(const storage::Task &task) = 0;
    virtual void onTaskEditRequested(int taskId) = 0;
    virtual void onTaskUpdateRequested(const storage::Task &task) = 0;
    virtual void onTaskDeleteRequested(int taskId) = 0;
    virtual void onCompleteRequested(int taskId) = 0;
    virtual void onDateSelected(const QDate &date) = 0;
    virtual void onSortRequested(storage::Criterion criterion) = 0;
    virtual void onFilterChanged(storage::Filter filter, const QVariant &value) = 0;
  };
}

#endif
