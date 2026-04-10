#include "GraphicVisual.hpp"

void GraphicVisual::render(Model& model) {
  window_.clear();

  for (auto& snake: model.getSnakes()) {
    if (snake.getState() == SnakeStatus::DEAD) {
      clearSnake(snake);
      snake.rot();
    }
      
    else
      drawSnake(snake);
  }

  for (auto& rabbit: model.getRabbits()) {
    drawRabbit(rabbit);
  }

  window_.display();
};

GraphicVisual::GraphicVisual(Model& model) 
    : Width(model.getWidth()) , Height(model.getHeight()), 
      window_(sf::VideoMode(Width*block_size, Height*block_size), "Snake") {}

Event GraphicVisual::getEvent(long time_mcsec) {
  sf::Event sfmlEvent;
  
  // Проверяем, есть ли события в очереди
  if (window_.pollEvent(sfmlEvent)) {
    switch (sfmlEvent.type) {
      case sf::Event::Closed:
        return Event(EventType::HALT);
          
      case sf::Event::KeyPressed:
        switch (sfmlEvent.key.code) {
          case sf::Keyboard::Up:
            return Event(EventType::UP_2);
          case sf::Keyboard::Down:
            return Event(EventType::DOWN_2);
          case sf::Keyboard::Left:
            return Event(EventType::LEFT_2);
          case sf::Keyboard::Right:
            return Event(EventType::RIGHT_2);

          case sf::Keyboard::W:
            return Event(EventType::UP_1);
          case sf::Keyboard::S:
            return Event(EventType::DOWN_1);
          case sf::Keyboard::A:
            return Event(EventType::LEFT_1);
          case sf::Keyboard::D:
            return Event(EventType::RIGHT_1);

          default:
              return Event(); // BAD событие
        }
          
      default:
        return Event();
    }
  }

  if (time_mcsec > 0) {
    usleep(time_mcsec*500);
  }

  return Event(); // BAD событие
} 

void GraphicVisual::drawSnake(const Snake& snake) {
  const auto& body = snake.getBody();
  
  if (body.empty()) return;
  
  // Получаем цвет змейки (предполагаем, что color_ это int от 0 до 7 или RGB)
  sf::Color snakeColor = convertToSfmlColor(snake.getColor()); // или snake.color_
  
  sf::RectangleShape square(sf::Vector2f(block_size/4*3, block_size/4*3));
  
  // Рисуем тело (используем цвет змейки)
  square.setFillColor(snakeColor);
  for (auto it = std::next(body.begin()); it != body.end(); ++it) {
      square.setPosition(it->x * block_size, it->y * block_size);
      window_.draw(square);
  }
  
  // Рисуем голову (более яркий/темный оттенок или другой цвет)
  sf::Color headColor = convertToSfmlColor(snake.getColor()+10);
  square.setFillColor(headColor);
  square.setPosition(body.front().x * block_size, body.front().y * block_size);
  window_.draw(square);
}

void GraphicVisual::drawRabbit(const Rabbit& rabbit) {
  sf::RectangleShape square(sf::Vector2f(block_size/4*3, block_size/4*3));
  square.setFillColor(sf::Color(200, 40, 30));
  square.setPosition(rabbit.getX() * block_size, rabbit.getY() * block_size);
  window_.draw(square);
}

void GraphicVisual::clearPosition(int x, int y) {
  sf::RectangleShape square(sf::Vector2f(block_size, block_size));

  square.setFillColor(sf::Color(0, 0, 0));
  square.setPosition(x, y);
  window_.draw(square);
}

void GraphicVisual::clearSnake(const Snake& snake) {
  for (const auto& seg : snake.getBody()) {
    clearPosition(seg.x, seg.y);
  }

  Segment sg = snake.getTail();
  clearPosition(sg.x, sg.y);
}

sf::Color GraphicVisual::convertToSfmlColor(int colorId) {
  // Соответствие вашим ID цветов (32, 33, 34, 36 и т.д.)
  switch(colorId) {
    case 32: return sf::Color(255, 100, 50);   // Оранжевый
    case 33: return sf::Color(50, 255, 100);   // Зеленый
    case 34: return sf::Color(100, 50, 255);   // Синий
    case 35: return sf::Color(255, 255, 50);   // Желтый
    case 36: return sf::Color(255, 50, 255);   // Пурпурный
    // Добавьте другие цвета по необходимости
    default: return sf::Color::White;          // Цвет по умолчанию
  }
}