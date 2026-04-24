#pragma once
#include <unistd.h>
#include <chrono>
#include <optional>
#include "Model.hpp"
#include "View.hpp"


class Controller {
  Model& model_;
  View& view_;

  void setSnakes(int num_of_silly_bots = 1, int num_of_smart_bots = 1, int num_of_human = 2);

  public: 
    Controller(Model& model, View& view) : model_(model), view_(view) {}
    
    void run(int num_of_silly_bots, int num_of_smart_bots, int num_of_human);
    void run();
};
