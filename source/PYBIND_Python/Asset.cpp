#include <pybind11/pybind11.h>
#include "../Utils/GetAssetId.hpp"

namespace py = pybind11;

int add(int a, int b)
{
    return a + b;
}

PYBIND11_MODULE(_native, module)
{
    module.def("add", &add, "Add two integers");
    module.def("get_asset_id", &Pretop::Utils::GetAssetId, "Get an ID for an asset given a path");
}
