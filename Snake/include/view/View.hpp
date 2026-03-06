#pragma once

#include "../Model.hpp"

class View {
  std::list<Event> events;

  public:
    virtual std::list<Event> getEvents() = 0;

    virtual void clearScreen() = 0;
    virtual void render(Model& model) = 0;
    virtual void drawRabbit(Rabbit& rabbit) = 0;
    virtual void drawSnake(Snake& snake) = 0;
    virtual void drawSpace(Snake& snake) = 0;

    virtual void gotoxy(const int x, const int y) = 0;
    virtual void setColor(int color) = 0;
    virtual void hideCursor() = 0;
    virtual void showCursor() = 0;
};  