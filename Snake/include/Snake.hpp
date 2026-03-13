#pragma once

#include <list>

enum class Direction{
  UP,
  DOWN,
  RIGHT,
  LEFT,
};

struct Segment {
  int position_x, position_y; 
  char sbl;

  Segment(int x, int y, char symbol): 
    position_x(x), 
    position_y(y), 
    sbl(symbol) {}
};

class Snake {
  Direction direction_;
  std::list<Segment> body_;
  Segment rudimentary_tail_;

  public: 
    Snake(int width, int height): rudimentary_tail_(0, 0, '\0') {
      direction_ = Direction::UP;
      for (int i = 0; i < 5; i++) {
        body_.push_back({width/2+i, height/2, '*'});
      }
    }

    std::list<Segment> getBody() const {return body_;}
    Direction getDirection() const noexcept {return direction_;}
    void setDirection(Direction dir) {direction_ = dir;}

    void move() {
     if (body_.empty()) return;
      Segment newHead = body_.front();
      switch (direction_) {
        case Direction::UP:    newHead.position_y--; break;
        case Direction::DOWN:  newHead.position_y++; break;
        case Direction::LEFT:  newHead.position_x--; break;
        case Direction::RIGHT: newHead.position_x++; break;
      }

      rudimentary_tail_ = body_.back();
      body_.push_front(newHead);
      body_.pop_back();
    }

    Segment getTail() const {return rudimentary_tail_;}
};