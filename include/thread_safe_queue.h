/**
// Created by lyc on 9/2/21.
**/
#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H
#include <queue>
#include <chrono>
#include <condition_variable>

namespace FLOW_COUNT
{
    template <typename T>
    class ThreadSafeQueue
    {
    public:
        ThreadSafeQueue() = default;
        ThreadSafeQueue(const ThreadSafeQueue &) = delete;
        ThreadSafeQueue(ThreadSafeQueue &&) = delete;
        ThreadSafeQueue &operator=(const ThreadSafeQueue &) = delete;
        ThreadSafeQueue &operator=(const ThreadSafeQueue &&) = delete;

        bool Push(const T &new_value);

        size_t Size() const;

        bool WaitAndTryPop(T &value, const std::chrono::microseconds rel_time);

    private:
        const int max_size_{10};
        std::mutex data_m_;
        std::queue<T> q_;
        std::condition_variable notempty_cond_;
    };

    template <typename T>
    size_t ThreadSafeQueue<T>::Size() const
    {
        return q_.size();
    }

    template <typename T>
    bool ThreadSafeQueue<T>::Push(const T &new_value)
    {
        std::unique_lock<std::mutex> lk(data_m_);
        if (q_.size() > max_size_)
        {
            while (q_.size() > (max_size_ / 2))
            {
                q_.pop();
            }
        }

        q_.push(new_value);
        lk.unlock();
        notempty_cond_.notify_one();
        return true;
    }

    template <typename T>
    bool ThreadSafeQueue<T>::WaitAndTryPop(T &value, const std::chrono::microseconds rel_time)
    {
        std::unique_lock<std::mutex> lk(data_m_);
        if (notempty_cond_.wait_for(lk, rel_time, [&]
                                    { return !q_.empty(); }))
        {
            value = q_.front();
            q_.pop();
            return true;
        }

        return false;
    }
}
#endif //THREAD_SAFE_QUEUE_H
