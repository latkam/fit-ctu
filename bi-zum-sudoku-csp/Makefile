CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -pedantic -Wextra -g
LD = g++
LDLIBS =

SRC_DIR = src
BUILD_DIR = build
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/*.cpp))
EXECUTABLE = sudoku
INPUT_FILE = "examples/sudoku0a.in"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@echo "Building $@."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

all: deps compile

deps:
	@echo "Creating dependencies."
	@g++ -MM $(SRC_DIR)/* > Makefile.d
	
manual: deps compile
	@./$(EXECUTABLE) $(INPUT_FILE) manual

half-automatic: deps compile
	@./$(EXECUTABLE) $(INPUT_FILE) half-automatic

automatic: deps compile
	@./$(EXECUTABLE) $(INPUT_FILE) automatic

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


$(BUILD_DIR):
	@echo "Creating $(BUILD_DIR) directory."
	@mkdir -p $(BUILD_DIR)

-include Makefile.d
