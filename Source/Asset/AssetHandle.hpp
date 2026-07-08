#pragma once
#include "AssetTypes.hpp"
#include "Task.hpp"

#include <stdexcept>
#include <utility>

namespace Pretop::Asset
{
    template <typename T>
    class AssetHandle
    {
    public:
        using ValueType = T;
        using ResultType = AssetResult<T>;

        AssetHandle() = default;

        AssetHandle(AssetId id, AssetKind kind, Task<ResultType> task)
            : _id(std::move(id)), _kind(kind), _task(std::move(task))
        {
        }

        const AssetId &Id() const noexcept
        {
            return _id;
        }

        AssetKind Kind() const noexcept
        {
            return _kind;
        }

        bool Valid() const noexcept
        {
            return _task.Valid();
        }

        bool Ready() const noexcept
        {
            return Valid() && _task.Ready();
        }

        void Wait()
        {
            if (!Valid())
            {
                throw std::logic_error("AssetHandle is not valid.");
            }

            _task.Wait();
        }

        ResultType Get()
        {
            if (!Valid())
            {
                throw std::logic_error("AssetHandle is not valid.");
            }

            return _task.Get();
        }

        explicit operator bool() const noexcept
        {
            return Valid();
        }

    private:
        AssetId _id;
        AssetKind _kind = AssetKind::Binary;
        Task<ResultType> _task;
    };
} // namespace Pretop::Asset
