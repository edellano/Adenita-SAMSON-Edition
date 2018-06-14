#pragma once 

#include <time.h>
#include <QString>
#include <QEasingCurve>

#include "SBMVisualModel.hpp"

#include "SBBaseEvent.hpp"
#include "SBDocumentEvent.hpp"
#include "SBStructuralEvent.hpp"
#include "SBAtom.hpp"
#include "SBMolecule.hpp"
#include "SBBond.hpp"
#include "MSVModel.hpp"
#include "SEConfig.hpp"
#include "DASAlgorithms.hpp"
#include "MSVDisplayHelper.hpp"
#include "MSVPolyhedronRenderer.hpp"


// This class creates a visual model for a DNA nanostructure (top down / bottom up)
class SENucleotidesVisualModel : public SBMVisualModel {

  SB_CLASS

public:

  /// Constructors and destructors
  SENucleotidesVisualModel();																													
  SENucleotidesVisualModel(const SBNodeIndexer& nodeIndexer);												
  SENucleotidesVisualModel(std::vector<SBDDataGraphNode*>& nodeVector);
  virtual ~SENucleotidesVisualModel();																					

  virtual bool												isSerializable() const; // Returns true when the class is serializable
  virtual void												serialize(SBCSerializer* serializer, const SBNodeIndexer& nodeIndexer, const SBVersionNumber& sdkVersionNumber = SB_SDK_VERSION_NUMBER, const SBVersionNumber& classVersionNumber = SBVersionNumber(1, 0, 0)) const;		// Serializes the node
  virtual void												unserialize(SBCSerializer* serializer, const SBNodeIndexer& nodeIndexer, const SBVersionNumber& sdkVersionNumber = SB_SDK_VERSION_NUMBER, const SBVersionNumber& classVersionNumber = SBVersionNumber(1, 0, 0));			// Unserializes the node
  virtual	void												eraseImplementation(); // Erases the visual model
  
  virtual void												getNodes(SBNodeIndexer& nodeIndexer, const SBNodePredicate& selectionPredicate = SBDDataGraphNode::All(), const SBNodePredicate&
    visitPredicate = SBDDataGraphNode::All());

  //exposed methods
  void                                changeScale(double dScale);
  void                                createThis();
  void                                initialize(); //initializes bottom up model
  void                                setMSVModel(MSVModel* model);
  void                                bindingRegionPropertiesChanged(int index);
  void                                changeOverlay(std::vector<string> texts, std::vector<SBPosition3> positions);
  void                                setBindingRegionParameters(int oligo_conc, int mv, int dv);
  void                                changeDimension(double dimension);
  void                                setSorting(int index);  // set sorting type
  void                                setOverlay(int index);
  void                                rotateX(double theta);
  void                                rotateY(double theta);
  void                                rotateZ(double theta);
  void                                rotate(ublas::vector<double> dir, double theta);
  void                                applyRotation(); //apply transformation to antpart from rotation and translation
  void                                translate(double x, double y, double z);
  void                                applyTranslation(double x, double y, double z);
  void                                setPossibleCrossovers();
  void                                setCrossovers();

private:
  //setting up visual model
  virtual void												initAtomVs(); //creates arrays for atom 
  virtual void												initAtomEs(); //creates arrays for bonds
  virtual void                        init1DSorting();
  virtual void												createBindingRegions(); //compute the binding regions from the base pairings
  virtual void                        checkConsecutiveGs(int num);  // check the number of consecutive Gs in staples

  virtual void												configure(); //configure the options for the visual model

  //methods for displaying visual models
  virtual void												display(); // Displays the visual model
  virtual void												displayScale0to1(double iv, bool forSelection = false); // scale 0 -> 1:  transition from the atoms and bonds in the licorice representation to thick bond representation
  virtual void												displayScale1to2(double iv, bool forSelection = false); // scale 1 -> 2: transition from thick bond to atoms without bonds
  virtual void												displayScale2to3(double iv, bool forSelection = false); // scale 2 -> 3: transition from atoms to nucleotides along the backbone (a tube appears along the backbone)
  virtual void												displayScale3to4(double iv, bool forSelection = false); // scale 3 -> 4: transition nucleotides from backbone to sidechain
  virtual void												displayScale4to5(double iv, bool forSelection = false); // scale 4 -> 5: transition scaffold to single strand representation
  virtual void												displayScale5to6(double iv, bool forSelection = false); // scale 5 -> 6: transition staple to single strand representation
  virtual void												displayScale7to8(double iv, bool forSelection = false); // scale 6 -> 7: transition single strands from sidechain to backbone
  virtual void												displayScale6to7(double iv, bool forSelection = false); // scale 7 -> 8: transition single strands to duplex representation
  virtual void												displayScale8to9(double iv, bool forSelection = false); // scale 8 -> 9: transition from duplex to polyhedron
  virtual void												displayScale9(bool forSelection = false); //scale 9: display polyhedron 
  virtual void                        interpolateDimension(SBPosition3 pos1D, SBPosition3 pos2D, SBPosition3 pos3D, ANTAuxiliary::ANTArray<float> & positions, int index);
  virtual void                        interpolateAtomVPosDimension(ANTAuxiliary::ANTArray<float> & positions, int index);
  virtual void                        interpolateAtomEPosDimension(ANTAuxiliary::ANTArray<float> & positions, int index);
  virtual void												sort1DPos(); //
  virtual void												make1DVertical(SBPosition3 & pos1D); //
  virtual void                        highlightStrands(ANTAuxiliary::ANTArray<float> & colorsV, ANTAuxiliary::ANTArray<float> & colorsE, int index, ANTNucleotide* n);
  virtual void                        highlightAtoms(ANTAuxiliary::ANTArray<float> & colorsV, ANTAuxiliary::ANTArray<float> & colorsE, int aIndex, int nIndex, ANTNucleotide* n);

  virtual ANTAuxiliary::ANTArray<unsigned int> getNucleotideIndices();

  virtual void												displayOverlay(); //display meta information for a nucleotide
  virtual void												displayText(SBPosition3 pos, ANTNucleotide * n); //displays labels for nucleotides (A, T, C, G)
  virtual void												displayText(SBPosition3 pos, string text); //displays labels for nucleotides (A, T, C, G)
  virtual void												displayBasePairConnections(float scale); //displays the connection between two paired bases
  virtual void                        displayPossibleCrossovers(bool selection = false); //display the connections between nucleotides, where a crossover could be formed
  virtual void                        displayNucleotideBasis(); //display the basis vectors for the nucleotides for debugging purposes
  virtual void                        displaySkips(float scale, float dimension);

  ANTAuxiliary::ANTArray<float>       getPropertyColor(float min, float max, float val);
  virtual void												expandBounds(SBIAPosition3& bounds) const; // Expands the bounds to make sure the visual model fits inside them
  virtual void												collectAmbientOcclusion(const SBPosition3& boxOrigin, const SBPosition3& boxSize, unsigned int nCellsX, unsigned int nCellsY, unsigned int nCellsZ, float* ambientOcclusionData);		// To collect ambient occlusion data
  virtual float                       getNucleotideRadius(ANTNucleotide* nt);

  virtual void												onBaseEvent(SBBaseEvent* baseEvent); // Handles base events
  virtual void												onDocumentEvent(SBDocumentEvent* documentEvent);						// Handles document events
  virtual void												onStructuralEvent(SBStructuralEvent* documentEvent);					// Handles structural events
  virtual void												displayForShadow();														// Displays the visual model for shadow purposes
  virtual void												displayForSelection();													// Displays the visual model for selection purposes

  enum Scale {
    ALL_ATOMS_LINES = 0,
    ALL_ATOMS_STICKS = 1,
    ALL_ATOMS_BALLS = 2,
    NUCLEOTIDES_BACKBONE = 3,
    NUCLEOTIDES_SIDECHAIN = 4,
    NUCLEOTIDES_SCAFFOLD = 5,
    STAPLES_SCAFFOLD_PLAITING_SIDECHAIN = 6,
    STAPLES_SCAFFOLD_PLAITING_BACKBONE = 7,
    DOUBLE_HELIX_PATH = 8,
    EDGES_VERTICES = 9,
    OBJECTS = 10
  };

  typedef	MSVModel::MSVBasePairList MSVBasePairList;
  typedef	MSVModel::MSVSingleStrandList MSVSingleStrandList;
  typedef ANTNucleotideList MSVNucleotideList;

  //Controls
  bool showMeltingTemperature_ = false;
  bool showGibbsFreeEnergy_ = false;
  bool showConsecutivesGs_ = false;
  int oligo_conc_;
  int mv_;
  int dv_;
  ANTAuxiliary::SortingType sorting_index_ = ANTAuxiliary::StrandID;

  //Configurations
  SEConfig *                                  configuration_;
  bool														            allScalesInitialized_;
  double										                  scale_ = 6.0f; //scales with interpolations
  double                                      dimension_ = 3.0f;
  bool                                        strandsHighlighted_ = false;

  //SB Objects
  SBPointerIndexer<SBAtom>										atomIndexer_;
  SBPointerIndexer<SBNode>										bondIndexer_;
  SBPointerIndexer<SBNode>										residueIndexer_;
  SBPointerIndexer<SBNode>										chainIndexer_;

  MSVModel *                                  model_ = nullptr;

  //this needs to be removed in the future when atoms will not be rendering in the visual model anymore but instead use directly samson
  //Atoms
  unsigned int numAtomV_;
  unsigned int numAtomE_;
  ANTAuxiliary::ANTArray<float>               atomVPositions3D_;
  ANTAuxiliary::ANTArray<float>               atomVPositions2D_;
  ANTAuxiliary::ANTArray<float>               atomVPositions1D_;
  ANTAuxiliary::ANTArray<float>               atomVRadii_;
  ANTAuxiliary::ANTArray<float>               atomVColorData_;
  ANTAuxiliary::ANTArray<unsigned int>        atomVFlagData_;
  ANTAuxiliary::ANTArray<unsigned int>        atomVNodeIndexData_;
  ANTAuxiliary::ANTArray<float>               atomEPositions_;
  ANTAuxiliary::ANTArray<float>               atomEPositions2D_; //todo, inefficent
  ANTAuxiliary::ANTArray<float>               atomEPositions1D_; //todo, inefficent
  ANTAuxiliary::ANTArray<unsigned int>        atomEIndexData_;
  ANTAuxiliary::ANTArray<float>               atomEColorData_;
  ANTAuxiliary::ANTArray<unsigned int>        atomEFlagData_;

  ANTAuxiliary::ANTArray<float>               atomNucleotideVColorData_; //use for speeding up fps in transition from atoms to nucleotides
  
  //Binding Regions
  ANTAuxiliary::ANTArray<float>               propertyNucleotideColorsV_;
  ANTAuxiliary::ANTArray<float>               propertyJointColorsV_;

  //Labels
  std::vector<string>							            overlayTexts_;
  std::vector<SBPosition3>				            overlayPositions_;
  float *													            overlayTextColor_;

  // maps to keep track of nucleotides

  typedef boost::bimap<SBAtom*, ANTAtom*> AtomMap;
  AtomMap atomMap_;
  typedef boost::bimap<unsigned int, SBAtom*> AtomIndices;  // we need to use sbAtom here because we don't have ANTAtoms
  AtomIndices atomIndices_;                                 // when using placeholders like Einstenium or Fermium

  // sorting maps
  std::vector<std::pair<ANTSingleStrand*, double>> gcSortValues_;
  std::vector<std::pair<ANTSingleStrand*, int>> lengthSortValues_;
  std::vector<std::pair<ANTSingleStrand*, double>> meltingTempSortValues_;
  std::vector<std::pair<ANTSingleStrand*, double>> gibbsEnergSortValues_;
  std::vector<std::pair<ANTNucleotide*, ANTNucleotide*>> crossovers_;
  std::vector<std::pair<ANTNucleotide*, ANTNucleotide*>> possibleCrossovers_;

  std::map<ANTSingleStrand*, int> lengthMap_;
  std::map<ANTSingleStrand*, int> gcMap_;
  std::map<ANTSingleStrand*, int> meltingTempMap_;
  std::map<ANTSingleStrand*, int> gibbsEnergMap_;

  // for transformations using editors
  ublas::matrix<double> rotMat_;
  
};

SB_REGISTER_TARGET_TYPE(SENucleotidesVisualModel, "SENucleotidesVisualModel", "4379472F-8DB9-EA85-ED84-98FD92EE76B3");
SB_DECLARE_BASE_TYPE(SENucleotidesVisualModel, SBMVisualModel);
