#include <catch2/catch_test_macros.hpp>
#include "../../source/Utils/NormalizePath.hpp"
#include <iostream>

TEST_CASE("NormalizePath returns correct values for already normalized paths", "[Utils][NormalizePath]")
{
    std::string path = "test/player.png";
    REQUIRE(Pretop::Utils::NormalizePath(path) == path);
}

TEST_CASE("NormalizePath returns correct values for ./", "[Utils][NormalizePath]")
{
    std::string path = "./test/player.png";
    REQUIRE(Pretop::Utils::NormalizePath(path) == "test/player.png");
}

TEST_CASE("NormalizePath Correctly resovles ../", "[Utils][NormalizePath]")
{
    std::string path = "/test../player.png";
    REQUIRE(Pretop::Utils::NormalizePath(path) == "/player.png");
}

TEST_CASE("NormalizePath Correctly resovles ../ with immediately preceeding /", "[Utils][NormalizePath]")
{
    std::string path = "/test/../player.png";
    REQUIRE(Pretop::Utils::NormalizePath(path) == "/player.png");
}

TEST_CASE("NormalizePath Correctly resovles ../ with no preceeding /", "[Utils][NormalizePath]")
{
    std::string path = "test/../player.png";
    REQUIRE(Pretop::Utils::NormalizePath(path) == "player.png");

    std::string path2 = "test../player.png";
    REQUIRE(Pretop::Utils::NormalizePath(path2) == "player.png");
}

TEST_CASE("NormalizePath Correctly resovles ../ with no preceeding path", "[Utils][NormalizePath]")
{
    std::string path = "../player.png";
    REQUIRE(Pretop::Utils::NormalizePath(path) == "../player.png");
}

TEST_CASE("NormalizePath Correctly resovles ../ with two preceeding paths", "[Utils][NormalizePath]")
{
    std::string path = "assets/test/../player.png";
    REQUIRE(Pretop::Utils::NormalizePath(path) == "assets/player.png");
}

TEST_CASE("NormalizePath Correctly resovles two ../ with no preceeding path", "[Utils][NormalizePath]")
{
    std::string path = "../../player.png";
    REQUIRE(Pretop::Utils::NormalizePath(path) == "../../player.png");
}

TEST_CASE("NormalizePath Correctly resovles three ../ with no preceeding path", "[Utils][NormalizePath]")
{
    std::string path = "../../../player.png";
    REQUIRE(Pretop::Utils::NormalizePath(path) == "../../../player.png");
}

TEST_CASE("NormalizePath Correctly resovles several ../", "[Utils][NormalizePath]")
{
    std::string path = "../test/../test/../player.png";
    REQUIRE(Pretop::Utils::NormalizePath(path) == "../player.png");

    std::string path2 = "../../test/../test../../test/../player.png";
    REQUIRE(Pretop::Utils::NormalizePath(path2) == "../../../player.png");
}
