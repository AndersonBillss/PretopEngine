#include <string>

#include <catch2/catch_test_macros.hpp>

#include "../../source/Utils/NormalizePath.hpp"

TEST_CASE("NormalizePath returns already normalized paths unchanged",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("test/player.png")
            == "test/player.png");
}

TEST_CASE("NormalizePath resolves current-directory segments",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("./test/player.png")
            == "test/player.png");

    REQUIRE(Pretop::Utils::NormalizePath("././././")
            == "");
}

TEST_CASE("NormalizePath preserves .. inside path components",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("/test../player.png")
            == "/test../player.png");

    REQUIRE(Pretop::Utils::NormalizePath("test../player.png")
            == "test../player.png");
}

TEST_CASE("NormalizePath resolves parent-directory segments",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("/test/../player.png")
            == "/player.png");

    REQUIRE(Pretop::Utils::NormalizePath("test/../player.png")
            == "player.png");

    REQUIRE(Pretop::Utils::NormalizePath("assets/test/../player.png")
            == "assets/player.png");
}

TEST_CASE("NormalizePath preserves parent traversal in relative paths",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("../player.png")
            == "../player.png");

    REQUIRE(Pretop::Utils::NormalizePath("../../player.png")
            == "../../player.png");

    REQUIRE(Pretop::Utils::NormalizePath("../../../player.png")
            == "../../../player.png");
}

TEST_CASE("NormalizePath resolves several parent-directory segments",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("../test/../test/../player.png")
            == "../player.png");

    REQUIRE(Pretop::Utils::NormalizePath(
                "../../test/../assets/../player.png")
            == "../../player.png");

    REQUIRE(Pretop::Utils::NormalizePath(
                "../../test/../test../../test/../player.png")
            == "../../player.png");
}

TEST_CASE("NormalizePath resolves parent-directory segments with current-directory segments",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("./../player.png")
            == "../player.png");

    REQUIRE(Pretop::Utils::NormalizePath("./test/../")
            == "");
}

TEST_CASE("NormalizePath resolves backslashes",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath(
                "tests\\files\\assets\\player.png")
            == "tests/files/assets/player.png");
}

TEST_CASE("NormalizePath resolves mixed path separators",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath(
                "../..\\tests\\files/../..\\assets/player.png")
            == "../../assets/player.png");
}

TEST_CASE("NormalizePath removes trailing separators",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("test/")
            == "test");
}

TEST_CASE("NormalizePath handles empty and current-directory-only paths",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("")
            == "");

    REQUIRE(Pretop::Utils::NormalizePath(".")
            == "");
}

TEST_CASE("NormalizePath does not allow absolute paths to escape their root",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("/../../player.png")
            == "/player.png");

    REQUIRE(Pretop::Utils::NormalizePath("/test/../../player.png")
            == "/player.png");
}

void testIdempotent(const std::string& path)
{
    const auto normalized =
        Pretop::Utils::NormalizePath(path);

    REQUIRE(Pretop::Utils::NormalizePath(normalized)
            == normalized);
}

TEST_CASE("NormalizePath is idempotent",
          "[Utils][NormalizePath]")
{
    testIdempotent("///assets/./test/../player.png");
    testIdempotent("../..../../....../././......");
    testIdempotent("\\.\\..\\...\\....\\");
    testIdempotent("   ../ . / . //. /./  ");
    testIdempotent(" This is a test path / player character");
}

TEST_CASE("NormalizePath collapses repeated separators",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath(
                "assets//test///player.png")
            == "assets/test/player.png");

    REQUIRE(Pretop::Utils::NormalizePath(
                "///assets/player.png")
            == "/assets/player.png");
}

TEST_CASE("NormalizePath resolves ordinary current-directory segments",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath(
                "assets/./test/./player.png")
            == "assets/test/player.png");

    REQUIRE(Pretop::Utils::NormalizePath(
                "assets/./../player.png")
            == "player.png");
}

TEST_CASE("NormalizePath preserves trailing parent-directory segments",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("../")
            == "..");

    REQUIRE(Pretop::Utils::NormalizePath("..")
            == "..");

    REQUIRE(Pretop::Utils::NormalizePath("../../")
            == "../..");

    REQUIRE(Pretop::Utils::NormalizePath("../..")
            == "../..");
}

TEST_CASE("NormalizePath resolves multiple parent segments",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath(
                "assets/test/../../player.png")
            == "player.png");

    REQUIRE(Pretop::Utils::NormalizePath(
                "a/b/../../player.png")
            == "player.png");

    REQUIRE(Pretop::Utils::NormalizePath(
                "a/../../player.png")
            == "../player.png");
}

TEST_CASE("NormalizePath preserves non-special path components",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath(
                "..hidden/player.png")
            == "..hidden/player.png");

    REQUIRE(Pretop::Utils::NormalizePath("file..txt")
            == "file..txt");

    REQUIRE(Pretop::Utils::NormalizePath(
                "test.../player.png")
            == "test.../player.png");

    REQUIRE(Pretop::Utils::NormalizePath(".../..")
            == "");

    REQUIRE(Pretop::Utils::NormalizePath("...../..")
            == "");
}

TEST_CASE("NormalizePath handles root paths",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath("/")
            == "/");

    REQUIRE(Pretop::Utils::NormalizePath("/test/..")
            == "/");

    REQUIRE(Pretop::Utils::NormalizePath("/test/../")
            == "/");
}

TEST_CASE("NormalizePath preserves whitespace",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath(
                "  assets/player.png  ")
            == "  assets/player.png  ");

    REQUIRE(Pretop::Utils::NormalizePath(
                "\t./assets/player.png\r\n")
            == "\t./assets/player.png\r\n");

    REQUIRE(Pretop::Utils::NormalizePath(" \t\r\n ")
            == " \t\r\n ");

    REQUIRE(Pretop::Utils::NormalizePath(" \t. \r\n")
            == " \t. \r\n");

    REQUIRE(Pretop::Utils::NormalizePath("   /   ")
            == "   /   ");
}

TEST_CASE("NormalizePath preserves internal whitespace",
          "[Utils][NormalizePath]")
{
    REQUIRE(Pretop::Utils::NormalizePath(
                "assets/my player.png")
            == "assets/my player.png");

    REQUIRE(Pretop::Utils::NormalizePath(
                "assets/ player.png")
            == "assets/ player.png");

    REQUIRE(Pretop::Utils::NormalizePath(
                "assets/player .png")
            == "assets/player .png");
}