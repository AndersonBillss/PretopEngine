#pragma once

#include "Handle.hpp"
#include "Assert.hpp"
#include "PagedVector.hpp"
#include <unordered_set>
#include <iostream>

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

        class Iterator
        {
        public:
            explicit Iterator(RecordTable *recordTable, Handle handle) : _recordTable(recordTable)
            {
                this->_handle = handle;
            }

            Handle operator*() const { return this->_handle; }

            Iterator &operator++()
            {
                this->_handle = this->_recordTable->_getNextValidHandle(this->_handle.Index + 1);
                return *this;
            }

            bool operator==(const Iterator &other) const
            {
                return this->_handle.Generation == other._handle.Generation && this->_handle.Index == other._handle.Index;
            }
            bool operator!=(const Iterator &other) const
            {
                return !(this->_handle.Generation == other._handle.Generation && this->_handle.Index == other._handle.Index);
            }

        private:
            RecordTable *_recordTable;
            Handle _handle;
        };

        Iterator begin() { return Iterator(this, this->_getNextValidHandle(0)); }
        Iterator end() { return Iterator(this, Handle{0, 0}); }

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

        Handle _getNextValidHandle(uint32_t index)
        {
            Handle result = Handle{0, 0};
            while (
                index <= this->_records.Size() &&
                this->_free.find(index) != this->_free.end())
            {
                index++;
            }
            if (index < this->_records.Size())
            {
                result = Handle{index, this->_records[index].Generation};
            }
            return result;
        }
    };
}
