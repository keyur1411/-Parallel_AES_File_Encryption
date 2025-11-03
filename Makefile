# Compiler
CXX = g++

# Compiler flags
# -g = debug info
# -Wall = all warnings
CXXFLAGS = -std=c++17 -g -Wall

# Include paths
INCLUDES = -I. \
           -ISrc/App/encryptDecrypt \
           -ISrc/App/FileHandling \
           -ISrc/App/Processes

# List *all* your .cpp source files here
SRCS = main.cpp \
       Src/App/FileHandling/IO.cpp \
       Src/App/FileHandling/ReadEnv.cpp \
       Src/App/Processes/ProcessManagement.cpp \
       Src/App/encryptDecrypt/Cryption.cpp

# Automatically create a list of .o files from the .cpp files
# (e.g., main.cpp -> main.o)
OBJS = $(SRCS:.cpp=.o)

# The name of your final program
TARGET = my_program

# --- RULES ---

# The default rule (what runs when you just type "make")
# It depends on the final executable
all: $(TARGET)

# Rule to build the final executable
# It depends on all the object files (.o)
$(TARGET): $(OBJS)
	@echo "Linking..."
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Generic rule to compile any .cpp file into a .o file
# $< is the .cpp file (the dependency)
# $@ is the .o file (the target)
%.o: %.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Rule to clean up compiled files
clean:
	@echo "Cleaning up..."
	rm -f $(OBJS) $(TARGET)