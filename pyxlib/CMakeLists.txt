add_subdirectory(pybind11)

add_library(pyxlib MODULE math.cpp bindings.cpp gui.cpp storage.cpp)
target_link_libraries(pyxlib PRIVATE pybind11::module pixlib nanogui ${NANOGUI_EXTRA_LIBS} glfw libglew_static pthread sqlite3)
set_target_properties(pyxlib PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}"
                                         SUFFIX "${PYTHON_MODULE_EXTENSION}")
