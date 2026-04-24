#pragma once

class Portal {
  int x_;
  int y_;

  public: 
    Portal() {x_ = 0, y_ = 0;};
    Portal(int x, int y): x_(x), y_(y) {}; 

    void setPosition(int x, int y) {
      x_ = x;
      y_ = y;
    };

    int getX() const noexcept {return x_;}
    int getY() const noexcept {return y_;}
};