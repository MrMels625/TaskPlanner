#include "controller.hpp"
#include "../storage/istorage.hpp"

controller::Controller::Controller(QObject *parent):
  m_storage(nullptr),
  m_view(nullptr)
{}

void controller::Controller::setStorage(storage::IStorage *storage)
{
  if (!storage)
  {
    qWarning() << "Controller::setStorage: storage pointer is nullptr";
    return;
  }
  m_storage = storage;
}

void controller::Controller::setView(view::IView *view)
{
  if (!view)
  {
    qWarning() << "Controller::SetView: view pointer is nullptr";
    return;
  }
  m_view = view;
}

void controller::Controller::start()
{
  if (!m_storage || !m_view)
  {
    qCritical() << "Controller::start: storage or view module is not provided";
    qCritical() << "Storage status: " << (m_storage ? "ok" : "nullptr");
    qCritical() << "View status: " << (m_view ? "ok" : "nullptr");
    return;
  }
}

void controller::Controller::onViewReady()
{}

void controller::Controller::onTaskAddRequested(const storage::Task &task)
{}

void controller::Controller::onTaskEditRequested(int taskId)
{}

void controller::Controller::onTaskUpdateRequested(const storage::Task &task)
{}

void controller::Controller::onTaskDeleteRequested(int taskId)
{}

void controller::Controller::onCompleteRequested(int taskId)
{}

void controller::Controller::onDateSelected(const QDate &date)
{}

void controller::Controller::onSortRequested(storage::Criterion criterion)
{}

void controller::Controller::onFilterChanged(storage::Filter filter, const QVariant &value)
{}
