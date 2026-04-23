#pragma once
#include "View.hpp"
#include <csignal>
#include <atomic>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

std::atomic<bool> screen_needs_update{true};

void handle_winch(int sig) {
  screen_needs_update.store(true);
}

class TextVisual: public View {
  int offsetX;
  int offsetY;  
  std::string buffer;
  struct termios old_attr_;

  public:

  inline void drawBox(Model& model) {
    int screen_width  = model.getWidth();   // ширина терминала
    int screen_height = model.getHeight(); // высота терминала
    
    // Вычисляем размер внутреннего поля
    int field_width = screen_width - offsetX * 2;   // минус отступы и рамку
    int field_height = screen_height - offsetY * 2; // минус отступы и рамку
    
    // Верхняя граница
    gotoxy(offsetX, offsetY);
    buffer += "┌";
    for (int i = 0; i < field_width; i++) {
        buffer += "─";
    }
    buffer += "┐";
    
    // Заголовок
    setColor(45);
    gotoxy(offsetX + field_width / 2, 1);
    buffer.append("Slizarin");
    setColor(0);
    
    // Боковые границы
    for (int row = 0; row < field_height; row++) {  // row от 0 до field_height-1
        // Левая граница
        gotoxy(offsetX, offsetY + 1 + row);
        buffer += "│";
        
        // Правая граница (на той же строке)
        gotoxy(offsetX + field_width + 1, offsetY + 1 + row);
        buffer += "│";
    }
    
    // Нижняя граница
    gotoxy(offsetX, offsetY + field_height + 1);
    buffer += "└";
    for (int i = 0; i < field_width; i++) {
        buffer += "─";
    }
    buffer += "┘";
}

    virtual void gotoxy(const int x, const int y) override {
      int safeX = (x < 1) ? 1 : x;
      int safeY = (y < 1) ? 1 : y;
      buffer += "\033[" + std::to_string(safeY) + ";" + std::to_string(safeX) + "H";
    }

    virtual void hideCursor() override {
      buffer+="\033[?25l";
    }
    virtual void showCursor() override {
      buffer+="\033[?25h";
    }

    void clearScreen() override {
      buffer += "\033[2J\033[3J\033[H";
    }

    void clearSymbol() {buffer+="\033[P";}

      void setColor(int color) override{
        buffer.append("\033[");
        buffer.append(std::to_string(color));
        buffer.append("m");
      }

    void render(Model& model) override {
      hideCursor(); 
      if (screen_needs_update.load()) { //checkig whether there were changes in sizes of the window
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        
        model.setWidth(w.ws_col);
        model.setHeight(w.ws_row);

        buffer.clear();
        clearScreen(); 
        drawBox(model);
        
        screen_needs_update.store(false);
      }

      else {
        buffer.clear();
      }

      for (auto& snake: model.getSnakes()) {
        if (snake.getState() == SnakeStatus::DEAD) {
          clearSnake(snake);
          snake.rot();
          drawBox(model);
        }
          
        else
          drawSnake(snake);
      }

      for (auto& rabbit: model.getRabbits()) {
        drawRabbit(rabbit);
      }

      drawScorePanel(model);

      std::cout << buffer << std::flush;
    }

    void clearPosition(int x, int y, char ch = ' ') {
      gotoxy(x, y);
      buffer += ch;
    }

    void clearSnake(const Snake& snake) {
      for (const auto& seg : snake.getBody()) {
        clearPosition(seg.x, seg.y);
      }

      Segment sg = snake.getTail();
      clearPosition(sg.x, sg.y);
    }

    void drawRabbit(const Rabbit& rabbit) override {
      gotoxy(rabbit.getX(), rabbit.getY());
      buffer+='@';
    };
    void drawSnake(const Snake& snake) override{
      //deleting tail
      Segment tail = snake.getTail();
      gotoxy(tail.x, tail.y);
      buffer.append(" ");
      hideCursor();
      
      auto body = snake.getBody();
      auto it = body.begin();
      if (it != body.end()) {
        // Отрисовываем голову
        char symbol;
        switch(snake.getDirection()) {
          case Direction::UP:    symbol = '^'; break;
          case Direction::DOWN:  symbol = 'v'; break;
          case Direction::LEFT:  symbol = '<'; break;
          case Direction::RIGHT: symbol = '>'; break;
        }
        setColor(snake.getColor());
        gotoxy(it->x, it->y);
        buffer += symbol;
        setColor(0);
        
        ++it;
        // Отрисовываем тело
        for (; it != body.end(); ++it) {
          setColor(snake.getColor());
          gotoxy(it->x, it->y);
          buffer += 'o';
          setColor(0);
        }
      }
    };

    void drawSpace(Snake& snake) override{};

 void showFeatures(int round, int smart_wins, int silly_wins) override {
    // Очищаем буфер и экран
    buffer.clear();
    clearScreen();
    hideCursor();
    
    int screen_width = 80;
    
    // Функция для безопасного добавления пробелов
    auto safe_append = [&](int count) {
        if (count > 0) buffer.append(count, ' ');
    };
    
    // Верхняя граница
    setColor(36);
    buffer += "╔";
    for (int i = 0; i < screen_width - 2; i++) buffer += "═";
    buffer += "╗\n";
    
    // Заголовок
    buffer += "║";
    std::string title = "=== TOURNAMENT RESULTS ===";
    int title_pos = (screen_width - (int)title.length()) / 2;
    safe_append(title_pos - 1);
    buffer += title;
    safe_append(screen_width - title_pos - (int)title.length() - 1);
    buffer += "║\n";
    
    // Разделитель
    buffer += "╠";
    for (int i = 0; i < screen_width - 2; i++) buffer += "═";
    buffer += "╣\n";
    
    // Информация о раунде
    buffer += "║";
    std::string round_text = "ROUND " + std::to_string(round + 1) + " / 100";
    int round_pos = (screen_width - (int)round_text.length()) / 2;
    safe_append(round_pos - 1);
    buffer += round_text;
    safe_append(screen_width - round_pos - (int)round_text.length() - 1);
    buffer += "║\n";
    
    // Пустая строка
    buffer += "║";
    safe_append(screen_width - 2);
    buffer += "║\n";
    
    // Smart Bot статистика
    setColor(32);
    buffer += "║  █ SMART BOT";
    safe_append(screen_width - 15);
    buffer += "║\n";
    
    setColor(37);
    std::string smart_score = "    Score: " + std::to_string(smart_wins);
    buffer += "║";
    buffer += smart_score;
    safe_append(screen_width - (int)smart_score.length() - 1);
    buffer += "║\n";
    
    // Silly Bot статистика  
    setColor(31);
    buffer += "║  █ SILLY BOT";
    safe_append(screen_width - 15);
    buffer += "║\n";
    
    setColor(37);
    std::string silly_score = "    Score: " + std::to_string(silly_wins);
    buffer += "║";
    buffer += silly_score;
    safe_append(screen_width - (int)silly_score.length() - 1);
    buffer += "║\n";
    
    // VS строка
    buffer += "║";
    std::string vs_text = "         VS";
    buffer += vs_text;
    safe_append(screen_width - (int)vs_text.length() - 1);
    buffer += "║\n";
    
    // Пустая строка
    buffer += "║";
    safe_append(screen_width - 2);
    buffer += "║\n";
    
    // Проценты побед
    int total = smart_wins + silly_wins;
    if (total > 0) {
        int smart_percent = (smart_wins * 100) / total;
        int silly_percent = (silly_wins * 100) / total;
        
        setColor(36);
        std::string percent_text = "Win Rate: Smart " + std::to_string(smart_percent) + 
                                   "%  |  Silly " + std::to_string(silly_percent) + "%";
        buffer += "║";
        int percent_pos = (screen_width - (int)percent_text.length()) / 2;
        safe_append(percent_pos - 1);
        buffer += percent_text;
        safe_append(screen_width - percent_pos - (int)percent_text.length() - 1);
        buffer += "║\n";
        
        // Пустая строка
        buffer += "║";
        safe_append(screen_width - 2);
        buffer += "║\n";
    }
    
    // Прогресс-бар
    setColor(33);
    buffer += "║  Tournament Progress: ";
    
    int bar_width = screen_width - 30;
    int filled = (bar_width * (round + 1)) / 100;
    
    // Зелёный фон для заполненной части
    buffer += "\033[42m";
    if (filled > 0) buffer.append(filled, ' ');
    buffer += "\033[0m";
    
    // Тёмно-серый фон для пустой части
    buffer += "\033[100m";
    if (bar_width - filled > 0) buffer.append(bar_width - filled, ' ');
    buffer += "\033[0m";
    
    setColor(37);
    std::string percent = " " + std::to_string(((round + 1) * 100) / 100) + "%";
    buffer += percent;
    safe_append(screen_width - 30 - bar_width - (int)percent.length() - 2);
    buffer += "║\n";
    
    // Нижняя граница
    setColor(36);
    buffer += "╚";
    for (int i = 0; i < screen_width - 2; i++) buffer += "═";
    buffer += "╝\n";
    
    // Инструкция
    setColor(90);
    std::string instruction = "Press SPACE or ENTER to continue...";
    int inst_pos = (screen_width - (int)instruction.length()) / 2;
    buffer += "\n";
    safe_append(inst_pos);
    buffer += instruction;
    buffer += "\n";
    
    setColor(0);
    
    // Выводим буфер
    std::cout << buffer << std::flush;
    
    // Ждём нажатия клавиши
    char ch;
    while (true) {
        if (read(STDIN_FILENO, &ch, 1) > 0) {
            if (ch == ' ' || ch == '\n' || ch == '\r') {
                break;
            }
        }
        usleep(10000);
    }
    
    // Просто очищаем буфер, не трогая экран
    screen_needs_update.store(true);
    std::cout << buffer << std::endl;
    buffer.clear();
    // Не вызываем clearScreen() здесь, так как экран будет очищен в следующей итерации
}

  Event flushEvents() override {
    // in Text visual we don't have to worry about emplying buffer
    return Event();
  }
  

  Event getEvent(long time_mlsec) override {
    fd_set read_fds;
    Event event; 
    
    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO, &read_fds);
    
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = time_mlsec*1000; //need to be more than tick of model

    int retval = select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &timeout);
    
    if (retval == -1) {
      fprintf(stderr, "select error\n");
      return event;
    } 
    else if (retval > 0) {
      char ch;
      while (read(STDIN_FILENO, &ch, 1) > 0) {
        EventType type;
        bool valid = true;
        switch(ch) {
          //player number 1
          case 'w': type = EventType::UP_1;    break;
          case 'a': type = EventType::LEFT_1;  break;
          case 's': type = EventType::DOWN_1;  break;
          case 'd': type = EventType::RIGHT_1; break;

          case 'p': type = EventType::PAUSE;   break;

          case '\x1b': { 
            char seq[2];
            const ssize_t n1 = ::read(STDIN_FILENO, &seq[0], 1);
            const ssize_t n2 = ::read(STDIN_FILENO, &seq[1], 1);

            if (n1 <= 0 || n2 <= 0) {
              event.setEventType(EventType::BAD);
              break;
            }

            if (seq[0] == '[') {
              switch (seq[1]) {
                case 'A': type = EventType::UP_2;    break;   
                case 'B': type = EventType::DOWN_2;  break;
                case 'C': type = EventType::RIGHT_2; break;
                case 'D': type = EventType::LEFT_2;  break;
                default: valid = false; break;;
              }
            }

            break;
          }

          default:  valid = false; break;          
        }

        if (valid) {
          event.setEventType(type); 
        }
      }
    }
    
    return event;
  }

    TextVisual(Model& model) {
      offsetX = model.getRowShift();
      offsetY = model.getColShift();

      if (tcgetattr(STDIN_FILENO, &old_attr_) == -1) {
        perror("tcgetattr");
        return;
      }

      struct termios new_attr = old_attr_;

      new_attr.c_lflag &= ~ECHO;  // turning off ECHO
      new_attr.c_lflag &= ~ICANON; // turning off canoniccal mode

      new_attr.c_cc[VMIN] = 0;  // Не ждать ни одного символа
      new_attr.c_cc[VTIME] = 0; // Не ждать по времени
    
      if (tcsetattr(STDIN_FILENO, TCSANOW, &new_attr) == -1) {
        perror("tcsetattr");
        return;
      }
    }

    void drawScorePanel(Model& model) {
      // Определяем позицию панели (правый верхний угол)
      int panelX = model.getWidth() - 25;  // отступ справа
      int panelY = 1;                       // отступ сверху
      
      // Заголовок панели
      setColor(33);  // Жёлтый
      gotoxy(panelX + 6, panelY);
      buffer += "SCORES";
      
      // Рамка панели
      setColor(36);  // Циан для рамки
      int panel_width = 20;
      int panel_height = 5 + model.getSnakes().size();  // Высота в зависимости от количества змей
      
      // Верхняя граница
      gotoxy(panelX, panelY);
      buffer += "╔";
      for (int i = 0; i < panel_width; i++) buffer += "═";
      buffer += "╗";
      
      // Боковые границы и содержимое
      int current_y = panelY + 1;
      
      // Пустая строка под заголовком
      gotoxy(panelX, current_y);
      buffer += "║";
      setColor(0);
      for (int i = 0; i < panel_width; i++) buffer += " ";
      setColor(36);
      buffer += "║";
      current_y++;
      
      // Выводим очки для каждой змеи
      for (const auto& snake : model.getSnakes()) {
          gotoxy(panelX, current_y);
          buffer += "║ ";
          
          // Цвет змеи для её имени
          setColor(snake.getColor());
          
          // ID змеи
          std::string snakeText = "Snake " + std::to_string(snake.getID()) + ":";
          buffer += snakeText;
          
          // Очки (белым цветом)
          setColor(37);  // Белый
          std::string scoreText = std::to_string(snake.getLength());
          
          // Выравниваем по правому краю
          int padding = panel_width - 2 - snakeText.length() - scoreText.length();
          for (int i = 0; i < padding; i++) buffer += " ";
          buffer += scoreText;
          
          setColor(36);
          buffer += " ║";
          current_y++;
      }
      
      // Заполняем оставшееся пространство пустыми строками (если нужно)
      while (current_y < panelY + panel_height - 1) {
          gotoxy(panelX, current_y);
          buffer += "║";
          for (int i = 0; i < panel_width; i++) buffer += " ";
          buffer += "║";
          current_y++;
      }
      
      // Нижняя граница
      gotoxy(panelX, panelY + panel_height - 1);
      buffer += "╚";
      for (int i = 0; i < panel_width; i++) buffer += "═";
      buffer += "╝";
      
      setColor(0);  // Сброс цвета
  }

    ~TextVisual() {
      std::cout << "\033[0m\033[?25h" << std::flush;
      if (tcsetattr(STDIN_FILENO, TCSANOW, &old_attr_) == -1) {
        perror("Critical: Failed to restore terminal attributes");
      }
    } 
};