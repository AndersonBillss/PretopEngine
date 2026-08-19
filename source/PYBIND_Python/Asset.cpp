#include <pybind11/pybind11.h>

namespace py = pybind11;

int add(int a, int b)
{
    return a + b;
}

PYBIND11_MODULE(pretop_engine_python, module)
{
    module.def("add", &add, "Add two integers");
}
