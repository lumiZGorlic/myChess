OBJDIR = obj

# Declaration of variables
CC = g++
CC_FLAGS = -std=c++11
OBJ_FLAGS = -g -rdynamic

# File names
EXEC = myChess
SOURCES = $(wildcard *.cpp)
OBJECTS = $(addprefix obj/,$(notdir $(SOURCES:.cpp=.o)))

# Main target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJ_FLAGS) $(OBJECTS) -o $(EXEC)

# To obtain object files
$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CC) $(CC_FLAGS) -c $< -o $@

# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS)

