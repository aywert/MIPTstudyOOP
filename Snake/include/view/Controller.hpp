#pragma once

#include "Model.hpp"
#include <unistd.h>
#include <chrono>

class Controller {
  Model& model_;
  View& view_;
  public: 
    Controller(Model& model, View& view) : model_(model), view_(view) {}
    
    void run(int num_of_silly_bots, int num_of_smart_bots, int num_of_human) {
      setSnakes(num_of_silly_bots, num_of_smart_bots, num_of_human);

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
          std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start_time).count();
        }

        model_.update(events);
        view_.render(model_);
      }
    }

    void run() {

      int smart_wins = 0;
      int silly_wins = 0;

      for (int i = 0; i < 100; i++) {
        setSnakes(1, 1, 0);
        model_.setSpawnInterval(30);

        while (!model_.over()) { 
          usleep(500);
          if (model_.try_kill()) {
            if      (model_.getLastSnakeType() == Controlled_By::silly_bot) silly_wins++;
            else if (model_.getLastSnakeType() == Controlled_By::smart_bot) smart_wins++;
          }

          Event event = view_.flushEvents();//flashing events in order to empty sfml buffer
          if (event.type_ == EventType::HALT) return;

          std::vector<Event> events;
          model_.update(events);
          view_.render(model_);
        }

       // usleep(200000);
        view_.showFeatures(i, smart_wins, silly_wins);

        model_.refresh();
        usleep(2000000);
      }
    }


  void setSnakes(int num_of_silly_bots = 1, int num_of_smart_bots = 1, int num_of_human = 2) {
    
    if (num_of_human > 2) {
      std::cerr << "\033[31mGame only supports up to 2 players...\033[0m\n";
      num_of_human = 2;
    }
      
    int min_x = model_.getColShift() + 1;
    int max_x = model_.getWidth()  - model_.getColShift();
    int min_y = model_.getRowShift() + 1;
    int max_y = model_.getHeight() - model_.getRowShift();

    std::vector<int> colors = {32, 34, 33, 36, 35};

    static std::mt19937 gen(
      static_cast<unsigned int>(
        std::chrono::steady_clock::now().time_since_epoch().count()
      )
    );

    auto generateUniqueHeadPosition = [&]() -> std::optional<Segment> {
      for (int attempt = 0; attempt < 200; ++attempt) {
        std::uniform_int_distribution<int> distribX(min_x, max_x);
        std::uniform_int_distribution<int> distribY(min_y, max_y);
        
        int x = distribX(gen);
        int y = distribY(gen);
        
        // Проверяем, свободна ли позиция (не занята другими змеями)
        if (model_.isPositionFree(x, y)) {
          return Segment(x, y, SegmentType::HEAD);
        }
      }
      return std::nullopt;
    };

    std::vector<Segment> human_start_positions = {
      Segment(model_.getWidth()/4,   model_.getHeight()*3/4, SegmentType::HEAD),  // Первый игрок
      Segment(model_.getWidth()*3/4, model_.getHeight()*3/4, SegmentType::HEAD)  // Второй игрок (правый верхний угол)
    };

    for (int i = 0; i < num_of_human; ++i) {
      Segment head_pos = human_start_positions[i];
      
      Snake snake = Snake::Builder()
        .setControlledBy(Controlled_By::human)
        .setDirection(Direction::UP)
        .setColor(colors[i])
        .setBody(head_pos)
        .build();
      
      model_.addSnake(snake);
    }

    for (int i = 0; i < num_of_silly_bots; ++i) {
      auto head_pos = generateUniqueHeadPosition();
      
      if (head_pos.has_value()) {

        Snake snake = Snake::Builder()
          .setControlledBy(Controlled_By::silly_bot)
          .setColor(colors[2])
          .setBody(head_pos.value())
          .build();
        
        model_.addSnake(snake);
      } else {
        std::cerr << "Warning: Could not spawn silly bots" << i << std::endl;
      }
    }

    for (int i = 0; i < num_of_smart_bots; ++i) {
      auto head_pos = generateUniqueHeadPosition();
      
      if (head_pos.has_value()) {

        Snake snake = Snake::Builder()
          .setControlledBy(Controlled_By::smart_bot)
          .setColor(colors[3])
          .setBody(head_pos.value())
          .build();
        
        model_.addSnake(snake);
      } else {
        std::cerr << "Warning: Could not spawn smart bots" << i << std::endl;
      }
    }


  }
};
