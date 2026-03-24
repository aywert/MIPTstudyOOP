#pragma once
#include "View.hpp"
#include <vector>

class GraphicVisual: public View {
  public: 
    void clearScreen() override {}
    void setColor(int color) override{};
    void render(Model& model) override {};
    void drawRabbit(const Rabbit& rabbit) override{};
    void drawSnake(const Snake& snake) override{};
    void drawSpace(Snake& snake) override{};

    Event getEvent(long time_mcsec) override {
      Event events;
      return events;;
    }
};