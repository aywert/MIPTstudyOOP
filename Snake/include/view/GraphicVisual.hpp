#pragma once
#include "View.hpp"
#include <unistd.h>
#include <vector>
#include <memory>
#include <optional>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class GraphicVisual: public View {

  int Height;
  int Width;

  int block_size = 20;
  sf::RenderWindow window_;
  
  sf::Font font_;

  sf::Color convertToSfmlColor(int colorId);

  void clearScreen() override {window_.clear(sf::Color(0, 0, 0));}
  void setColor(int color) override{};

  void drawBox(Model& model);
  void drawRabbit(const Rabbit& rabbit) override;
  void drawSnake(const Snake& snake)    override;
  void drawSpace(Snake& snake) override{};
  void drawPortal(Model& model);

  void clearPosition(int x, int y);
  void clearSnake(const Snake& snake);

  void gotoxy(const int x, const int y) {};   
  void hideCursor() {};
  void showCursor() {};

  void handleResize(unsigned int newWidth, unsigned int newHeight);
  void drawScorePanel(Model& model);

  public: 
    void render(Model& model) override;
    Event getEvent(long time_mcsec) override;
    Event flushEvents()  override;
    void showFeatures(int round, int smart_wins, int silly_wins) override;
    
    GraphicVisual(Model& model);
};