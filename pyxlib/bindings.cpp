#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "math.hpp"

#include <pixlib/data_model.hpp>

PYBIND11_PLUGIN(pyxlib)
{
  pybind11::module m("pyxlib");

  m.def("gui", &gui);

  pybind11::class_<Pixlib::Point>(m, "Point")
    .def(pybind11::init<float, float, float>())
    .def_readwrite("x", &Pixlib::Point::x)
    .def_readwrite("y", &Pixlib::Point::y)
    .def_readwrite("z", &Pixlib::Point::z)
    .def("__eq__", [](const Pixlib::Point &a, const Pixlib::Point &b) {
          return a == b;
      }, pybind11::is_operator())
    ;

  pybind11::class_<Pixlib::LedGeometry>(m, "LedGeometry")
    .def(pybind11::init<>())
    .def(pybind11::init<const std::string&>())
    .def_readwrite("id", &Pixlib::LedGeometry::id)
    .def_readwrite("fadecandy_host", &Pixlib::LedGeometry::fadecandy_host)
    .def_readwrite("locations", &Pixlib::LedGeometry::locations)
    ;

  pybind11::class_<Pixlib::Perspective>(m, "Perspective")
    .def(pybind11::init<>())
    .def_readwrite("id", &Pixlib::Perspective::id)
    .def_readwrite("scope", &Pixlib::Perspective::scope)
    .def_readwrite("yaw", &Pixlib::Perspective::yaw)
    .def_readwrite("pitch", &Pixlib::Perspective::pitch)
    .def_readwrite("zoom", &Pixlib::Perspective::zoom)
    .def("__eq__", [](const Pixlib::Perspective &a, const Pixlib::Perspective &b) {
          return a.scope == b.scope &&
                 a.yaw == b.yaw &&
                 a.pitch == b.pitch &&
                 a.zoom == b.zoom;
      }, pybind11::is_operator())
    ;

  pybind11::class_<Pixlib::Sculpture>(m, "Sculpture")
    .def(pybind11::init<const std::vector<std::string>&, unsigned int>())
    .def_readwrite("camera_perspective", &Pixlib::Sculpture::camera_perspective)
    .def_readwrite("projection_perspective", &Pixlib::Sculpture::projection_perspective)
    // .def_readwrite("shader_code", &Pixlib::Sculpture::shader_code)
    // .def_readwrite("enabled", &Pixlib::Sculpture::enabled);
    ;

  pybind11::class_<Pixlib::PatternCode>(m, "Pattern")
    .def(pybind11::init<const std::string &, const::std::string &>())
    .def_readwrite("name", &Pixlib::PatternCode::name)
    .def_readwrite("shader_code", &Pixlib::PatternCode::shader_code)
    .def_readwrite("enabled", &Pixlib::PatternCode::enabled)
    ;

  return m.ptr();
}
