# CS527 Lab 8: OpenMP — Arrays and Matrices
# Build all programs with: mingw32-make

CXX = C:/msys64/ucrt64/bin/g++.exe
CXXFLAGS = -O2 -Wall -Wextra -fopenmp -std=c++17
PROGRAMS = task1.exe task2.exe task3.exe task4.exe task5.exe

.PHONY: all clean
all: $(PROGRAMS)

task1.exe: task1_array_search.cpp
	$(CXX) $(CXXFLAGS) task1_array_search.cpp -o task1.exe
task2.exe: task2_array_sort.cpp
	$(CXX) $(CXXFLAGS) task2_array_sort.cpp -o task2.exe
task3.exe: task3_histogram.cpp
	$(CXX) $(CXXFLAGS) task3_histogram.cpp -o task3.exe
task4.exe: task4_matrix_transpose.cpp
	$(CXX) $(CXXFLAGS) task4_matrix_transpose.cpp -o task4.exe
task5.exe: task5_convolution.cpp
	$(CXX) $(CXXFLAGS) task5_convolution.cpp -o task5.exe

clean:
	powershell -NoProfile -Command "Remove-Item task1.exe,task2.exe,task3.exe,task4.exe,task5.exe -ErrorAction SilentlyContinue"
