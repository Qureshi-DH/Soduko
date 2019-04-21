CC := g++
CFLAGS := -pthread
TARGET := Soduko
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
OUT_DIR = .

# $(wildcard *.cpp /xxx/xxx/*.cpp): get all .cpp files from the current directory and dir "/xxx/xxx/"
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
# $(patsubst %.cpp,%.o,$(SRCS)): substitute all ".cpp" file name strings to ".o" file name strings
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o,$(SRCS))

all: $(OBJ_DIR) $(BIN_DIR) $(TARGET)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(BIN_DIR):
	mkdir $(BIN_DIR)


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp 
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) -pthread -o $(BIN_DIR)/$@ $^

	
clean:
	rm $(OBJ_DIR)/*.o
	rm $(BIN_DIR)/$(TARGET)
	rm $(OUT_DIR)/*

	
.PHONY: all clean
