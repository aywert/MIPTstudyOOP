#pragma once
#include "View.hpp"
#include <csignal>
#include <atomic>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

extern std::atomic<bool> screen_needs_update;
void handle_winch(int sig);

class TextVisual: public View {
  int offsetX;
  int offsetY;  
  std::string buffer;
  struct termios old_attr_;

  void drawScorePanel(Model& model);
  void drawBox(Model& model);
  void gotoxy(const int x, const int y) override;
  void hideCursor() override {
    buffer+="\033[?25l";
  }
  void showCursor() override {
    buffer+="\033[?25h";
  }
  void clearScreen() override {
    buffer += "\033[2J\033[3J\033[H";
  }
  void clearSymbol() {buffer+="\033[P";}
  void setColor(int color) override;
  void clearPosition(int x, int y, char ch = ' ');
  void clearSnake(const Snake& snake);

  void drawRabbit(const Rabbit& rabbit) override;
  void drawPortal(Model& model);
  void drawSnake(const Snake& snake) override;
  void drawSpace(Snake& snake) override{};

  public:
    void render(Model& model) override;
    void showFeatures(int round, int smart_wins, int silly_wins) override;
    Event flushEvents() override {
      // in Text visual we don't have to worry about emplying buffer
      return Event();
    }

    Event getEvent(long time_mlsec) override;

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