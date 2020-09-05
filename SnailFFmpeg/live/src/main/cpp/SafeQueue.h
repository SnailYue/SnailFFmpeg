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


template<typename T>
SafeQueue<T>::SafeQueue() {
}

template<typename T>
SafeQueue<T>::~SafeQueue() {
}

template<typename T>
void SafeQueue<T>::push(T new_value) {
    lock_guard<mutex> lk(mt);
    if (work) {
        q.push(new_value);
        cv.notify_one();
    }
}

template<typename T>
int SafeQueue<T>::pop(T &value) {
    int ret = 0;
    unique_lock<mutex> lk(mt);
    cv.wait(lk, [this] { return !work || !q.empty(); });
    if (!q.empty()) {
        value = q.front();
        q.pop();
        ret = 1;
    }
    return ret;
}

template<typename T>
void SafeQueue<T>::setWork(int work) {
    lock_guard<mutex> lk(mt);
    this->work = work;
}


template<typename T>
int SafeQueue<T>::empty() {
    return q.empty();
}

template<typename T>
int SafeQueue<T>::size() {
    return q.size();
}

template<typename T>
void SafeQueue<T>::clear() {
    lock_guard<mutex> lk(mt);
    int size = q.size();
    for (int i = 0; i < size; ++i) {
        T value = q.front();
        releaseCallback(value);
        q.pop();
    }
}

template<typename T>
void SafeQueue<T>::sync() {
    lock_guard<mutex> lk(mt);
    syncHandle(q);
}

template<typename T>
void SafeQueue<T>::setReleaseCallback(SafeQueue<T>::ReleaseCallback r) {
    releaseCallback = r;
}

template<typename T>
void SafeQueue<T>::setSyncHandle(SyncHandle s) {
    syncHandle = s;
}

#endif //SNAILFFMPEG_SAFEQUEUE_H
