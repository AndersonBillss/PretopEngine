#include <catch2/catch_test_macros.hpp>
#include "../../source/Utils/NormalizePath.hpp"
#include <iostream>

TEST_CASE("NormalizePath returns correct values for already normalized paths", "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("test/player.png") == "test/player.png");
}

TEST_CASE("NormalizePath returns correct values for ./", "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("./test/player.png") == "test/player.png");
}

TEST_CASE("NormalizePath Correctly resovles ../", "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("/test../player.png") == "/player.png");
}

TEST_CASE("NormalizePath Correctly resovles ../ with immediately preceeding /", "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("/test/../player.png") == "/player.png");
}

TEST_CASE("NormalizePath Correctly resovles ../ with no preceeding /", "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("test/../player.png") == "player.png");
    REQUIRE(Pretop::Utils::NormalizePath("test../player.png") == "player.png");
}

TEST_CASE("NormalizePath Correctly resovles ../ with no preceeding path", "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("../player.png") == "../player.png");
}

TEST_CASE("NormalizePath Correctly resovles ../ with two preceeding paths", "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("assets/test/../player.png") == "assets/player.png");
}

TEST_CASE("NormalizePath Correctly resovles two ../ with no preceeding path", "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("../../player.png") == "../../player.png");
}

TEST_CASE("NormalizePath Correctly resovles three ../ with no preceeding path", "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("../../../player.png") == "../../../player.png");
}

TEST_CASE("NormalizePath Correctly resovles several ../", "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("../test/../test/../player.png") == "../player.png");

    REQUIRE(Pretop::Utils::NormalizePath("../../test/../test../../test/../player.png") == "../../../player.png");
}

TEST_CASE("NormalizePath Correctly resovles ../ with ./", "[Utils][NormalizePath][only]")
{
    REQUIRE(Pretop::Utils::NormalizePath("./../player.png") == "../player.png");
}

TEST_CASE("NormalizePath Correctly resovles empty paths", "[Utils][NormalizePath][only]")
{
    REQUIRE(Pretop::Utils::NormalizePath("./test/../") == "");
}

TEST_CASE("NormalizePath Correctly resovles a bunch of ./ paths", "[Utils][NormalizePath][only]")
{
    REQUIRE(Pretop::Utils::NormalizePath("./././././") == "");
}

TEST_CASE("NormalizePath Converts \\ to /", "[Utils][NormalizePath][only]")
{
    REQUIRE(Pretop::Utils::NormalizePath("tests\\files\\assets\\player.png") == "tests/files/assets/player.png");
}

TEST_CASE("NormalizePath resolves a mix of \\ and /", "[Utils][NormalizePath][only]")
{
    REQUIRE(Pretop::Utils::NormalizePath("../..\\tests\\files/../..\\assets/player.png") == "../../assets/player.png");
}

TEST_CASE("NormalizePath resolves a path that ends with /", "[Utils][NormalizePath][only]")
{
    REQUIRE(Pretop::Utils::NormalizePath("test/") == "test");
}
