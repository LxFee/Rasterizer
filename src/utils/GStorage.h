#ifndef UTILS_GStrage_H_
#define UTILS_GStrage_H_

/**
 * reference:
 * https://github.com/Ubpa/LearnOpenGL/blob/master/include/Utility/GStorage.h
 */

#include <map>
#include <string>

template <typename T>
class GStorage : public Singleton<GStorage<T>> {
    friend class Singleton<GStorage<T>>;

public:
    bool registerItem(const std::string& uniqueID, const T& item);

    bool registerItem(const std::string& uniqueID);

    bool unregisterItem(const std::string& uniqueID);

    T* getPtr(const std::string& uniqueID);

private:
    std::map<std::string, T> directory;
};

template <typename T>
bool GStorage<T>::registerItem(const std::string& uniqueID, const T& item) {
    auto target = directory.find(uniqueID);
    if(target != directory.end()) {
        target->second = item;
        return false;
    }

    directory[uniqueID] = item;
    return true;
}

template <typename T>
bool GStorage<T>::registerItem(const std::string& uniqueID) {
    auto target = directory.find(uniqueID);
    if(target != directory.end()) return false;

    directory[uniqueID];
    return true;
}

template <typename T>
bool GStorage<T>::unregisterItem(const std::string& uniqueID) {
    auto target = directory.find(uniqueID);
    if(target == directory.end()) return false;

    directory.erase(target);
    return true;
}

template <typename T>
T* GStorage<T>::getPtr(const std::string& uniqueID) {
    auto target = directory.find(uniqueID);
    if(target == directory.end()) return NULL;
    return &(target->second);
}

#endif  // UTILS_GStrage_H_