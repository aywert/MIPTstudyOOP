#include <SFML/Graphics.hpp>
#include "Model.hpp"
#include "TextVisual.hpp"
#include "Controller.hpp"

int main() {
  Model model(30, 10, 10);
  TextVisual tv;
  Controller ctrl(model, tv);
  ctrl.run();
}