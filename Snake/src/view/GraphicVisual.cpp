#include "GraphicVisual.hpp"

void GraphicVisual::render(Model& model) {
  window_.clear(sf::Color(0, 0, 0));
  
  // Получаем размеры окна
  int windowWidth = Width * block_size;
  int windowHeight = Height * block_size;
  
  // Градиент на весь экран (от темно-синего к черному)
  for (int i = 0; i < windowHeight; i++) {
    sf::RectangleShape gradient(sf::Vector2f(windowWidth, 1));
    // Плавный переход цвета сверху вниз
    int r = 30 + i * 40 / windowHeight;  // красный увеличивается
    int g = 10 + i * 20 / windowHeight;  // Зеленый увеличивается
    int b = 100;  // Постоянный синий
    
    gradient.setFillColor(sf::Color(r, g, b));
    gradient.setPosition(0, i);
    window_.draw(gradient);
  }

  drawBox(model);
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
      window_(sf::VideoMode(Width*block_size, Height*block_size), "Snake") {
        if (!font_.loadFromFile("font/arialmt.ttf")) {  // Загрузите любой шрифт
          throw std::runtime_error("Failed to load font from font/arialmt.ttf");
        }
      }

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

void GraphicVisual::drawBox(Model& model) {
  int screen_width = model.getWidth();
  int screen_height = model.getHeight();
  int offsetX = model.getColShift();
  int offsetY = model.getRowShift();
  
  int min_x = offsetX + 1;
  int max_x = screen_width - offsetX;
  int min_y = offsetY + 1;
  int max_y = screen_height - offsetY;
  
  int field_width = max_x - min_x + 1;
  int field_height = max_y - min_y + 1;
  
  int boxOffsetX = min_x * block_size;
  int boxOffsetY = min_y * block_size;
  float boxWidth = field_width * block_size;
  float boxHeight = field_height * block_size;
  
  
  // Основная рамка
  sf::RectangleShape outerBox(sf::Vector2f(boxWidth, boxHeight));
  outerBox.setFillColor(sf::Color::Transparent);
  outerBox.setOutlineColor(sf::Color(150, 150, 150));
  outerBox.setOutlineThickness(3.0f);
  outerBox.setPosition(boxOffsetX, boxOffsetY);
  window_.draw(outerBox);
  
  // Внутренняя рамка
  sf::RectangleShape innerBox(sf::Vector2f(boxWidth - 4, boxHeight - 4));
  innerBox.setFillColor(sf::Color::Transparent);
  innerBox.setOutlineColor(sf::Color(100, 100, 100));
  innerBox.setOutlineThickness(1.0f);
  innerBox.setPosition(boxOffsetX + 2, boxOffsetY + 2);
  window_.draw(innerBox);
  
  // Рисуем декоративных змеек по углам
  float startX, startY;
  int bodySize = 8;
  
  // Верхний левый угол - змейка ползет вправо
  startX = boxOffsetX - 30;
  startY = boxOffsetY - 20;
  for (int i = 0; i < 3; i++) {
    sf::RectangleShape body(sf::Vector2f(bodySize, bodySize));
    body.setFillColor(sf::Color(50 + i*30, 150, 50));
    body.setPosition(startX + i * (bodySize + 2), startY);
    window_.draw(body);
  }
  // Голова
  sf::RectangleShape head(sf::Vector2f(bodySize + 2, bodySize + 2));
  head.setFillColor(sf::Color(50, 200, 50));
  head.setPosition(startX + 3 * (bodySize + 2), startY - 1);
  window_.draw(head);
  // Глаз
  sf::CircleShape eye(2);
  eye.setFillColor(sf::Color::White);
  eye.setPosition(startX + 3 * (bodySize + 2) + bodySize - 3, startY + 2);
  window_.draw(eye);
  
  // Верхний правый угол - змейка ползет влево
  startX = boxOffsetX + boxWidth + 10;
  startY = boxOffsetY - 20;
  for (int i = 0; i < 3; i++) {
    sf::RectangleShape body(sf::Vector2f(bodySize, bodySize));
    body.setFillColor(sf::Color(50 + i*30, 150, 50));
    body.setPosition(startX - i * (bodySize + 2), startY);
    window_.draw(body);
  }
  // Голова
  head.setPosition(startX - 3 * (bodySize + 2) - (bodySize + 2) + 10, startY - 1);
  window_.draw(head);
  // Глаз
  eye.setPosition(startX - 3 * (bodySize + 2) - 5, startY + 2);
  window_.draw(eye);
  
  // Нижний левый угол - змейка ползет вниз (смещена повыше)
  startX = boxOffsetX - 20;
  startY = boxOffsetY + boxHeight - 40;  // Смещено повыше
  for (int i = 0; i < 3; i++) {
    sf::RectangleShape body(sf::Vector2f(bodySize, bodySize));
    body.setFillColor(sf::Color(50 + i*30, 150, 50));
    body.setPosition(startX, startY + i * (bodySize + 2));
    window_.draw(body);
  }
  // Голова
  head.setPosition(startX - 1, startY + 3 * (bodySize + 2) + 2);
  window_.draw(head);
  // Глаз
  eye.setPosition(startX + 2, startY + 3 * (bodySize + 2) + 7);
  window_.draw(eye);
  
  // Нижний правый угол - змейка ползет вверх
  startX = boxOffsetX + boxWidth + 10;
  startY = boxOffsetY + boxHeight - 20;
  for (int i = 0; i < 3; i++) {
    sf::RectangleShape body(sf::Vector2f(bodySize, bodySize));
    body.setFillColor(sf::Color(50 + i*30, 150, 50));
    body.setPosition(startX, startY - i * (bodySize + 2));
    window_.draw(body);
  }
  // Голова
  head.setPosition(startX - 1, startY - 3 * (bodySize + 2) - (bodySize + 2) + 6);
  window_.draw(head);
  // Глаз
  eye.setPosition(startX + 2, startY - 3 * (bodySize + 2) - 3);
  window_.draw(eye);
  
  // Заголовок с тенью
  sf::Text title;
  title.setFont(font_);
  title.setString("SNAKE GAME");
  title.setCharacterSize(28);
  title.setStyle(sf::Text::Bold);
  
  float titleX = boxOffsetX + boxWidth / 2 - title.getLocalBounds().width / 2;
  float titleY = boxOffsetY - 40;
  
  // Тень
  title.setFillColor(sf::Color(40, 40, 40));
  title.setPosition(titleX + 2, titleY + 2);
  window_.draw(title);
  
  // Основной текст
  title.setFillColor(sf::Color(200, 150, 50));
  title.setPosition(titleX, titleY);
  window_.draw(title);
}