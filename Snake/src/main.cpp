#include <SFML/Graphics.hpp>
#include "Model.hpp"
#include "TextVisual.hpp"
#include "GraphicVisual.hpp"
#include "Controller.hpp"

int main() {
  
    std::signal(SIGWINCH, handle_winch);
    
    Model model(50, 40, 120);

    try {
      GraphicVisual tv(model);  
    
    Controller ctrl(model, tv);
    ctrl.run(1, 2, 2);

    } catch (const std::runtime_error& e) {
    std::cerr << "Runtime error: " << e.what() << std::endl;
    return 1;
    
  } 
}