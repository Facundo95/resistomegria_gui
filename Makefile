# --- COMPILER SETTINGS ---
# Cross-compiler for 32-bit Windows
CXX = i686-w64-mingw32-g++

# --- IEEE / GPIB HARDWARE OPTIONS ---
# Set to 1 to compile and link real IEEE-488 hardware support
ENABLE_IEEE ?= 1
# Library short name used by -l<name> (ieee_32m.lib -> -lieee_32m)
IEEE_LIB_NAME ?= ieee_32m
# Extra search path where IEEE import/static library is located
IEEE_LIB_PATH ?= .

# --- PATHS & BITS ---
INCLUDES = -I. 
LIB_PATHS = -L./lib -L.

ifeq ($(ENABLE_IEEE),1)
INCLUDES += -DENABLE_IEEE_HARDWARE
LIB_PATHS += -L$(IEEE_LIB_PATH)
endif

# --- LINK LIBRARIES ---
# Project usage: FLTK widgets + FLTK OpenGL context (Fl::gl_visual)
# Keep required Win32 deps for static FLTK + native dialogs
LIBS = -lfltk_gl -lfltk -lopengl32 -lole32 -luuid -lcomctl32 -lgdi32

ifeq ($(ENABLE_IEEE),1)
LIBS += -l$(IEEE_LIB_NAME)
endif

# --- COMPILER & LINKER FLAGS ---
# Windows XP compatibility: subsystem 5.1 is for XP 32-bit
CXXFLAGS = -O2 -Wall $(INCLUDES) -DMULTIPLOT_FLTK -static-libgcc -static-libstdc++
LDFLAGS = -mwindows -static -Wl,--subsystem,windows:5.1 $(LIB_PATHS) $(LIBS)

# --- TARGETS & SOURCE FILES ---
TARGET = resistometry.exe
IEEE_DLL ?= ieee_32m.dll

# Ensure all source files are listed
SRCS = main.cpp measurement.cpp ui.cpp simple_plot.cpp

# Automatically create list of .o files
OBJS = $(SRCS:.cpp=.o)

# --- BUILD RULES ---

all: $(TARGET)

# Link the object files into the final executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)
	$(MAKE) copy_ieee_dll

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

copy_ieee_dll:
	@if [ -f "$(IEEE_DLL)" ]; then \
		echo "Copying $(IEEE_DLL) next to $(TARGET)..."; \
		cp -f "$(IEEE_DLL)" "./$$(basename "$(IEEE_DLL)")"; \
	else \
		echo "Warning: $(IEEE_DLL) not found; skipping DLL copy."; \
	fi

.PHONY: all clean distclean copy_ieee_dll