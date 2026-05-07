#include "task.hpp"

bool storage::Task::operator<(const Task &other) const
{
  if (deadline != other.deadline)
  {
    return deadline < other.deadline;
  }
  return priority > other.priority;
}

bool storage::Task::operator==(const Task &other) const
{
  return id == other.id;
}
