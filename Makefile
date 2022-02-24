
PROJ_NAME = base

CC = g++-10 -std=c++20
FLAGS = -Wall
LIBS = -lGL -ldl -lglfw

SRC = src
BUILD = build
BIN = bin
INCLUDE = include

OBJS = $(patsubst $(SRC)/%.c, $(BUILD)/%.o, $(wildcard $(SRC)/*.c)) $(patsubst $(SRC)/%.cpp, $(BUILD)/%.o, $(wildcard $(SRC)/*.cpp))
TARGET = bin/$(PROJ_NAME)


all: $(TARGET)

release: FLAGS += -DNDEBUG -O3
release: $(TARGET)


$(TARGET): $(BIN) $(BUILD) $(OBJS)
	$(CC) $(FLAGS) $(OBJS) $(LIBS) -o $@

$(BUILD):
	mkdir $@

$(BIN):
	mkdir $@

$(BUILD)/glad.o: $(SRC)/glad.c
	$(CC) $(FLAGS) -I $(INCLUDE) -c $< -o $@

$(BUILD)/%.o: $(SRC)/%.cpp
	$(CC) $(FLAGS) -I $(INCLUDE) -c $< -o $@

.PHONY: clean
clean:
	rm -r $(BIN)
	rm -r $(BUILD)
