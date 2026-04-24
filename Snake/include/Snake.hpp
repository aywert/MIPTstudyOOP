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

enum class Controlled_By {
  human,
  smart_bot, 
  silly_bot,
};

struct Segment {
  int x, y;
  SegmentType type;

  Segment(int x, int y, SegmentType t = SegmentType::BODY) : x(x), y(y), type(t) {}
  Segment() = default; 
  Segment operator+(const Segment& other) const {return Segment(x+other.x, y+other.y, type);}
};

class Snake {
  int id_;
  Controlled_By      cntrl_;
  SnakeStatus        state_ = SnakeStatus::ALIVE;
  std::list<Segment>  body_;
  Direction      direction_;
  int                color_;
  Segment rudimentary_tail_ = {0, 0, SegmentType::BODY};

  bool should_grow_ = false;

  public: 
    Snake(const Controlled_By&          cntrl, 
          const std::list<Segment>&      body, 
          const Direction&          direction,
          const int                     color): 
      cntrl_(cntrl), body_(body), direction_(direction), color_(color) {}

    struct Builder {
      Controlled_By      cntrl_{Controlled_By::silly_bot};
      SnakeStatus        state_{SnakeStatus::ALIVE};
      std::list<Segment>  body_{};
      Direction      direction_{Direction::RIGHT};
      Segment rudimentary_tail_{};
      int color_{37}; // default white

      Builder& setState(const SnakeStatus& state) {
        state_ = state; 
        return *this;
      }

      Builder& setBody(const Segment& seg) {
        body_.emplace_back(seg.x, seg.y, SegmentType::HEAD); 
        body_.emplace_back(seg);        
        body_.emplace_back(seg);
        return *this;
      }

      Builder& setDirection(const Direction& dir) {
        direction_ = dir; return *this;
      }

      Builder& setRudTail(const Segment& seg) {
        body_.push_back(seg);
        return *this;
      }

      Builder& setControlledBy(const Controlled_By& cntrl) {
        cntrl_ = cntrl;
        return *this;
      }

      Builder& setColor(const int& color) {
        color_ = color;
        return *this;
      }

      Snake build() const {
        return Snake(cntrl_, body_, direction_, color_);
      }
    };

    void move();

    void grow() {should_grow_ = true;}
    void kill() {state_ = SnakeStatus::DEAD;}
    void rot() {state_ = SnakeStatus::ROTTED;}

    Segment            getTail() const noexcept {return rudimentary_tail_;}
    std::list<Segment> getBody() const noexcept {return body_;}
    Segment            getHead() const noexcept {return body_.front();}
    SnakeStatus       getState() const noexcept {return state_;}
    int               getHeadX() const noexcept {return body_.front().x;}
    int               getHeadY() const noexcept {return body_.front().y;}
    int                  getID() const noexcept {return id_;}
    int               getColor() const noexcept {return color_;}
    int              getLength() const noexcept {return body_.size();}
    bool   isControlledByHuman() const noexcept {return cntrl_ == Controlled_By::human;}
    bool     isControlledByBot() const noexcept {return cntrl_ != Controlled_By::human;}
    Controlled_By   getCntrlBy() const noexcept {return cntrl_;}
    Direction     getDirection() const noexcept {return direction_;}

    void setHeadPosition(int x, int y) {
      body_.front().x = x;
      body_.front().y = y;
    }
    void setDirection(Direction dir) noexcept {direction_ = dir;}
    void setID(size_t id) noexcept {id_ = id;}
};