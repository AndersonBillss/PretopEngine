#pragma once

#include "AssetTypes.hpp"

#include <chrono>
#include <future>
#include <stdexcept>
#include <utility>

template <typename T>
class AssetHandle
{
public:
    using value_type = T;
    using result_type = AssetResult<T>;

    AssetHandle() = default;

    AssetHandle(AssetId id, AssetKind kind, std::future<result_type> future)
        : _id(std::move(id)), _kind(kind), _future(std::move(future))
    {
    }

    const AssetId &id() const noexcept
    {
        return _id;
    }

    AssetKind kind() const noexcept
    {
        return _kind;
    }

    bool valid() const noexcept
    {
        return _future.valid();
    }

    bool ready() const noexcept
    {
        return valid() &&
               _future.wait_for(std::chrono::seconds{0}) == std::future_status::ready;
    }

    void wait() const
    {
        if (!valid())
        {
            throw std::logic_error("AssetHandle is not valid.");
        }

        _future.wait();
    }

    result_type get() const
    {
        if (!valid())
        {
            throw std::logic_error("AssetHandle is not valid.");
        }

        return _future.get();
    }

    explicit operator bool() const noexcept
    {
        return valid();
    }

private:
    AssetId _id;
    AssetKind _kind = AssetKind::Binary;
    std::shared_future<result_type> _future;
};
