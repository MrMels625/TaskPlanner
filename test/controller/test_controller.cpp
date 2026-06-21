#include <QtTest>
#include <QSignalSpy>
#include "controller.hpp"
#include "mockstorage.hpp"
#include "mockview.hpp"

class ControllerTest: public QObject
{
  Q_OBJECT

private slots:
  void init();
  void cleanup();

  void setStorage_nullptr_doesNotCrash();
  void setView_nullptr_doesNotCrash();
  void start_withoutDependencies_doesNotCrash();

  void onViewReady_showsAllTasksByDefault();
  void onViewReady_withoutDependencies_doesNotCrash();

  void onTaskAddRequested_validTask_addsToStorage();
  void onTaskAddRequested_emptyName_rejectsAndShowsError();
  void onTaskAddRequested_invalidDeadline_rejectsAndShowsError();
  void onTaskAddRequested_validTask_refreshesView();

  void onTaskEditRequested_existingTask_showsForm();
  void onTaskEditRequested_missingTask_showsError();

  void onTaskUpdateRequested_validTask_updatesStorage();
  void onTaskUpdateRequested_emptyName_rejectsAndShowsError();

  void onTaskDeleteRequested_existingTask_removesAndNotifies();
  void onTaskDeleteRequested_missingTask_stillRemovesQuietly();

  void onCompleteRequested_existingTask_togglesCompletion();
  void onCompleteRequested_missingTask_showsError();
  void onCompleteRequested_calledTwice_togglesBackAndForth();

  void onDateSelected_firstClick_setsTitleToSelectedDate();
  void onDateSelected_sameDataTwice_deselectsDate();
  void onDateSelected_ignoresScopeFilter();

  void onSortRequested_changesCriterionAndRefreshes();

  void onFilterChanged_showAll_setsCorrectTitle();
  void onFilterChanged_showToday_setsCorrectTitle();
  void onFilterChanged_showOverdue_setsCorrectTitle();
  void onFilterChanged_scopeFilter_resetsSelectedDate();

  void onFilterChanged_priorityFilter_doesNotResetScope();
  void onFilterChanged_priorityFilter_doesNotResetSelectedDate();
  void onFilterChanged_priorityAll_showsEverything();
  void onFilterChanged_priorityLow_filtersOnlyLowTasks();

  void onFilterChanged_search_resetsDateAndScope();
  void onFilterChanged_search_callsGetTasksFilteredWithText();

  void combination_scopeTodayPlusPriorityLow_bothApply();
  void combination_dateSelectedPlusPriority_bothApply();
  void combination_dateSelectedPlusScope_scopeIgnored();

private:
  storage::Task makeTask(int id, const QString &name, storage::Priority priority,
                         const QDateTime &deadline, bool completed = false) const;

  test::MockStorage *m_storage = nullptr;
  test::MockView *m_view = nullptr;
  controller::Controller *m_controller = nullptr;
};

void ControllerTest::init()
{
  m_storage = new test::MockStorage();
  m_view = new test::MockView();
  m_controller = new controller::Controller();
  m_controller->setStorage(m_storage);
  m_controller->setView(m_view);
}

void ControllerTest::cleanup()
{
  delete m_controller;
  delete m_view;
  delete m_storage;
  m_controller = nullptr;
  m_view = nullptr;
  m_storage = nullptr;
}

storage::Task ControllerTest::makeTask(
  int id,
  const QString &name,
  storage::Priority priority,
  const QDateTime &deadline,
  bool completed) const
{
  storage::Task task;
  task.id = id;
  task.name = name;
  task.description = "test description";
  task.discipline = "test discipline";
  task.deadline = deadline;
  task.priority = priority;
  task.completed = completed;
  return task;
}

void ControllerTest::setStorage_nullptr_doesNotCrash()
{
  controller::Controller controller;
  controller.setStorage(nullptr);
  controller.onViewReady();
  QCOMPARE(m_view->showTaskListCallCount, 0);
}

void ControllerTest::setView_nullptr_doesNotCrash()
{
  controller::Controller controller;
  controller.setStorage(m_storage);
  controller.setView(nullptr);
  controller.onViewReady();
  QCOMPARE(m_storage->getAllTasksCallCount, 0);
}

void ControllerTest::start_withoutDependencies_doesNotCrash()
{
  controller::Controller controller;
  controller.start();
  QVERIFY(true);
}

void ControllerTest::onViewReady_showsAllTasksByDefault()
{
  m_storage->setTasks({
    makeTask(1, "Task 1", storage::Priority::Low, QDateTime::currentDateTime().addDays(1))
  });

  m_controller->onViewReady();

  QCOMPARE(m_storage->getAllTasksCallCount, 2);
  QCOMPARE(m_view->showTaskListCallCount, 1);
  QCOMPARE(m_view->lastShownTasks.size(), 1);
  QCOMPARE(m_view->lastTitle, QString("Все задачи"));
}

void ControllerTest::onViewReady_withoutDependencies_doesNotCrash()
{
  controller::Controller controller;
  controller.onViewReady();
  QVERIFY(true);
}

void ControllerTest::onTaskAddRequested_validTask_addsToStorage()
{
  const storage::Task task = makeTask(
    1, "New task", storage::Priority::Medium, QDateTime::currentDateTime().addDays(2));

  m_controller->onTaskAddRequested(task);

  QCOMPARE(m_storage->addTaskCallCount, 1);
  QCOMPARE(m_storage->lastAddedTask.name, QString("New task"));
  QCOMPARE(m_view->showInfoMessageCallCount, 1);
}

void ControllerTest::onTaskAddRequested_emptyName_rejectsAndShowsError()
{
  const storage::Task task = makeTask(
    1, "", storage::Priority::Low, QDateTime::currentDateTime().addDays(1));

  m_controller->onTaskAddRequested(task);

  QCOMPARE(m_storage->addTaskCallCount, 0);
  QCOMPARE(m_view->showErrorMessageCallCount, 1);
}

void ControllerTest::onTaskAddRequested_invalidDeadline_rejectsAndShowsError()
{
  storage::Task task = makeTask(
    1, "Task without deadline", storage::Priority::Low, QDateTime());

  m_controller->onTaskAddRequested(task);

  QCOMPARE(m_storage->addTaskCallCount, 0);
  QCOMPARE(m_view->showErrorMessageCallCount, 1);
}

void ControllerTest::onTaskAddRequested_validTask_refreshesView()
{
  const storage::Task task = makeTask(
    1, "Task", storage::Priority::Low, QDateTime::currentDateTime().addDays(1));

  m_controller->onTaskAddRequested(task);

  QCOMPARE(m_view->showTaskListCallCount, 1);
  QCOMPARE(m_view->updateStatsCallCount, 1);
}

void ControllerTest::onTaskEditRequested_existingTask_showsForm()
{
  const storage::Task task = makeTask(
    42, "Editable", storage::Priority::Hard, QDateTime::currentDateTime().addDays(1));
  m_storage->setTasks({task});

  m_controller->onTaskEditRequested(42);

  QCOMPARE(m_view->showTaskCreationFormCallCount, 1);
  QVERIFY(m_view->lastFormHadTask);
  QCOMPARE(m_view->lastFormTask.id, 42);
}

void ControllerTest::onTaskEditRequested_missingTask_showsError()
{
  m_storage->setTasks({});

  m_controller->onTaskEditRequested(999);

  QCOMPARE(m_view->showTaskCreationFormCallCount, 0);
  QCOMPARE(m_view->showErrorMessageCallCount, 1);
}

void ControllerTest::onTaskUpdateRequested_validTask_updatesStorage()
{
  const storage::Task task = makeTask(
    1, "Updated", storage::Priority::Medium, QDateTime::currentDateTime().addDays(1));

  m_controller->onTaskUpdateRequested(task);

  QCOMPARE(m_storage->updateTaskCallCount, 1);
  QCOMPARE(m_storage->lastUpdatedTask.name, QString("Updated"));
}

void ControllerTest::onTaskUpdateRequested_emptyName_rejectsAndShowsError()
{
  const storage::Task task = makeTask(
    1, "   ", storage::Priority::Medium, QDateTime::currentDateTime().addDays(1));

  m_controller->onTaskUpdateRequested(task);

  QCOMPARE(m_storage->updateTaskCallCount, 0);
  QCOMPARE(m_view->showErrorMessageCallCount, 1);
}

void ControllerTest::onTaskDeleteRequested_existingTask_removesAndNotifies()
{
  const storage::Task task = makeTask(
    7, "To delete", storage::Priority::Low, QDateTime::currentDateTime().addDays(1));
  m_storage->setTasks({task});

  m_controller->onTaskDeleteRequested(7);

  QCOMPARE(m_storage->removeTaskCallCount, 1);
  QCOMPARE(m_storage->lastRemovedId, 7);
  QCOMPARE(m_view->showInfoMessageCallCount, 1);
}

void ControllerTest::onTaskDeleteRequested_missingTask_stillRemovesQuietly()
{
  m_storage->setTasks({});

  m_controller->onTaskDeleteRequested(123);

  QCOMPARE(m_storage->removeTaskCallCount, 1);
  QCOMPARE(m_view->showInfoMessageCallCount, 1);
}

void ControllerTest::onCompleteRequested_existingTask_togglesCompletion()
{
  const storage::Task task = makeTask(
    3, "Toggle me", storage::Priority::Low,
    QDateTime::currentDateTime().addDays(1), false);
  m_storage->setTasks({task});

  m_controller->onCompleteRequested(3);

  QCOMPARE(m_storage->updateTaskCallCount, 1);
  QVERIFY(m_storage->lastUpdatedTask.completed);
}

void ControllerTest::onCompleteRequested_missingTask_showsError()
{
  m_storage->setTasks({});

  m_controller->onCompleteRequested(999);

  QCOMPARE(m_storage->updateTaskCallCount, 0);
  QCOMPARE(m_view->showErrorMessageCallCount, 1);
}

void ControllerTest::onCompleteRequested_calledTwice_togglesBackAndForth()
{
  const storage::Task task = makeTask(
    3, "Toggle twice", storage::Priority::Low,
    QDateTime::currentDateTime().addDays(1), false);
  m_storage->setTasks({task});

  m_controller->onCompleteRequested(3);
  QVERIFY(m_storage->lastUpdatedTask.completed);

  m_controller->onCompleteRequested(3);
  QVERIFY(!m_storage->lastUpdatedTask.completed);
}

void ControllerTest::onDateSelected_firstClick_setsTitleToSelectedDate()
{
  const QDate date(2026, 6, 25);

  m_controller->onDateSelected(date);

  QCOMPARE(m_storage->getTasksForDateCallCount, 1);
  QCOMPARE(m_storage->lastQueriedDate, date);
  QCOMPARE(m_view->lastTitle, QString("Задачи на " + date.toString("dd.MM.yyyy")));
}

void ControllerTest::onDateSelected_sameDataTwice_deselectsDate()
{
  const QDate date(2026, 6, 25);

  m_controller->onDateSelected(date);
  QCOMPARE(m_storage->getTasksForDateCallCount, 1);

  m_controller->onDateSelected(date);

  QCOMPARE(m_view->lastTitle, QString("Все задачи"));
  QCOMPARE(m_storage->getAllTasksCallCount, 3);
}

void ControllerTest::onDateSelected_ignoresScopeFilter()
{
  m_controller->onFilterChanged(storage::Filter::ShowOverdue, QVariant());
  QCOMPARE(m_view->lastTitle, QString("Просроченные задачи"));

  const QDate date(2026, 7, 1);
  m_controller->onDateSelected(date);

  QCOMPARE(m_storage->getTasksForDateCallCount, 1);
  QCOMPARE(m_view->lastTitle, QString("Задачи на " + date.toString("dd.MM.yyyy")));
}

void ControllerTest::onSortRequested_changesCriterionAndRefreshes()
{
  m_controller->onSortRequested(storage::Criterion::Priority);

  QCOMPARE(m_storage->getSortedTasksCallCount, 1);
  QCOMPARE(m_storage->lastCriterion, storage::Criterion::Priority);
}

void ControllerTest::onFilterChanged_showAll_setsCorrectTitle()
{
  m_controller->onFilterChanged(storage::Filter::ShowAll, QVariant());
  QCOMPARE(m_view->lastTitle, QString("Все задачи"));
  QCOMPARE(m_storage->getAllTasksCallCount, 2);
}

void ControllerTest::onFilterChanged_showToday_setsCorrectTitle()
{
  m_controller->onFilterChanged(storage::Filter::ShowToday, QVariant());
  QCOMPARE(m_view->lastTitle,
           QString("Задачи на " + QDate::currentDate().toString("dd.MM.yyyy")));
  QCOMPARE(m_storage->getTasksForTodayCallCount, 2);
}

void ControllerTest::onFilterChanged_showOverdue_setsCorrectTitle()
{
  m_controller->onFilterChanged(storage::Filter::ShowOverdue, QVariant());
  QCOMPARE(m_view->lastTitle, QString("Просроченные задачи"));
  QCOMPARE(m_storage->getOverdueTasksCallCount, 1);
}

void ControllerTest::onFilterChanged_scopeFilter_resetsSelectedDate()
{
  m_controller->onDateSelected(QDate(2026, 6, 25));
  QCOMPARE(m_storage->getTasksForDateCallCount, 1);
  QCOMPARE(m_storage->getTasksForTodayCallCount, 1);

  m_controller->onFilterChanged(storage::Filter::ShowToday, QVariant());

  QCOMPARE(m_storage->getTasksForTodayCallCount, 3);
  QCOMPARE(m_view->lastTitle,
           QString("Задачи на " + QDate::currentDate().toString("dd.MM.yyyy")));

  QCOMPARE(m_storage->getTasksForDateCallCount, 1);
}

void ControllerTest::onFilterChanged_priorityFilter_doesNotResetScope()
{
  m_controller->onFilterChanged(storage::Filter::ShowToday, QVariant());
  QCOMPARE(m_storage->getTasksForTodayCallCount, 2);
  const int allTasksAfterFirst = m_storage->getAllTasksCallCount;

  m_controller->onFilterChanged(
    storage::Filter::Priority, QVariant::fromValue(storage::Priority::Hard));

  QCOMPARE(m_storage->getTasksForTodayCallCount, 4);
  QCOMPARE(m_storage->getAllTasksCallCount, allTasksAfterFirst + 1);

  QCOMPARE(m_view->lastTitle,
           QString("Задачи на " + QDate::currentDate().toString("dd.MM.yyyy")));
}

void ControllerTest::onFilterChanged_priorityFilter_doesNotResetSelectedDate()
{
  const QDate date(2026, 6, 25);
  m_controller->onDateSelected(date);
  QCOMPARE(m_storage->getTasksForDateCallCount, 1);

  m_controller->onFilterChanged(
    storage::Filter::Priority, QVariant::fromValue(storage::Priority::Low));

  QCOMPARE(m_storage->getTasksForDateCallCount, 2);
  QCOMPARE(m_view->lastTitle, QString("Задачи на " + date.toString("dd.MM.yyyy")));
}

void ControllerTest::onFilterChanged_priorityAll_showsEverything()
{
  m_storage->setTasks({
    makeTask(1, "Low", storage::Priority::Low, QDateTime::currentDateTime().addDays(1)),
    makeTask(2, "Medium", storage::Priority::Medium, QDateTime::currentDateTime().addDays(1)),
    makeTask(3, "Hard", storage::Priority::Hard, QDateTime::currentDateTime().addDays(1)),
  });

  m_controller->onFilterChanged(
    storage::Filter::Priority, QVariant::fromValue(storage::Priority::All));

  QCOMPARE(m_view->lastShownTasks.size(), 3);
}

void ControllerTest::onFilterChanged_priorityLow_filtersOnlyLowTasks()
{
  m_storage->setTasks({
    makeTask(1, "Low", storage::Priority::Low, QDateTime::currentDateTime().addDays(1)),
    makeTask(2, "Medium", storage::Priority::Medium, QDateTime::currentDateTime().addDays(1)),
    makeTask(3, "Hard", storage::Priority::Hard, QDateTime::currentDateTime().addDays(1)),
  });

  m_controller->onFilterChanged(
    storage::Filter::Priority, QVariant::fromValue(storage::Priority::Low));

  QCOMPARE(m_view->lastShownTasks.size(), 1);
  QCOMPARE(m_view->lastShownTasks.first().id, 1);
}

void ControllerTest::onFilterChanged_search_resetsDateAndScope()
{
  m_controller->onDateSelected(QDate(2026, 6, 25));
  m_controller->onFilterChanged(storage::Filter::ShowOverdue, QVariant());

  m_storage->filteredTasksToReturn = {
    makeTask(1, "Found", storage::Priority::Low, QDateTime::currentDateTime().addDays(1))
};

m_controller->onFilterChanged(storage::Filter::Search, QVariant("найти"));

QCOMPARE(m_storage->getTasksFilteredCallCount, 1);
QCOMPARE(m_storage->lastSearchText, QString("найти"));

const int todayCountBefore = m_storage->getTasksForTodayCallCount;
const int dateCountBefore = m_storage->getTasksForDateCallCount;

m_controller->onFilterChanged(storage::Filter::ShowToday, QVariant());

QCOMPARE(m_storage->getTasksForTodayCallCount, todayCountBefore + 2);
QCOMPARE(m_storage->getTasksForDateCallCount, dateCountBefore);
}

void ControllerTest::onFilterChanged_search_callsGetTasksFilteredWithText()
{
  m_storage->filteredTasksToReturn = {};

  m_controller->onFilterChanged(storage::Filter::Search, QVariant("экзамен"));

  QCOMPARE(m_storage->getTasksFilteredCallCount, 1);
  QCOMPARE(m_storage->lastSearchText, QString("экзамен"));
  QCOMPARE(m_view->showTaskListCallCount, 1);
}

void ControllerTest::combination_scopeTodayPlusPriorityLow_bothApply()
{
  m_storage->tasksForTodayToReturn = {
    makeTask(1, "Today Low", storage::Priority::Low, QDateTime::currentDateTime()),
    makeTask(2, "Today Hard", storage::Priority::Hard, QDateTime::currentDateTime()),
  };

  m_controller->onFilterChanged(storage::Filter::ShowToday, QVariant());
  const int todayCountAfterFirst = m_storage->getTasksForTodayCallCount;

  m_controller->onFilterChanged(
    storage::Filter::Priority, QVariant::fromValue(storage::Priority::Low));

  QCOMPARE(m_storage->getTasksForTodayCallCount, todayCountAfterFirst + 2);
  QCOMPARE(m_view->lastShownTasks.size(), 1);
  QCOMPARE(m_view->lastShownTasks.first().id, 1);
}

void ControllerTest::combination_dateSelectedPlusPriority_bothApply()
{
  const QDate date(2026, 6, 25);
  m_storage->setTasks({
    makeTask(1, "On date, Low", storage::Priority::Low,
             QDateTime(date, QTime(10, 0))),
    makeTask(2, "On date, Hard", storage::Priority::Hard,
             QDateTime(date, QTime(11, 0))),
  });

  m_controller->onDateSelected(date);
  m_controller->onFilterChanged(
    storage::Filter::Priority, QVariant::fromValue(storage::Priority::Hard));

  QCOMPARE(m_view->lastShownTasks.size(), 1);
  QCOMPARE(m_view->lastShownTasks.first().id, 2);
  QCOMPARE(m_view->lastTitle, QString("Задачи на " + date.toString("dd.MM.yyyy")));
}

void ControllerTest::combination_dateSelectedPlusScope_scopeIgnored()
{
  const QDate date(2026, 6, 25);

  m_controller->onDateSelected(date);
  QCOMPARE(m_storage->getTasksForDateCallCount, 1);

  m_controller->onFilterChanged(storage::Filter::ShowOverdue, QVariant());

  QCOMPARE(m_storage->getOverdueTasksCallCount, 1);
  QCOMPARE(m_view->lastTitle, QString("Просроченные задачи"));
}

QTEST_APPLESS_MAIN(ControllerTest)
#include "test_controller.moc"
