# --- COMPILER SETTINGS ---
# Cross-compiler for 32-bit Windows
CXX = i686-w64-mingw32-g++

# --- PATHS & BITS ---
INCLUDES = -I. 
LIB_PATHS = -L./lib -L.

# --- UPDATED LIBRARIES ---
# Added -lfltk_gl, -lopengl32, and -lglu32 for Multiplot support
LIBS = -lfltk_gl -lfltk -lopengl32 -lglu32 -lole32 -luuid -lcomctl32 -lgdi32 -lws2_32

# --- COMPILER & LINKER FLAGS ---
# Windows XP compatibility: subsystem 5.1 is for XP 32-bit
CXXFLAGS = -O2 -Wall $(INCLUDES) -DMULTIPLOT_FLTK -static-libgcc -static-libstdc++
LDFLAGS = -mwindows -static -Wl,--subsystem,windows:5.1 $(LIB_PATHS) $(LIBS)

# --- TARGETS & SOURCE FILES ---
TARGET = resistometry.exe

# Ensure all source files are listed
SRCS = main.cpp measurement.cpp ui.cpp simple_plot.cpp

# Automatically create list of .o files
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