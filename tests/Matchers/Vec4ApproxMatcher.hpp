#pragma once
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <sstream>
#include "../../Source/Math/Linalg/Vec4.hpp"

class Vec4ApproxMatcher : public Catch::Matchers::MatcherBase<Vec4>
{
    Vec4 _expected;
    float _epsilon;

public:
    Vec4ApproxMatcher(Vec4 expected, float epsilon)
        : _expected(expected), _epsilon(epsilon) {}

    bool match(const Vec4 &actual) const override { return actual.NearlyEquals(_expected, _epsilon); }

    std::string describe() const override
    {
        std::ostringstream ss;
        ss << "~= "
           << _expected
           << "; epsilon "
           << _epsilon;
        return ss.str();
    }
};
