add_subdirectory(pybind11)

pybind11_add_module(
 pyxlib
 math.cpp
 bindings.cpp )
