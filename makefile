CC                = clang
CFLAGS            = -Wall
LDFLAGS           = -c -g
VPATH             = src src/core src/tests
LIBS              = -lfftw3f -lm -lsndfile -lmad -lmp3lame

TEST_CFLAGS       = --coverage $(CFLAGS)
TEST_LIBS         = -lprofile_rt

LCOV_FLAGS        = -c -b ./ -d ./

MAIN_BUILD_DIR    = build/main
TEST_BUILD_DIR    = build/tests
HEADER_DIRS       = include
COVERAGE_DIR      = coverage
COVERAGE_FILE     = $(COVERAGE_DIR)/coverage.info

INCLUDES          = $(addprefix -I, $(HEADER_DIRS))

CORE_SOURCES      = $(notdir $(wildcard src/core/*.c))
TEST_SOURCES      = $(notdir $(wildcard src/tests/*.c)) $(CORE_SOURCES)
DVM_SOURCES       = wav_stretch.c $(CORE_SOURCES)

DVM_OBJECTS       = $(addprefix $(MAIN_BUILD_DIR)/, $(DVM_SOURCES:.c=.o))
TEST_OBJECTS      = $(addprefix $(TEST_BUILD_DIR)/, $(TEST_SOURCES:.c=.o))

EXECUTABLE        = wavstretch
TEST_EXECUTABLE   = $(addprefix $(TEST_BUILD_DIR)/, test_wavstretch)

all: $(EXECUTABLE)

tests: $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE)

coverage: $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE)
	lcov $(LCOV_FLAGS) -o $(COVERAGE_FILE)
	lcov -r $(COVERAGE_FILE) test\* -o $(COVERAGE_FILE)
	genhtml $(COVERAGE_FILE) -o $(COVERAGE_DIR)

$(MAIN_BUILD_DIR)/%.o: %.c
	$(CC) $(LDFLAGS) $(CFLAGS) $(INCLUDES) $< -o $@

$(TEST_BUILD_DIR)/%.o: %.c
	$(CC) $(LDFLAGS) $(TEST_CFLAGS) $(INCLUDES) $< -o $@

$(EXECUTABLE): $(DVM_OBJECTS)
	$(CC) $(LIBS) $(CFLAGS) $(DVM_OBJECTS) -o $@

$(TEST_EXECUTABLE): $(TEST_OBJECTS)
	$(CC) $(TEST_LIBS) $(TEST_CFLAGS) $(TEST_OBJECTS) -o $@

clean:
	rm -rf $(MAIN_BUILD_DIR)/* $(EXECUTABLE)
	rm -rf $(TEST_BUILD_DIR)/*

