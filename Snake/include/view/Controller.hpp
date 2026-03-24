#pragma once

#include "Model.hpp"
#include <unistd.h>
#include <chrono>

class Controller {
  Model& model_;
  View& view_;
  public: 
    Controller(Model& model, View& view) : model_(model), view_(view) {}
    
    void run() {
      model_.addSnake(Snake(model_.getWidth(), model_.getHeight()));
     
      while (!model_.over()) 
      {
        std::vector<Event> events;
        auto start_time = std::chrono::steady_clock::now();
        std::chrono::milliseconds::rep elapsed = 0;
        long tick_time = model_.getTicks();

        while (elapsed < tick_time) {
          events.push_back(view_.getEvent(tick_time - elapsed));
          
          elapsed = 
          std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count();
        }

        //std::cout << events.size();
        model_.update(events);
        view_.render(model_);
      }
    }
};

