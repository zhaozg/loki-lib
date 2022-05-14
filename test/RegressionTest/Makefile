include ../Makefile.common

BIN := Test$(BIN_SUFFIX)
SRC := Test.cpp
OBJ := $(SRC:.cpp=.o)

.PHONY: all clean
all: $(BIN)
clean: cleandeps
	$(RM) $(BIN)
	$(RM) $(OBJ)

$(BIN): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

test: $(BIN)
	./$(BIN)

include ../../Makefile.deps
