#ifdef CREATE_PYTHON_BINDINGS

#include "PythonBindings.hpp"

#include "SEAdenitaCoreSEApp.hpp"


/*!
  Bindings for specific functions. To use them:
  import SE_DDA2A078_1AB6_96BA_0D14_EE1717632D7A as adenita
  adenita.createDoubleStrand()
*/
void exposeAdenita(py::module& m) {

  m.def("createComponent", []() {
    ADNPart* part = new ADNPart();
    return part;
  },
    "Creates an empty component");

  m.def("refreshVisualModel", []() {
    SEAdenitaCoreSEApp* adenita = static_cast<SEAdenitaCoreSEApp*>(SAMSON::getApp(SBCContainerUUID("85DB7CE6-AE36-0CF1-7195-4A5DF69B1528"), SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));
    adenita->ResetVisualModel();
  },
    "Refreshes the visual model");

  m.def("addComponentToActiveLayer", [](ADNPart* part) {
    SEAdenitaCoreSEApp* adenita = static_cast<SEAdenitaCoreSEApp*>(SAMSON::getApp(SBCContainerUUID("85DB7CE6-AE36-0CF1-7195-4A5DF69B1528"), SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));
    adenita->AddPartToActiveLayer(part);
    adenita->ResetVisualModel();
  },
    "Adds a component to the active layer and refreshes the visual model");

  m.def("addDoubleStrandToComponent", [](ADNPart* part, int length, std::vector<double> initPos, std::vector<double> v) {
    SBPosition3 start = ADNAuxiliary::VectorToSBPosition(initPos);
    SBVector3 dir = ADNAuxiliary::VectorToSBVector(v);
    DASCreator::CreateDoubleStrand(part, length, start, dir);
  },
    "Add a double strand to a component");

  m.def("addCircularStrandToComponent", [](ADNPart* part, double radius, std::vector<double> initPos, std::vector<double> v, bool ssDNA) {
    SBPosition3 center = ADNAuxiliary::VectorToSBPosition(initPos);
    SBVector3 n = ADNAuxiliary::VectorToSBVector(v);
    SBQuantity::length r = SBQuantity::nanometer(radius);
    DASCreator::AddRingToADNPart(part, r, center, n, ssDNA);
  },
    "Add a double or single circular strand to a component");

  m.def("addSingleStrandToComponent", [](ADNPart* part, int length, std::vector<double> initPos, std::vector<double> v) {
    SBPosition3 start = ADNAuxiliary::VectorToSBPosition(initPos);
    SBVector3 dir = ADNAuxiliary::VectorToSBVector(v);
    DASCreator::CreateSingleStrand(part, length, start, dir);
  },
    "Add a single strand to a component");

  m.def("createNanotube", [](double radius, int length, std::vector<double> initPos, std::vector<double> v) {
    SBPosition3 start = ADNAuxiliary::VectorToSBPosition(initPos);
    SBVector3 dir = ADNAuxiliary::VectorToSBVector(v);
    SBQuantity::length r = SBQuantity::nanometer(radius);
    auto part = DASCreator::CreateNanotube(r, start, dir, length);
    return part();
  },
    "Create a nanotube component");
}

#endif // CREATE_PYTHON_BINDINGS