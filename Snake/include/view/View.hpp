#pragma once

#include "Model.hpp"

#include "view/Controller.hpp"
#include "view/TextVisual.hpp"
#include "view/GraphicVisual.hpp"

enum class EventType {
  PR_UP, 
  PR_DOWN,
  PR_LEFT,
  PR_RIGHT,

  PAUSE,
  HALT,
};

class Event {
  EventType type_;

  public:
    Event(){}
};

class View {
  TextVisual& Tview_;
  GraphicVisual& Gview_;
  std::list<Event> Gevents;
  std::list<Event> Tevents;

  public:
    View(TextVisual& Tview, GraphicVisual& Gview): 
                     Tview_(Tview),        Gview_(Gview) {} 
    void process_input() {
      Tevents = Tview_.getEvents();
      Gevents = Gview_.getEvents();
    }; 

    std::list<Event>& getEvents() {return Gevents;}
    
    void render(Model& model) {};
};  