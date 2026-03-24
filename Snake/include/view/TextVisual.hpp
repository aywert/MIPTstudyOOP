#pragma once
#include "View.hpp"
#include <csignal>
#include <atomic>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>

std::atomic<bool> screen_needs_update{true};

void handle_winch(int sig) {
  screen_needs_update.store(true);
}

class TextVisual: public View {
  std::string buffer;
  struct termios old_attr_;

  public:

  inline void drawBox(Model& model) {

    int width  = model.getWidth(); 
    int height = model.getHeight(); 
    
    int offsetX = 2;
    int offsetY = 2;
    
    gotoxy(offsetX, offsetY);
    
    buffer += "┌";
    
    for (int index = 0; index < width; index++) {buffer += "─";}
      buffer += "┐";

    setColor(45);
    gotoxy(width/2, 1);
    buffer.append("Slizarin");
    setColor(0);
    
    
    for (int row = 1; row <= height; row++) {
      gotoxy(offsetX, offsetY + row);
      buffer += "│";
      
      gotoxy(offsetX + width + 1, offsetY + row);
      buffer += "│";
    }
    
    gotoxy(offsetX, offsetY + height + 1);
    buffer += "└";
    for (int i = 0; i < width; i++) {
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
        
        model.setWidth(w.ws_col - 5);
        model.setHeight(w.ws_row - 5);

        buffer.clear();
        clearScreen(); 
        drawBox(model);
        
        screen_needs_update.store(false);
      }

      else {
        buffer.clear();
      }

      for (const auto& snake: model.getSnakes()) {
        drawSnake(snake);
      }

      std::cout << buffer << std::flush;
    }

    void drawRabbit(const Rabbit& rabbit) override{};
    void drawSnake(const Snake& snake) override{
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
      //deleting tail
      Segment tail = snake.getTail();
      gotoxy(tail.x, tail.y);
      buffer.append(" ");
      hideCursor();
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
        return Event(EventType::BAD);
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

    TextVisual() {
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