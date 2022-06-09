CC := clang
CFLAGS := -Wall -Wextra -pthread -std=gnu17

# directories
OBJ := obj
SRC := src
TEST := test
UNITY := $(HOME)/repos/Unity/src
BUILD := build

VALGRIND := valgrind -s --leak-check=full --show-leak-kinds=possible --track-origins=yes

# include directories
TESTINC = -I$(SRC) -I$(UNITY)

.PHONY: $(shell mkdir -p obj) $(shell mkdir -p build) clean target test
.DEFAULT_GOAL := target

# rule to build .o from src/
$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# rule to build .o from test/
$(OBJ)/%.o: $(TEST)/%.c
	$(CC) $(CFLAGS) $(TESTINC) -c $< -o $@

# rule to build .o from unity/
$(OBJ)/%.o: $(UNITY)/%.c
	$(CC) -c $< -o $@

clean:
	rm -f $(OBJ)/* $(BUILD)/*

target: server client

server: $(OBJ)/server.o $(OBJ)/util.o $(OBJ)/list.o
	$(CC) $(CFLAGS) $^ -o $(BUILD)/$@

client: $(OBJ)/client.o $(OBJ)/util.o
	$(CC) $(CFLAGS) $^ -o $(BUILD)/$@

test%: CFLAGS += -g
test: test_dynstring

# tests
test_dynstring: $(OBJ)/unity.o $(OBJ)/test_dynstring.o $(OBJ)/dynstring.o
	$(CC) $(CFLAGS) $^ -o $(BUILD)/$@
	$(VALGRIND) --log-file=$(BUILD)/$@_val.log $(BUILD)/$@
