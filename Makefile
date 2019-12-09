CFLAGS=$(WARN_OPTS) $(INCLUDES) $(FLAGS) -std=c11

CXXFLAGS=$(WARN_OPTS) $(INCLUDES) $(FLAGS) -std=c++11

LDFLAGS=$(FLAGS) -lm -lpthread

MAIN=lisp_machine.c
MAIN_OBJECT=lisp_machine.o
SOURCES=utils.c function_pool.c object.c simple_functions.c vector.c init.c \
	token.c objectio.c state.c stream_reader.c runner.c
OBJECTS=$(SOURCES:.c=.o)
TEST_SOURCES=test_main.cpp function_pool_ut.cpp simple_functions_ut.cpp \
            object_ut.cpp token_ut.cpp objectio_ut.cpp state_ut.cpp test_utils.cpp
TEST_OBJECTS=$(TEST_SOURCES:.cpp=.o)

all: check lisp_machine

refresh: clean all clean.o

clean: clean.o
	rm -rf lisp_machine check

clean.o:
	rm -rf *.o

lisp_machine: $(MAIN_OBJECT) $(OBJECTS)
	$(LD) -o lisp_machine $(MAIN_OBJECT) $(OBJECTS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

check: $(OBJECTS) $(TEST_OBJECTS)
	$(LD) -o check $(OBJECTS) $(TEST_OBJECTS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@
