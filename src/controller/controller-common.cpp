#include "controller.hpp"

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
}