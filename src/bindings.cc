#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "chase-core.hh"
#include "Console.hh"
#include "LogicsSpecsBuilder.hh"

namespace py = pybind11;
using namespace chase;

template <typename... Args>
using overload_cast_ = pybind11::detail::overload_cast_impl<Args...>;

PYBIND11_MODULE(pychase_logicsLang, m) {

    /*
     * Parser and Front-end.
     */    
    py::class_<LogicsSpecsBuilder,
        std::unique_ptr<LogicsSpecsBuilder,
        py::nodelete> >(m, "LogicsSpecsBuilder")
        .def(py::init<>())
        .def("getSystem", &LogicsSpecsBuilder::getSystem)
        .def("parseSpecificationFile", &LogicsSpecsBuilder::parseSpecificationFile,
             py::arg("infile").none(false));

    /*
     * Logics Console.
     */
    py::class_<Console,
        std::unique_ptr<Console,
        py::nodelete> >(m, "Console")
            .def(py::init<System *, std::string>())
            .def("run", &Console::run,
                 py::arg("cmd").none(false));
}
 
