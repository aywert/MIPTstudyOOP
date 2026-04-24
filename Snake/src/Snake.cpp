#include "Snake.hpp"

void Snake::move() {
  if (body_.empty()) return;
  
  Segment newHead = body_.front();
  newHead.type       = SegmentType::HEAD; 
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
    
  } else {
    rudimentary_tail_ = body_.back();
    body_.pop_back();
  }
}