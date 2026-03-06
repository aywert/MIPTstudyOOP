#pragma once

#include <list>

enum class SNAKE_DIRECTION{
  DIR_UP,
  DIR_DOWN,
  DIR_RIGHT,
  DIR_LEFT,
};

struct Segment {
  int position_x;
  int position_y; 
  // also shoud contain colour
};

class Snake {
  SNAKE_DIRECTION direction_;
  std::list<Segment> body_;

  public: 
    Snake(){}
    std::list<Segment> getBody() {return body_;}
};