#include "ceilToBufferOffset.hpp"
#include "../limits/limits.hpp"

uint32_t ceilToBufferOffset(uint32_t uniformSize)
{
    uint32_t bufferOffsetAlignment = compatibilityLimits.minStorageBufferOffsetAlignment;
    uint32_t remainder = uniformSize % bufferOffsetAlignment;
    return remainder > 0 ? (uniformSize - remainder + bufferOffsetAlignment) : uniformSize;
}
