CXX      := g++
CXXFLAGS := -std=c++17 -g -O0 -Wall -Wextra

SRC_DIR  := src/storage
TEST_DIR := test/storage

PROJECT_SOURCES := \
  $(SRC_DIR)/task.cpp \
  $(SRC_DIR)/memorystorage.cpp \
  $(SRC_DIR)/serial-utils.cpp

TEST_SOURCES := \
  $(TEST_DIR)/main_test.cpp \
  $(TEST_DIR)/test_task.cpp \
  $(TEST_DIR)/test_memorystorage.cpp \
  $(TEST_DIR)/test_serial.cpp

TEST_BIN := $(TEST_DIR)/run_tests

ifndef QT_ROOT
  QMAKE := $(shell which qmake 2>/dev/null)
  ifdef QMAKE
    QT_ROOT := $(shell $(QMAKE) -query QT_INSTALL_PREFIX 2>/dev/null)
  endif
endif

ifndef QT_ROOT
  ifneq ($(shell pkg-config --exists Qt6Core 2>/dev/null && echo yes),)
    QT_VERSION := 6
    QT_ROOT    := $(shell pkg-config --variable=prefix Qt6Core)
  else ifneq ($(shell pkg-config --exists Qt5Core 2>/dev/null && echo yes),)
    QT_VERSION := 5
    QT_ROOT    := $(shell pkg-config --variable=prefix Qt5Core)
  endif
endif

ifndef QT_ROOT
  QT_CANDIDATES := $(wildcard /opt/Qt/6.*/gcc_64) $(wildcard /opt/Qt/5.*/gcc_64)
  ifneq ($(QT_CANDIDATES),)
    QT_ROOT := $(firstword $(QT_CANDIDATES))
  endif
endif

ifndef QT_ROOT
  ifneq ($(wildcard /usr/include/x86_64-linux-gnu/qt6),)
    QT_ROOT    := /usr
    QT_VERSION := 6
  else ifneq ($(wildcard /usr/include/x86_64-linux-gnu/qt5),)
    QT_ROOT    := /usr
    QT_VERSION := 5
  endif
endif

ifndef QT_VERSION
  ifneq ($(wildcard $(QT_ROOT)/include/QtCore/qglobal.h),)
    _QT_VER_RAW := $(shell grep "QT_VERSION_STR" $(QT_ROOT)/include/QtCore/qglobal.h 2>/dev/null | head -1)
    ifneq ($(findstring "6.,$(_QT_VER_RAW)),)
      QT_VERSION := 6
    else
      QT_VERSION := 5
    endif
  endif
  QT_VERSION ?= 6
endif

ifeq ($(QT_VERSION),6)
  QT_MOD_PREFIX := Qt6
  QT_INC_SUFFIX := qt6
else
  QT_MOD_PREFIX := Qt5
  QT_INC_SUFFIX := qt5
endif

ifeq ($(QT_ROOT),/usr)
  QT_ARCH_INC := $(firstword \
    $(wildcard /usr/include/x86_64-linux-gnu/$(QT_INC_SUFFIX)) \
    $(wildcard /usr/include/$(QT_INC_SUFFIX)))
  QT_INCLUDES := -I$(QT_ARCH_INC) -I$(QT_ARCH_INC)/QtCore
  QT_LIBDIR   := $(firstword $(wildcard /usr/lib/x86_64-linux-gnu) /usr/lib)
else
  QT_INCLUDES := -I$(QT_ROOT)/include -I$(QT_ROOT)/include/QtCore
  QT_LIBDIR   := $(QT_ROOT)/lib
endif

QT_CXXFLAGS := $(QT_INCLUDES) -fPIC -DQT_CORE_LIB
QT_LIBS     := -L$(QT_LIBDIR) -l$(QT_MOD_PREFIX)Core -Wl,-rpath,$(QT_LIBDIR)

ifndef BOOST_ROOT
  _BOOST_CANDIDATES := /usr/include /usr/local/include /opt/homebrew/include /opt/local/include
  BOOST_ROOT := $(firstword $(foreach d,$(_BOOST_CANDIDATES),\
    $(if $(wildcard $(d)/boost/test/unit_test.hpp),$(d:%/include=%),)))
endif

ifdef BOOST_ROOT
  ifeq ($(BOOST_ROOT),/usr)
    BOOST_INC    := /usr/include
    BOOST_LIBDIR := $(firstword $(wildcard /usr/lib/x86_64-linux-gnu) /usr/lib)
  else
    BOOST_INC    := $(BOOST_ROOT)/include
    BOOST_LIBDIR := $(BOOST_ROOT)/lib
  endif
  BOOST_CXXFLAGS := -I$(BOOST_INC)
  BOOST_LIB_NAME := $(firstword \
    $(if $(wildcard $(BOOST_LIBDIR)/libboost_unit_test_framework.so),boost_unit_test_framework) \
    $(if $(wildcard $(BOOST_LIBDIR)/libboost_unit_test_framework-mt.so),boost_unit_test_framework-mt) \
    boost_unit_test_framework)
  BOOST_LIBS := -L$(BOOST_LIBDIR) -l$(BOOST_LIB_NAME) -Wl,-rpath,$(BOOST_LIBDIR)
else
  BOOST_NOT_FOUND := 1
endif

ALL_CXXFLAGS := $(CXXFLAGS) $(QT_CXXFLAGS) $(BOOST_CXXFLAGS) -I$(SRC_DIR)
ALL_LIBS     := $(QT_LIBS) $(BOOST_LIBS)

.PHONY: all test build-test clean help check-deps

all: help

test: build-test
	@echo ""
	@echo "=== Testing ==="
	@cd $(TEST_DIR) && ./run_tests --log_level=message 2>&1; \
	  code=$$?; \
	  if [ $$code -ne 0 ]; then echo ""; echo "TESTS FAILED (code $$code)"; exit $$code; fi

build-test: check-deps $(TEST_BIN)

$(TEST_BIN): $(PROJECT_SOURCES) $(TEST_SOURCES)
	@echo "=== Tests building ==="
	@echo "  Qt:    $(QT_ROOT) (Qt$(QT_VERSION))"
	@echo "  Boost: $(BOOST_ROOT)"
	$(CXX) $(ALL_CXXFLAGS) \
	  $(PROJECT_SOURCES) $(TEST_SOURCES) \
	  $(ALL_LIBS) \
	  -o $@
	@echo "  Binary: $@"

check-deps:
ifndef QT_ROOT
	$(error Qt not found. Download Qt (apt install qt6-base-dev) or try: make test QT_ROOT=/path/to/Qt/6.x/gcc_64)
endif
ifdef BOOST_NOT_FOUND
	$(error Boost.Test not found. Download (apt install libboost-test-dev) or try: make test BOOST_ROOT=/path/to/boost)
endif

clean:
	rm -f $(TEST_BIN)
	@echo "Cleaned."

help:
	@echo "Makefile for boost tests for storage module"
	@echo ""
	@echo "Targets:"
	@echo "  make test          — build and run tests"
	@echo "  make build-test    — build tests"
	@echo "  make clean         — remove binary"
	@echo "  make check-deps    — check dependencies"
	@echo ""
	@echo "Variables:"
	@echo "  QT_ROOT=...        — path to Qt (if not automatically found)"
	@echo "  BOOST_ROOT=...     — path to Boost (if not automatically found)"
	@echo ""
