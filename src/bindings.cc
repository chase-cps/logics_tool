#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "chase-core.hh"
#include "Console.hh"

namespace py = pybind11;
using namespace chase;

template <typename... Args>
using overload_cast_ = pybind11::detail::overload_cast_impl<Args...>;

PYBIND11_MODULE(pychase_logics_console, m) {
        /*
         * LOGICS CONSOLE
         */
        py::class_<Console,
        std::unique_ptr<Console,
        py::nodelete> >(m, "Console")
            .def(py::init<System *, std::string>())
            .def("run", &Console::run,
                 py::arg("cmd").none(false));
}
 
