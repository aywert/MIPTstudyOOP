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
      Snake snake_1 = Snake::Builder()
        .setHumanControlled(Controlled_By::human)
        .setBody(Segment(10, 10, SegmentType::BODY))
        .build();

      Snake snake_2 = Snake::Builder()
        .setHumanControlled(Controlled_By::human)
        .setBody(Segment(10, 15, SegmentType::BODY))
        .build();
      
      model_.addSnake(snake_1);
      model_.addSnake(snake_2);

      while (!model_.over()) 
      { 
        std::vector<Event> events;
        auto start_time = std::chrono::steady_clock::now();
        std::chrono::milliseconds::rep elapsed = 0;
        long tick_time = model_.getTicks();

        while (elapsed < tick_time) {
          Event event = view_.getEvent(tick_time - elapsed);
          if (event.type_!=EventType::BAD)
            events.push_back(event);
          
          elapsed = 
          std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count();
        }

        model_.update(events);
        view_.render(model_);
      }
    }
};

