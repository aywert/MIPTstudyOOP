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

enum class SnakeStatus {
  ALIVE, 
  DEAD, 
  ROTTED, // hiden  
};

struct Segment {
  int x, y;
  SegmentType type; // Вместо char sbl

  Segment(int x, int y, SegmentType t) : x(x), y(y), type(t) {}
};

class Snake {
  SnakeStatus        state_;
  std::list<Segment>  body_;
  Direction      direction_;
  Segment rudimentary_tail_;

  bool should_grow_;

  public: 
    Snake(int width, int height) : rudimentary_tail_(0, 0, SegmentType::BODY) {
      direction_ = Direction::UP;
      state_     = SnakeStatus::ALIVE;

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

    if (should_grow_) {
      should_grow_ = false;
      rudimentary_tail_ = Segment(0, 0, SegmentType::BODY);
    } else {
      rudimentary_tail_ = body_.back();
      body_.pop_back();
    }

  }

    void grow() {
      should_grow_ = true;
    }

    void kill() {
      state_ = SnakeStatus::DEAD;
    }

    void rot() {
      state_ = SnakeStatus::ROTTED;
    }

    Segment            getTail() const noexcept {return rudimentary_tail_;}
    std::list<Segment> getBody() const noexcept {return body_;}
    Segment            getHead() const noexcept {return body_.front();}
    SnakeStatus       getState() const noexcept {return state_;}
    Direction getDirection()     const noexcept {return direction_;}
    void      setDirection(Direction dir) noexcept {direction_ = dir;}
};