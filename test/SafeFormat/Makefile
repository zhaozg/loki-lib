include ../Makefile.common

BIN := main$(BIN_SUFFIX)
SRC := main.cpp ThreadPool.cpp
OBJ := $(SRC:.cpp=.o)
CXXFLAGS := $(CXXWARNFLAGS) -g -fexpensive-optimizations -O3
LDLIBS += -lpthread

.PHONY: all clean
all: $(BIN)
clean: cleandeps
	$(RM) $(BIN)
	$(RM) $(OBJ)

$(BIN): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

test: $(BIN)
	./$(BIN) -f
	./$(BIN) -r:100 -s:100

include ../../Makefile.deps
