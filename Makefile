# --- COMPILER SETTINGS ---
CXX = i686-w64-mingw32-g++

# --- PATHS & BITS ---
INCLUDES = -I. 
LIB_PATHS = -L./lib -L.
LIBS = -lieee_32m -lfltk -lole32 -luuid -lcomctl32 -lgdi32 -lws2_32

# --- COMPILER & LINKER FLAGS ---
# Keep the Windows XP compatibility flags
CXXFLAGS = -O2 -Wall $(INCLUDES) -static-libgcc -static-libstdc++
LDFLAGS = -mwindows -static -Wl,--subsystem,windows:5.1 $(LIB_PATHS) $(LIBS)

# --- TARGETS & SOURCE FILES ---
TARGET = resistometry.exe

# List all your .cpp files here
SRCS = main.cpp measurement.cpp ui.cpp

# This automatically creates a list of .o files from your .cpp files
OBJS = $(SRCS:.cpp=.o)

# --- BUILD RULES ---

all: $(TARGET)

# Link the object files into the final executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Rule to compile .cpp files into .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- CLEAN METHODS ---

clean:
	@echo "Cleaning up build artifacts..."
	rm -f $(TARGET) $(OBJS)

distclean: clean
	@echo "Removing generated data and logs..."
	rm -f *.txt *.log *.csv

.PHONY: all clean distclean