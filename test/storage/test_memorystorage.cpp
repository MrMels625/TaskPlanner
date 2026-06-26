#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <QTemporaryDir>
#include "memorystorage.hpp"
#include "istorage.hpp"
#include "serial-utils.hpp"

struct StorageFixture
{
  QTemporaryDir tmpDir;

  StorageFixture()
  {
    BOOST_REQUIRE(tmpDir.isValid());
    storage::serial::setBaseDirOverride(tmpDir.path());
  }

  ~StorageFixture()
  {
    storage::serial::setBaseDirOverride("");
  }
};

static storage::Task makeTask(const QString &name, storage::Priority priority,
    const QDateTime &deadline, bool completed = false)
{
  storage::Task t;
  t.id = 0;
  t.name = name;
  t.description = "";
  t.discipline = "";
  t.deadline = deadline;
  t.priority = priority;
  t.completed = completed;
  return t;
}

BOOST_FIXTURE_TEST_SUITE(MemoryStorageSuite, StorageFixture)

BOOST_AUTO_TEST_CASE(AddTask_AssignsId)
{
  storage::MemoryStorage s;
  s.addTask(makeTask("First", storage::Priority::Low, QDateTime::currentDateTime()));
  const auto all = s.getAllTasks();
  BOOST_REQUIRE(all.size() == 1);
  BOOST_TEST(all[0].id > 0);
  BOOST_TEST(all[0].name == "First");
}

BOOST_AUTO_TEST_CASE(AddMultipleTasks_IdsIncrement)
{
  storage::MemoryStorage s;
  s.addTask(makeTask("A", storage::Priority::Low, QDateTime::currentDateTime()));
  s.addTask(makeTask("B", storage::Priority::Hard, QDateTime::currentDateTime()));
  const auto all = s.getAllTasks();
  BOOST_REQUIRE(all.size() == 2);
  BOOST_TEST(all[1].id == all[0].id + 1);
}

BOOST_AUTO_TEST_CASE(RemoveTask_RemovesCorrectOne)
{
  storage::MemoryStorage s;
  s.addTask(makeTask("Keep", storage::Priority::Low, QDateTime::currentDateTime()));
  s.addTask(makeTask("Remove", storage::Priority::Low, QDateTime::currentDateTime()));
  const int removeId = s.getAllTasks()[1].id;
  s.removeTask(removeId);
  const auto all = s.getAllTasks();
  BOOST_REQUIRE(all.size() == 1);
  BOOST_TEST(all[0].name == "Keep");
}

BOOST_AUTO_TEST_CASE(RemoveTask_NonExistentId_NoEffect)
{
  storage::MemoryStorage s;
  s.addTask(makeTask("A", storage::Priority::Low, QDateTime::currentDateTime()));
  s.removeTask(9999);
  BOOST_TEST(s.getAllTasks().size() == 1);
}

BOOST_AUTO_TEST_CASE(UpdateTask_ChangesFields)
{
  storage::MemoryStorage s;
  s.addTask(makeTask("Old name", storage::Priority::Low, QDateTime::currentDateTime()));
  auto updated = s.getAllTasks()[0];
  updated.name = "New name";
  updated.priority = storage::Priority::Hard;
  s.updateTask(updated);
  const auto all = s.getAllTasks();
  BOOST_REQUIRE(all.size() == 1);
  BOOST_TEST(all[0].name == "New name");
  BOOST_TEST(all[0].priority == storage::Priority::Hard);
}

BOOST_AUTO_TEST_CASE(UpdateTask_NonExistentId_NoEffect)
{
  storage::MemoryStorage s;
  s.addTask(makeTask("A", storage::Priority::Low, QDateTime::currentDateTime()));
  storage::Task ghost;
  ghost.id = 9999;
  ghost.name = "Ghost";
  ghost.deadline = QDateTime::currentDateTime();
  ghost.priority = storage::Priority::Low;
  s.updateTask(ghost);
  BOOST_TEST(s.getAllTasks().size() == 1);
  BOOST_TEST(s.getAllTasks()[0].name == "A");
}

BOOST_AUTO_TEST_CASE(GetTasksForDate_ReturnsOnlyMatchingDate)
{
  storage::MemoryStorage s;
  const QDate today = QDate::currentDate();
  const QDate tomorrow = today.addDays(1);
  s.addTask(makeTask("Today", storage::Priority::Low, QDateTime(today, QTime(12, 0))));
  s.addTask(makeTask("Tomorrow", storage::Priority::Low, QDateTime(tomorrow, QTime(12, 0))));
  const auto result = s.getTasksForDate(today);
  BOOST_REQUIRE(result.size() == 1);
  BOOST_TEST(result[0].name == "Today");
}

BOOST_AUTO_TEST_CASE(GetTasksForToday_ReturnsToday)
{
  storage::MemoryStorage s;
  const QDate today = QDate::currentDate();
  s.addTask(makeTask("Today", storage::Priority::Low, QDateTime(today, QTime(9, 0))));
  s.addTask(makeTask("Past", storage::Priority::Low, QDateTime(today.addDays(-1), QTime(9, 0))));
  const auto result = s.getTasksForToday();
  BOOST_REQUIRE(result.size() == 1);
  BOOST_TEST(result[0].name == "Today");
}

BOOST_AUTO_TEST_CASE(GetOverdueTasks_ReturnsExpired_NotCompleted)
{
  storage::MemoryStorage s;
  const QDateTime past = QDateTime::currentDateTime().addDays(-1);
  const QDateTime future = QDateTime::currentDateTime().addDays(1);
  s.addTask(makeTask("Overdue", storage::Priority::Low, past, false));
  s.addTask(makeTask("Overdue+Done", storage::Priority::Low, past, true));
  s.addTask(makeTask("Future", storage::Priority::Low, future, false));
  const auto result = s.getOverdueTasks();
  BOOST_REQUIRE(result.size() == 1);
  BOOST_TEST(result[0].name == "Overdue");
}

BOOST_AUTO_TEST_CASE(FilterBySearchText_CaseInsensitive)
{
  storage::MemoryStorage s;
  s.addTask(makeTask("Buy Milk", storage::Priority::Low, QDateTime::currentDateTime()));
  s.addTask(makeTask("Buy Bread", storage::Priority::Low, QDateTime::currentDateTime()));
  s.addTask(makeTask("Call Mom", storage::Priority::Low, QDateTime::currentDateTime()));
  const auto result = s.getTasksFiltered("buy", false, false, storage::Priority::All);
  BOOST_TEST(result.size() == 2);
}

BOOST_AUTO_TEST_CASE(FilterByPriority_ReturnsOnlyMatching)
{
  storage::MemoryStorage s;
  s.addTask(makeTask("Low task", storage::Priority::Low, QDateTime::currentDateTime()));
  s.addTask(makeTask("Hard task", storage::Priority::Hard, QDateTime::currentDateTime()));
  s.addTask(makeTask("Med task", storage::Priority::Medium, QDateTime::currentDateTime()));
  const auto result = s.getTasksFiltered("", false, false, storage::Priority::Hard);
  BOOST_REQUIRE(result.size() == 1);
  BOOST_TEST(result[0].name == "Hard task");
}

BOOST_AUTO_TEST_CASE(FilterAll_ReturnsEverything)
{
  storage::MemoryStorage s;
  s.addTask(makeTask("A", storage::Priority::Low, QDateTime::currentDateTime()));
  s.addTask(makeTask("B", storage::Priority::Hard, QDateTime::currentDateTime()));
  const auto result = s.getTasksFiltered("", false, false, storage::Priority::All);
  BOOST_TEST(result.size() == 2);
}

BOOST_AUTO_TEST_CASE(FilterToday_CombinedWithSearch)
{
  storage::MemoryStorage s;
  const QDate today = QDate::currentDate();
  s.addTask(makeTask("Buy milk today", storage::Priority::Low, QDateTime(today, QTime(8, 0))));
  s.addTask(makeTask(
      "Buy milk tomorrow", storage::Priority::Low, QDateTime(today.addDays(1), QTime(8, 0))));
  s.addTask(makeTask("Call today", storage::Priority::Low, QDateTime(today, QTime(9, 0))));
  const auto result = s.getTasksFiltered("buy", true, false, storage::Priority::All);
  BOOST_REQUIRE(result.size() == 1);
  BOOST_TEST(result[0].name == "Buy milk today");
}

BOOST_AUTO_TEST_CASE(SortByDate_Ascending)
{
  storage::MemoryStorage s;
  const QDateTime d1 = QDateTime::currentDateTime().addDays(2);
  const QDateTime d2 = QDateTime::currentDateTime().addDays(0);
  const QDateTime d3 = QDateTime::currentDateTime().addDays(4);
  s.addTask(makeTask("Late", storage::Priority::Low, d1));
  s.addTask(makeTask("Early", storage::Priority::Low, d2));
  s.addTask(makeTask("Latest", storage::Priority::Low, d3));
  const auto sorted = s.getSortedTasks(s.getAllTasks(), storage::Criterion::Date);
  BOOST_REQUIRE(sorted.size() == 3);
  BOOST_TEST(sorted[0].name == "Early");
  BOOST_TEST(sorted[1].name == "Late");
  BOOST_TEST(sorted[2].name == "Latest");
}

BOOST_AUTO_TEST_CASE(SortByPriority_HighestFirst)
{
  storage::MemoryStorage s;
  const QDateTime dt = QDateTime::currentDateTime();
  s.addTask(makeTask("Low", storage::Priority::Low, dt));
  s.addTask(makeTask("Hard", storage::Priority::Hard, dt));
  s.addTask(makeTask("Medium", storage::Priority::Medium, dt));
  const auto sorted = s.getSortedTasks(s.getAllTasks(), storage::Criterion::Priority);
  BOOST_REQUIRE(sorted.size() == 3);
  BOOST_TEST(sorted[0].name == "Hard");
  BOOST_TEST(sorted[1].name == "Medium");
  BOOST_TEST(sorted[2].name == "Low");
}

BOOST_AUTO_TEST_CASE(SortByCompleted_NotDoneFirst)
{
  storage::MemoryStorage s;
  const QDateTime dt = QDateTime::currentDateTime();
  s.addTask(makeTask("Done", storage::Priority::Low, dt, true));
  s.addTask(makeTask("Not done", storage::Priority::Low, dt, false));
  const auto sorted = s.getSortedTasks(s.getAllTasks(), storage::Criterion::Completed);
  BOOST_REQUIRE(sorted.size() == 2);
  BOOST_TEST(sorted[0].name == "Not done");
  BOOST_TEST(sorted[1].name == "Done");
}

BOOST_AUTO_TEST_CASE(SortEmptyList_ReturnsEmpty)
{
  storage::MemoryStorage s;
  const auto sorted = s.getSortedTasks({}, storage::Criterion::Date);
  BOOST_TEST(sorted.isEmpty());
}

BOOST_AUTO_TEST_CASE(Persistence_SaveAndReload)
{
  {
    storage::MemoryStorage s;
    s.addTask(makeTask(
        "Saved task",
        storage::Priority::Medium,
        QDateTime::fromString("2025-12-01T09:00:00", Qt::ISODate)));
  }

  {
    storage::MemoryStorage s;
    const auto all = s.getAllTasks();
    BOOST_REQUIRE(all.size() == 1);
    BOOST_TEST(all[0].name == "Saved task");
    BOOST_TEST(all[0].priority == storage::Priority::Medium);
  }
}

BOOST_AUTO_TEST_CASE(Persistence_RemoveAndReload)
{
  {
    storage::MemoryStorage s;
    s.addTask(makeTask("Keep", storage::Priority::Low, QDateTime::currentDateTime()));
    s.addTask(makeTask("Remove", storage::Priority::Low, QDateTime::currentDateTime()));
    s.removeTask(s.getAllTasks()[1].id);
  }

  {
    storage::MemoryStorage s;
    BOOST_REQUIRE(s.getAllTasks().size() == 1);
    BOOST_TEST(s.getAllTasks()[0].name == "Keep");
  }
}

BOOST_AUTO_TEST_SUITE_END()
