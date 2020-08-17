//
// Created by surface on 2020/8/17.
//

#ifndef SNAILFFMPEG_SAFEQUEUE_H
#define SNAILFFMPEG_SAFEQUEUE_H

#include <queue>
#include <thread>

using namespace std;

template<typename T>

class SafeQueue {
    typedef void (*ReleaseCallback)(T &);

    typedef void (*SyncHandle)(queue<T> &);

public:
    SafeQueue();

    ~SafeQueue();

    void push(T new_value);

    int pop(T &value);

    void setWork(int work);

    int empty();

    int size();

    void clear();

    void sync();

    void setReleaseCallback(ReleaseCallback r);

    void setSyncHandle(SyncHandle s);

private:
    mutex mt;
    condition_variable cv;
    queue<T> q;
    int work;
    ReleaseCallback releaseCallback;
    SyncHandle syncHandle;
};


#endif //SNAILFFMPEG_SAFEQUEUE_H
