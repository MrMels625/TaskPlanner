#include "controller.hpp"
#include "../storage/istorage.hpp"
#include "../view/iview.hpp"

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
    qWarning() << "Controller::setView: view pointer is nullptr";
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
  connectSignals();
}


bool controller::Controller::checkReady() const
{
  if (!m_storage || !m_view)
  {
    qCritical() << "Controller::checkReady: storage or view module is not provided";
    qCritical() << "Storage status:" << (m_storage ? "ok" : "nullptr");
    qCritical() << "View status:" << (m_view ? "ok" : "nullptr");
    return false;
  }
  return true;
}

bool controller::Controller::validateTask(const storage::Task &task) const
{
  if (task.name.trimmed().isEmpty())
  {
    m_view->showErrorMessage("Название задачи не может быть пустым.");
    return false;
  }
  if (!task.deadline.isValid())
  {
    m_view->showErrorMessage("Укажите корректный срок выполнения задачи.");
    return false;
  }
  return true;
}

void controller::Controller::connectSignals()
{
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

void controller::Controller::refreshView()
{
  if (!checkReady())
  {
    return;
  }

  QList< storage::Task > tasks;

  switch (m_activeFilter)
  {
  case storage::Filter::ShowAll:
    tasks = m_storage->getAllTasks();
    break;

  case storage::Filter::ShowToday:
    tasks = m_storage->getTasksForToday();
    break;

  case storage::Filter::ShowOverdue:
    tasks = m_storage->getOverdueTasks();
    break;

  case storage::Filter::Search:
  {
    const QString text = m_filterValue.toString();
    tasks = m_storage->getTasksFiltered(text, false, false, storage::Priority::All);
    break;
  }

  case storage::Filter::Priority:
  {
    const auto priority = m_filterValue.value< storage::Priority >();
    tasks = m_storage->getTasksFiltered(QString(), false, false, priority);
    break;
  }
  }

  tasks = m_storage->getSortedTasks(tasks, m_activeCriterion);
  m_view->showTaskList(tasks);
}


void controller::Controller::onTaskEditRequested(int taskId)
{
  if (!checkReady())
  {
    return;
  }

  const QList< storage::Task > all = m_storage->getAllTasks();

  for (const storage::Task &task : all)
  {
    if (task.id == taskId)
    {
      m_view->showTaskCreationForm();
      return;
    }
  }

  m_view->showErrorMessage("Задача с идентификатором " + QString::number(taskId) + " не найдена.");
}

void controller::Controller::onTaskUpdateRequested(const storage::Task &task)
{
  if (!checkReady())
  {
    return;
  }
  if (!validateTask(task))
  {
    return;
  }

  m_storage->updateTask(task);
  m_view->showInfoMessage("Задача «" + task.name + "» успешно обновлена.");
  refreshView();
}



void controller::Controller::onDateSelected(const QDate &date)
{
  if (!checkReady())
  {
    return;
  }

  const QList< storage::Task > tasks = m_storage->getTasksForDate(date);
  m_view->showTasksForDate(date, tasks);
}

void controller::Controller::onSortRequested(storage::Criterion criterion)
{
  if (!checkReady())
  {
    return;
  }

  m_activeCriterion = criterion;
  refreshView();
}

void controller::Controller::onFilterChanged(storage::Filter filter, const QVariant &value)
{
  if (!checkReady())
  {
    return;
  }

  m_activeFilter = filter;
  m_filterValue = value;
  refreshView();
}