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
  virtual void												prepareScale6to7(double iv, bool forSelection = false); // scale 7 -> 8: transition single strands to duplex representation
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
