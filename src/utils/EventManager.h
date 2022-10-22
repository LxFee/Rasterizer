#ifndef UTILS_EventManagerImpl_H_
#define UTILS_EventManagerImpl_H_

/**
 * reference:
 * https://github.com/Ubpa/LearnOpenGL/blob/master/include/Utility/EventManagerImpl.h
 */

#include "Singleton.h"
#include <functional>
#include <map>
#include <cassert>

namespace Events {
enum ENUM_EVENT {
    // KEY = 0x00000000 ~ 0x0000FFFE
    KEYBOARD_PRESS = 0x00010000,
    KEYBOARD_RELEASE = 0x00020000,
    KEYBOARD_TRIGGER = KEYBOARD_PRESS | KEYBOARD_RELEASE,

    KEYBOARD_ANYKEY = 0x0000FFFF,

    MOUSE_SCROLL = 0x00040000,
    
    // MOUSE_MOVE = 0x00040001,

    MOUSE_BUTTON_LEFT = 0x00040010,
    MOUSE_BUTTON_RIGHT = 0x00040020,
    MOUSE_BUTTON_ANY = MOUSE_BUTTON_LEFT | MOUSE_BUTTON_RIGHT,

    MOUSE_BUTTON_RELEASE = 0x00000001,
    MOUSE_BUTTON_PRESS = 0x00000002,
    MOUSE_BUTTON_TRIGGER = MOUSE_BUTTON_RELEASE | MOUSE_BUTTON_PRESS,

    WINDOW_ID = 0x00080000
};
}  // namespace Events

template <class T, class ...Args>
class EventManagerImpl : public Singleton<EventManagerImpl<T, Args...>> {
public:
    friend class Singleton<EventManagerImpl<T, Args...>>;
    
    static void registerEvent(size_t event, const std::function<T(Args... args)>& op) {
        auto& instance = getInstance();
        auto& directory = instance.directory;

        directory[event] = op;
    }

    static T responseEvent(size_t event, Args... args) {
        auto& instance = getInstance();
        auto& directory = instance.directory;

        auto it = directory.find(event);
        if(it != directory.end()) return it->second(args...);
        if(!std::is_same<T, void>::value) {            
            assert(0 && "This operation has no this kind of return value!");
        }
        return T();
    }

private:
    EventManagerImpl() = default;
    ~EventManagerImpl() = default;
    
    static EventManagerImpl<T, Args...>& getInstance() {
        return Singleton<EventManagerImpl<T, Args...>>::getInstance();
    }

    std::map<size_t, std::function<T(Args... args)>> directory;
};

class EventManager final {
public:

    template <class T, class ...Args>
    static void registerEvent(size_t event, const std::function<T(Args... args)>& op) {
        EventManagerImpl<T, Args...>::registerEvent(event, op);
    }

    template <class T>
    static void registerEvent(size_t event, const T& op) {
        registerEvent(event, std::function(op));
    }

    template <class T = void, class ...Args>
    static T responseEvent(size_t event, Args... args) {
        return EventManagerImpl<T, Args...>::responseEvent(event, args...);
    }

private:
    virtual ~EventManager() = 0;
};

#endif  // UTILS_EventManagerImpl_H_