#ifdef CREATE_PYTHON_BINDINGS

// include pybind11 headers
#include "pybind11/pybind11.h"
#include "pybind11/operators.h"
#include "pybind11/stl.h"
#include "pybind11/stl_bind.h"
namespace py = pybind11;

#include "ADNPart.hpp"


void exposeADNPart(py::module& m) {

  /* Since the SBStructuralModel class is based on the SBStructuralModel class,
   * you do not need to create python bindings for functionality of the SBStructuralModel class,
   * because they are already exposed thanks to the Python Scripting Element.
   */

  // The py::class_ creates bindings for a C++ 
  py::class_<SBStructuralModel>(m, "SBStructuralModel");
  py::class_<
    ADNPart,									/* the class */
    std::unique_ptr<ADNPart, py::nodelete>,	/* the class type */
    SBStructuralModel										/* the base class */
  >
    c(m,													/* pybind11::module */
      "ADNPart",								/* the class name in python*/
      R"(Class describing a basic component of a DNA nanostructure)" /* the docstring */
    );

  // constructors

  c.def(py::init<>(), "Constructs a ADNPart component");

  // attributes

  // read-only attributes
  c.def_property_readonly("getNumberOfDoubleStrands", &ADNPart::GetNumberOfDoubleStrands, "Returns the number of double strands");
  c.def_property_readonly("getNumberOfSingleStrands", &ADNPart::GetNumberOfSingleStrands, "Returns the number of single strands");
  c.def_property_readonly("getNumberOfNucleotides", &ADNPart::GetNumberOfNucleotides, "Returns the number of nucleotides");
  c.def_property_readonly("getNumberOfAtoms", &ADNPart::GetNumberOfAtoms, "Returns the number of atoms");
  c.def_property_readonly("getNumberOfBaseSegments", &ADNPart::GetNumberOfBaseSegments, "Returns the number of base pairs");

  // read-and-write attributes
  c.def_property("Name", &ADNPart::GetName, &ADNPart::SetName, "The name of the Component");

  // functions

  //c.def("clearCustomComment", &CustomStructuralModel::clearCustomComment, "Clears the custom comment");

}

#endif // CREATE_PYTHON_BINDINGS