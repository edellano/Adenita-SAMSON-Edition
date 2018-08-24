#pragma once 

#include "SBDApp.hpp"
#include "SEAdenitaCoreSEAppGUI.hpp"
#include "ADNNanorobot.hpp"
#include "ADNSaveAndLoad.hpp"
#include "DASDaedalus.hpp"
#include "DASBackToTheAtom.hpp"
#include "DASCadnano.hpp"
#include "DASComplexOperations.hpp"
#include "SEConfig.hpp"
#include "SEAdenitaVisualModel.hpp"


/// This class implements the functionality of the app

// SAMSON Element generator pro tip: add non-GUI functionality in this class. The GUI functionality should go in the SEAdenitaCoreSEAppGUI class

class SEAdenitaCoreSEApp : public SBDApp {

	SB_CLASS

public :

	/// \name Constructors and destructors
	//@{

	SEAdenitaCoreSEApp();																													///< Constructs an app
	virtual ~SEAdenitaCoreSEApp();																											///< Destructs the app

	//@}

	/// \name GUI
	//@{

	SEAdenitaCoreSEAppGUI*											getGUI() const;															///< Returns a pointer to the GUI of the app

	//@}

  void LoadPart(QString filename);
  void SaveFile(QString filename, bool all);
  void LoadPartWithDaedalus(QString filename, int minEdgeSize);
  void ImportFromCadnano(QString filename);
  void ExportToOxDNA(QString folder, ADNAuxiliary::OxDNAOptions options, bool all);
  void ExportToSequenceList(QString filename, bool all);
  void SetScaffoldSequence(std::string seq);
  void CenterPart();
  void ResetVisualModel();
  // Modifications
  void ConnectSingleStrands();
  void BreakSingleStrand();
  void DeleteNucleotide();

  virtual void onDocumentEvent(SBDocumentEvent* documentEvent);						///< Handles document events
  virtual void onStructuralEvent(SBStructuralEvent* documentEvent);					///< Handles structural events

  ADNNanorobot* GetNanorobot();

  // Exposed for editors
  void AddPartToActiveLayer(ADNPointer<ADNPart> part);

private:
  void ConnectStructuralSignalSlots(ADNPointer<ADNPart> part);

  std::map<SBDocument*, ADNNanorobot*> nanorobots_;
};


SB_REGISTER_TARGET_TYPE(SEAdenitaCoreSEApp, "SEAdenitaCoreSEApp", "85DB7CE6-AE36-0CF1-7195-4A5DF69B1528");
SB_DECLARE_BASE_TYPE(SEAdenitaCoreSEApp, SBDApp);
