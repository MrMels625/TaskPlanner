#include "controller.hpp"
#include "../storage/istorage.hpp"

controller::Controller::Controller(QObject *parent):
  IController(parent),
  m_storage(nullptr),
  m_view(nullptr),
  m_activeFilter(storage::Filter::ShowAll),
  m_filterValue(QVariant()),
  m_activeCriterion(storage::Criterion::Date)
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
  if (!checkReady())
  {
    return;
  }
  connect(m_view, &view::IView::viewReady, this, &Controller::onViewReady);
  connect(m_view, &view::IView::taskAddRequested, this, &Controller::onTaskAddRequested);
  connect(m_view, &view::IView::taskEditRequested, this, &Controller::onTaskEditRequested);
  connect(m_view, &view::IView::taskUpdateRequested, this, &Controller::onTaskUpdateRequested);
  connect(m_view, &view::IView::taskDeleteRequested, this, &Controller::onTaskDeleteRequested);
  connect(m_view, &view::IView::taskCompleteRequested, this, &Controller::onCompleteRequested);
  connect(m_view, &view::IView::dateSelected, this, &Controller::onDateSelected);
  connect(m_view, &view::IView::sortRequested, this, &Controller::onSortRequested);
  connect(m_view, &view::IView::filterChanged, this, &Controller::onFilterChanged);
}

void controller::Controller::onTaskEditRequested(int taskId)
{}

void controller::Controller::onTaskUpdateRequested(const storage::Task &task)
{}

void controller::Controller::onDateSelected(const QDate &date)
{}

void controller::Controller::onSortRequested(storage::Criterion criterion)
{}

void controller::Controller::onFilterChanged(storage::Filter filter, const QVariant &value)
{}
