#pragma once

class Rabbit {
  int x_;
  int y_;

  public: 
    Rabbit(int x, int y): x_(x), y_(y) {}; 

    void setPosition(int x, int y) {
      x_ = x;
      y_ = y;
    };

    int getX() const noexcept {return x_;}
    int getY() const noexcept {return y_;}
};