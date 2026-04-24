#include <SFML/Graphics.hpp>
#include <getopt.h>
#include <unistd.h>
#include <cstring>
#include "Model.hpp"
#include "TextVisual.hpp"
#include "GraphicVisual.hpp"
#include "Controller.hpp"

// argv[] ./game -a human smart silly
int main(int argc, char* argv[]) {

  int num_human      = 1;
  int num_smart_bots = 1;
  int num_silly_bots = 1;

  MODEL_STATE state = MODEL_STATE::NOT_LAUNCHED;
  bool view_is_graphic = false;
  // Опции для getopt_long
  static struct option long_options[] = {
    {"human",   required_argument, 0, 'u'},
    {"smart",   required_argument, 0, 's'},
    {"silly",   required_argument, 0, 'l'},
    {"all",     required_argument, 0, 'a'},
    {"tour",    no_argument,       0, 't'},
    {"gview",    no_argument,      0,  0 },
    {0, 0, 0, 0}
  };
  
  int opt;
  int option_index = 0;
  
  while ((opt = getopt_long(argc, argv, "hu:s:l:a:t", long_options, &option_index)) != -1) {
    if (opt == 0) {
      const char* name = long_options[option_index].name;
      if (strcmp(name, "gview") == 0) {
        view_is_graphic = true;
      }
    }
    else 
    switch (opt) {
      case 'u':
        num_human = std::atoi(optarg);
        if (num_human > 2) {
          std::cerr << "\033[31mWarning: Maximum 2 human players\033[0m\n";
          num_human = 2;
        }
        break;
      case 's':
        num_smart_bots = std::atoi(optarg);
        break;
      case 'l':
        num_silly_bots = std::atoi(optarg);
        break;
      case 'a': {
        std::string all_args = optarg;
        if (std::sscanf(all_args.c_str(), "%d %d %d", &num_human, &num_smart_bots, &num_silly_bots) != 3) {
          std::cerr << "\033[31mError: Invalid format for -a. Expected: \"human smart silly\"\033[0m\n";
          return 1;
        }
        break;
      }

      case 't': {
        state = MODEL_STATE::TOURNAMENT;
        break;
      }

      default: {
        std::cerr << "\033[31munknown flag\033[0m\n";
        return 1;
      }
    }
  }

  std::signal(SIGWINCH, handle_winch);
    
  Model model(50, 40, 120);

  try {
    if (view_is_graphic) {
      //graphic view
      GraphicVisual tv(model);  
      Controller ctrl(model, tv);
      
      if (state == MODEL_STATE::TOURNAMENT)
        ctrl.run();
      else 
        ctrl.run(num_silly_bots, num_smart_bots, num_human);
    } 

    else {
      //text view
      TextVisual tv(model);  
      Controller ctrl(model, tv);
      
      if (state == MODEL_STATE::TOURNAMENT)
        ctrl.run();
      else 
        ctrl.run(num_silly_bots, num_smart_bots, num_human);
    }

  } catch (const std::runtime_error& e) {
    std::cerr << "Runtime error: " << e.what() << std::endl;
    return 1;
  } 
}