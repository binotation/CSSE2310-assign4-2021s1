CC := clang
CFLAGS := -Wall -Wextra -pthread -std=gnu17

# directories
OBJ := obj
SRC := src
TEST := test
UNITY := $(HOME)/repos/Unity/src
BUILD := build

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

#test: test_get test_math

# tests
#test_get: $(OBJ)/unity.o $(OBJ)/test_get.o $(OBJ)/get.o
#	$(CC) $^ -o $(BUILD)/$@
#	$(BUILD)/$@

#test_math: $(OBJ)/unity.o $(OBJ)/test_math.o $(OBJ)/math.o
#	$(CC) $^ -o $(BUILD)/$@
#	$(BUILD)/$@
