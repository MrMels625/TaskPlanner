#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "icontroller.hpp"
#include "../storage/istorage.hpp"
#include "../view/iview.hpp"

namespace controller
{

  class Controller: public IController
  {
    Q_OBJECT

  public:
    explicit Controller(QObject *parent);
    ~Controller() override = default;

    void setStorage(storage::IStorage *storage) override;
    void setView(view::IView *view) override;
    void start() override;

  public slots:
    void onViewReady() override;
    void onTaskAddRequested(const storage::Task &task) override;
    void onTaskEditRequested(int taskId) override;
    void onTaskUpdateRequested(const storage::Task &task) override;
    void onTaskDeleteRequested(int taskId) override;
    void onCompleteRequested(int taskId) override;
    void onDateSelected(const QDate &date) override;
    void onSortRequested(storage::Criterion criterion) override;
    void onFilterChanged(storage::Filter filter, const QVariant &value) override;

  private:
    storage::IStorage *m_storage;
    view::IView *m_view;
  };

}

#endif
