#include <catch2/catch_test_macros.hpp>
#include "../../source/Utils/NormalizePath.hpp"
#include <iostream>

TEST_CASE("NormalizePath returns correct values for already normalized paths", "[Utils][NormalizePath]")
{
    std::string path = "test/player.png";
    REQUIRE(Pretop::Utils::NormalizePath(path) == path);
}
