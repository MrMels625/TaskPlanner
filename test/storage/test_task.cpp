#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "task.hpp"
#include "istorage.hpp"

static storage::Task makeTask(int id, storage::Priority priority = storage::Priority::Medium, 
    const QDateTime &deadline  = QDateTime::currentDateTime())
{
  storage::Task t;
  t.id = id;
  t.name = QString("Task %1").arg(id);
  t.description = "desc";
  t.discipline = "math";
  t.deadline = deadline;
  t.priority = priority;
  t.completed = false;
  return t;
}

BOOST_AUTO_TEST_SUITE(TaskOperators)

BOOST_AUTO_TEST_CASE(EqualityById)
{
  auto a = makeTask(1);
  auto b = makeTask(1);
  b.name = "Different name";
  BOOST_CHECK(a == b);
}

BOOST_AUTO_TEST_CASE(InequalityDifferentIds)
{
  auto a = makeTask(1);
  auto b = makeTask(2);
  BOOST_CHECK(!(a == b));
}

BOOST_AUTO_TEST_CASE(LessByDeadlineEarlierIsSmaller)
{
  QDateTime earlier = QDateTime::currentDateTime();
  QDateTime later = earlier.addDays(1);

  auto a = makeTask(1, storage::Priority::Low, earlier);
  auto b = makeTask(2, storage::Priority::Low, later);

  BOOST_CHECK(a < b);
  BOOST_CHECK(!(b < a));
}

BOOST_AUTO_TEST_CASE(LessBySamePriorityDeadlineEqual)
{
  QDateTime dt = QDateTime::currentDateTime();

  auto a = makeTask(1, storage::Priority::Low, dt);
  auto b = makeTask(2, storage::Priority::Medium, dt);
  auto c = makeTask(3, storage::Priority::Hard, dt);

  BOOST_CHECK(c < b);
  BOOST_CHECK(b < a);
  BOOST_CHECK(c < a);
}

BOOST_AUTO_TEST_CASE(LessDeadlineTakesPriorityOverPriority)
{
  QDateTime earlier = QDateTime::currentDateTime();
  QDateTime later = earlier.addDays(2);

  auto low_early = makeTask(1, storage::Priority::Low, earlier);
  auto hard_later = makeTask(2, storage::Priority::Hard, later);

  BOOST_CHECK(low_early < hard_later);
}

BOOST_AUTO_TEST_CASE(EqualToSelf)
{
  auto a = makeTask(42);
  BOOST_CHECK(a == a);
  BOOST_CHECK(!(a < a));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(TaskFields)

BOOST_AUTO_TEST_CASE(DefaultConstructorCompiles)
{
  storage::Task t;
  (void)t;
  BOOST_TEST(true);
}

BOOST_AUTO_TEST_CASE(TagsStoredCorrectly)
{
  auto t = makeTask(1);
  t.tags = { "boost", "qt", "cpp" };
  BOOST_TEST(t.tags.size() == 3);
  BOOST_CHECK(t.tags.contains("qt"));
}

BOOST_AUTO_TEST_CASE(CompletedFlagDefault)
{
  auto t = makeTask(1);
  BOOST_CHECK(t.completed == false);
}

BOOST_AUTO_TEST_SUITE_END()

