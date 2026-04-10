#pragma once
#include "View.hpp"
#include <unistd.h>
#include <vector>
#include <memory>
#include <optional>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

constexpr int block_size = 20;

class GraphicVisual: public View {

  int Height;
  int Width;
  sf::RenderWindow window_;
  
  // Текстуры для разных типов объектов
  sf::Texture headTexture_;
  sf::Texture bodyTexture_;
  sf::Texture rabbitTexture_;

  sf::Sprite headSprite_;
  sf::Sprite bodySprite_;
  sf::Sprite rabbitSprite_;
  // Шрифт для текста
  sf::Font font_;
  sf::Text scoreText_;

  sf::Color convertToSfmlColor(int colorId);

  public: 
    void clearScreen() override {}
    void setColor(int color) override{};
    void render(Model& model) override;
    void drawRabbit(const Rabbit& rabbit) override;
    void drawSnake(const Snake& snake)    override;
    void drawSpace(Snake& snake) override{};

    void clearPosition(int x, int y);
    void clearSnake(const Snake& snake);

    void gotoxy(const int x, const int y) {};   
    void hideCursor() {};
    void showCursor() {};

    Event getEvent(long time_mcsec) override;
    GraphicVisual(Model& model);
};