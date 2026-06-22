#ifndef TASKPLANNERVIEW_HPP
#define TASKPLANNERVIEW_HPP

#include "iview.hpp"
#include "gamification_view.hpp"
#include "ui_taskplanner.h"
#include <QMainWindow>
#include <QTimer>

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
    void setTaskListTitle(const QString &title) override;
    void showTaskCreationForm() override;
    void showTaskCreationForm(const storage::Task &task) override;
    void showTaskDetails(const storage::Task &task) override;
    void closeTaskCreationForm() override;
    void showErrorMessage(const QString &message) override;
    void showInfoMessage(const QString &message) override;
    void updateStats(int total, int completed, int today) override;

    void showUserLevel(int level, int currentXP, int xpToNext) override;
    void showStreak(int days) override;
    void showUserTitle(const QString &title) override;
    void showXPNotification(int amount, const QString &reason) override;
    void showAchievementUnlocked(const storage::Achievement &achievement) override;
    void showAchievementsList(const QList< storage::Achievement > &achievements) override;
    void showCampusMap(const QList< QString > &unlockedLocations) override;
    void showLocationUnlocked(const QString &locationName) override;
    void showLevelUpAnimation(int newLevel, const QString &newTitle) override;
    void updateGamificationPanel() override;

  signals:
    void viewReady();
    void taskAddRequested(const storage::Task &task);
    void taskEditRequested(int taskId);
    void taskViewRequested(int taskId);
    void taskUpdateRequested(const storage::Task &task);
    void taskDeleteRequested(int taskId);
    void taskCompleteRequested(int taskId);
    void dateSelected(const QDate &date);
    void sortRequested(storage::Criterion criterion);
    void filterChanged(storage::Filter filter, const QVariant &value);
    void achievementsRequested();
    void mapRequested();

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
    void clearStatusMessage();
    void onGamificationAchievementsRequested();
    void onGamificationMapRequested();
    void onGroupBoxAchievementsClicked(bool checked);

  private:
    void connectSignals();
    void setupFilterLogic();
    void setupGamification();
    storage::Task formToTask() const;
    void taskToForm(const storage::Task &task);
    void clearFormFields();
    void setFormReadOnly(bool readOnly);
    storage::Priority indexToPriority(int index) const;
    int priorityToIndex(storage::Priority priority) const;
    int getSelectedTaskId() const;

    Ui::TaskPlanner *ui;
    GamificationView *m_gamificationView;
    int m_currentTaskId;
    storage::Criterion m_currentSortCriterion;
    QTimer *m_statusTimer;
    bool m_isFormReadOnly;
  };

}

#endif
