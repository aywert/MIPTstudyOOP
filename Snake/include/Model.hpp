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
  UP, 
  DOWN,
  LEFT,
  RIGHT,

  PAUSE,
  HALT,
  BAD,
};

constexpr long long SPAWN_INTERVAL =   10;
constexpr int       MAX_RABBITS    = 1000;
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
      
      for (auto& snake: snakes_) {
        Direction nextDir = snake.getDirection();
          
        for (const auto& event : events) {
          if      (event.type_ == EventType::UP    && nextDir != Direction::DOWN)  snake.setDirection(Direction::UP);
          else if (event.type_ == EventType::DOWN  && nextDir != Direction::UP)    snake.setDirection(Direction::DOWN);
          else if (event.type_ == EventType::LEFT  && nextDir != Direction::RIGHT) snake.setDirection(Direction::LEFT);
          else if (event.type_ == EventType::RIGHT && nextDir != Direction::LEFT)  snake.setDirection(Direction::RIGHT);
          
          if (event.type_ == EventType::HALT) status_ = MODEL_STATE::GAME_OVER;
        }
      }
      
      auto it = snakes_.begin();
      while (it != snakes_.end()) {

        switch(it->getState()) {
          case SnakeStatus::ROTTED: it = snakes_.erase(it); break;
          case SnakeStatus::DEAD: ++it; break;
          case SnakeStatus::ALIVE: {
            it->move();
            if (checkBoundaryCollision(*it) || checkSelfCollision(*it)) {
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

    void addSnake(Snake snake)    {snakes_.push_back(snake);  }
    void addRabbit(Rabbit rabbit) {rabbits_.push_back(rabbit);}
    std::list<Snake>  getSnakes()  {return snakes_; }
    std::list<Rabbit> getRabbits(){return rabbits_;}

    void spawnRabbit() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
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
  
  bool checkSelfCollision(const Snake& snake) {
    const auto& body = snake.getBody();
    
    const auto& head = body.front();
    auto it = body.begin();
    ++it;
    
    for (; it != body.end(); ++it) {
      if (it->x == head.x && it->y == head.y) {
        return true;
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
};