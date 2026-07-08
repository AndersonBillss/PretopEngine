#include "CeilToBufferOffset.hpp"
#include "../Limits/Limits.hpp"

namespace Pretop::Math
{
    uint32_t CeilToBufferOffset(uint32_t uniformSize)
    {
        uint32_t bufferOffsetAlignment = Limits::CompatibilityLimits.minStorageBufferOffsetAlignment;
        uint32_t remainder = uniformSize % bufferOffsetAlignment;
        return remainder > 0 ? (uniformSize - remainder + bufferOffsetAlignment) : uniformSize;
    }
} // namespace Pretop::Math
