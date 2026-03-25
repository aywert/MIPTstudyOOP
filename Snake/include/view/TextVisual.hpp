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
    int screen_width = model.getWidth();   // ширина терминала
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
      
      for (const auto& seg : snake.getBody()) {
        char symbol;
        if (seg.type == SegmentType::HEAD) {
          switch(snake.getDirection()) {
            case Direction::UP:    symbol = '^'; break;
            case Direction::DOWN:  symbol = 'v'; break;
            case Direction::LEFT:  symbol = '<'; break;
            case Direction::RIGHT: symbol = '>'; break;
          }
        } else {
          symbol = 'o';
        }

        gotoxy(seg.x, seg.y);
        buffer+=symbol;
      }
      
    };

    void drawSpace(Snake& snake) override{};

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
            case 'w': type = EventType::UP;    break;
            case 'a': type = EventType::LEFT;  break;
            case 's': type = EventType::DOWN;  break;
            case 'd': type = EventType::RIGHT; break;
            case 'p': type = EventType::PAUSE; break;
            default:  valid = false;           break;
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

    ~TextVisual() {
      std::cout << "\033[0m\033[?25h" << std::flush;
      if (tcsetattr(STDIN_FILENO, TCSANOW, &old_attr_) == -1) {
        perror("Critical: Failed to restore terminal attributes");
      }
    } 
};