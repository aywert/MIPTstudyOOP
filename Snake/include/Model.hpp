#pragma once
#include <iostream>
#include <list>
#include <variant>
#include <utility>
#include <random>
#include <chrono>
#include "Snake.hpp"
#include "Rabbit.hpp"

enum class MODEL_STATE {
  NOT_LAUNCHED,
  IN_PROCCESS,
  GAME_OVER,
  TOURNAMENT,
  ERROR,
};

enum class EventType {
  UP_1, 
  DOWN_1,
  LEFT_1,
  RIGHT_1,

  UP_2, 
  DOWN_2,
  LEFT_2,
  RIGHT_2,

  PAUSE,
  HALT,
  BAD,
};

struct Point {
  int x, y;
};



struct Event {
  EventType type_;

  Event(EventType type) {type_ = type;}
  Event() {type_ = EventType::BAD;}
  void setEventType(EventType type) {type_ = type;}
};

class Model {
  MODEL_STATE status_;

  long long SPAWN_INTERVAL =  500;
  int       MAX_RABBITS    =   20;
  int       SHIFT_COL      =    2;
  int       SHIFT_ROW      =    2;

  int       window_width_  =    0; 
  int       window_height_ =    0;

  int shift_col = SHIFT_COL; 
  int shift_row = SHIFT_ROW; 
  Controlled_By last_snake_type_ = Controlled_By::human; // keeps a record of the last killed snakes type
  size_t tick_;

  std::list<Rabbit> rabbits_;
  std::list<Snake>  snakes_;
  std::vector<int>  human_snakes_ids_; // vector which contains IDs of Snakes controlled by human (up to 2 people)
  int next_snake_id_ = 0; // Счётчик для генерации уникальных ID змей

  using time_t = std::chrono::steady_clock::time_point;
  class Clock {
    time_t last_time;
    public:
      Clock() : last_time(std::chrono::steady_clock::now()) {}

    long long getDeltaMS() {
      time_t now = std::chrono::steady_clock::now();
      auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time);
      return diff.count();
    }

    void reset() {
      last_time = std::chrono::steady_clock::now();
    }
  };

  public: 
    Model(size_t window_width, size_t window_height, size_t tick): 
      status_(MODEL_STATE::IN_PROCCESS),
      window_width_(window_width), 
      window_height_(window_height), 
      tick_(tick){}

    MODEL_STATE getStatus() { return status_; };

    Clock RabbitTimer;
    bool try_kill() {
      // Подсчитываем только живых змей
      if (snakes_.size() == 1) { 
        status_ = MODEL_STATE::GAME_OVER;
        last_snake_type_ = snakes_.front().getCntrlBy();
        return true;
      }

      if (snakes_.size() == 0) { 
        status_ = MODEL_STATE::GAME_OVER;
        last_snake_type_ = Controlled_By::human;
        return true;
      }
      
      return false;
    }

    bool over()  { return status_ == MODEL_STATE::GAME_OVER;}
    void refresh() {
      SPAWN_INTERVAL =  500;
      MAX_RABBITS    =   20;
      SHIFT_COL      =    2;
      SHIFT_ROW      =    2;

      next_snake_id_ = 0;
      snakes_.clear();
      rabbits_.clear();
      human_snakes_ids_.clear();

      last_snake_type_ = Controlled_By::human;

      status_ = MODEL_STATE::IN_PROCCESS;
      RabbitTimer.reset();
    }

    void update(const std::vector<Event>& events) {
      std::optional<Direction> new_direction_1;
      std::optional<Direction> new_direction_2;

      for (const auto& event : events) {
        switch(event.type_) {
          case EventType::UP_1:    new_direction_1 = Direction::UP;    break;
          case EventType::DOWN_1:  new_direction_1 = Direction::DOWN;  break;
          case EventType::LEFT_1:  new_direction_1 = Direction::LEFT;  break;
          case EventType::RIGHT_1: new_direction_1 = Direction::RIGHT; break;

          case EventType::UP_2:    new_direction_2 = Direction::UP;    break;
          case EventType::DOWN_2:  new_direction_2 = Direction::DOWN;  break;
          case EventType::LEFT_2:  new_direction_2 = Direction::LEFT;  break;
          case EventType::RIGHT_2: new_direction_2 = Direction::RIGHT; break;

          case EventType::HALT: status_ = MODEL_STATE::GAME_OVER; break;
          default: break;
        }
      }

      // Применяем только последнее направление для каждого игрока
      if (new_direction_1.has_value()) {
        updateSnakeDirection(0, new_direction_1.value());
      }
      if (new_direction_2.has_value()) {
        updateSnakeDirection(1, new_direction_2.value());
      }

      botMovementHandle();
      
      auto it = snakes_.begin();
      while (it != snakes_.end()) {
        switch(it->getState()) {
          case SnakeStatus::ROTTED: it = removeSnake(it->getID()); break;
          case SnakeStatus::DEAD: ++it; break;
          case SnakeStatus::ALIVE: {
            it->move();
            if (checkBoundaryCollision(*it) || checkSnakeCollisions(*it)) {
              it->kill(); ++it;
            } else { handleRabbitCollision(*it); ++it;}  // Переходим к следующему элементу

            break;
          }
        }
      }

      long long spawn_time = RabbitTimer.getDeltaMS();

      if (spawn_time >= SPAWN_INTERVAL) {
        if (rabbits_.size() < MAX_RABBITS) {
          spawnRabbit();
        }

        RabbitTimer.reset();
      }
    };

    void setWidth(int width)   {window_width_  = width;}
    void setHeight(int height) {window_height_ = height;}
    void setStatus(MODEL_STATE state) {status_ = state;}
    void setSpawnInterval(long long time) {
      SPAWN_INTERVAL =  time;
    }    

    size_t getTicks() const noexcept {return tick_;}
    int    getWidth() const noexcept {return window_width_;}
    int   getHeight() const noexcept {return window_height_;}
    int getColShift() const noexcept {return shift_col;}
    int getRowShift() const noexcept {return shift_row;}
    Controlled_By getLastSnakeType() const noexcept {return last_snake_type_;}

     // Helper для поиска змеи по ID (возвращает итератор или end())
    std::list<Snake>::iterator findSnakeById(int id) {
      return std::find_if(snakes_.begin(), snakes_.end(), 
        [id](const Snake& s){ return s.getID() == id; });
    }

    std::list<Snake>::iterator removeSnake(int id) {
      auto it = findSnakeById(id);
      std::list<Snake>::iterator new_it = it;
      if (it != snakes_.end()) {
        new_it = snakes_.erase(it);
        for (int i = 0; i < human_snakes_ids_.size(); ++i) {
          if (human_snakes_ids_[i] == id) {
            human_snakes_ids_[i] = 0xDEAD;
            break;
          }
        }
      }

      return new_it;
    }

    void addSnake(Snake snake) {
      snake.setID(next_snake_id_++);
      snakes_.push_back(snake);

      if (snake.isControlledByHuman()) {
        human_snakes_ids_.push_back(snake.getID());
      }
    }

    void addRabbit(Rabbit rabbit) {
      rabbits_.push_back(rabbit);
    }

    std::list<Snake>&  getSnakes()  {return snakes_; }
    std::list<Rabbit> getRabbits(){return rabbits_;}

    void spawnRabbit() {
      static std::mt19937 gen(
        static_cast<unsigned int>(
          std::chrono::steady_clock::now().time_since_epoch().count()
        )
      );
      
      int min_x = shift_col + 1;
      int max_x = window_width_ - shift_col;
      int min_y = shift_row + 1;
      int max_y = window_height_ - shift_row;
      
      for (int attempt = 0; attempt < 100; ++attempt) {
        std::uniform_int_distribution<int> distribX(min_x, max_x);
        std::uniform_int_distribution<int> distribY(min_y, max_y);
        
        int x = distribX(gen);
        int y = distribY(gen);
        
        if (isPositionFree(x, y)) {
          rabbits_.push_back(Rabbit(x, y));
          return;
        }
      }
    }

    bool checkBoundaryCollision(const Snake& snake) {
    const auto& head = snake.getHead();
    
    // Вычисляем реальные границы игрового поля
    int min_x = shift_col + 1;
    int max_x = window_width_ - shift_col;
    int min_y = shift_row + 1;
    int max_y = window_height_ - shift_row;
    
    bool collision = (head.x < min_x || head.x > max_x || 
                      head.y < min_y || head.y > max_y);
    
    return collision;
}
  
  bool checkSnakeCollisions(const Snake& headSnake) {
    const auto& head = headSnake.getHead();

    for (const auto& otherSnake : snakes_) {

      // Проверяем только живых змеек
      if (otherSnake.getState() != SnakeStatus::ALIVE) {
        continue;
      }

      const auto& body = otherSnake.getBody(); // Предполагаем, что это const std::list<Point>&
      
      // Если проверяем саму себя — начинаем со второго элемента (пропускаем голову).
      // Если чужую змейку — проверяем весь список.
      auto it = (&headSnake == &otherSnake) ? std::next(body.begin()) : body.begin();

      // Идем до конца списка
      for (; it != body.end(); ++it) {
        if (head.x == it->x && head.y == it->y) {
            return true; 
        }
      }
    }

    return false;
}

  void handleRabbitCollision(Snake& snake) {
    const auto& head = snake.getHead();
    
    auto it = rabbits_.begin();
    while (it != rabbits_.end()) {
      if (it->getX() == head.x && it->getY() == head.y) {
        it = rabbits_.erase(it);
        snake.grow();
      } else {
        ++it;
      }
    }
  }
  
  bool isPositionFree(int x, int y) const {
    
    int min_x = shift_col + 1;
    int max_x = window_width_ - shift_col;
    int min_y = shift_row + 1;
    int max_y = window_height_ - shift_row;
    
    bool collision = (x < min_x || x > max_x || 
                      y < min_y || y > max_y);

    if (collision) {
      return false;
    }
    
    // Проверка змеек
    for (const auto& snake : snakes_) {
      for (const auto& segment : snake.getBody()) {
        if (segment.x == x && segment.y == y) {
          return false;
        }
      }
    }

    // Проверка кроликов
    for (const auto& rabbit : rabbits_) {
      if (x == rabbit.getX() && y == rabbit.getY()) 
        return false;
    }

    return true;
  }

  void updateSnakeDirection(size_t playerIdx, Direction newDir) {
    if (playerIdx < human_snakes_ids_.size()) {
      int snakeId = human_snakes_ids_[playerIdx];

      if (snakeId == 0xDEAD) 
        return;
      
      auto snake = findSnakeById(snakeId);
      
      if (snake != snakes_.end() && snake->getState() == SnakeStatus::ALIVE) {  
        Direction currentDir = snake->getDirection();
        bool isOpposite = 
          (newDir == Direction::UP    && currentDir == Direction::DOWN)  ||
          (newDir == Direction::DOWN  && currentDir == Direction::UP)    ||
          (newDir == Direction::LEFT  && currentDir == Direction::RIGHT) ||
          (newDir == Direction::RIGHT && currentDir == Direction::LEFT);

        if (!isOpposite) {
          snake->setDirection(newDir);
        }
      }
    }
  }


void botMovementHandle() {
  for (auto& snake : snakes_) {
    if (snake.isControlledByHuman()) continue;

    Direction nextDir = snake.getDirection();
    if (snake.getCntrlBy() == Controlled_By::smart_bot)
      nextDir = smart_bot_calcul_direction(snake);
    else 
      nextDir = easy_bot_calcul_direction(snake);

    snake.setDirection(nextDir);
  }
}

Direction smart_bot_calcul_direction(const Snake& bot) {
  const auto heatMap = generateHeatmap(window_width_, window_height_, bot);
  return chooseBestMoveBasedOnHeatMap(bot, window_width_, window_height_, heatMap);
}

Direction easy_bot_calcul_direction(const Snake& bot) {
    Segment head = bot.getHead();
    Direction currentDir = bot.getDirection();
    Direction desiredDir = currentDir;
    
    // Находим ближайшего кролика
    Rabbit food = nearestRabbit(bot);
    if (food.getX() != -1) {
      int x = food.getX();
      int y = food.getY();
      
      // Жадное приближение к кролику (без разворота)
      if      (x > head.x && currentDir != Direction::LEFT)  desiredDir = Direction::RIGHT;
      else if (x < head.x && currentDir != Direction::RIGHT) desiredDir = Direction::LEFT;
      else if (y > head.y && currentDir != Direction::UP)    desiredDir = Direction::DOWN;
      else if (y < head.y && currentDir != Direction::DOWN)  desiredDir = Direction::UP;
    }
    
    // Проверяем, безопасно ли желаемое направление
    Segment next = head;
    switch (desiredDir) {
      case Direction::UP:    next.y--; break;
      case Direction::DOWN:  next.y++; break;
      case Direction::LEFT:  next.x--; break;
      case Direction::RIGHT: next.x++; break;
    }
    
    if (isPositionSafe(next.x, next.y)) {
      return desiredDir;  // Желаемое направление безопасно
    }
    
    // Если желаемое направление небезопасно, ищем альтернативу
    // Приоритет: прямо, налево, направо, назад (но без разворота)
    std::vector<Direction> priorities;
    priorities.push_back(currentDir);  // Сначала пробуем текущее направление
    
    // Добавляем боковые направления
    if (currentDir == Direction::UP || currentDir == Direction::DOWN) {
      priorities.push_back(Direction::LEFT);
      priorities.push_back(Direction::RIGHT);
    } else {
      priorities.push_back(Direction::UP);
      priorities.push_back(Direction::DOWN);
    }
    
    
    // Проверяем все направления в порядке приоритета
    for (Direction dir : priorities) {
      Segment test = head;
      switch (dir) {
        case Direction::UP:    test.y--; break;
        case Direction::DOWN:  test.y++; break;
        case Direction::LEFT:  test.x--; break;
        case Direction::RIGHT: test.x++; break;
      }
      
      if (isPositionSafe(test.x, test.y)) {
        return dir;
      }
    }
    
    // Если всё заблокировано (скорое всего смерть), остаёмся в текущем направлении
    return currentDir;
}


  Rabbit nearestRabbit(const Snake& snake) {
    const auto& head = snake.getHead();
    Rabbit* closest = nullptr;
    int minDistance = window_height_* window_height_ + window_width_*window_width_;

    for (auto& rabbit : rabbits_) {
      // Вычисляем квадрат расстояния
      int dx = head.x - rabbit.getX();
      int dy = head.y - rabbit.getY();
      int distanceSq = dx * dx + dy * dy;

      if (distanceSq <= minDistance) {
        minDistance = distanceSq;
        closest = &rabbit;
      }
    }

    return (closest != nullptr) ? *closest : Rabbit(-1, -1); 
  }


  bool isPositionSafe(int x, int y) const {
    
    int min_x = shift_col + 1;
    int max_x = window_width_ - shift_col;
    int min_y = shift_row + 1;
    int max_y = window_height_ - shift_row;
    
    bool collision = (x < min_x || x > max_x || 
                      y < min_y || y > max_y);

    if (collision) {
      return false;
    }
    
    // Проверка змеек
    for (const auto& snake : snakes_) {
      for (const auto& segment : snake.getBody()) {
        if (segment.x == x && segment.y == y) {
          return false;
        }
      }
    }

    return true;
  }

  std::vector<std::vector<double>> generateHeatmap(int width, int height, const Snake& snake) {
    // Создаем матрицу, заполненную нулями
    std::vector<std::vector<double>> heatmap(width, std::vector<double>(height, 0.0));

    // Проходим по каждому яблоку
    for (const auto& rabbit : rabbits_) {
      for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
          // Считаем Манхэттенское расстояние
          int dist = std::abs(x - rabbit.getX()) + std::abs(y - rabbit.getY());
          
          heatmap[x][y] += 500.0 / (dist + 1.0);
          
        }
      }
    }

    for (const auto& enemy : snakes_) {
      if (&enemy == &snake) continue;
      
      for (const auto& segment : enemy.getBody()) {
        double penalty = (segment.type == SegmentType::HEAD) ? 150.0 : 80.0;
        int sx = segment.x;
        int sy = segment.y;
        
        int radius = 15;
        int min_x = std::max(0, sx - radius);
        int max_x = std::min(width, sx + radius);
        int min_y = std::max(0, sy - radius);
        int max_y = std::min(height, sy + radius);
        
        for (int x = min_x; x < max_x; ++x) {
          for (int y = min_y; y < max_y; ++y) {
            int dist = std::abs(x - sx) + std::abs(y - sy);
            heatmap[x][y] -= penalty / (dist + 1.0);
          }
        }
      }
    }

    for (const auto& segment : snake.getBody()) {
      for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
          int dist = std::abs(x - segment.x) + std::abs(y - segment.y);
          // Свое тело - сильный штраф
          heatmap[x][y] -= 80.0 / (dist + 1.0);
        }
      }
    }

    for (int x = 0; x < width; ++x) {
      for (int y = 0; y < height; ++y) {
        // Находим минимальное расстояние до любой из 4-х стен
        int distToEdgeX = std::min(x, (width - 1) - x);
        int distToEdgeY = std::min(y, (height - 1) - y);
        int minDistToEdge = std::min(distToEdgeX, distToEdgeY);

        if (minDistToEdge < 3) { 
          heatmap[x][y] -= 50.0 / (minDistToEdge + 1.0);
        }
      }
    }
    return heatmap;
  }

  Direction chooseBestMoveBasedOnHeatMap(const Snake& snake, int width, int height, const std::vector<std::vector<double>>& heatmap) {
    struct MoveOption {
      Direction dir;
      int dx;
      int dy;
    };

    std::vector<MoveOption> options = {
      {Direction::UP,    0, -1},
      {Direction::DOWN,  0,  1},
      {Direction::LEFT, -1,  0},
      {Direction::RIGHT, 1,  0}
    };
    
    Direction bestDirection = snake.getDirection(); // Значение по умолчанию
    double maxScore = -1e18; // Очень маленькое число для инициализации

    Segment head = snake.getHead();
    for (const auto& option : options) {
      int nextX = head.x + option.dx;
      int nextY = head.y + option.dy;

      if (isPositionSafe(nextX, nextY)) {
        if (heatmap[nextX][nextY] > maxScore) {
          maxScore = heatmap[nextX][nextY];
          bestDirection = option.dir;
        }
      }
      
    }

    return bestDirection;
  }
};