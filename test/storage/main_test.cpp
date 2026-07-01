#define BOOST_TEST_MODULE StorageTests
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <QCoreApplication>

struct GlobalQtFixture
{
  GlobalQtFixture()
  {
    static int argc = 1;
    static char name[] = "run_tests";
    static char *argv[] = { name, nullptr };
    app_ = new QCoreApplication(argc, argv);
  }

  ~GlobalQtFixture()
  {
    delete app_;
  }

private:
  QCoreApplication *app_;
};

BOOST_GLOBAL_FIXTURE(GlobalQtFixture);
