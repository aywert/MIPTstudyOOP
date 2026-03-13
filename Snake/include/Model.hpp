#pragma once
#include <iostream>
#include <list>
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
};

struct Event {
  EventType type_;

  public:
    Event(EventType type) {type_ = type;}
};

class Model {
  MODEL_STATE status_;

  int window_width_ = 0, window_height_ = 0;
  size_t tick_;

  std::list<Rabbit> rabbits_;
  std::list<Snake>  snakes_;

  public: 
    Model(size_t window_width, size_t window_height, size_t tick): 
      status_(MODEL_STATE::IN_PROCCESS),
      window_width_(window_width), 
      window_height_(window_height), 
      tick_(tick){}

    MODEL_STATE getStatus() { return status_; };

    bool over() { return status_ == MODEL_STATE::GAME_OVER;}
    void update(const std::vector<Event>& events) {
      for (const auto& event: events) {
        for (auto& snake: snakes_) {
          Direction nextDir = snake.getDirection();
            
          for (const auto& event : events) {
            if (event.type_ == EventType::UP    && nextDir != Direction::DOWN)  nextDir = Direction::UP;
            else if (event.type_ == EventType::DOWN  && nextDir != Direction::UP)    nextDir = Direction::DOWN;
            else if (event.type_ == EventType::LEFT  && nextDir != Direction::RIGHT) nextDir = Direction::LEFT;
            else if (event.type_ == EventType::RIGHT && nextDir != Direction::LEFT)  nextDir = Direction::RIGHT;
            
            if (event.type_ == EventType::HALT) status_ = MODEL_STATE::GAME_OVER;
        }
          snake.setDirection(nextDir);
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