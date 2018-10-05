#pragma once 

#include "SBDApp.hpp"
#include "SEAdenitaCoreSEAppGUI.hpp"
#include "ADNNanorobot.hpp"
#include "ADNSaveAndLoad.hpp"
#include "ADNNeighbors.hpp"
#include "DASDaedalus.hpp"
#include "DASBackToTheAtom.hpp"
#include "DASCadnano.hpp"
#include "DASComplexOperations.hpp"
#include "SEConfig.hpp"
#include "SEAdenitaVisualModel.hpp"
#include "PIPrimer3.hpp"


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
  void ExportToOxDNA(QString folder, ADNAuxiliary::OxDNAOptions options, ADNPointer<ADNPart> part = nullptr);
  void ExportToSequenceList(QString filename, ADNPointer<ADNPart> part = nullptr);
  void SetScaffoldSequence(std::string seq);
  void ResetVisualModel();
  // Modifications
  void ConnectSingleStrands();
  void BreakSingleStrand();
  void DeleteNucleotide();
  // Debug
  void CenterPart();
  void CreateDSRing(SBQuantity::length radius, SBPosition3 center, SBVector3 normal);
  void LinearCatenanes(SBQuantity::length radius, SBPosition3 center, SBVector3 normal, int num);
  void Kinetoplast(SBQuantity::length radius, SBPosition3 center, SBVector3 normal, int rows, int cols);
  void CalculateBindingRegions();
  void SetStart();
  void MergeComponents();
  void TwistDoubleHelix();
  void TestNeighbors();
  void ImportFromOxDNA(std::string topoFile, std::string configFile);

  virtual void onDocumentEvent(SBDocumentEvent* documentEvent);						///< Handles document events
  virtual void onStructuralEvent(SBStructuralEvent* documentEvent);					///< Handles structural events

  ADNNanorobot* GetNanorobot();

  QStringList GetPartsNameList();

  // Exposed for editors
  void AddPartToActiveLayer(ADNPointer<ADNPart> part);

private:
  void ConnectStructuralSignalSlots(ADNPointer<ADNPart> part);

  std::map<SBDocument*, ADNNanorobot*> nanorobots_;
};


SB_REGISTER_TARGET_TYPE(SEAdenitaCoreSEApp, "SEAdenitaCoreSEApp", "85DB7CE6-AE36-0CF1-7195-4A5DF69B1528");
SB_DECLARE_BASE_TYPE(SEAdenitaCoreSEApp, SBDApp);
