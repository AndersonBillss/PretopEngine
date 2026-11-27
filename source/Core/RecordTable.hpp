#pragma once

#include "Handle.hpp"
#include "Assert.hpp"
#include "PagedVector.hpp"
#include <unordered_set>

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
            Record &record = _records[handle.Index];
            record.Generation = _getNextGeneration(record.Generation);
            _free.insert(handle.Index);
        }

    private:
        PagedVector<Record> _records;
        std::unordered_set<uint32_t> _free;

        template <class U>
        Handle _addImpl(U &&data)
        {
            int nextAvailableSlot = _reserveAvailableSlot();

            if (nextAvailableSlot < 0)
            {
                uint32_t index = static_cast<uint32_t>(_records.Size());
                uint32_t generation = _getNextGeneration(invalidGeneration);
                _records.PushBack(Record{std::forward<U>(data), generation});
                return Handle{index, generation};
            }

            uint32_t index = static_cast<uint32_t>(nextAvailableSlot);
            _records[index].Data = std::forward<U>(data);
            return Handle{index, _records[index].Generation};
        }

        int _reserveAvailableSlot()
        {
            if (_free.size() > 0)
            {
                auto handle = this->_free.extract(this->_free.begin());
                return std::move(handle.value());
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
