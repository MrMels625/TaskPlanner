#ifndef TASKPLANNERVIEW_HPP
#define TASKPLANNERVIEW_HPP

#include "iview.hpp"
#include "ui_taskplanner.h"

#include <QMainWindow>

namespace view
{
class TaskPlannerView: public QMainWindow, public IView
{
  Q_OBJECT

public:
  explicit TaskPlannerView(QWidget *parent = nullptr);
  ~TaskPlannerView() override = default;

  void showTaskList(const QList< storage::Task > &tasks) override;
  void showTasksForDate(const QDate &date, const QList< storage::Task > &tasks) override;
  void showTaskCreationForm() override;
  void showTaskCreationForm(const storage::Task &task) override;
  void closeTaskCreationForm() override;
  void showErrorMessage(const QString &message) override;
  void showInfoMessage(const QString &message) override;
  void updateStats(int total, int completed, int today) override;

private slots:
  void onCalendarClicked(const QDate &date);
  void onSearchTextChanged(const QString &text);
  void onFilterStateChanged(Qt::CheckState state);
  void onPriorityIndexChanged(int index);
  void onAddClicked();
  void onEditClicked();
  void onDeleteClicked();
  void onMarkCompleteClicked();
  void onSortClicked();
  void onFormSaveClicked();
  void onFormCancelClicked();

private:
  void connectSignals();
  void setupFilterLogic();
  storage::Task formToTask() const;
  void taskToForm(const storage::Task &task);
  void clearFormFields();
  storage::Priority indexToPriority(int index) const;
  int priorityToIndex(storage::Priority priority) const;
  int getSelectedTaskId() const;

  Ui::TaskPlanner *ui;
  int m_currentTaskId;
  storage::Criterion m_currentSortCriterion;
};
}

#endif
