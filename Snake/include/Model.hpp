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

constexpr long long SPAWN_INTERVAL = 1000;
constexpr int       MAX_RABBITS    =   10;
constexpr int       SHIFT_COL      =    2;
constexpr int       SHIFT_ROW      =    2;

struct Event {
  EventType type_;

  Event(EventType type) {type_ = type;}
  Event() {type_ = EventType::BAD;}
  void setEventType(EventType type) {type_ = type;}
};

class Model {
  MODEL_STATE status_;

  int window_width_ = 0, window_height_ = 0;
  int shift_col = SHIFT_COL; 
  int shift_row = SHIFT_ROW; 
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

    bool over() { return status_ == MODEL_STATE::GAME_OVER;}
    void update(const std::vector<Event>& events) {
      for (const auto& event : events) {
        switch(event.type_) {

          case EventType::UP_1:    updateSnakeDirection(0, Direction::UP);    break;
          case EventType::DOWN_1:  updateSnakeDirection(0, Direction::DOWN);  break;
          case EventType::LEFT_1:  updateSnakeDirection(0, Direction::LEFT);  break;
          case EventType::RIGHT_1: updateSnakeDirection(0, Direction::RIGHT); break;

          // Управление вторым игроком (индекс 1)
          case EventType::UP_2:    updateSnakeDirection(1, Direction::UP);    break;
          case EventType::DOWN_2:  updateSnakeDirection(1, Direction::DOWN);  break;
          case EventType::LEFT_2:  updateSnakeDirection(1, Direction::LEFT);  break;
          case EventType::RIGHT_2: updateSnakeDirection(1, Direction::RIGHT); break;

          case EventType::HALT: status_ = MODEL_STATE::GAME_OVER; break;
        }
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

    size_t getTicks() const noexcept {return tick_;}
    int    getWidth() const noexcept {return window_width_;}
    int   getHeight() const noexcept {return window_height_;}
    int getColShift() const noexcept {return shift_col;}
    int getRowShift() const noexcept {return shift_row;}

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
            human_snakes_ids_.erase(human_snakes_ids_.begin() + i);
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
      int snakeId = human_snakes_ids_[playerIdx];  // Берём реальный ID из вектора
      auto snake = findSnakeById(snakeId);
      
      if (snake != snakes_.end()) {  // Всегда проверяй, найдена ли змея!
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
    for (auto& bot : snakes_) {
        if (bot.isControlledByHuman()) continue;

        Segment head = bot.getHead();
        Direction currentDir = bot.getDirection();
        Direction nextDir = currentDir;

        Rabbit food = nearestRabbit(bot);
        
        if (food.getX() == -1) continue; 

        int x = food.getX();
        int y = food.getY();
       
        if (x > head.x && currentDir != Direction::LEFT) nextDir = Direction::RIGHT;
        else if (x < head.x && currentDir != Direction::RIGHT) nextDir = Direction::LEFT;
        else if (y > head.y && currentDir != Direction::UP) nextDir = Direction::DOWN;
        else if (y < head.y && currentDir != Direction::DOWN) nextDir = Direction::UP;
        

        bot.setDirection(nextDir);
    }
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

};


