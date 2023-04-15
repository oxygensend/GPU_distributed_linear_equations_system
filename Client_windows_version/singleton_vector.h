#include <vector>

class SingletonVector {
public:
    static SingletonVector *getInstance() {
        static SingletonVector instance;
        return &instance;
    }

    void addElement(int element) {
        vec.push_back(element);
    }

    void removeElement(int element) {
        vec.erase(std::remove(vec.begin(), vec.end(), element), vec.end());
    }

    std::vector<int> getVector() const {
        return vec;
    }

    bool isEmpty() {
        return vec.empty();
    }

    void setVector(const std::vector<int> &newVec) {
        vec.clear();
        vec = newVec;
    }

private:
    SingletonVector() {}

    std::vector<int> vec;
    static SingletonVector *instance;
};

