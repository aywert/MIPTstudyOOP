#include <SFML/Graphics.hpp>
#include "Model.hpp"
#include "TextVisual.hpp"
#include "Controller.hpp"

int main() {
  std::signal(SIGWINCH, handle_winch);

  Model model(100, 100, 100);
  TextVisual tv;
  Controller ctrl(model, tv);
  ctrl.run();
}