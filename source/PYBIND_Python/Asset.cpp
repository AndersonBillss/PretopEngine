#include <pybind11/pybind11.h>
#include "../Utils/GetAssetId.hpp"
#include "../Utils/NormalizePath.hpp"

namespace py = pybind11;

int add(int a, int b)
{
    return a + b;
}

PYBIND11_MODULE(_native, module)
{
    module.def("add", &add, "Add two integers");
    module.def("get_asset_id", &Pretop::Utils::GetAssetId, "Get an ID for an asset given a path");
    module.def("normalize_path", &Pretop::Utils::NormalizePath, "Normalize a path");
}
