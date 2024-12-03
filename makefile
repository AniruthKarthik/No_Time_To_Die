# Compiler and flags
CXX = g++
CXXFLAGS = -O1 -Wall -std=c++17 -Wno-missing-braces -I include/ -L lib/
LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm

# Target executable
TARGET = game.exe

# Source files
SRC = main.cpp

# Build target
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LIBS)

# Clean target
clean:
	rm -f $(TARGET)
