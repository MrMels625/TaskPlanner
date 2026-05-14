#include "controller.hpp"

#include <cassert>
#include <algorithm>
#include <QDate>
#include <QList>
#include <QVariant>
#include <QDebug>

#include "../storage/istorage.hpp"
#include "../view/iview.hpp"
#include "../view/taskplannerview.hpp"

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

  auto *view_ptr = dynamic_cast< view::TaskPlannerView* >(m_view);
  if (!view_ptr)
  {
    qCritical() << "Controller::start: view is not a TaskPlannerView instance";
    return;
  }

  QObject::connect(view_ptr, &view::TaskPlannerView::viewReady, this, &controller::Controller::onViewReady);
  QObject::connect(view_ptr, &view::TaskPlannerView::taskAddRequested, this, &controller::Controller::onTaskAddRequested);
  QObject::connect(view_ptr, &view::TaskPlannerView::taskEditRequested, this, &controller::Controller::onTaskEditRequested);
  QObject::connect(view_ptr, &view::TaskPlannerView::taskViewRequested, this, &controller::Controller::onTaskViewRequested);
  QObject::connect(view_ptr, &view::TaskPlannerView::taskUpdateRequested, this, &controller::Controller::onTaskUpdateRequested);
  QObject::connect(view_ptr, &view::TaskPlannerView::taskDeleteRequested, this, &controller::Controller::onTaskDeleteRequested);
  QObject::connect(view_ptr, &view::TaskPlannerView::taskCompleteRequested, this, &controller::Controller::onCompleteRequested);
  QObject::connect(view_ptr, &view::TaskPlannerView::dateSelected, this, &controller::Controller::onDateSelected);
  QObject::connect(view_ptr, &view::TaskPlannerView::sortRequested, this, &controller::Controller::onSortRequested);
  QObject::connect(view_ptr, &view::TaskPlannerView::filterChanged, this, &controller::Controller::onFilterChanged);
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
  if (!m_view)
  {
    qCritical() << "Controller::validateTask: m_view is nullptr";
    return false;
  }
  if (task.name.trimmed().isEmpty())
  {
    m_view->showErrorMessage("Task name can't be empty.");
    return false;
  }
  if (!task.deadline.isValid())
  {
    m_view->showErrorMessage("Specify the correct deadline for completing the task.");
    return false;
  }
  return true;
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
  {
    tasks = m_storage->getAllTasks();
    break;
  }
  case storage::Filter::ShowToday:
  {
    tasks = m_storage->getTasksForToday();
    break;
  }
  case storage::Filter::ShowOverdue:
  {
    tasks = m_storage->getOverdueTasks();
    break;
  }
  case storage::Filter::Search:
  {
    assert(m_filterValue.canConvert< QString >());
    const QString text = m_filterValue.toString();
    tasks = m_storage->getTasksFiltered(text, false, false, storage::Priority::All);
    break;
  }
  case storage::Filter::Priority:
  {
    assert(m_filterValue.canConvert< storage::Priority >());
    const auto priority = m_filterValue.value< storage::Priority >();
    tasks = m_storage->getTasksFiltered(QString(), false, false, priority);
    break;
  }
  }

  tasks = m_storage->getSortedTasks(tasks, m_activeCriterion);
  m_view->showTaskList(tasks);
  updateStats();
}

void controller::Controller::updateStats()
{
  if (!checkReady())
  {
    return;
  }

  const QList< storage::Task > all_tasks = m_storage->getAllTasks();
  const int completed_count = std::count_if(all_tasks.begin(), all_tasks.end(),
                                            [](const storage::Task &task){ return task.completed; });
  const int today_count = m_storage->getTasksForToday().size();

  m_view->updateStats(all_tasks.size(), completed_count, today_count);
}

void controller::Controller::onViewReady()
{
  if (!checkReady())
  {
    return;
  }
  refreshView();
}

void controller::Controller::onTaskAddRequested(const storage::Task &task)
{
  if (!checkReady())
  {
    return;
  }
  if (!validateTask(task))
  {
    return;
  }

  m_storage->addTask(task);
  m_view->showInfoMessage("Task \"" + task.name + "\" added successfully.");
  refreshView();
}

void controller::Controller::onTaskEditRequested(int task_id)
{
  if (!checkReady())
  {
    return;
  }

  const QList< storage::Task > all_tasks = m_storage->getAllTasks();

  for (const storage::Task &task: all_tasks)
  {
    if (task.id == task_id)
    {
      m_view->showTaskCreationForm(task);
      return;
    }
  }

  m_view->showErrorMessage("Task with ID " + QString::number(task_id) + " not found.");
}

void controller::Controller::onTaskViewRequested(int task_id)
{
  if (!checkReady())
  {
    return;
  }

  const QList< storage::Task > all_tasks = m_storage->getAllTasks();

  for (const storage::Task &task: all_tasks)
  {
    if (task.id == task_id)
    {
      auto *view_ptr = dynamic_cast< view::TaskPlannerView* >(m_view);
      if (view_ptr)
      {
        view_ptr->showTaskDetails(task);
      }
      return;
    }
  }

  m_view->showErrorMessage("Task with ID " + QString::number(task_id) + " not found.");
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
  m_view->showInfoMessage("Task \"" + task.name + "\" successfully updated.");
  refreshView();
}

void controller::Controller::onTaskDeleteRequested(int task_id)
{
  if (!checkReady())
  {
    return;
  }

  const QList< storage::Task > all_tasks = m_storage->getAllTasks();
  QString task_name;

  for (const storage::Task &task: all_tasks)
  {
    if (task.id == task_id)
    {
      task_name = task.name;
      break;
    }
  }

  m_storage->removeTask(task_id);

  if (!task_name.isEmpty())
  {
    m_view->showInfoMessage("Task \"" + task_name + "\" deleted successfully.");
  }
  else
  {
    m_view->showInfoMessage("Task deleted successfully.");
  }

  refreshView();
}

void controller::Controller::onCompleteRequested(int task_id)
{
  if (!checkReady())
  {
    return;
  }

  QList< storage::Task > all_tasks = m_storage->getAllTasks();

  for (storage::Task &task: all_tasks)
  {
    if (task.id == task_id)
    {
      task.completed = !task.completed;
      m_storage->updateTask(task);
      const QString message = task.completed ? "Task \"" + task.name + "\" marked as completed." :
        "Task \"" + task.name + "\" marked as not completed.";
      m_view->showInfoMessage(message);
      refreshView();
      return;
    }
  }

  m_view->showErrorMessage("Task with ID " + QString::number(task_id) + " not found.");
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
