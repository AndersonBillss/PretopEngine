#pragma once

#include "Handle.hpp"
#include "Assert.hpp"
#include "PagedVector.hpp"

namespace Pretop::Core
{
    template <class T>
    class RecordTable
    {
    private:
        static constexpr uint32_t invalidGeneration = 0;
        struct Record
        {
            T Data;
            uint32_t Generation;
        };

    public:
        RecordTable() : _records() {};

        Handle Add(T &data)
        {
            return _addImpl(data);
        }

        Handle Add(T &&data)
        {
            return _addImpl(std::move(data));
        }

        inline T *operator[](Handle handle)
        {
            PRETOP_ASSERT(IsValid(handle), "Handle is invalid");
            return &_records[handle.Index].Data;
        }

        inline const T *operator[](Handle handle) const
        {
            PRETOP_ASSERT(IsValid(handle), "Handle is invalid");
            return &_records[handle.Index].Data;
        }

        bool IsValid(Handle handle) const
        {
            if (handle.Index >= _records.Size())
                return false;
            if (_isStale(handle))
                return false;
            return handle.Generation != 0 && _isValid(_records[handle.Index]);
        }

        void Release(Handle handle)
        {
            PRETOP_ASSERT(IsValid(handle), "Handle is invalid");
            _records[handle.Index].Generation = 0;
            _free.push_back(handle.Index);
        }

    private:
        PagedVector<Record> _records;
        std::vector<uint32_t> _free;

        template <class U>
        Handle _addImpl(U &&data)
        {
            const uint32_t generation = _getNextGeneration(invalidGeneration);
            int nextAvailableSlot = _reserveAvailableSlot();

            if (nextAvailableSlot < 0)
            {
                uint32_t index = static_cast<uint32_t>(_records.Size());
                _records.PushBack(Record{std::forward<U>(data), generation});
                return Handle{index, generation};
            }

            uint32_t index = static_cast<uint32_t>(nextAvailableSlot);
            _records[index] = Record{std::forward<U>(data), generation};
            return Handle{index, generation};
        }

        int _reserveAvailableSlot()
        {
            if (_free.size() > 0)
            {
                int result = static_cast<int>(_free[_free.size() - 1]);
                _free.pop_back();
                return result;
            }
            return -1;
        }

        uint32_t _getNextGeneration(uint32_t generation) const
        {
            uint32_t result = generation + 1;
            if (result == 0)
                result++;
            return result;
        }

        bool _isStale(Handle Handle) const
        {
            if (Handle.Index >= _records.Size())
                return true;
            return Handle.Generation != _records[Handle.Index].Generation;
        }
        bool _isValid(const Record &record) const
        {
            return record.Generation != invalidGeneration;
        }
    };
}
