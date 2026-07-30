#pragma once
#include <cstddef>
#include <vector>
#include <memory>

namespace Pretop::Core
{

    template <typename T, size_t PageSize = 256>
    class PagedVector
    {
    public:
        PagedVector()
        {
            _size = 0;
        }

        ~PagedVector()
        {
            _size = 0;
        }

        void PushBack(T &data)
        {
            _pushBackImpl(data);
        }

        void PushBack(T &&data)
        {
            _pushBackImpl(data);
        }

        inline T &operator[](std::size_t index)
        {
            return _at(index);
        }

        inline const T &operator[](std::size_t index) const
        {
            return _at(index);
        }

        inline uint32_t Size()
        {
            return _size;
        }

    private:
        struct DataLocation
        {
            uint32_t Page;
            uint32_t Offset;
        };

        uint32_t _size;
        std::vector<std::unique_ptr<T[]>> _data;

        template <typename U>
        void _pushBackImpl(U &&data)
        {
            if (_isEmpty() || (_size >= _capacity()))
                _addPage();
            _at(_size) = std::forward<U>(data);
            _size++;
        }

        inline uint32_t _capacity()
        {
            return _data.size() * PageSize;
        }

        inline void _addPage()
        {
            _data.push_back(std::make_unique<T[]>(PageSize));
        }

        inline bool _isEmpty()
        {
            return _data.size() == 0;
        }

        inline DataLocation _toLocation(uint32_t index)
        {
            DataLocation loc;
            loc.Page = index / PageSize;
            loc.Offset = index % PageSize;
            return loc;
        }

        inline T &_at(std::size_t index)
        {
            DataLocation loc = _toLocation(index);
            return _data[loc.Page][loc.Offset];
        }

        inline const T &_at(std::size_t index) const
        {
            DataLocation loc = _toLocation(index);
            return _data[loc.Page][loc.Offset];
        }
    };
}
