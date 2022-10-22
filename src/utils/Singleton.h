#ifndef UTILS_SINGLETON_H_
#define UTILS_SINGLETON_H_

template <typename T>
class Singleton {
public:
protected:
    Singleton() = default;
    virtual ~Singleton() = default;

    static T& getInstance() {
        static T instance;
        return instance;
    }
};

#endif  // UTILS_SINGLETION_H_