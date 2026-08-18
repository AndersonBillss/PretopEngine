#include <cstdint>
#include <string>

std::uint64_t stable_hash(const std::string &str)
{
    constexpr std::uint64_t offset_basis = 14695981039346656037ULL;
    constexpr std::uint64_t prime = 1099511628211ULL;

    std::uint64_t hash = offset_basis;

    for (unsigned char c : str)
    {
        hash ^= c;
        hash *= prime;
    }

    return hash;
}
