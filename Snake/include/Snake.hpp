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
  Segment(): 
    position_x(0), 
    position_y(0), 
    sbl('\0') {}
  Segment(const Segment& other): 
    position_x(other.position_x), 
    position_y(other.position_y), 
    sbl(other.sbl) {}
    
  void setHead(char head) {sbl = head;}
};

class Snake {
  Direction direction_;
  std::list<Segment> body_;
  Segment rudimentary_tail_;

  public: 
    Snake(int width, int height): rudimentary_tail_(0, 0, '\0') {
      direction_ = Direction::UP;
      body_.push_back({width/2, height/2, '^'});
      for (int i = 1; i < 5; i++) {
        body_.push_back({width/2+i, height/2, 'o'});
      }
    }

    std::list<Segment> getBody() const {return body_;}
    Direction getDirection() const noexcept {return direction_;}
    void setDirection(Direction dir) {direction_ = dir;}

    void move() {
    if (body_.empty()) return;

    Segment newHead = body_.front();
    body_.front().sbl = body_.back().sbl; 

    switch (direction_) {
        case Direction::UP:    newHead.position_y--; newHead.setHead('^'); break;
        case Direction::DOWN:  newHead.position_y++; newHead.setHead('v'); break;
        case Direction::LEFT:  newHead.position_x--; newHead.setHead('<'); break;
        case Direction::RIGHT: newHead.position_x++; newHead.setHead('>'); break;
    }

    body_.push_front(newHead);

    rudimentary_tail_ = body_.back();
    body_.pop_back();
}

    Segment getTail() const {return rudimentary_tail_;}
};