#ifdef CREATE_PYTHON_BINDINGS

#include "PythonBindings.hpp"

#include "SEAdenitaCoreSEApp.hpp"


void exposeAdenita(py::module& m) {

  /* You can create bindings for a function from the SEPyBindTutorialApp class without creating bindings for the class itself
  * In this case, you can call it in Python scripting as follows:
  *	import SE_DDA2A078_1AB6_96BA_0D14_EE1717632D7A as adenita
  *	adenita.createDoubleStrand()
  */

  m.def("createDoubleStrand", []() {
    ADNPointer<ADNPart> part = new ADNPart();
    int length = 12;
    SBPosition3 start = SBPosition3();
    SBVector3 dir = SBVector3(1.0, 0.0, 0.0);
    DASCreator::CreateDoubleStrand(part, length, start, dir);
    SEAdenitaCoreSEApp* adenita = static_cast<SEAdenitaCoreSEApp*>(SAMSON::getApp(SBCContainerUUID("85DB7CE6-AE36-0CF1-7195-4A5DF69B1528"), SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));
    adenita->AddPartToActiveLayer(part);
  },
    "Add a component with a double strand");

  /* Or you can create bindings for the class and its functionality
  * In this case, you can call it in Python scripting as follows:
  *	import SE_F2078F9E_F2CB_BA72_EE86_1E01A10B63D4 as pybindtutorial
  *	tutorialApp = pybindtutorial.SEPyBindTutorialApp() # create a class instance
  *	tutorialApp.addCustomStructuralModel()             # call a function on this instance
  */

  //py::class_<SEAdenitaCoreSEApp> c(m, "SEAdenitaCoreSEApp", "Adenita class");

  // constructor

  //c.def(py::init<>(), "Constructs a custom part");

  // functions

  //c.def("createDoubleStrand", &SEPyBindTutorialApp::addCustomStructuralModel, "Add a custom structural model with a custom group");

}

#endif // CREATE_PYTHON_BINDINGS