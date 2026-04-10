#include <SFML/Graphics.hpp>
#include "Model.hpp"
#include "TextVisual.hpp"
#include "GraphicVisual.hpp"
#include "Controller.hpp"

int main() {
  std::signal(SIGWINCH, handle_winch);

  // struct winsize w;
  // ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

  Model model(50, 40, 120);
  GraphicVisual tv(model);
  Controller ctrl(model, tv);
  ctrl.run();
}