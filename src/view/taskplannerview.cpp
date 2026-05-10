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
