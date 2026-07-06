#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <utility>

#if defined(__EMSCRIPTEN__)
    #include <emscripten.h>
#endif

template <typename T>
class TaskState
{
public:
    void set_result(T value)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_ready)
        {
            throw std::logic_error("Task already completed.");
        }

        _value = std::move(value);
        _ready = true;
        _cv.notify_all();
    }

    bool ready() const noexcept
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _ready;
    }

    void wait() const
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _cv.wait(lock, [this] { return _ready; });
    }

    T get() const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (!_ready)
        {
            throw std::logic_error("Task is not ready.");
        }

        return *_value;
    }

private:
    mutable std::mutex _mutex;
    mutable std::condition_variable _cv;
    bool _ready = false;
    std::optional<T> _value;
};

template <typename T>
class Task
{
public:
    Task() = default;

    explicit Task(std::shared_ptr<TaskState<T>> state)
        : _state(std::move(state))
    {
    }

    bool valid() const noexcept
    {
        return static_cast<bool>(_state);
    }

    bool ready() const noexcept
    {
        return _state && _state->ready();
    }

    void wait() const
    {
        if (!valid())
        {
            throw std::logic_error("Task is not valid.");
        }

#if defined(__EMSCRIPTEN__)
        while (!ready())
        {
            emscripten_sleep(0);
        }
#else
        _state->wait();
#endif
    }

    T get() const
    {
        if (!valid())
        {
            throw std::logic_error("Task is not valid.");
        }

        if (!ready())
        {
            throw std::logic_error("Task is not ready.");
        }

        return _state->get();
    }

private:
    std::shared_ptr<TaskState<T>> _state;

    template <typename U>
    friend class TaskCompletion;
};

template <typename T>
class TaskCompletion
{
public:
    TaskCompletion()
        : _state(std::make_shared<TaskState<T>>())
    {
    }

    Task<T> task() const
    {
        return Task<T>{_state};
    }

    void set_result(T value)
    {
        _state->set_result(std::move(value));
    }

private:
    std::shared_ptr<TaskState<T>> _state;
};