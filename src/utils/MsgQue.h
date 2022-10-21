#ifndef UTILS_DEBUG_H_
#define UTILS_DEBUG_H_

#include <string>
#include <vector>
#include <cassert>
#include "utils/Singleton.h"

#define MSG_MAX_SIZE 100

// 注意！非线程安全
class MsgQue : public Singleton<MsgQue> {
public:
    MsgQue() : msgQue(), head(0), tail(0), once(false), loop(false), first() {
        msgQue.resize(MSG_MAX_SIZE);
    }
    void loop_begin() {
        loop = true;
        once = true;
    }

    void loop_end() {
        loop = false;
        once = false;
    }

    void once_begin() {
        if(loop && once) {
            first = true;
            loop = false;
        }
    }

    void once_end() {
        first = false;
    }
    
    template<class... Args>
    void push(const std::string& msg, Args... args) {
        if(once && !first) return ;
        char MsgStr[1000];
        snprintf(MsgStr, sizeof(MsgStr), msg.c_str(), args...);
        std::string formatMsg(MsgStr);
        size_t nextone = (tail + 1) % msgQue.size();
        if(nextone == head) pop();
        msgQue[tail].update(formatMsg);
        tail = nextone;
    }

    bool pop() {
        if(!empty()) {
            head = (head + 1) % msgQue.size();
            return true;
        }
        return false;
    }

    std::string front(size_t p = 0) const {
        std::string result;
        if(p >= size()) return result;
        return msgQue[(head + p) % msgQue.size()].get();
    }

    std::string back(size_t p = 0) const {
        std::string result;
        if(p >= size()) return result;
        return msgQue[(head + size() - p - 1) % msgQue.size()].get();

    }

    bool empty() const {
        return head == tail;
    }

    size_t size() const {
        if(head <= tail) return tail - head;
        return msgQue.size() - (head - tail);
    }
    

private:
    class Msg {
    public:
        Msg() = default;
        void update(const std::string& msg) {
            this->msg = msg;
        }
        std::string get() const {
            return msg;
        }
    private:
        std::string msg;
    };

    std::vector<Msg> msgQue;
    size_t head, tail;
    bool once, first, loop;
};


#endif // UTILS_DEBUG_H_