#pragma once 

#include "SBMVisualModel.hpp"
#include "SBGApp.hpp" 
#include "SEAdenitaCoreSEApp.hpp"

#include "SBBaseEvent.hpp"
#include "SBDocumentEvent.hpp"
#include "SBStructuralEvent.hpp"
#include "ADNAuxiliary.hpp"
#include "ADNArray.hpp"
#include "ADNMixins.hpp"
#include "ADNPart.hpp"
#include "ADNNanorobot.hpp"
#include "SEConfig.hpp"


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
    NUCLEOTIDES_SCAFFOLD = 5,
    STAPLES_SCAFFOLD_PLAITING_SIDECHAIN = 6,
    STAPLES_SCAFFOLD_PLAITING_BACKBONE = 7,
    DOUBLE_HELIX_PATH = 8,
    EDGES_VERTICES = 9,
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
  virtual void												changeScale(double scale);																///< Displays the visual model
	
  virtual void												display();																///< Displays the visual model
	virtual void												displayForShadow();														///< Displays the visual model for shadow purposes
	virtual void												displayForSelection();													///< Displays the visual model for selection purposes

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
  virtual void												initArraysForDisplay(unsigned int nPositions, unsigned int nCylinders); // Clear the arrays for displaying 
  virtual ADNArray<unsigned int>      getNucleotideIndices();
  virtual void												prepareArraysForDisplay(); // Prepare the arrays for displaying (this separates the interpolation from display)
  virtual void												prepareScale0to1(double iv, bool forSelection = false); // scale 0 -> 1: depicting licorice to sticks representation
  virtual void												prepareScale1to2(double iv, bool forSelection = false); // scale 1 -> 2: transition from thick bond to atoms without bonds
  virtual void												prepareScale2to3(double iv, bool forSelection = false); // scale 2 -> 3: transition from atoms to nucleotides along the backbone (a tube appears along the backbone)
  virtual void												prepareScale3to4(double iv, bool forSelection = false); // scale 3 -> 4: transition nucleotides from backbone to sidechain
  virtual void												prepareScale4to5(double iv, bool forSelection = false); // scale 4 -> 5: transition scaffold to single strand representation
  virtual void												prepareScale5to6(double iv, bool forSelection = false); // scale 5 -> 6: transition staple to single strand representation
  virtual void												prepareScale6to7(double iv, bool forSelection = false); // scale 6 -> 7: transition single strands to duplex representation
  virtual void												prepareScale7to8(double iv, bool forSelection = false); // scale 7 -> 8: transition single strands from sidechain to backbone
  virtual void												prepareScale8to9(double iv, bool forSelection = false); // scale 8 -> 9: transition from duplex to polyhedron
  virtual void												prepareScale9(bool forSelection = false); //scale 9: display polyhedron 
  SEAdenitaCoreSEApp*									getAdenitaApp() const;															///< Returns a pointer to the app

  double scale_;

  ADNNanorobot * nanorobot_;

  // current arrays for being displayed (only spheres and cylinders)
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

};


SB_REGISTER_TARGET_TYPE(SEAdenitaVisualModel, "SEAdenitaVisualModel", "315CE7EA-009E-CE93-E00C-E921865953AB");
SB_DECLARE_BASE_TYPE(SEAdenitaVisualModel, SBMVisualModel);
