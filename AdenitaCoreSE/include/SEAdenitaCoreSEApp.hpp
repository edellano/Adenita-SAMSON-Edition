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
  void LoadParts(QString filename);
  void SaveFile(QString filename, ADNPointer<ADNPart> part = nullptr);
  void LoadPartWithDaedalus(QString filename, int minEdgeSize);
  void ImportFromCadnano(QString filename);
  void ExportToOxDNA(QString folder, ADNAuxiliary::OxDNAOptions options, ADNPointer<ADNPart> part = nullptr);
  void ExportToSequenceList(QString filename, ADNPointer<ADNPart> part = nullptr);
  void SetScaffoldSequence(std::string seq);
  void GenerateSequence(double gcCont, int maxContGs, bool overwrite = true);
  void ResetVisualModel();
  SBVisualModel* GetVisualModel();
  // Modifications
  void BreakSingleStrand(bool fPrime = false);
  void TwistDoubleHelix(CollectionMap<ADNDoubleStrand> dss, double angle);
  bool CalculateBindingRegions(int oligoConc, int monovalentConc, int divalentConc);
  void MergeComponents(ADNPointer<ADNPart> p1, ADNPointer<ADNPart> p2);
  void MoveDoubleStrand(ADNPointer<ADNDoubleStrand> ds, ADNPointer<ADNPart> p);
  void MoveSingleStrand(ADNPointer<ADNSingleStrand> ss, ADNPointer<ADNPart> p);
  void CreateBasePair();
  // Debug
  void AddNtThreeP(int numNt);
  void CenterPart();
  void LinearCatenanes(SBQuantity::length radius, SBPosition3 center, SBVector3 normal, int num);
  void Kinetoplast(SBQuantity::length radius, SBPosition3 center, SBVector3 normal, int rows, int cols);
  void SetStart();
  void TwistDoubleHelix();
  void TestNeighbors();
  void ImportFromOxDNA(std::string topoFile, std::string configFile);
  void FromDatagraph();
  void HighlightXOs();
  void HighlightPosXOs();
  void ExportToCanDo(QString filename);
  void FixDesigns();

  virtual void onDocumentEvent(SBDocumentEvent* documentEvent);						///< Handles document events
  virtual void onStructuralEvent(SBStructuralEvent* documentEvent);					///< Handles structural events
  void ConnectToDocument();
  void ConnectToDocument(SBDocument* doc);

  // Helper functions
  ADNNanorobot* GetNanorobot();
  std::string ReadScaffoldFilename(std::string filename);
  QStringList GetPartsNameList();

  // Adding things to data graph
  void AddPartToActiveLayer(ADNPointer<ADNPart> part, bool positionsData = false);
  void AddConformationToActiveLayer(ADNPointer<ADNConformation> conf);
  void AddLoadedPartToNanorobot(ADNPointer<ADNPart> part);

  virtual void keyPressEvent(QKeyEvent* event);

  void SetMod(bool m);

private:
  void ConnectStructuralSignalSlots(ADNPointer<ADNPart> part);

  std::map<SBDocument*, ADNNanorobot*> nanorobots_;

  bool mod_ = false;
};

SB_REGISTER_TARGET_TYPE(SEAdenitaCoreSEApp, "SEAdenitaCoreSEApp", "85DB7CE6-AE36-0CF1-7195-4A5DF69B1528");
SB_DECLARE_BASE_TYPE(SEAdenitaCoreSEApp, SBDApp);
