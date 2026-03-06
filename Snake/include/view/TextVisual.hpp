#pragma once
#include "View.hpp"

class TextVisual: public View {
  std::string buffer;

  public:

    
  void drawBox(Model& model) {
    int width = model.getWidth();   
    int height = model.getHeight(); 
    
    int offsetX = 2;
    int offsetY = 2;
    
    gotoxy(offsetX, offsetY);
    

    buffer += "┌";
    int index = 0;
    for (index; index < width; index++) {buffer += "─";}
    buffer += "┐\n";

    setColor(45);
    gotoxy(width/2, 0);
    buffer.append("Slizarin");
    setColor(0);
    
    
    for (int row = 0; row < height; row++) {
      gotoxy(offsetX, offsetY + row + 1);
      buffer += "│";
      
      gotoxy(offsetX + 1, offsetY + row + 1);
      for (int i = 0; i < width; i++) {
        buffer += " ";
      }
      
      gotoxy(offsetX + width + 1, offsetY + row + 1);
      buffer += "│\n";
    }
    
      gotoxy(offsetX, offsetY + height + 1);
      buffer += "└";
      for (int i = 0; i < width; i++) {
          buffer += "─";
      }
      buffer += "┘\n";
    
    }

    virtual void gotoxy(const int x, const int y) override {
      buffer+="\033[" + std::to_string(y) + ";" + std::to_string(x) + "H";
    }

    virtual void hideCursor() override {
      buffer+="\033[?25l";
    }
    virtual void showCursor() override {
      buffer+="\033[?25h";
    }

    void clearScreen() override {
      buffer+="\033[2J";
      buffer+="\033[H";
    }

    void setColor(int color) override{
      buffer.append("\033[");
      buffer.append(std::to_string(color));
      buffer.append("m");
    }

    void render(Model& model) override {
      buffer.clear();  // ОЧИЩАЕМ буфер перед новой отрисовкой!
        
      clearScreen(); 
      drawBox(model);
      std::cout << buffer << std::flush;
    }

    void drawRabbit(Rabbit& rabbit) override{};
    void drawSnake(Snake& snake) override{
      std::list<Segment> body = snake.getBody();
      for (const auto& seg: body) {
        gotoxy(seg.position_x, seg.position_y);
        buffer.append("*");
      }
    };
    void drawSpace(Snake& snake) override{};

    std::list<Event> getEvents() override {
      return std::list<Event>{};
    }
};