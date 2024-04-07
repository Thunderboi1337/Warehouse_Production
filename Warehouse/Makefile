# Compiler and flags
CXX=g++
CXXFLAGS=-Wall -Werror

# Preprocessor definitions
DEALERS_SIZE=2
WAREHOUSE_SIZE=8

# Executable name
EXECUTABLE=main

# Source file
SOURCE=Warehouse.cpp

# Default target
all: $(EXECUTABLE)

# Compile and link the program
$(EXECUTABLE):
	$(CXX) $(CXXFLAGS) -DDEALERS_size=$(DEALERS_SIZE) -DWarehouse_size=$(WAREHOUSE_SIZE) $(SOURCE) -o $(EXECUTABLE)

# Clean up compiled files
clean:
	rm -f $(EXECUTABLE)

.PHONY: all clean
