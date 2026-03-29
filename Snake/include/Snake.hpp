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
  bot, 
};

struct Segment {
  int x, y;
  SegmentType type; // Вместо char sbl

  Segment(int x, int y, SegmentType t = SegmentType::BODY) : x(x), y(y), type(t) {}
  Segment() = default; 
  Segment operator+(const Segment& other) const {return Segment(x+other.x, y+other.y, type);}
};

class Snake {
  int id_;
  Controlled_By      cntrl_;
  SnakeStatus        state_;
  std::list<Segment>  body_;
  Direction      direction_;
  Segment rudimentary_tail_;

  bool should_grow_;

  public: 
    Snake(const Controlled_By& cntrl, const SnakeStatus& state, const std::list<Segment>& body, const Direction& direction, const Segment& rudimentary_tail): 
      cntrl_(cntrl), state_(state), body_(body), direction_(direction), rudimentary_tail_(rudimentary_tail) {}

    struct Builder {
      Controlled_By      cntrl_{Controlled_By::bot};
      SnakeStatus        state_{SnakeStatus::ALIVE};
      std::list<Segment>  body_{};
      Direction      direction_{Direction::RIGHT};
      Segment rudimentary_tail_{};

      Builder& setState(const SnakeStatus& state) {
        state_ = state; return *this;
      }

      Builder& setBody(const Segment& seg) {
        Segment start = seg;
        Segment offset = {-1, 0};
        body_.emplace_back(seg);
        body_.emplace_back(seg + offset);        
        body_.emplace_back(seg + offset + offset);

        return *this;
      }

      Builder& setDirection(const Direction& dir) {
        direction_ = dir; return *this;
      }

      Builder& setRudTail(const Segment& seg) {
        body_.push_back(seg);
        return *this;
      }

      Builder& setHumanControlled(const Controlled_By& cntrl) {
        cntrl_ = cntrl;
        return *this;
      }

      Snake build() const {
        return Snake(cntrl_, state_, body_, direction_, rudimentary_tail_);
      }
    };

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
      // При росте не удаляем хвост, но и не обновляем rudimentary_tail_
      // Просто оставляем старый rudimentary_tail_
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
    SnakeStatus        getState() const noexcept {return state_;}
    int                getID()    const noexcept {return id_;}
    bool   isControlledByHyman() const noexcept {return cntrl_ == Controlled_By::human;}
    bool     isControlledByBot() const noexcept {return cntrl_ == Controlled_By::bot;}
    Direction     getDirection() const noexcept {return direction_;}

    void setDirection(Direction dir) noexcept {direction_ = dir;}
    void setID(size_t id) noexcept {id_ = id;}
};