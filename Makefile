# Makefile adapted from Zed Shaw's book,
# Learn C the Hard Way, Ex. 28 (pg 153-154)

CXXFLAGS = -Wall -Wextra -Iinclude -faligned-new $(OPTFLAGS)
LDFLAGS = $(OPTLIBS)
LDLIBS = -lcheck -lm -pthread -lrt -lsubunit

SOURCES = $(wildcard src/**/*.cpp src/*.cpp)
OBJECTS = $(patsubst src/%.cpp,build/%.o,$(SOURCES))

TEST_SRC = $(wildcard tests/*_tests.cpp)
TESTS = $(patsubst %.cpp,%,$(TEST_SRC))

all: $(TARGET) tests

.PHONY: build
build:
	mkdir -p build
	mkdir -p bin

.PHONY: tests
tests: LDLIBS += $(TARGET)
tests: $(TESTS)
	sh ./tests/unit-tests.sh

clean:
	rm -rf $(TARGET)
	rm -rf build $(OBJECTS) $(TESTS)
	rm -f tests/tests.log
	rm -rf tests/testdb
