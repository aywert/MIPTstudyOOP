#pragma once

#include <list>

enum class Direction{
  UP,
  DOWN,
  RIGHT,
  LEFT,
};

enum class SegmentType {
  HEAD,
  BODY
};

struct Segment {
  int x, y;
  SegmentType type; // Вместо char sbl

  Segment(int x, int y, SegmentType t) : x(x), y(y), type(t) {}
};

class Snake {
  Direction direction_;
  std::list<Segment> body_;
  Segment rudimentary_tail_;

  public: 
    Snake(int width, int height) : rudimentary_tail_(0, 0, SegmentType::BODY) {
      direction_ = Direction::UP;
    
      body_.emplace_back(width / 2, height / 2, SegmentType::HEAD);
      for (int i = 1; i < 5; i++) {
          body_.emplace_back(width / 2 + i, height / 2, SegmentType::BODY);
      }
    }

    void move() {
    if (body_.empty()) return;

    Segment newHead = body_.front();
    body_.front().type = SegmentType::BODY; 

    switch (direction_) {
        case Direction::UP:    newHead.y--; break;
        case Direction::DOWN:  newHead.y++; break;
        case Direction::LEFT:  newHead.x--; break;
        case Direction::RIGHT: newHead.x++; break;
    }

    body_.push_front(newHead);

    rudimentary_tail_ = body_.back();
    body_.pop_back();
}

    Segment            getTail() const {return rudimentary_tail_;}
    std::list<Segment> getBody() const noexcept {return body_;}
    Direction getDirection()     const noexcept {return direction_;}
    void      setDirection(Direction dir) noexcept {direction_ = dir;}
};