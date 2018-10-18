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
#include <QOpenGLFunctions_4_3_Core>


/// This class implements a visual model

class SEAdenitaVisualModel : public SBMVisualModel {

	SB_CLASS

public :

  double const MAX_SCALE = 9.0;

  enum Scale {
    ALL_ATOMS_LINES = 0,
    ALL_ATOMS_STICKS = 1,
    ALL_ATOMS_BALLS = 2,
    NUCLEOTIDES_BACKBONE = 3,
    NUCLEOTIDES_SIDECHAIN = 4,
    NUCLEOTIDES_PARALLEL = 5,
    NUCLEOTIDES_SCAFFOLD = 6,
    STAPLES_SCAFFOLD_PLAITING_SIDECHAIN = 7,
    STAPLES_SCAFFOLD_PLAITING_BACKBONE = 8,
    DOUBLE_STRANDS = 9,
    OBJECTS = 10
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
  virtual void												changeDiscreteScale(int scale, bool createIndex = true);																///< Displays the visual model
  virtual void												changeScale(double scale, bool createIndex = true);																///< Displays the visual model
  virtual void												changeVisibility(double layer);																///< Displays the visual model
	
  virtual void												display();																///< Displays the visual model
  virtual void												displayAtomsLines();
  virtual void												displayAtomsSticks();
  virtual void												displayAtomsBalls();
  virtual void												displayNucleotideBackbone();
  virtual void												displayNucleotideSideChain();
  virtual void												displayNucleotideParallel();
  virtual void												displayNucleotideScaffoldPlaiting();
  virtual void												displayPlatingSideChain();
  virtual void												displayPlatingBackbone();
  virtual void												displayDoubleStrands();
  virtual void												displayForShadow();														///< Displays the visual model for shadow purposes
	virtual void												displayForSelection();													///< Displays the visual model for selection purposes
  virtual void                        displayBaseBairConnections(bool onlySelected);
  virtual void                        displayForDebugging();

	virtual void												expandBounds(SBIAPosition3& bounds) const;								///< Expands the bounds to make sure the visual model fits inside them

	virtual void												collectAmbientOcclusion(const SBPosition3& boxOrigin, const SBPosition3& boxSize, unsigned int nCellsX, unsigned int nCellsY, unsigned int nCellsZ, float* ambientOcclusionData);		///< To collect ambient occlusion data

	//@}

	/// \name Events
	//@{

	virtual void												onBaseEvent(SBBaseEvent* baseEvent);									///< Handles base events
	virtual void												onDocumentEvent(SBDocumentEvent* documentEvent);						///< Handles document events
	virtual void												onStructuralEvent(SBStructuralEvent* documentEvent);					///< Handles structural events

	//@}

private:
  void												initNucleotideArraysForDisplay(bool createIndex = true);
  void												initBaseSegmentArraysForDisplay(bool createIndex = true);
  ADNArray<unsigned int>      getNucleotideIndices();
  ADNArray<unsigned int>      getBaseSegmentIndices();
  void												prepareArraysForDisplay(); // Prepare the arrays for displaying (this separates the interpolation from display)
  void												prepareScale0to1(double iv, bool forSelection = false); // scale 0 -> 1: depicting licorice to sticks representation
  void												prepareScale1to2(double iv, bool forSelection = false); // scale 1 -> 2: transition from thick bond to atoms without bonds
  void												prepareScale2to3(double iv, bool forSelection = false); // scale 2 -> 3: transition from atoms to nucleotides along the backbone (a tube appears along the backbone)
  void												prepareScale3to4(double iv, bool forSelection = false); // scale 3 -> 4: transition nucleotides from backbone to sidechain
  void												prepareScale4to5(double iv, bool forSelection = false); // scale 4 -> 5: transition scaffold to single strand representation
  void												prepareScale5to6(double iv, bool forSelection = false); // scale 5 -> 6: transition staple to single strand representation
  void												prepareScale6to7(double iv, bool forSelection = false); // scale 6 -> 7: transition single strands to duplex representation
  void												prepareScale7to8(double iv, bool forSelection = false); // scale 7 -> 8: transition single strands from sidechain to backbone
  void												prepareScale8to9(double iv, bool forSelection = false); // scale 8 -> 9: transition from duplex to polyhedron
  void												prepareScale9(bool forSelection = false); //scale 9: display polyhedron 
  void												highlightFlagChanged(); //scale 9: display polyhedron 
  SEAdenitaCoreSEApp*					getAdenitaApp() const;															///< Returns a pointer to the app
  ADNArray<float>             getBaseColor(SBResidue::ResidueType baseSymbol);
  void                        orderVisibility();

  // general display properties 
  ADNArray<float> nucleotideEColor_;
  ADNArray<float> stapleColors_;
  ADNArray<float> adenineColor_;
  ADNArray<float>	thymineColor_;
  ADNArray<float>	cytosineColor_;
  ADNArray<float>	guanineColor_;
  ADNArray<float> undefinedNucleotideColor_;
  ADNArray<float>	doubleHelixVColor_;
  ADNArray<float>	doubleHelixEColor_;
  ADNArray<float>	basePairingEColor_;

  double scale_;

  ADNNanorobot * nanorobot_;

  map<ADNNucleotide*, float> sortedNucleotidesByDist_;
  map<ADNSingleStrand*, float> sortedSingleStrandsByDist_;
 
  // current arrays for being displayed (only spheres and cylinders)
  unsigned int nPositions_;
  unsigned int nCylinders_;
  ADNArray<float> colorsV_;
  ADNArray<float> colorsE_;
  ADNArray<float> positions_;
  ADNArray<float> radiiV_;
  ADNArray<float> radiiE_;
  ADNArray<unsigned int> capData_;
  ADNArray<unsigned int> flags_;
  ADNArray<unsigned int> nodeIndices_;
  ADNArray<unsigned int> indices_;

  std::map<ADNNucleotide*, unsigned int> ntMap_;

  // new color implementation
  enum ColorType {
    REGULAR = 0,  // default color map
    GIBBS = 1,  // gibbs free energy color map
    MELTTEMP = 2  // melting temperatures color map
  };
  std::map<ColorType, std::shared_ptr<MSVColors>> colors_;
};


SB_REGISTER_TARGET_TYPE(SEAdenitaVisualModel, "SEAdenitaVisualModel", "315CE7EA-009E-CE93-E00C-E921865953AB");
SB_DECLARE_BASE_TYPE(SEAdenitaVisualModel, SBMVisualModel);
