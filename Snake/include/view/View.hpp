#pragma once

#include "../Model.hpp"

class View {
  std::vector<Event> events;

  public:
    virtual Event getEvent(long time_mcsec) = 0;

    virtual void clearScreen() = 0;
    virtual void render(Model& model) = 0;
    virtual void drawRabbit(const Rabbit& rabbit) = 0;
    virtual void drawSnake(const Snake& snake) = 0;
    virtual void drawSpace(Snake& snake) = 0;

    virtual void gotoxy(const int x, const int y) = 0;
    virtual void setColor(int color) = 0;
    virtual void hideCursor() = 0;
    virtual void showCursor() = 0;
};  