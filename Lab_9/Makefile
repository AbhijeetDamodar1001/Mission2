CXX := g++
CXXFLAGS := -O2 -Wall -Wextra -std=c++17 -fopenmp
SRC := src
BIN := bin
TARGETS := $(BIN)/task1.exe $(BIN)/task2.exe $(BIN)/task3.exe

.PHONY: all clean
all: $(TARGETS)

$(BIN):
	mkdir -p $(BIN)

$(BIN)/task1.exe: $(SRC)/task1_merge_sort.cpp | $(BIN)
	$(CXX) $(CXXFLAGS) $< -o $@

$(BIN)/task2.exe: $(SRC)/task2_scheduling.cpp | $(BIN)
	$(CXX) $(CXXFLAGS) $< -o $@

$(BIN)/task3.exe: $(SRC)/task3_even_odd.cpp | $(BIN)
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -rf $(BIN)
