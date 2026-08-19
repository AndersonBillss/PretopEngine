#include <catch2/catch_test_macros.hpp>
#include "../../source/Utils/NormalizePath.hpp"
#include <iostream>

TEST_CASE("NormalizePath returns correct values for already normalized paths", "[Utils][NormalizePath]")
{
    std::string path = "test/player.png";
    REQUIRE(Pretop::Utils::NormalizePath(path) == path);
}

TEST_CASE("NormalizePath returns correct values for relative paths", "[Utils][NormalizePath]")
{
    std::string path = "./test/player.png";
    REQUIRE(Pretop::Utils::NormalizePath(path) == "test/player.png");
}

TEST_CASE("NormalizePath Correctly resovles backtrack", "[Utils][NormalizePath]")
{
    std::string path = "/test../player.png";
    REQUIRE(Pretop::Utils::NormalizePath(path) == "player.png");
}

TEST_CASE("NormalizePath Correctly resovles backtrack with immediately preceeding /", "[Utils][NormalizePath]")
{
    std::string path = "/test/../player.png";
    REQUIRE(Pretop::Utils::NormalizePath(path) == "player.png");
}

TEST_CASE("NormalizePath Correctly resovles backtrack no preceeding /", "[Utils][NormalizePath]")
{
    std::string path = "test/../player.png";
    REQUIRE(Pretop::Utils::NormalizePath(path) == "player.png");

    std::string path2 = "test../player.png";
    REQUIRE(Pretop::Utils::NormalizePath(path2) == "player.png");
}

TEST_CASE("NormalizePath Correctly resovles backtrack no preceeding path", "[Utils][NormalizePath]")
{
    std::string path = "../player.png";
    REQUIRE(Pretop::Utils::NormalizePath(path) == "../player.png");
}
