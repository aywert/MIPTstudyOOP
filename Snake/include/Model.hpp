#pragma once
#include <iostream>
#include <list>
#include "Snake.hpp"
#include "Rabbit.hpp"
#include "View.hpp"

enum class MODEL_STATE {
  NOT_LAUNCHED,
  IN_PROCCESS,
  GAME_OVER,
  ERROR,
};

class Model {
  MODEL_STATE status_;

  int window_width_, window_height_;
  size_t tick_;

  std::list<Rabbit> rabbits_;
  std::list<Snake>  snakes_;

  public: 
    Model(size_t tick): tick_(tick){}
    MODEL_STATE getStatus() { return status_; };

    bool over() {if (status_ == MODEL_STATE::GAME_OVER) return true;}
    size_t get_ticks() {return tick_;}
    void update(std::list<Event>) {};
};