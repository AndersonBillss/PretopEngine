#pragma once
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <sstream>
#include "../../src/math/linalg/vec4.hpp"

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