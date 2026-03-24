#pragma once
#include <iostream>
#include <list>
#include <variant>
#include <utility>
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

struct Event {
  EventType type_;

  Event(EventType type) {type_ = type;}
  Event() {type_ = EventType::BAD;}
  void setEventType(EventType type) {type_ = type;}
};


struct SnakeContent { char symbol = '*'; };
struct RabbitContent { char symbol = '@'; };
struct EmptyContent { char symbol = ' '; };

using CellContent = std::variant<SnakeContent, RabbitContent, EmptyContent>;

struct Cell {
  int pos_x, pos_y;
  int color;
  CellContent what;
};

class Model {
  MODEL_STATE status_;

  int window_width_ = 0, window_height_ = 0;
  size_t tick_;

  std::list<Rabbit> rabbits_;
  std::list<Snake>  snakes_;

  std::vector<Cell> changes_;

  public: 
    Model(size_t window_width, size_t window_height, size_t tick): 
      status_(MODEL_STATE::IN_PROCCESS),
      window_width_(window_width), 
      window_height_(window_height), 
      tick_(tick){}

    MODEL_STATE getStatus() { return status_; };

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
      

      for (auto& snake : snakes_) {
        snake.move();
      }
    };

    void setWidth(int width)   {window_width_  = width;}
    void setHeight(int height) {window_height_ = height;}

    size_t getTicks()  {return tick_;}
    int getWidth()  {return window_width_;}
    int getHeight() {return window_height_;}

    void addSnake(Snake snake) {snakes_.push_back(snake);}
    std::list<Snake> getSnakes()  {return snakes_; }
    std::list<Rabbit> getRabbits(){return rabbits_;}
};