#include <catch2/catch_test_macros.hpp>
#include "../../source/Utils/NormalizePath.hpp"

TEST_CASE("NormalizePath returns already normalized paths unchanged",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("test/player.png") == "test/player.png");
}

TEST_CASE("NormalizePath resolves current-directory segments",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("./test/player.png") == "test/player.png");

    REQUIRE(Pretop::Utils::NormalizePath("././././") == "");
}

TEST_CASE("NormalizePath preserves .. inside path components",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("/test../player.png") == "/test../player.png");

    REQUIRE(Pretop::Utils::NormalizePath("test../player.png") == "test../player.png");
}

TEST_CASE("NormalizePath resolves parent-directory segments",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("/test/../player.png") == "/player.png");

    REQUIRE(Pretop::Utils::NormalizePath("test/../player.png") == "player.png");

    REQUIRE(Pretop::Utils::NormalizePath("assets/test/../player.png") == "assets/player.png");
}

TEST_CASE("NormalizePath preserves parent traversal in relative paths",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("../player.png") == "../player.png");

    REQUIRE(Pretop::Utils::NormalizePath("../../player.png") == "../../player.png");

    REQUIRE(Pretop::Utils::NormalizePath("../../../player.png") == "../../../player.png");
}

TEST_CASE("NormalizePath resolves several parent-directory segments",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("../test/../test/../player.png") == "../player.png");

    REQUIRE(Pretop::Utils::NormalizePath(
                "../../test/../assets/../player.png") == "../../player.png");

    REQUIRE(Pretop::Utils::NormalizePath(
                "../../test/../test../../test/../player.png") == "../../player.png");
}

TEST_CASE("NormalizePath resolves parent-directory segments with current-directory segments",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("./../player.png") == "../player.png");

    REQUIRE(Pretop::Utils::NormalizePath("./test/../") == "");
}

TEST_CASE("NormalizePath resolves backslashes",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath(
                "tests\\files\\assets\\player.png") == "tests/files/assets/player.png");
}

TEST_CASE("NormalizePath resolves mixed path separators",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath(
                "../..\\tests\\files/../..\\assets/player.png") == "../../assets/player.png");
}

TEST_CASE("NormalizePath removes trailing separators",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("test/") == "test");
}

TEST_CASE("NormalizePath resolves empty paths",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("") == "");

    REQUIRE(Pretop::Utils::NormalizePath(".") == "");
}

TEST_CASE("NormalizePath does not allow absolute paths to escape their root",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("/../../player.png") == "/player.png");

    REQUIRE(Pretop::Utils::NormalizePath("/test/../../player.png") == "/player.png");
}

TEST_CASE("NormalizePath is idempotent",
          "[Utils][NormalizePath]")
{
    const auto normalized =
        Pretop::Utils::NormalizePath("///assets/./test/../player.png");

    REQUIRE(Pretop::Utils::NormalizePath(normalized) == normalized);
}