#ifndef UTILS_GStrage_H_
#define UTILS_GStrage_H_

/**
 * reference:
 * https://github.com/Ubpa/LearnOpenGL/blob/master/include/Utility/GStorage.h
 */

#include <map>
#include <string>
#include "Singleton.h"

template <typename K, typename V>
class GStorage : public Singleton<GStorage<K, V>> {
    friend class Singleton<GStorage<K, V>>;
public:
    template <class... Args>
    static bool registerItem(const K& uniqueID, const Args&... args) {
        auto& instance = getInstance();
        auto& directory = instance.directory;

        auto target = directory.find(uniqueID);
        if(target != directory.end()) {
            target->second = V(args...);
            return false;
        }
        directory.emplace(uniqueID, V(args...));
        return true;
    }

    static bool unregisterItem(const K& uniqueID) {
        auto& instance = getInstance();
        auto& directory = instance.directory;

        auto target = directory.find(uniqueID);
        if(target != directory.end()) return false;
        directory[uniqueID];
        return true;
    }

    static V* getPtr(const K& uniqueID) {
        auto& instance = getInstance();
        auto& directory = instance.directory;

        auto target = directory.find(uniqueID);
        if(target == directory.end()) return NULL;
        return &(target->second);
    }

private:
    static GStorage<K, V>& getInstance() {
        return Singleton<GStorage<K, V>>::getInstance();
    }

    std::map<K, V> directory;
};

template <typename V>
using GStrStorage = GStorage<std::string, V>;

#endif  // UTILS_GStrage_H_