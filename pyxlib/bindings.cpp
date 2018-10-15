#include <pybind11/pybind11.h>
#include "math.hpp"

PYBIND11_PLUGIN(pyxlib)
{
    pybind11::module m("pyxlib");
    
    m.def("add", &add);
    m.def("subtract", &subtract);
    return m.ptr();
}
