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
    int b = 70;  // Постоянный синий
    
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

  drawScorePanel(model);

  window_.display();
};

GraphicVisual::GraphicVisual(Model& model) 
    : Width(model.getWidth()) , Height(model.getHeight()), 
      window_(sf::VideoMode(Width*block_size, Height*block_size), "Snake") {
        if (!font_.loadFromFile("font/arialmt.ttf")) {  // Загрузите любой шрифт
          throw std::runtime_error("Failed to load font from font/arialmt.ttf");
        }
      }

void GraphicVisual::flashEvents() {
  sf::Event sfmlEvent;
  while (window_.pollEvent(sfmlEvent)) {
    if (sfmlEvent.type == sf::Event::Closed) {
      return;
    }

    if (sfmlEvent.type == sf::Event::Resized)
      handleResize(sfmlEvent.size.width, sfmlEvent.size.height);
      return;
  }
}

void GraphicVisual::showFeatures(int round, int smart_wins, int silly_wins) {
    window_.clear(sf::Color(20, 20, 30)); // Темно-синий фон вместо черного
    
    sf::Text text;
    text.setFont(font_);
    
    // Получаем центр окна
    float centerX = Width  * block_size / 2.0f;
    float centerY = Height * block_size / 2.0f;
    
    // Заголовок
    text.setString("=== TOURNAMENT RESULTS ===");
    text.setCharacterSize(32);
    text.setFillColor(sf::Color::Yellow);
    text.setStyle(sf::Text::Bold);
    // Центрируем заголовок
    sf::FloatRect bounds = text.getLocalBounds();
    text.setPosition(centerX - bounds.width / 2, 50);
    window_.draw(text);
    
    // Разделительная линия
    sf::RectangleShape line(sf::Vector2f(400, 2));
    line.setFillColor(sf::Color(100, 100, 150));
    line.setPosition(centerX - 200, 110);
    window_.draw(line);
    
    // Текущий раунд
    text.setString("ROUND " + std::to_string(round + 1) + " / 100");
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);
    text.setStyle(sf::Text::Regular);
    bounds = text.getLocalBounds();
    text.setPosition(centerX - bounds.width / 2, 130);
    window_.draw(text);
    
    // Контейнер для результатов
    float resultsY = 200;
    float boxWidth = 250;
    float boxHeight = 120;
    float spacing = 40;
    
    // Левая панель (Smart Bot)
    sf::RectangleShape smartBox(sf::Vector2f(boxWidth, boxHeight));
    smartBox.setFillColor(sf::Color(0, 40, 0, 200));
    smartBox.setOutlineColor(sf::Color::Green);
    smartBox.setOutlineThickness(3);
    smartBox.setPosition(centerX - boxWidth - spacing/2, resultsY);
    window_.draw(smartBox);
    
    // Правая панель (Silly Bot)
    sf::RectangleShape sillyBox(sf::Vector2f(boxWidth, boxHeight));
    sillyBox.setFillColor(sf::Color(40, 0, 0, 200));
    sillyBox.setOutlineColor(sf::Color::Red);
    sillyBox.setOutlineThickness(3);
    sillyBox.setPosition(centerX + spacing/2, resultsY);
    window_.draw(sillyBox);
    
    // Smart Bot текст
    text.setString("SMART BOT");
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::Green);
    text.setStyle(sf::Text::Bold);
    bounds = text.getLocalBounds();
    text.setPosition(centerX - boxWidth - spacing/2 + (boxWidth - bounds.width) / 2, resultsY + 20);
    window_.draw(text);
    
    // Smart Bot очки
    text.setString(std::to_string(smart_wins));
    text.setCharacterSize(48);
    text.setFillColor(sf::Color::White);
    text.setStyle(sf::Text::Bold);
    bounds = text.getLocalBounds();
    text.setPosition(centerX - boxWidth - spacing/2 + (boxWidth - bounds.width) / 2, resultsY + 50);
    window_.draw(text);
    
    // Silly Bot текст
    text.setString("SILLY BOT");
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::Red);
    text.setStyle(sf::Text::Bold);
    bounds = text.getLocalBounds();
    text.setPosition(centerX + spacing/2 + (boxWidth - bounds.width) / 2, resultsY + 20);
    window_.draw(text);
    
    // Silly Bot очки
    text.setString(std::to_string(silly_wins));
    text.setCharacterSize(48);
    text.setFillColor(sf::Color::White);
    text.setStyle(sf::Text::Bold);
    bounds = text.getLocalBounds();
    text.setPosition(centerX + spacing/2 + (boxWidth - bounds.width) / 2, resultsY + 50);
    window_.draw(text);
    
    // VS между ними
    text.setString("VS");
    text.setCharacterSize(36);
    text.setFillColor(sf::Color::Cyan);
    text.setStyle(sf::Text::Bold);
    bounds = text.getLocalBounds();
    text.setPosition(centerX - bounds.width / 2, resultsY + boxHeight/2 - 20);
    window_.draw(text);
    
    // Проценты
    int total = smart_wins + silly_wins;
    if (total > 0) {
        float smart_percent = (smart_wins * 100.0f) / total;
        float silly_percent = (silly_wins * 100.0f) / total;
        
        text.setString("WIN RATE");
        text.setCharacterSize(16);
        text.setFillColor(sf::Color(180, 180, 180));
        text.setStyle(sf::Text::Regular);
        bounds = text.getLocalBounds();
        text.setPosition(centerX - bounds.width / 2, resultsY + boxHeight + 20);
        window_.draw(text);
        
        text.setString("Smart " + std::to_string((int)smart_percent) + "%  |  Silly " + 
                      std::to_string((int)silly_percent) + "%");
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::Cyan);
        text.setStyle(sf::Text::Bold);
        bounds = text.getLocalBounds();
        text.setPosition(centerX - bounds.width / 2, resultsY + boxHeight + 45);
        window_.draw(text);
    }
    
    // Прогресс-бар
    float progressY = resultsY + boxHeight + 100;
    float progressWidth = 500;
    float progress = (round + 1) / 100.0f;
    
    // Заголовок прогресса
    text.setString("TOURNAMENT PROGRESS");
    text.setCharacterSize(14);
    text.setFillColor(sf::Color(180, 180, 180));
    text.setStyle(sf::Text::Regular);
    bounds = text.getLocalBounds();
    text.setPosition(centerX - bounds.width / 2, progressY - 20);
    window_.draw(text);
    
    // Фон прогресс-бара
    sf::RectangleShape progressBg(sf::Vector2f(progressWidth, 25));
    progressBg.setFillColor(sf::Color(40, 40, 50));
    progressBg.setPosition(centerX - progressWidth/2, progressY);
    window_.draw(progressBg);
    
    // Заполнение прогресс-бара
    sf::RectangleShape progressBar(sf::Vector2f(progressWidth * progress, 25));
    progressBar.setFillColor(sf::Color(50, 150, 250));
    progressBar.setPosition(centerX - progressWidth/2, progressY);
    window_.draw(progressBar);
    
    // Рамка прогресс-бара
    sf::RectangleShape progressFrame(sf::Vector2f(progressWidth, 25));
    progressFrame.setFillColor(sf::Color::Transparent);
    progressFrame.setOutlineColor(sf::Color::White);
    progressFrame.setOutlineThickness(2);
    progressFrame.setPosition(centerX - progressWidth/2, progressY);
    window_.draw(progressFrame);
    
    // Текст прогресса
    text.setString(std::to_string(round + 1) + " / 100");
    text.setCharacterSize(16);
    text.setFillColor(sf::Color::White);
    text.setStyle(sf::Text::Bold);
    bounds = text.getLocalBounds();
    text.setPosition(centerX - bounds.width / 2, progressY + 3);
    window_.draw(text);
    
    // Инструкция внизу
    text.setString("Press SPACE or ENTER to continue...");
    text.setCharacterSize(14);
    text.setFillColor(sf::Color(150, 150, 150));
    text.setStyle(sf::Text::Regular);
    bounds = text.getLocalBounds();
    text.setPosition(centerX - bounds.width / 2, Height - 40);
    window_.draw(text);
    
    window_.display();
}

Event GraphicVisual::getEvent(long time_mcsec) {
  sf::Event sfmlEvent;
  
  // Проверяем, есть ли события в очереди
  if (window_.pollEvent(sfmlEvent)) {
    switch (sfmlEvent.type) {
      case sf::Event::Closed:
        return Event(EventType::HALT);
      case sf::Event::Resized:  // Добавить обработку resize
        handleResize(sfmlEvent.size.width, sfmlEvent.size.height);
        return Event();
          
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

void GraphicVisual::handleResize(unsigned int newWidth, unsigned int newHeight) {
  float gameAspect = (float)(Width * block_size) / (Height * block_size);
  float windowAspect = (float)newWidth / newHeight;
  
  sf::View view(sf::FloatRect(0, 0, Width * block_size, Height * block_size));
  
  if (windowAspect > gameAspect) {
    float viewportWidth = gameAspect / windowAspect;
    view.setViewport(sf::FloatRect((1 - viewportWidth) / 2, 0, viewportWidth, 1));
  } else {
    float viewportHeight = windowAspect / gameAspect;
    view.setViewport(sf::FloatRect(0, (1 - viewportHeight) / 2, 1, viewportHeight));
  }
  window_.setView(view);
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
  // Соответствие ID цветов (32, 33, 34, 36 и т.д.)
  switch(colorId) {
    case 32: return sf::Color(255, 100, 50);   // Оранжевый
    case 33: return sf::Color(50, 255, 100);   // Зеленый
    case 34: return sf::Color(100, 50, 255);   // Синий
    case 35: return sf::Color(255, 255, 50);   // Желтый
    case 36: return sf::Color(255, 50, 255);   // Пурпурный
    
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

void GraphicVisual::drawScorePanel(Model& model) {
  sf::Text text;
  text.setFont(font_);
  
  // Позиция панели (левый верхний угол)
  float panelX = 10;
  float panelY = 10;
  
  // Фон панели (ширина автоматическая)
  sf::RectangleShape panel(sf::Vector2f(200, 30 + model.getSnakes().size() * 25));
  panel.setFillColor(sf::Color(0, 0, 0, 200));
  panel.setOutlineColor(sf::Color::White);
  panel.setOutlineThickness(1);
  panel.setPosition(panelX, panelY);
  window_.draw(panel);
  
  // Заголовок
  text.setFont(font_);
  text.setString("SCORES");
  text.setCharacterSize(16);
  text.setPosition(panelX + 10, panelY + 5);
  text.setFillColor(sf::Color::Yellow);
  window_.draw(text);
  
  // Очки
  int yOffset = 30;
  for (const auto& snake : model.getSnakes()) {
    text.setFillColor(convertToSfmlColor(snake.getColor()));
    text.setCharacterSize(14);
    
    std::string scoreText = "Snake " + std::to_string(snake.getID()) + 
                            ": " + std::to_string(snake.getLength());
    text.setString(scoreText);
    text.setPosition(panelX + 10, panelY + yOffset);
    window_.draw(text);
    
    yOffset += 25;
  }
}