#pragma once

#include "Model.hpp"
#include <unistd.h>

class Controller {
  Model& model_;
  View& view_;
  public: 
    Controller(Model& model, View& view) : model_(model), view_(view) {}
    
    void run() {
      model_.addSnake(Snake(model_.getWidth(), model_.getHeight()));
      while (!model_.over()) {
        model_.update(view_.getEvents());
        view_.render(model_);
        usleep(model_.getTicks());
      }
    }
};

