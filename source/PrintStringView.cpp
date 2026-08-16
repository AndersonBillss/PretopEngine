#include "PrintStringView.hpp"

namespace Pretop
{
    std::ostream &operator<<(std::ostream &os, WGPUStringView stringView)
    {
        for (size_t i = 0; i < stringView.length; i++)
        {
            os << stringView.data[i];
        }
        return os;
    }
} // namespace Pretop
