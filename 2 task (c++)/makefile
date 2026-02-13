.PHONY : all run clean 

CXX = g++
CFLAGS = -Wall -Wextra

NAMEEXE := main
OBJFILES := main.o

# Добавляем build/ к именам файлов
BUILD_DIR := build
EXECUTABLE := $(BUILD_DIR)/$(NAMEEXE)
OBJECTS := $(addprefix $(BUILD_DIR)/, $(OBJFILES))

all: $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

# Создаем папку build перед компиляцией
$(EXECUTABLE) : $(OBJECTS) | $(BUILD_DIR)
	$(CXX) $(OBJECTS) -o $@

# Правило для создания папки build
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Правило для компиляции .c файлов в .o файлы в папке build
$(BUILD_DIR)/%.o : %.cpp | $(BUILD_DIR)
	$(CXX) $< -c -o $@ $(CFLAGS)

clean: 
	rm -rf $(BUILD_DIR)