.PHONY : all run clean 

CC = gcc
CFLAGS = -Wall -Wextra -std=c99

NAMEEXE := main
OBJFILES := main.o cat.o dog.o obj.o 

# Добавляем build/ к именам файлов
BUILD_DIR := build
EXECUTABLE := $(BUILD_DIR)/$(NAMEEXE)
OBJECTS := $(addprefix $(BUILD_DIR)/, $(OBJFILES))

all: $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

# Создаем папку build перед компиляцией
$(EXECUTABLE) : $(OBJECTS) | $(BUILD_DIR)
	$(CC) $(OBJECTS) -o $@

# Правило для создания папки build
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Правило для компиляции .c файлов в .o файлы в папке build
$(BUILD_DIR)/%.o : %.c | $(BUILD_DIR)
	$(CC) $< -c -o $@ $(CFLAGS)

clean: 
	rm -rf $(BUILD_DIR)