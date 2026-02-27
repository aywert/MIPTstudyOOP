#pragma once
#include "View.hpp"

class TextVisual {

  public: 
    void clearScreen() {}
    void setColor() {};
    void render(Model& model) {};
    void drawRabbit(Rabbit& rabbit) {};
    void drawSnake(Snake& snake) {};
    void drawSpace(Snake& snake) {};

    std::list<Event> getEvents() {
      Event event();
      std::list<Event> lst;
      lst.emplace_back(event);
      return lst;
    }

    std::list<Event> runLoop() {}
};