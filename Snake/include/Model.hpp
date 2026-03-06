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
  PR_UP, 
  PR_DOWN,
  PR_LEFT,
  PR_RIGHT,

  PAUSE,
  HALT,
};

class Event {
  EventType type_;

  public:
    Event(){}
};

class Model {
  MODEL_STATE status_;

  int window_width_, window_height_;
  size_t tick_;

  std::list<Rabbit> rabbits_;
  std::list<Snake>  snakes_;

  public: 
    Model(size_t window_width, size_t window_height, size_t tick): 
      window_width_(window_width), 
      window_height_(window_height), 
      tick_(tick){}

    MODEL_STATE getStatus() { return status_; };

    bool over() { return status_ == MODEL_STATE::GAME_OVER;}
    void update(std::list<Event>) {};

    size_t getTicks()  {return tick_;}
    size_t getWidth()  {return window_width_;}
    size_t getHeight() {return window_height_;}
};