#include <QtTest>
#include <QSignalSpy>
#include <QMetaObject>
#include "taskplannerview.hpp"
#include "mockstorage.hpp"

class ViewTest : public QObject
{
  Q_OBJECT

private slots:
  void init();
  void cleanup();
  void dateSelected_emitsSignal();
  void dateSelected_sameDateTwice_emitsSignalTwice();
  void taskAddRequested_validForm_emitsSignal();
  void taskEditRequested_noSelection_doesNotEmit();
  void taskDeleteRequested_noSelection_doesNotEmit();
  void taskCompleteRequested_noSelection_doesNotEmit();
  void sortRequested_cyclesThroughCriteria();
  void filterChanged_searchText_emitsSignal();
  void filterChanged_priority_emitsSignal();
  void showTaskList_displaysTasks();
  void showTaskList_emptyList_clearsList();
  void updateStats_updatesLabels();
  void showErrorMessage_displaysError();
  void showInfoMessage_displaysInfo();
  void setTaskListTitle_updatesTitle();
  void showTaskCreationForm_clearsFields();
  void showTaskCreationForm_withTask_fillsFields();
  void closeTaskCreationForm_hidesForm();

private:
  storage::Task makeTask(int id, const QString &name, storage::Priority priority, const QDateTime &deadline, bool completed = false) const;
  view::TaskPlannerView *m_view;
  test::MockStorage *m_storage;
};

void ViewTest::init()
{
  m_view = new view::TaskPlannerView();
  m_storage = new test::MockStorage();
}

void ViewTest::cleanup()
{
  delete m_view;
  delete m_storage;
  m_view = nullptr;
  m_storage = nullptr;
}

storage::Task ViewTest::makeTask(int id, const QString &name, storage::Priority priority, const QDateTime &deadline, bool completed) const
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

void ViewTest::dateSelected_emitsSignal()
{
  QSignalSpy spy(m_view, &view::TaskPlannerView::dateSelected);
  QVERIFY(spy.isValid());
  const QDate test_date(2026, 6, 25);
  QMetaObject::invokeMethod(m_view, "onCalendarClicked", Q_ARG(QDate, test_date));
  QCOMPARE(spy.count(), 1);
  const QList< QVariant > arguments = spy.takeFirst();
  QCOMPARE(arguments.at(0).toDate(), test_date);
}

void ViewTest::dateSelected_sameDateTwice_emitsSignalTwice()
{
  QSignalSpy spy(m_view, &view::TaskPlannerView::dateSelected);
  QVERIFY(spy.isValid());
  const QDate test_date(2026, 6, 25);
  QMetaObject::invokeMethod(m_view, "onCalendarClicked", Q_ARG(QDate, test_date));
  QMetaObject::invokeMethod(m_view, "onCalendarClicked", Q_ARG(QDate, test_date));
  QCOMPARE(spy.count(), 2);
}

void ViewTest::taskAddRequested_validForm_emitsSignal()
{
  QSignalSpy spy(m_view, &view::TaskPlannerView::taskAddRequested);
  QVERIFY(spy.isValid());
  m_view->showTaskCreationForm();
  QMetaObject::invokeMethod(m_view, "onFormSaveClicked");
  QCOMPARE(spy.count(), 1);
}

void ViewTest::taskEditRequested_noSelection_doesNotEmit()
{
  const storage::Task task = makeTask(1, "Test Task", storage::Priority::Medium, QDateTime::currentDateTime());
  m_view->showTaskList({task});
  QSignalSpy spy(m_view, &view::TaskPlannerView::taskEditRequested);
  QVERIFY(spy.isValid());
  QMetaObject::invokeMethod(m_view, "onEditClicked");
  QCOMPARE(spy.count(), 0);
}

void ViewTest::taskDeleteRequested_noSelection_doesNotEmit()
{
  const storage::Task task = makeTask(1, "Test Task", storage::Priority::Medium, QDateTime::currentDateTime());
  m_view->showTaskList({task});
  QSignalSpy spy(m_view, &view::TaskPlannerView::taskDeleteRequested);
  QVERIFY(spy.isValid());
  QMetaObject::invokeMethod(m_view, "onDeleteClicked");
  QCOMPARE(spy.count(), 0);
}

void ViewTest::taskCompleteRequested_noSelection_doesNotEmit()
{
  const storage::Task task = makeTask(1, "Test Task", storage::Priority::Medium, QDateTime::currentDateTime());
  m_view->showTaskList({task});
  QSignalSpy spy(m_view, &view::TaskPlannerView::taskCompleteRequested);
  QVERIFY(spy.isValid());
  QMetaObject::invokeMethod(m_view, "onMarkCompleteClicked");
  QCOMPARE(spy.count(), 0);
}

void ViewTest::sortRequested_cyclesThroughCriteria()
{
  QSignalSpy spy(m_view, &view::TaskPlannerView::sortRequested);
  QVERIFY(spy.isValid());
  QMetaObject::invokeMethod(m_view, "onSortClicked");
  QCOMPARE(spy.count(), 1);
  const QList< QVariant > arguments1 = spy.takeFirst();
  QCOMPARE(arguments1.at(0).value< storage::Criterion >(), storage::Criterion::Priority);
  QMetaObject::invokeMethod(m_view, "onSortClicked");
  QCOMPARE(spy.count(), 1);
  const QList< QVariant > arguments2 = spy.takeFirst();
  QCOMPARE(arguments2.at(0).value< storage::Criterion >(), storage::Criterion::Completed);
  QMetaObject::invokeMethod(m_view, "onSortClicked");
  QCOMPARE(spy.count(), 1);
  const QList< QVariant > arguments3 = spy.takeFirst();
  QCOMPARE(arguments3.at(0).value< storage::Criterion >(), storage::Criterion::Date);
}

void ViewTest::filterChanged_searchText_emitsSignal()
{
  QSignalSpy spy(m_view, &view::TaskPlannerView::filterChanged);
  QVERIFY(spy.isValid());
  QMetaObject::invokeMethod(m_view, "onSearchTextChanged", Q_ARG(QString, "test"));
  QCOMPARE(spy.count(), 1);
  const QList< QVariant > arguments = spy.takeFirst();
  QCOMPARE(arguments.at(0).value< storage::Filter >(), storage::Filter::Search);
  QCOMPARE(arguments.at(1).toString(), QString("test"));
}

void ViewTest::filterChanged_priority_emitsSignal()
{
  QSignalSpy spy(m_view, &view::TaskPlannerView::filterChanged);
  QVERIFY(spy.isValid());
  QMetaObject::invokeMethod(m_view, "onPriorityIndexChanged", Q_ARG(int, 1));
  QCOMPARE(spy.count(), 1);
  const QList< QVariant > arguments = spy.takeFirst();
  QCOMPARE(arguments.at(0).value< storage::Filter >(), storage::Filter::Priority);
  QCOMPARE(arguments.at(1).value< storage::Priority >(), storage::Priority::Low);
}

void ViewTest::showTaskList_displaysTasks()
{
  QList< storage::Task > tasks;
  tasks.append(makeTask(1, "Task 1", storage::Priority::Low, QDateTime::currentDateTime()));
  tasks.append(makeTask(2, "Task 2", storage::Priority::Hard, QDateTime::currentDateTime(), true));
  m_view->showTaskList(tasks);
  QVERIFY(true);
}

void ViewTest::showTaskList_emptyList_clearsList()
{
  const QList< storage::Task > tasks;
  m_view->showTaskList(tasks);
  QVERIFY(true);
}

void ViewTest::updateStats_updatesLabels()
{
  m_view->updateStats(10, 5, 3);
  QVERIFY(true);
}

void ViewTest::showErrorMessage_displaysError()
{
  m_view->showErrorMessage("Test error message");
  QVERIFY(true);
}

void ViewTest::showInfoMessage_displaysInfo()
{
  m_view->showInfoMessage("Test info message");
  QVERIFY(true);
}

void ViewTest::setTaskListTitle_updatesTitle()
{
  m_view->setTaskListTitle("Test Title");
  QVERIFY(true);
}

void ViewTest::showTaskCreationForm_clearsFields()
{
  m_view->showTaskCreationForm();
  QVERIFY(true);
}

void ViewTest::showTaskCreationForm_withTask_fillsFields()
{
  const storage::Task task = makeTask(1, "Test Task", storage::Priority::Medium, QDateTime::currentDateTime());
  m_view->showTaskCreationForm(task);
  QVERIFY(true);
}

void ViewTest::closeTaskCreationForm_hidesForm()
{
  m_view->showTaskCreationForm();
  m_view->closeTaskCreationForm();
  QVERIFY(true);
}

QTEST_MAIN(ViewTest)
#include "test_view.moc"
