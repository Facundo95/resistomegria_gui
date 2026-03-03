# --- COMPILER SETTINGS ---
# Cross-compiler for Windows 32-bit (XP compatible)
CXX = i686-w64-mingw32-g++

# --- PATHS ---
# We assume FLTK headers are in ./FL and libraries in ./lib
INCLUDES = -I. 
LIB_PATHS = -L./lib -L.

# --- LIBRARIES ---
# Order matters for the linker! 
# 1. Your specific GPIB lib
# 2. FLTK core
# 3. Windows system libs required by FLTK
LIBS = -lieee_32m -lfltk -lole32 -luuid -lcomctl32 -lgdi32 -lws2_32

# --- COMPILER FLAGS ---
# -O2: Optimization
# -Wall: Show all warnings
# -static-libgcc -static-libstdc++: Includes runtimes so XP doesn't need extra DLLs
CXXFLAGS = -O2 -Wall $(INCLUDES) -static-libgcc -static-libstdc++

# --- LINKER FLAGS ---
# -mwindows: Creates a GUI app (no black console window)
# -static: Links everything statically for maximum XP portability
# -Wl,--subsystem,windows:5.1: Forces the EXE to identify as Windows XP compatible
LDFLAGS = -mwindows -static -Wl,--subsystem,windows:5.1 $(LIB_PATHS) $(LIBS)

# --- TARGETS ---
TARGET = resistometry.exe
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(SRC) $(CXXFLAGS) -o $(TARGET) $(LDFLAGS)

# --- CLEAN METHODS ---

# 1. Basic Clean: Removes the executable and object files
clean:
	@echo "Cleaning up build artifacts..."
	rm -f $(TARGET) $(OBJS)

# 2. Total Clean: Removes build files plus any data files generated during tests
distclean: clean
	@echo "Removing generated data and logs..."
	rm -f *.txt *.log *.csv

.PHONY: all clean distclean