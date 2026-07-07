#include "CeilToBufferOffset.hpp"
#include "../Limits/Limits.hpp"

uint32_t CeilToBufferOffset(uint32_t uniformSize)
{
    uint32_t bufferOffsetAlignment = CompatibilityLimits.minStorageBufferOffsetAlignment;
    uint32_t remainder = uniformSize % bufferOffsetAlignment;
    return remainder > 0 ? (uniformSize - remainder + bufferOffsetAlignment) : uniformSize;
}
