# CS527 Computer Systems - OpenMP Lab (C++)
# Build all five programs with: mingw32-make

CXX = C:/msys64/ucrt64/bin/g++.exe
CXXFLAGS = -O2 -Wall -fopenmp -std=c++17

PROGRAMS = task1.exe task2.exe task3.exe task4.exe task5.exe

.PHONY: all clean

all: $(PROGRAMS)

task1.exe: task1_vector_add.cpp
	$(CXX) $(CXXFLAGS) task1_vector_add.cpp -o task1.exe

task2.exe: task2_array_sum.cpp
	$(CXX) $(CXXFLAGS) task2_array_sum.cpp -o task2.exe

task3.exe: task3_min_max.cpp
	$(CXX) $(CXXFLAGS) task3_min_max.cpp -o task3.exe

task4.exe: task4_matrix_add.cpp
	$(CXX) $(CXXFLAGS) task4_matrix_add.cpp -o task4.exe

task5.exe: task5_matrix_multiply.cpp
	$(CXX) $(CXXFLAGS) task5_matrix_multiply.cpp -o task5.exe

clean:
	powershell -NoProfile -Command "Remove-Item task1.exe,task2.exe,task3.exe,task4.exe,task5.exe -ErrorAction SilentlyContinue"
