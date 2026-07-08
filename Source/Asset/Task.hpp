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

namespace Pretop::Asset
{
    template <typename T>
    class TaskState
    {
    public:
        void SetResult(T value)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (_ready)
            {
                throw std::logic_error("Task already completed.");
            }

            _value = std::move(value);
            _ready = true;
            _conditionVariable.notify_all();
        }

        bool Ready() const noexcept
        {
            std::lock_guard<std::mutex> lock(_mutex);
            return _ready;
        }

        void Wait() const
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _conditionVariable.wait(lock, [this]
                                    { return _ready; });
        }

        T Get() const
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
        mutable std::condition_variable _conditionVariable;
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

        bool Valid() const noexcept
        {
            return static_cast<bool>(_state);
        }

        bool Ready() const noexcept
        {
            return _state && _state->Ready();
        }

        void Wait() const
        {
            if (!Valid())
            {
                throw std::logic_error("Task is not valid.");
            }

#if defined(__EMSCRIPTEN__)
            while (!Ready())
            {
                emscripten_sleep(0);
            }
#else
            _state->Wait();
#endif
        }

        T Get() const
        {
            if (!Valid())
            {
                throw std::logic_error("Task is not valid.");
            }

            if (!Ready())
            {
                throw std::logic_error("Task is not ready.");
            }

            return _state->Get();
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

        Task<T> CreateTask() const
        {
            return Task<T>{_state};
        }

        void SetResult(T value)
        {
            _state->SetResult(std::move(value));
        }

    private:
        std::shared_ptr<TaskState<T>> _state;
    };
} // namespace Pretop::Asset
