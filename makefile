COMP = g++-10

GOOGLETEST_LIB = /usr/src/googletest/googletest/gcc-10-build/lib/
GOOGLETEST_INCLUDE = /usr/src/googletest/googletest/include/

OBJS = tests.o main.o
EXEC = ex4
CPP_COMP_FLAG = -std=c++20 -Werror -Wall -Wextra -pedantic-errors -DNDEBUG -I$(GOOGLETEST_INCLUDE) 
CPP_LINK_FLAG = -L$(GOOGLETEST_LIB) -lgtest -lgtest_main -pthread

$(EXEC): $(OBJS)
	$(COMP) $(OBJS) $(CPP_LINK_FLAG) -o $@

tests.o: tests.cpp Grid.h TestClasses.h GridMock.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

main.o: main.cpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

clean:
	rm -f $(OBJS) $(EXEC)
