#pragma once

#include "Model.hpp"
#include "View.hpp"
#include "unistd.h"

class Controller {
  Model& model_;
  View& view_;
  public: 
    Controller(Model& model, View& view) : model_(model), view_(view) {}
    
    void run() {
      while (!model_.over()) {
        view_.process_input();
        model_.update(view_.getEvents());
        view_.render(model_);
        sleep(model_.get_ticks());
      }
    }
};