#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <sstream>
#include "../../src/math/euler.hpp"
#include "../../src/math/constants.hpp"

class Vec4ApproxMatcher : public Catch::Matchers::MatcherBase<Vec4>
{
    Vec4 expected;
    float epsilon;

public:
    Vec4ApproxMatcher(Vec4 expected, float epsilon)
        : expected(expected), epsilon(epsilon) {}

    bool match(const Vec4 &actual) const override { return actual.nearlyEquals(expected, epsilon); }

    std::string describe() const override
    {
        std::ostringstream ss;
        ss << "~= "
           << expected
           << "; epsilon "
           << epsilon;
        return ss.str();
    }
};

TEST_CASE("Roll euler rotate basis vectors as expected", "[math][euler]")
{
    Euler e{90.0f * (float)deg2rad, 0, 0};
    Vec4 v1{1, 0, 0, 0};
    Vec4 result1{1, 0, 0, 0};
    REQUIRE_THAT(e.toMatrix() * v1, Vec4ApproxMatcher(result1, 1e-5f));

    Vec4 v2{0, 1, 0, 0};
    Vec4 result2{0, 0, 1, 0};
    REQUIRE_THAT(e.toMatrix() * v2, Vec4ApproxMatcher(result2, 1e-5f));

    Vec4 v3{0, 0, 1, 0};
    Vec4 result3{0, -1, 0, 0};
    REQUIRE_THAT(e.toMatrix() * v3, Vec4ApproxMatcher(result3, 1e-5f));
}
