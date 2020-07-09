CXX = g++
CXXFLAGS = -std=c++17 -Wall -pedantic -Wextra -g
LD = g++
LDLIBS = -lncurses

SRC_DIR = src
BUILD_DIR = build
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/*.cpp))
EXECUTABLE = latkamat

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@echo "Building $@."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

all: deps compile doc

deps:
	@echo "Creating dependencies."
	@g++ -MM $(SRC_DIR)/* > Makefile.d

run: deps compile
	@./$(EXECUTABLE)

valgrind: deps compile
	@valgrind ./$(EXECUTABLE) 2>valgrind-output.txt
	
valgrind-leaks: deps compile
	@valgrind --leak-check=full --show-leak-kinds=all ./$(EXECUTABLE) 2>valgrind-leaks-output.txt
	
valgrind-track: deps compile
	@valgrind --track-origins=yes ./$(EXECUTABLE) 2>valgrind-track-output.txt

compile: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ_FILES)
	@echo "Linking object files into executable \"$(EXECUTABLE)\"."
	@$(LD) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	@echo "Removing dependencies."
	@rm -rf Makefile.d
	@echo "Removing $(BUILD_DIR) directory."
	@rm -rf $(BUILD_DIR)
	@echo "Removing executable \"$(EXECUTABLE)\"."
	@rm -rf $(EXECUTABLE)
	@echo "Removing documentation."
	@rm -rf doc
	@echo "Removing valgrind outputs."
	@rm -rf valgrind*-output.txt
	@rm -rf vgcore*

doc:
	@echo "Creating documentation."
	@mkdir -p doc
	@doxygen Doxyfile

$(BUILD_DIR):
	@echo "Creating $(BUILD_DIR) directory."
	@mkdir -p $(BUILD_DIR)

-include Makefile.d
