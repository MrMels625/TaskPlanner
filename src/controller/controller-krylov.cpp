#include "controller.hpp"

void controller::Controller::onTaskEditRequested(int taskId)
{
  if (!checkReady())
  {
    return;
  }

  const QList< storage::Task > all = m_storage->getAllTasks();

  for (const storage::Task &task: all)
  {
    if (task.id == taskId)
    {
      m_view->showTaskCreationForm();
      return;
    }
  }

  m_view->showErrorMessage("Task with ID " + QString::number(taskId) + " not found.");
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
  m_view->showInfoMessage("Task «" + task.name + "» successfully updated.");
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