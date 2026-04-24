#pragma once
#include <iostream>
#include <list>
#include <variant>
#include <utility>
#include <random>
#include <chrono>
#include <optional>
#include <algorithm>
#include "Snake.hpp"
#include "Rabbit.hpp"
#include "Portal.hpp"

enum class MODEL_STATE {
  NOT_LAUNCHED,
  IN_PROCCESS,
  GAME_OVER,
  TOURNAMENT,
  ERROR,
};

enum class EventType {
  UP_1, 
  DOWN_1,
  LEFT_1,
  RIGHT_1,

  UP_2, 
  DOWN_2,
  LEFT_2,
  RIGHT_2,

  PAUSE,
  HALT,
  BAD,
};

struct Point {
  int x, y;
};



struct Event {
  EventType type_;

  Event(EventType type) {type_ = type;}
  Event() {type_ = EventType::BAD;}
  void setEventType(EventType type) {type_ = type;}
};

class Model {
  MODEL_STATE status_;

  long long SPAWN_INTERVAL =  500;
  int       MAX_RABBITS    =   20;
  int       SHIFT_COL      =    2;
  int       SHIFT_ROW      =    2;

  int       window_width_  =    0; 
  int       window_height_ =    0;

  int shift_col = SHIFT_COL; 
  int shift_row = SHIFT_ROW; 
  Controlled_By last_snake_type_ = Controlled_By::human; // keeps a record of the last killed snakes type
  size_t tick_;

  std::list<Rabbit> rabbits_;
  std::list<Snake>  snakes_;
  std::vector<int>  human_snakes_ids_; // vector which contains IDs of Snakes controlled by human (up to 2 people)
  int next_snake_id_ = 0; // Счётчик для генерации уникальных ID змей
  std::pair<Portal, Portal> portals_;

  using time_t = std::chrono::steady_clock::time_point;
  class Clock {
    time_t last_time;
    public:
      Clock() : last_time(std::chrono::steady_clock::now()) {}

    long long getDeltaMS() {
      time_t now = std::chrono::steady_clock::now();
      auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time);
      return diff.count();
    }

    void reset() {
      last_time = std::chrono::steady_clock::now();
    }
  };

  std::list<Snake>::iterator findSnakeById(int id);
  std::list<Snake>::iterator removeSnake(int id);

  bool checkBoundaryCollision(const Snake& snake);
  bool checkSnakeCollisions(const Snake& headSnake);\
  

  void handleRabbitCollision(Snake& snake);
  void handlePortalCollisions(Snake& snake);
  void botMovementHandle();
  
  void updateSnakeDirection(size_t playerIdx, Direction newDir);

  Direction smart_bot_calcul_direction(const Snake& bot);
  Direction easy_bot_calcul_direction(const Snake& bot);

  Rabbit nearestRabbit(const Snake& snake);

  std::vector<std::vector<double>> generateHeatmap(int width, int height, const Snake& snake);
  Direction chooseBestMoveBasedOnHeatMap(const Snake& snake, int width, int height, 
                                        const std::vector<std::vector<double>>& heatmap);

  public: 
    Clock RabbitTimer;

    Model(size_t window_width, size_t window_height, size_t tick): 
      status_(MODEL_STATE::IN_PROCCESS),
      window_width_(window_width), 
      window_height_(window_height), 
      tick_(tick){
        spawnPortal();
      }

    MODEL_STATE getStatus() { return status_; };
    bool try_kill();
    bool over()  { return status_ == MODEL_STATE::GAME_OVER;}
    void refresh();
    void update(const std::vector<Event>& events);


    size_t getTicks() const noexcept {return tick_;}
    int    getWidth() const noexcept {return window_width_;}
    int   getHeight() const noexcept {return window_height_;}
    int getColShift() const noexcept {return shift_col;}
    int getRowShift() const noexcept {return shift_row;}

    void addSnake(Snake snake);
    void addRabbit(Rabbit rabbit);
    void spawnRabbit();
    void spawnPortal();

    void setWidth(int width)   {window_width_  = width;}
    void setHeight(int height) {window_height_ = height;}
    void setStatus(MODEL_STATE state) {status_ = state;}
    void setSpawnInterval(long long time) {SPAWN_INTERVAL =  time;}    

    std::pair<int, int> getFirstPortal() {
      return std::pair(portals_.first.getX(), portals_.first.getY());
    }

      std::pair<int, int> getSecondPortal() {
      return std::pair(portals_.second.getX(), portals_.second.getY());
    }

    bool isPositionFree(int x, int y) const;
    bool isPositionSafe(int x, int y) const;
    
    std::list<Snake>&  getSnakes()   {return snakes_; }
    std::list<Rabbit> getRabbits()   {return rabbits_;}
    Controlled_By getLastSnakeType() const noexcept {return last_snake_type_;}
};