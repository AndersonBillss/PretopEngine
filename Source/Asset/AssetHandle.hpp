#pragma once

#include "AssetTypes.hpp"
#include "Task.hpp"

#include <stdexcept>
#include <utility>

template <typename T>
class AssetHandle
{
public:
    using value_type = T;
    using result_type = AssetResult<T>;

    AssetHandle() = default;

    AssetHandle(AssetId id, AssetKind kind, Task<result_type> task)
        : _id(std::move(id))
        , _kind(kind)
        , _task(std::move(task))
    {
    }

    const AssetId& id() const noexcept
    {
        return _id;
    }

    AssetKind kind() const noexcept
    {
        return _kind;
    }

    bool valid() const noexcept
    {
        return _task.valid();
    }

    bool ready() const noexcept
    {
        return valid() && _task.ready();
    }

    void wait()
    {
        if (!valid())
        {
            throw std::logic_error("AssetHandle is not valid.");
        }

        _task.wait();
    }

    result_type get()
    {
        if (!valid())
        {
            throw std::logic_error("AssetHandle is not valid.");
        }

        return _task.get();
    }

    explicit operator bool() const noexcept
    {
        return valid();
    }

private:
    AssetId _id;
    AssetKind _kind = AssetKind::Binary;
    Task<result_type> _task;
};
