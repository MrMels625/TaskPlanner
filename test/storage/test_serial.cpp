#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "serial-utils.hpp"
#include "istorage.hpp"
#include <QJsonObject>
#include <QTemporaryDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>

// ---------------------------------------------------------------------------
// Вспомогательные утилиты
// ---------------------------------------------------------------------------
static storage::Task fullTask()
{
  storage::Task t;
  t.id          = 7;
  t.name        = "Write tests";
  t.description = "Cover storage module";
  t.discipline  = "engineering";
  t.deadline    = QDateTime::fromString("2025-06-15T10:00:00", Qt::ISODate);
  t.priority    = storage::Priority::Hard;
  t.completed   = true;
  t.tags        = {"boost", "qt"};
  return t;
}

// Сравнение двух задач (без учёта id для некоторых тестов)
static bool tasksEqual(const storage::Task &a, const storage::Task &b)
{
  return a.id          == b.id
      && a.name        == b.name
      && a.description == b.description
      && a.discipline  == b.discipline
      && a.deadline    == b.deadline
      && a.priority    == b.priority
      && a.completed   == b.completed
      && a.tags        == b.tags;
}

// ---------------------------------------------------------------------------
BOOST_AUTO_TEST_SUITE(SerialUtils)

// --- taskToJson ------------------------------------------------------------

BOOST_AUTO_TEST_CASE(TaskToJson_ContainsAllFields)
{
  const auto t   = fullTask();
  const auto obj = storage::serial::taskToJson(t);

  BOOST_CHECK(obj["id"].toInt()          == t.id);
  BOOST_CHECK(obj["name"].toString()     == t.name);
  BOOST_CHECK(obj["description"].toString() == t.description);
  BOOST_CHECK(obj["discipline"].toString()  == t.discipline);
  BOOST_CHECK(obj["completed"].toBool()  == t.completed);
  BOOST_CHECK(obj["priority"].toInt()    == static_cast<int>(t.priority));

  const QDateTime dt = QDateTime::fromString(obj["deadline"].toString(), Qt::ISODate);
  BOOST_CHECK(dt == t.deadline);
}

BOOST_AUTO_TEST_CASE(TaskToJson_TagsArray)
{
  const auto t   = fullTask();
  const auto obj = storage::serial::taskToJson(t);

  const QJsonArray tags = obj["tags"].toArray();
  BOOST_CHECK(tags.size() == 2);
  BOOST_CHECK(tags[0].toString() == "boost");
  BOOST_CHECK(tags[1].toString() == "qt");
}

BOOST_AUTO_TEST_CASE(TaskToJson_EmptyTags)
{
  auto t  = fullTask();
  t.tags  = {};
  auto obj = storage::serial::taskToJson(t);
  BOOST_CHECK(obj["tags"].toArray().isEmpty());
}

// --- taskFromJson ----------------------------------------------------------

BOOST_AUTO_TEST_CASE(RoundTrip_FullTask)
{
  const auto original  = fullTask();
  const auto obj       = storage::serial::taskToJson(original);
  const auto recovered = storage::serial::taskFromJson(obj);

  BOOST_CHECK(tasksEqual(original, recovered));
}

BOOST_AUTO_TEST_CASE(TaskFromJson_InvalidId_ReturnsEmpty)
{
  QJsonObject obj;
  obj["id"]       = 0;        // невалидный
  obj["name"]     = "x";
  obj["deadline"] = QDateTime::currentDateTime().toString(Qt::ISODate);
  obj["priority"] = 1;

  const auto t = storage::serial::taskFromJson(obj);
  BOOST_CHECK(t.id <= 0);
}

BOOST_AUTO_TEST_CASE(TaskFromJson_MissingName_ReturnsEmpty)
{
  QJsonObject obj;
  obj["id"]       = 5;
  obj["name"]     = "";       // пустое имя — невалидно
  obj["deadline"] = QDateTime::currentDateTime().toString(Qt::ISODate);
  obj["priority"] = 1;

  const auto t = storage::serial::taskFromJson(obj);
  BOOST_CHECK(t.id <= 0);
}

BOOST_AUTO_TEST_CASE(TaskFromJson_InvalidDeadline_ReturnsEmpty)
{
  QJsonObject obj;
  obj["id"]       = 5;
  obj["name"]     = "valid";
  obj["deadline"] = "not-a-date";
  obj["priority"] = 1;

  const auto t = storage::serial::taskFromJson(obj);
  BOOST_CHECK(t.id <= 0);
}

BOOST_AUTO_TEST_CASE(TaskFromJson_OutOfRangePriority_ReturnsEmpty)
{
  QJsonObject obj;
  obj["id"]       = 5;
  obj["name"]     = "valid";
  obj["deadline"] = QDateTime::currentDateTime().toString(Qt::ISODate);
  obj["priority"] = 99;       // нет такого приоритета

  const auto t = storage::serial::taskFromJson(obj);
  BOOST_CHECK(t.id <= 0);
}

BOOST_AUTO_TEST_CASE(TaskFromJson_AllPrioritiesAccepted)
{
  using P = storage::Priority;
  for (auto p : {P::All, P::Low, P::Medium, P::Hard})
  {
    QJsonObject obj;
    obj["id"]       = 1;
    obj["name"]     = "task";
    obj["deadline"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    obj["priority"] = static_cast<int>(p);
    obj["completed"]= false;
    obj["tags"]     = QJsonArray{};

    const auto t = storage::serial::taskFromJson(obj);
    BOOST_CHECK(t.id > 0);
    BOOST_CHECK(t.priority == p);
  }
}

// --- tryLoad ---------------------------------------------------------------

BOOST_AUTO_TEST_CASE(TryLoad_ValidFile_ReturnsTrueAndPopulates)
{
  QTemporaryDir dir;
  BOOST_REQUIRE(dir.isValid());

  // Составляем валидный JSON вручную
  const auto t   = fullTask();
  QJsonArray arr;
  arr.append(storage::serial::taskToJson(t));

  QJsonObject root;
  root["tasks"]  = arr;
  root["nextId"] = 8;

  const QString path = dir.filePath("tasks.json");
  QFile f(path);
  BOOST_REQUIRE(f.open(QIODevice::WriteOnly));
  f.write(QJsonDocument(root).toJson());
  f.close();

  QList<storage::Task> tasks;
  int nextId = 0;
  const bool ok = storage::serial::tryLoad(path, tasks, nextId);

  BOOST_CHECK(ok);
  BOOST_CHECK(tasks.size() == 1);
  BOOST_CHECK(nextId == 8);
  BOOST_CHECK(tasksEqual(tasks[0], t));
}

BOOST_AUTO_TEST_CASE(TryLoad_MissingFile_ReturnsFalse)
{
  QList<storage::Task> tasks;
  int nextId = 0;
  BOOST_CHECK(!storage::serial::tryLoad("/nonexistent/path/tasks.json", tasks, nextId));
}

BOOST_AUTO_TEST_CASE(TryLoad_CorruptJson_ReturnsFalse)
{
  QTemporaryDir dir;
  BOOST_REQUIRE(dir.isValid());

  const QString path = dir.filePath("tasks.json");
  QFile f(path);
  BOOST_REQUIRE(f.open(QIODevice::WriteOnly));
  f.write("{ this is not valid json }}}");
  f.close();

  QList<storage::Task> tasks;
  int nextId = 0;
  BOOST_CHECK(!storage::serial::tryLoad(path, tasks, nextId));
}

BOOST_AUTO_TEST_CASE(TryLoad_MissingNextIdKey_ReturnsFalse)
{
  QTemporaryDir dir;
  BOOST_REQUIRE(dir.isValid());

  QJsonObject root;
  root["tasks"] = QJsonArray{};

  const QString path = dir.filePath("tasks.json");
  QFile f(path);
  BOOST_REQUIRE(f.open(QIODevice::WriteOnly));
  f.write(QJsonDocument(root).toJson());
  f.close();

  QList<storage::Task> tasks;
  int nextId = 0;
  BOOST_CHECK(!storage::serial::tryLoad(path, tasks, nextId));
}

BOOST_AUTO_TEST_CASE(TryLoad_InvalidTaskInArray_ReturnsFalse)
{
  QTemporaryDir dir;
  BOOST_REQUIRE(dir.isValid());

  QJsonObject badTask;
  badTask["id"]   = 0;
  badTask["name"] = "bad";

  QJsonArray arr;
  arr.append(badTask);

  QJsonObject root;
  root["tasks"]  = arr;
  root["nextId"] = 1;

  const QString path = dir.filePath("tasks.json");
  QFile f(path);
  BOOST_REQUIRE(f.open(QIODevice::WriteOnly));
  f.write(QJsonDocument(root).toJson());
  f.close();

  QList<storage::Task> tasks;
  int nextId = 0;
  BOOST_CHECK(!storage::serial::tryLoad(path, tasks, nextId));
}

BOOST_AUTO_TEST_SUITE_END()

