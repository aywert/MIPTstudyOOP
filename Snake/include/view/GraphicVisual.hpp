#pragma once
#include "View.hpp"

class GraphicVisual: public View {
  public: 
    void clearScreen() override {}
    void setColor() override{};
    void render(Model& model) override {};
    void drawRabbit(Rabbit& rabbit) override{};
    void drawSnake(Snake& snake) override{};
    void drawSpace(Snake& snake) override{};

    std::list<Event> getEvents() override {
      Event event();
      std::list<Event> lst;
      lst.emplace_back(event);
      return lst;
    }
};