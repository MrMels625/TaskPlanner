#include "taskplannerview.hpp"

#include <QDate>
#include <QListWidgetItem>
#include <QTimer>

view::TaskPlannerView::TaskPlannerView(QWidget *parent):
    QMainWindow(parent),
    IView(parent),
    ui(new Ui::TaskPlanner),
    m_currentTaskId(-1),
    m_currentSortCriterion(storage::Criterion::Date)
{
  ui->setupUi(this);
  connectSignals();
  setupFilterLogic();
  ui->frameTaskForm->setVisible(false);
  QTimer::singleShot(0, [this]() { emit viewReady(); });
}

void view::TaskPlannerView::showTaskList(const QList< storage::Task > &tasks)
{
  ui->listWidgetTasks->clear();
  for (const storage::Task &task: tasks)
  {
    QString line = "[" + QString::number(task.id) + "] ";
    line += task.name;
    line += " | " + task.deadline.toString("dd.MM.yyyy HH:mm");
    line += task.priority != storage::Priority::All ? " | " + QString::number(static_cast<int>(task.priority)) : "";
    line += task.completed ? " | ✅" : " | ⬜";

    QListWidgetItem *item = new QListWidgetItem(line, ui->listWidgetTasks);
    item->setData(Qt::UserRole, task.id);
  }
}

void view::TaskPlannerView::showTasksForDate(const QDate &date, const QList< storage::Task > &tasks)
{
  ui->labelTaskListTitle->setText("📋 ЗАДАЧИ НА " + date.toString("dd.MM.yyyy"));
  showTaskList(tasks);
}

void view::TaskPlannerView::showTaskCreationForm()
{
  clearFormFields();
  m_currentTaskId = -1;
  ui->frameTaskForm->setVisible(true);
}

void view::TaskPlannerView::showTaskCreationForm(const storage::Task &task)
{
  taskToForm(task);
  m_currentTaskId = task.id;
  ui->frameTaskForm->setVisible(true);
}

void view::TaskPlannerView::closeTaskCreationForm()
{
  ui->frameTaskForm->setVisible(false);
  clearFormFields();
}

void view::TaskPlannerView::showErrorMessage(const QString &message)
{
  ui->labelStatus->setStyleSheet("color: #e53935; font-weight: bold;");
  ui->labelStatus->setText("⚠️ " + message);
}

void view::TaskPlannerView::showInfoMessage(const QString &message)
{
  ui->labelStatus->setStyleSheet("color: #43a047; font-weight: bold;");
  ui->labelStatus->setText("ℹ️ " + message);
}

void view::TaskPlannerView::updateStats(int total, int completed, int today)
{
  ui->labelStatsTotal->setText("Всего задач: " + QString::number(total));
  ui->labelStatsCompleted->setText("✅ Выполнено: " + QString::number(completed));
  ui->labelStatsToday->setText("📌 На сегодня: " + QString::number(today));
}

void view::TaskPlannerView::onCalendarClicked(const QDate &date)
{
  emit dateSelected(date);
}

void view::TaskPlannerView::onSearchTextChanged(const QString &text)
{
  emit filterChanged(storage::Filter::Search, QVariant(text));
}

void view::TaskPlannerView::onFilterStateChanged(int state)
{
  Q_UNUSED(state)
  if (ui->checkBoxAll->isChecked())
  {
    emit filterChanged(storage::Filter::ShowAll, QVariant());
  }
  else if (ui->checkBoxToday->isChecked())
  {
    emit filterChanged(storage::Filter::ShowToday, QVariant());
  }
  else if (ui->checkBoxOverdue->isChecked())
  {
    emit filterChanged(storage::Filter::ShowOverdue, QVariant());
  }
}

void view::TaskPlannerView::onPriorityIndexChanged(int index)
{
  const storage::Priority priority = indexToPriority(index);
  emit filterChanged(storage::Filter::Priority, QVariant::fromValue(priority));
}

void view::TaskPlannerView::onAddClicked()
{
  showTaskCreationForm();
}

void view::TaskPlannerView::onEditClicked()
{
  const int taskId = getSelectedTaskId();
  if (taskId != -1)
  {
    emit taskEditRequested(taskId);
  }
}

void view::TaskPlannerView::onDeleteClicked()
{
  const int taskId = getSelectedTaskId();
  if (taskId != -1)
  {
    emit taskDeleteRequested(taskId);
  }
}

void view::TaskPlannerView::onMarkCompleteClicked()
{
  const int taskId = getSelectedTaskId();
  if (taskId != -1)
  {
    emit taskCompleteRequested(taskId);
  }
}

void view::TaskPlannerView::onSortClicked()
{
  if (m_currentSortCriterion == storage::Criterion::Date)
  {
    m_currentSortCriterion = storage::Criterion::Priority;
  }
  else if (m_currentSortCriterion == storage::Criterion::Priority)
  {
    m_currentSortCriterion = storage::Criterion::Completed;
  }
  else
  {
    m_currentSortCriterion = storage::Criterion::Date;
  }
  emit sortRequested(m_currentSortCriterion);
}

void view::TaskPlannerView::onFormSaveClicked()
{
  storage::Task task = formToTask();
  if (m_currentTaskId == -1)
  {
    emit taskAddRequested(task);
  }
  else
  {
    task.id = m_currentTaskId;
    emit taskUpdateRequested(task);
  }
  closeTaskCreationForm();
}

void view::TaskPlannerView::onFormCancelClicked()
{
  closeTaskCreationForm();
}
