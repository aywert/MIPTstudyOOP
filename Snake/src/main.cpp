#include <SFML/Graphics.hpp>
#include "Model.hpp"
#include "TextVisual.hpp"
#include "Controller.hpp"

int main() {
  std::signal(SIGWINCH, handle_winch);

  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

  Model model(w.ws_col, w.ws_row, 120);
  TextVisual tv(model);
  Controller ctrl(model, tv);
  ctrl.run();
}