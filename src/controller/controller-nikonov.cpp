#include "controller.hpp"

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
  m_view->showInfoMessage("Task «" + task.name + "» added successfully.");
  refreshView();
}

void controller::Controller::onTaskDeleteRequested(int taskId)
{
  if (!checkReady())
  {
    return;
  }

  m_storage->removeTask(taskId);
  refreshView();
}

void controller::Controller::onCompleteRequested(int taskId)
{
  if (!checkReady())
  {
    return;
  }

  QList< storage::Task > all = m_storage->getAllTasks();

  for (storage::Task &task: all)
  {
    if (task.id == taskId)
    {
      task.completed = !task.completed;
      m_storage->updateTask(task);
      refreshView();
      return;
    }
  }

  m_view->showErrorMessage("The issue with the ID " + QString::number(taskId) + " was not found.");
}
