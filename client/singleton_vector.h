#include <vector>

class SingletonVector {
public:
  static SingletonVector* getInstance() {
     static SingletonVector instance;
     return &instance;
    }
  
  void addElement(int element) {
    vec.push_back(element);
  }
  
  std::vector<int> getVector() const {
    return vec;
  }

  void setVector(const std::vector<int>& newVec) {
    vec.clear();
    vec = newVec;
  }
  
private:
  SingletonVector() {}
  std::vector<int> vec;
  static  SingletonVector* instance;
};

