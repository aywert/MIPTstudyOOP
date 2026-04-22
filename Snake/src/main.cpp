#include <SFML/Graphics.hpp>
#include <getopt.h>
#include <unistd.h>
#include "Model.hpp"
#include "TextVisual.hpp"
#include "GraphicVisual.hpp"
#include "Controller.hpp"

// argv[] ./game -a human smart silly
int main(int argc, char* argv[]) {

  int num_human = 1;
  int num_smart_bots = 1;
  int num_silly_bots = 1;
  
  // Опции для getopt_long
  static struct option long_options[] = {
    {"human",   required_argument, 0, 'u'},
    {"smart",   required_argument, 0, 's'},
    {"silly",   required_argument, 0, 'l'},
    {"all",     required_argument, 0, 'a'},
    {0, 0, 0, 0}
  };
  
  int opt;
  int option_index = 0;
  
  while ((opt = getopt_long(argc, argv, "hu:s:l:a:", long_options, &option_index)) != -1) {
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
        std::cout << all_args.c_str() << "\n";
        if (std::sscanf(all_args.c_str(), "%d %d %d", &num_human, &num_smart_bots, &num_silly_bots) != 3) {
          std::cerr << "\033[31mError: Invalid format for -a. Expected: \"human smart silly\"\033[0m\n";
          return 1;
        }
        break;
      }

      default: {
        std::cerr << "unknown flag";
        return 1;
      }
    }
  }

  std::signal(SIGWINCH, handle_winch);
    
  Model model(30, 30, 120);

  try {
    GraphicVisual tv(model);  
  
  Controller ctrl(model, tv);
  ctrl.run(num_silly_bots, num_smart_bots, num_human);

  } catch (const std::runtime_error& e) {
    std::cerr << "Runtime error: " << e.what() << std::endl;
    return 1;
  } 
}