#include <iostream>

class animal {
  protected: 
    std::string name;
    std::string sound;
    
  public:
    virtual void voice() const = 0;
    animal (): name("Bobik"), sound("animal") {};
    virtual ~animal() = default;
};

class dog final: public animal {
  public: 
    void voice() const override { std::cout << sound << " Gouf" << std::endl; }
};

class cat final: public animal {
  public: 
    void voice() const override { std::cout << sound << " Meow" << std::endl; }
};

int main() {
  cat cat;
  cat.voice();
  dog dog;
  dog.voice();
}

