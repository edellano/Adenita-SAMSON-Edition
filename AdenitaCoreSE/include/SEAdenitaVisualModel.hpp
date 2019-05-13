#pragma once 

#include "SBMVisualModel.hpp"
#include "SBGApp.hpp" 
#include "SEAdenitaCoreSEApp.hpp"
#include "ADNDisplayHelper.hpp"

#include "SBBaseEvent.hpp"
#include "SBDocumentEvent.hpp"
#include "SBStructuralEvent.hpp"
#include "ADNAuxiliary.hpp"
#include "ADNArray.hpp"
#include "ADNMixins.hpp"
#include "ADNPart.hpp"
#include "ADNNanorobot.hpp"
#include "SEConfig.hpp"
#include "MSVColors.hpp"
#include "PIPrimer3.hpp"
#include <QOpenGLFunctions_4_3_Core>


/// This class implements a visual model

class SEAdenitaVisualModel : public SBMVisualModel {

	SB_CLASS

public :

  enum Scale {
    ATOMS_STICKS = 0,
    ATOMS_BALLS = 1,
    NUCLEOTIDES = 2,
    SINGLE_STRANDS = 3,
    DOUBLE_STRANDS = 4,
    OBJECTS = 5
  };

	/// \name Constructors and destructors
	//@{

	SEAdenitaVisualModel();																													///< Builds a visual model					
	SEAdenitaVisualModel(const SBNodeIndexer& nodeIndexer);																					///< Builds a visual model 
	virtual ~SEAdenitaVisualModel();																											///< Destructs the visual model

	//@}

	/// \name Serialization
	//@{

	virtual bool												isSerializable() const;													///< Returns true when the class is serializable

	virtual void												serialize(SBCSerializer* serializer, const SBNodeIndexer& nodeIndexer, const SBVersionNumber& sdkVersionNumber = SB_SDK_VERSION_NUMBER, const SBVersionNumber& classVersionNumber = SBVersionNumber(1, 0, 0)) const;		///< Serializes the node
	virtual void												unserialize(SBCSerializer* serializer, const SBNodeIndexer& nodeIndexer, const SBVersionNumber& sdkVersionNumber = SB_SDK_VERSION_NUMBER, const SBVersionNumber& classVersionNumber = SBVersionNumber(1, 0, 0));			///< Unserializes the node

	//@}

	/// \name Topology
	//@{

	virtual	void												eraseImplementation();													///< Erases the visual model

	//@}

	/// \name Rendering
	//@{
  float       												getScale();																
  void												        changeScaleDiscrete(int scale, bool createIndex = true);																///< Displays the visual model
  void												        changeScale(float scale, bool createIndex = true);																///< Displays the visual model
  void												        changeDimension(float dimension);																///< Displays the visual model
  void												        changeVisibility(double layer);																///< Displays the visual model
  void                                changePropertyColors(int propertyIdx, int colorSchemeIdx);
  void                                changeHighlight(int highlightIdx);
  void                                setSingleStrandColors(int index);
  void                                setNucleotideColors(int index);
  void                                setDoubleStrandColors(int index);
	void                                update();
	void                                setHighlightMinLen(unsigned int min);
	void                                setHighlightMaxLen(unsigned int max);

  virtual void												display();																///< Displays the visual model
  virtual void												displayForShadow();														///< Displays the visual model for shadow purposes
	virtual void												displayForSelection();													///< Displays the visual model for selection purposes

  virtual void                        highlightNucleotides();
	virtual void												expandBounds(SBIAPosition3& bounds) const;								///< Expands the bounds to make sure the visual model fits inside them
	virtual void												collectAmbientOcclusion(const SBPosition3& boxOrigin, const SBPosition3& boxSize, unsigned int nCellsX, unsigned int nCellsY, unsigned int nCellsZ, float* ambientOcclusionData);		///< To collect ambient occlusion data

	//@}

	/// \name Events
	//@{

	virtual void												onBaseEvent(SBBaseEvent* baseEvent);									///< Handles base events
	virtual void												onDocumentEvent(SBDocumentEvent* documentEvent);						///< Handles document events
	virtual void												onStructuralEvent(SBStructuralEvent* documentEvent);					///< Handles structural events
  void                                showBasePairing(bool show);

	//@}

private:
  void                                init();
  void												        initAtoms(bool createIndex = true);
  void												        initNucleotidesAndSingleStrands(bool createIndex = true);
  void												        initDoubleStrands(bool createIndex = true);
  void                                initDisplayIndices();
  ADNArray<unsigned int>              getAtomIndices();
  ADNArray<unsigned int>              getNucleotideIndices();
  ADNArray<unsigned int>              getBaseSegmentIndices();
  void												        changeHighlightFlag(); //scale 9: display polyhedron 
  SEAdenitaCoreSEApp*					        getAdenitaApp() const;															///< Returns a pointer to the app
  void                                orderVisibility();
  void                                setupPropertyColors();
  ADNArray<float>                     calcPropertyColor(int colorSchemeIdx, float min, float max, float val);
  void                                displayBasePairConnections(bool onlySelected);
  void                                displayForDebugging();
  void                                displayCircularDNAConnection();
  void                                displayTags();

  void												        prepareAtoms();
  void												        prepareNucleotides();
  void												        prepareSingleStrands();
  void												        prepareDoubleStrands();
  void												        displayNucleotides(bool forSelection = false);
  void												        displaySingleStrands(bool forSelection = false);
  void												        displayDoubleStrands(bool forSelection = false);
  void												        prepareDiscreteScalesDim();
  void												        prepareDimensions();
  void												        displayTransition(bool forSelection); 
  void												        prepareSticksToBalls(double iv);
  void												        prepareBallsToNucleotides(double iv);
  void												        prepareNucleotidesToSingleStrands(double iv);
  void												        prepareSingleStrandsToDoubleStrands(double iv);
  void												        prepareDoubleStrandsToObjects(double iv);
  void                                prepare1Dto2D(double iv);
  void                                prepare2Dto3D(double iv);
  void                                prepare3D(double iv);
  void                                emphasizeColors(ADNArray<float> & colors, vector<unsigned int> & indices, float r, float g, float b, float a);
  void                                replaceColors(ADNArray<float> & colors, vector<unsigned int> & indices, float * color);
  // general display properties 
  ADNArray<float> nucleotideEColor_;
  
  float scale_ = 3.0f;
  float dim_ = 3.0f;

  ADNNanorobot * nanorobot_;

  //transitional scale
  unsigned int nPositions_;
  unsigned int nCylinders_;
  ADNArray<float> colorsV_;
  ADNArray<float> colorsE_;
  ADNArray<float> positions_;
  ADNArray<float> radiiV_;
  ADNArray<float> radiiE_;
  ADNArray<unsigned int> flags_;
  ADNArray<unsigned int> nodeIndices_;
  ADNArray<unsigned int> indices_;

  //atom scale
  unsigned int nPositionsAtom_;
  unsigned int nCylindersAtom_;
  ADNArray<float> colorsVAtom_;
  ADNArray<float> colorsEAtom_;
  ADNArray<float> positionsAtom_;
  ADNArray<float> radiiVAtom_;
  ADNArray<float> radiiEAtom_;
  ADNArray<unsigned int> flagsAtom_;
  ADNArray<unsigned int> nodeIndicesAtom_;
  ADNArray<unsigned int> indicesAtom_;

  //nucleotide scale
  unsigned int nPositionsNt_;
  unsigned int nCylindersNt_;
  ADNArray<float> colorsVNt_;
  ADNArray<float> colorsENt_;
  ADNArray<float> positionsNt_;
  ADNArray<float> radiiVNt_;
  ADNArray<float> radiiENt_;
  ADNArray<unsigned int> flagsNt_;
  ADNArray<unsigned int> nodeIndicesNt_;
  ADNArray<unsigned int> indicesNt_;

  //single strand scale
  ADNArray<float> colorsVSS_;
  ADNArray<float> colorsESS_;
  ADNArray<float> radiiVSS_;
  ADNArray<float> radiiESS_;

  //double strand scale
  unsigned int nPositionsDS_;
  unsigned int nCylindersDS_;
  ADNArray<float> colorsVDS_;
  ADNArray<float> positionsDS_;
  ADNArray<float> radiiVDS_;
  ADNArray<unsigned int> flagsDS_;
  ADNArray<unsigned int> nodeIndicesDS_;

  //2D
  ADNArray<float> positionsNt2D_;
  ADNArray<float> positionsNt1D_;

  std::map<ADNAtom*, unsigned int> atomMap_;
  std::map<ADNNucleotide*, unsigned int> ntMap_;
  std::map<ADNBaseSegment*, unsigned int> bsMap_;
  map<unsigned int, unsigned> atomNtIndexMap_;
  map<unsigned int, unsigned> ntBsIndexMap_;

  map<ADNNucleotide*, float> sortedNucleotidesByDist_;
  map<ADNSingleStrand*, float> sortedSingleStrandsByDist_;
 
  // current arrays for being displayed (only spheres and cylinders)

  vector<ADNArray<float>> propertyColorSchemes_;
  
  enum ColorType {
    REGULAR = 0,  // default color map
    MELTTEMP = 1,  // melting temperatures color map
    GIBBS = 2  // gibbs free energy color map
  };

  ColorType curColorType_ = REGULAR;

  std::map<ColorType, MSVColors*> colors_;

  enum HighlightType {
    NONE = 0,
    CROSSOVERS = 1,
    GC = 2,
    TAGGED = 3,
		LENGTH = 4,
		NOBASE = 5
  };

  HighlightType highlightType_ = NONE;
  bool showBasePairing_ = false;
  unsigned int highlightMinLen_ = 0;
  unsigned int highlightMaxLen_ = UINT_MAX;
};


SB_REGISTER_TARGET_TYPE(SEAdenitaVisualModel, "SEAdenitaVisualModel", "315CE7EA-009E-CE93-E00C-E921865953AB");
SB_DECLARE_BASE_TYPE(SEAdenitaVisualModel, SBMVisualModel);
