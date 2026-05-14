#include "taskplannerview.hpp"

#include <QCalendarWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QLineEdit>
#include <QListWidgetItem>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>
#include <Qt>

view::TaskPlannerView::TaskPlannerView(QWidget *parent):
    QMainWindow(parent),
    IView(),
    ui(new Ui::TaskPlanner),
    m_currentTaskId(-1),
    m_currentSortCriterion(storage::Criterion::Date)
{
  ui->setupUi(this);
  connectSignals();
  setupFilterLogic();
  ui->frameTaskForm->setVisible(false);

  QTimer::singleShot(0, [this](){ emit viewReady(); });
}

void view::TaskPlannerView::showTaskList(const QList< storage::Task > &tasks)
{
  ui->listWidgetTasks->clear();
  for (const storage::Task &task: tasks)
  {
    QString line = "[" + QString::number(task.id) + "] ";
    line += task.name;

    if (!task.discipline.trimmed().isEmpty())
    {
      line += " | 📚 " + task.discipline;
    }

    line += " | " + task.deadline.toString("dd.MM.yyyy HH:mm");

    if (task.priority != storage::Priority::All)
    {
      line += " | Priority: " + QString::number(static_cast<int>(task.priority));
    }

    line += task.completed ? " | ✅" : " | ⬜";

    auto *item = new QListWidgetItem(line, ui->listWidgetTasks);
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

void view::TaskPlannerView::onFilterStateChanged(Qt::CheckState state)
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

void view::TaskPlannerView::connectSignals()
{
  QObject::connect(ui->calendarWidget, &QCalendarWidget::clicked, this, &TaskPlannerView::onCalendarClicked);
  QObject::connect(ui->lineEditSearch, &QLineEdit::textChanged, this, &TaskPlannerView::onSearchTextChanged);
  QObject::connect(ui->checkBoxAll, &QCheckBox::checkStateChanged, this, &TaskPlannerView::onFilterStateChanged);
  QObject::connect(ui->checkBoxToday, &QCheckBox::checkStateChanged, this, &TaskPlannerView::onFilterStateChanged);
  QObject::connect(ui->checkBoxOverdue, &QCheckBox::checkStateChanged, this, &TaskPlannerView::onFilterStateChanged);
  QObject::connect(ui->comboBoxPriority, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TaskPlannerView::onPriorityIndexChanged);
  QObject::connect(ui->btnAdd, &QPushButton::clicked, this, &TaskPlannerView::onAddClicked);
  QObject::connect(ui->btnEdit, &QPushButton::clicked, this, &TaskPlannerView::onEditClicked);
  QObject::connect(ui->btnDelete, &QPushButton::clicked, this, &TaskPlannerView::onDeleteClicked);
  QObject::connect(ui->btnMarkComplete, &QPushButton::clicked, this, &TaskPlannerView::onMarkCompleteClicked);
  QObject::connect(ui->btnSort, &QPushButton::clicked, this, &TaskPlannerView::onSortClicked);
  QObject::connect(ui->btnFormSave, &QPushButton::clicked, this, &TaskPlannerView::onFormSaveClicked);
  QObject::connect(ui->btnFormCancel, &QPushButton::clicked, this, &TaskPlannerView::onFormCancelClicked);
}

void view::TaskPlannerView::setupFilterLogic()
{
  ui->comboBoxPriority->clear();
  ui->comboBoxPriority->addItem("🔴 Высокий", static_cast<int>(storage::Priority::Hard));
  ui->comboBoxPriority->addItem("🟡 Средний", static_cast<int>(storage::Priority::Medium));
  ui->comboBoxPriority->addItem("🟢 Низкий", static_cast<int>(storage::Priority::Low));

  ui->comboBoxFormPriority->clear();
  ui->comboBoxFormPriority->addItem("🟢 Низкий");
  ui->comboBoxFormPriority->addItem("🟡 Средний");
  ui->comboBoxFormPriority->addItem("🔴 Высокий");
}

storage::Task view::TaskPlannerView::formToTask() const
{
  storage::Task task;
  task.name = ui->lineEditFormName->text();
  task.description = ui->textEditFormDescription->toPlainText();
  task.discipline = ui->lineEditFormDiscipline->text();
  task.deadline = ui->dateTimeFormDeadline->dateTime();
  task.priority = indexToPriority(ui->comboBoxFormPriority->currentIndex());
  task.completed = false;

  const QStringList rawTags = ui->lineEditFormTags->text().split(",", Qt::SkipEmptyParts);
  for (const QString &tag: rawTags)
  {
    task.tags.append(tag.trimmed());
  }

  const QString rawLinks = ui->lineEditFormLinks->text();
  if (!rawLinks.isEmpty())
  {
    const QStringList links = rawLinks.split(",", Qt::SkipEmptyParts);
    for (const QString &link: links)
    {
      task.tags.append("link:" + link.trimmed());
    }
  }

  return task;
}

void view::TaskPlannerView::taskToForm(const storage::Task &task)
{
  ui->lineEditFormName->setText(task.name);
  ui->textEditFormDescription->setPlainText(task.description);
  ui->lineEditFormDiscipline->setText(task.discipline);
  ui->dateTimeFormDeadline->setDateTime(task.deadline);
  ui->comboBoxFormPriority->setCurrentIndex(priorityToIndex(task.priority));

  QString tagsStr;
  for (const QString &tag: task.tags)
  {
    if (!tag.startsWith("link:"))
    {
      tagsStr += tag + ", ";
    }
  }
  ui->lineEditFormTags->setText(tagsStr);

  QString linksStr;
  for (const QString &tag: task.tags)
  {
    if (tag.startsWith("link:"))
    {
      linksStr += tag.mid(5) + ", ";
    }
  }
  ui->lineEditFormLinks->setText(linksStr);
}

void view::TaskPlannerView::clearFormFields()
{
  ui->lineEditFormName->clear();
  ui->textEditFormDescription->clear();
  ui->lineEditFormDiscipline->clear();
  ui->dateTimeFormDeadline->setDateTime(QDateTime::currentDateTime());
  ui->comboBoxFormPriority->setCurrentIndex(0);
  ui->lineEditFormTags->clear();
  ui->lineEditFormLinks->clear();
}

storage::Priority view::TaskPlannerView::indexToPriority(int index) const
{
  switch (index)
  {
  case 0:
  {
    return storage::Priority::Low;
  }
  case 1:
  {
    return storage::Priority::Medium;
  }
  case 2:
  {
    return storage::Priority::Hard;
  }
  default:
  {
    return storage::Priority::Low;
  }
  }
}

int view::TaskPlannerView::priorityToIndex(storage::Priority priority) const
{
  switch (priority)
  {
  case storage::Priority::Low:
  {
    return 0;
  }
  case storage::Priority::Medium:
  {
    return 1;
  }
  case storage::Priority::Hard:
  {
    return 2;
  }
  default:
  {
    return 0;
  }
  }
}

int view::TaskPlannerView::getSelectedTaskId() const
{
  const QListWidgetItem *item = ui->listWidgetTasks->currentItem();
  if (!item)
  {
    return -1;
  }
  return item->data(Qt::UserRole).toInt();
}

