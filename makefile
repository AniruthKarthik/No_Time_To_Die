# Detect OS
ifeq ($(OS),Windows_NT)
    IS_WINDOWS := 1
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        IS_MACOS := 1
    endif
endif

# Compiler and basic flags
CXX = g++
CXXFLAGS = -O1 -Wall -std=c++17 -Wno-missing-braces

ifdef IS_WINDOWS
    TARGET_EXEC = game.exe
    # Windows uses local include/lib folders provided in the repo
    CXXFLAGS += -I include/ -L lib/
    LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
    RM = del /Q
    RUN_CMD = $(TARGET_EXEC)
    
    # Check if g++ is in path
    COMPILER_CHECK = where g++ >nul 2>nul
else
    TARGET_EXEC = game
    # Linux/macOS usually expect system-installed raylib
    LIBS = -lraylib -lm -lpthread -ldl -lrt -lX11
    RM = rm -f
    RUN_CMD = ./$(TARGET_EXEC)
    
    ifdef IS_MACOS
        LIBS = -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    else
        LIBS += -lGL
    endif
    
    # Check if g++ is in path
    COMPILER_CHECK = command -v g++ >/dev/null 2>&1
endif

.PHONY: all game run clean install_deps

all: game

game: install_deps $(TARGET_EXEC)

$(TARGET_EXEC): main.cpp
	$(CXX) main.cpp -o $(TARGET_EXEC) $(CXXFLAGS) $(LIBS)

run: game
	$(RUN_CMD)

clean:
	$(RM) $(TARGET_EXEC)

install_deps:
ifdef IS_WINDOWS
	@echo "Checking for C++ Compiler (MinGW)..."
	@$(COMPILER_CHECK) || (echo "g++ not found. Attempting to install via Chocolatey..." && choco install mingw -y)
	@echo "Windows detected. Using local libraries in lib/ and include/."
else
	@echo "Checking for C++ Compiler..."
	@$(COMPILER_CHECK) || (echo "g++ not found. Installing..." && \
		if [ -f /etc/arch-release ]; then sudo pacman -S --noconfirm gcc; \
		elif [ -f /etc/debian_version ]; then sudo apt-get update && sudo apt-get install -y g++; \
		elif [ "$(IS_MACOS)" = "1" ]; then xcode-select --install; \
		else echo "Please install g++ manually."; exit 1; fi)
	@echo "Checking for Raylib..."
	@if pkg-config --exists raylib || [ -f /usr/lib/libraylib.so ] || [ -f /usr/lib/libraylib.a ] || [ -f /usr/local/lib/libraylib.a ]; then \
		echo "Raylib found."; \
	else \
		echo "Raylib not found. Attempting to install..."; \
		if [ -f /etc/arch-release ]; then \
			sudo pacman -S --noconfirm raylib; \
		elif [ -f /etc/debian_version ]; then \
			sudo apt-get update && sudo apt-get install -y libraylib-dev; \
		elif [ "$(IS_MACOS)" = "1" ]; then \
			brew install raylib; \
		else \
			echo "Could not detect package manager. Please install Raylib manually."; \
			exit 1; \
		fi \
	fi
endif