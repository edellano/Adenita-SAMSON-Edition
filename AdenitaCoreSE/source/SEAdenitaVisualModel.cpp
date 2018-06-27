#include "SEAdenitaVisualModel.hpp"
#include "SAMSON.hpp"


SEAdenitaVisualModel::SEAdenitaVisualModel() {

  // SAMSON Element generator pro tip: this default constructor is called when unserializing the node, so it should perform all default initializations.
	

}

SEAdenitaVisualModel::SEAdenitaVisualModel(const SBNodeIndexer& nodeIndexer) {

	// SAMSON Element generator pro tip: implement this function if you want your visual model to be applied to a set of data graph nodes.
	// You might want to connect to various signals and handle the corresponding events. For example, if your visual model represents a sphere positioned at
	// the center of mass of a group of atoms, you might want to connect to the atoms' base signals (e.g. to update the center of mass when an atom is erased) and
	// the atoms' structural signals (e.g. to update the center of mass when an atom is moved).

  SBDocument* doc = SAMSON::getActiveDocument();
  SBNodeIndexer nodes;
  doc->getNodes(nodes, (SBNode::GetClass() == std::string("ADNPart")) && (SBNode::GetElementUUID() == SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));

  ADNPointer<ADNPart> part = nullptr;

  SB_FOR(SBNode* node, nodes) {
    if (node->isSelected()) {
      part = static_cast<ADNPart*>(node);
    }
  }
  
}

SEAdenitaVisualModel::~SEAdenitaVisualModel() {

	// SAMSON Element generator pro tip: disconnect from signals you might have connected to.

}

 bool SEAdenitaVisualModel::isSerializable() const {

	// SAMSON Element generator pro tip: serialization is used in SAMSON to e.g. save documents, copy nodes, etc. 
	// Please refer to the SDK documentation for more information.
	// Modify the line below to "return true;" if you want this visual model be serializable (hence copyable, savable, etc.)

	return false;
	
}

void SEAdenitaVisualModel::serialize(SBCSerializer* serializer, const SBNodeIndexer& nodeIndexer, const SBVersionNumber& sdkVersionNumber, const SBVersionNumber& classVersionNumber) const {

	SBMVisualModel::serialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);

	// SAMSON Element generator pro tip: serialization is used in SAMSON to e.g. save documents, copy nodes, etc. 
	// Please refer to the SDK documentation for more information.
	// Complete this function to serialize your visual model.

}

void SEAdenitaVisualModel::unserialize(SBCSerializer* serializer, const SBNodeIndexer& nodeIndexer, const SBVersionNumber& sdkVersionNumber, const SBVersionNumber& classVersionNumber) {

	SBMVisualModel::unserialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);
	
	// SAMSON Element generator pro tip: serialization is used in SAMSON to e.g. save documents, copy nodes, etc. 
	// Please refer to the SDK documentation for more information.
	// Complete this function to unserialize your visual model.

}

void SEAdenitaVisualModel::eraseImplementation() {

	// SAMSON Element generator pro tip: modify this function when you need to perform special tasks when your visual model is erased (e.g. disconnect from nodes you are connected to).
	// Important: this function must be undoable (i.e. only call undoable functions or add an undo command to the undo stack)

}

void SEAdenitaVisualModel::initArraysForDisplay(unsigned int nPositions, unsigned int nCylinders)
{
  nPositions_ = nPositions;
  nCylinders_ = nCylinders;
  positions_ = ADNArray<float>(3, nPositions);
  radiiV_ = ADNArray<float>(nPositions);
  radiiE_ = ADNArray<float>(nPositions);
  colorsV_ = ADNArray<float>(4, nPositions);
  colorsE_ = ADNArray<float>(4, nPositions);
  flags_ = ADNArray<unsigned int>(nPositions);
  nodeIndices_ = ADNArray<unsigned int>(nPositions);
  indices_ = getNucleotideIndices();

}

ADNArray<unsigned int> SEAdenitaVisualModel::getNucleotideIndices()
{
  //auto singleStrands = model_->nanorobot_->GetSingleStrands();
  //unsigned int nPositions = model_->nanorobot_->GetNumberOfNucleotides();
  //unsigned int nCylinders = boost::numeric_cast<unsigned int>(nPositions - singleStrands.size());

  //ADNArray<unsigned int> indices = ADNArray<unsigned int>(nCylinders * 2);

  //std::map<ANTNucleotide*, unsigned int> ntMap;

  //unsigned int index = 0;
  ////this init can be optimized in the future
  //for (auto & sid : singleStrands) {
  //  ANTSingleStrand* singleStrand = sid.second;
  //  MSVNucleotideList nucleotides = singleStrand->nucleotides_;

  //  for (auto & nid : nucleotides) {
  //    ANTNucleotide* nt = nid.second;

  //    ntMap.insert(make_pair(nt, index));
  //    ++index;
  //  }
  //}

  //size_t sumNumEdges = 0;

  //for (auto & sid : singleStrands) {
  //  ANTSingleStrand* singleStrand = sid.second;
  //  MSVNucleotideList nucleotides = singleStrand->nucleotides_;

  //  ANTNucleotide* cur = singleStrand->fivePrime_;

  //  size_t curNCylinders = nucleotides.size() - 1;
  //  ADNArray<unsigned int> curIndices = ADNArray<unsigned int>(2 * curNCylinders);

  //  unsigned int j = 0;

  //  while (cur->next_ != nullptr) {

  //    unsigned int curIndex = ntMap[cur];
  //    unsigned int nextIndex = ntMap[cur->next_];
  //    curIndices(2 * j) = curIndex;
  //    curIndices(2 * j + 1) = nextIndex;
  //    j++;

  //    cur = cur->next_;
  //  }

  //  for (int k = 0; k < curNCylinders * 2; ++k) {
  //    indices(sumNumEdges + k) = curIndices(k);
  //  }

  //  sumNumEdges += (2 * curNCylinders);

  //}

  //return indices;

  ADNArray<unsigned int> asdf;
  return asdf;
}

void SEAdenitaVisualModel::prepareArraysForDisplay()
{
  //SEConfig& config = SEConfig::GetInstance();

  ////bool show_nucleobase_text = config.show_nucleobase_text;
  //bool show_overlay = config.show_overlay;
  //int animation_step_size = config.animation_step_size;
  //float min_melting_temp = config.min_melting_temp;


  //if (model_ == nullptr) return;

  //if (!allScalesInitialized_) {
  //  return;
  //}

  //if (show_overlay) displayOverlay();

  //if (config.detect_possible_crossovers && config.display_possible_crossovers) {
  //  displayPossibleCrossovers();
  //}

  //if (config.display_nucleotide_basis) {
  //  displayNucleotideBasis();
  //  return;
  //}

  //scale_ += animation_step_size;

  //if (scale_ > 9.9) scale_ = 9.9;

  //float interpolated = 1.0f - (ceil(scale_) - scale_);

  //if (scale_ < (float)ALL_ATOMS_STICKS) {
  //  //0 - 9
  //  if (config.show_atomic_scales) {
  //    displayScale0to1(interpolated);
  //  }
  //}
  //else if (scale_ < (float)ALL_ATOMS_BALLS) {
  //  //10 - 19
  //  if (config.show_atomic_scales) {
  //    displayScale1to2(interpolated);
  //  }
  //}
  //else if (scale_ < (float)NUCLEOTIDES_BACKBONE) {
  //  //20 - 29
  //  if (config.show_atomic_scales) {
  //    prepareScale2to3(interpolated);
  //  }
  //}
  //else if (scale_ < (float)NUCLEOTIDES_SIDECHAIN) {
  //  //30 - 39
  //  prepareScale3to4(interpolated);
  //  if (config.display_base_pairing) displayBasePairConnections(scale_);
  //  //displaySkips(scale_, dimension_);
  //}
  //else if (scale_ < (float)NUCLEOTIDES_SCAFFOLD) {
  //  //40 - 49
  //  prepareScale4to5(interpolated);
  //  if (config.display_base_pairing) displayBasePairConnections(scale_);
  //  //displaySkips(scale_, dimension_);
  //}
  //else if (scale_ < (float)STAPLES_SCAFFOLD_PLAITING_SIDECHAIN) {
  //  //50 - 59
  //  prepareScale5to6(interpolated);
  //  if (config.display_base_pairing) displayBasePairConnections(scale_);
  //  //displaySkips(scale_, dimension_);
  //}
  //else if (scale_ < (float)STAPLES_SCAFFOLD_PLAITING_BACKBONE) {
  //  //60 - 69
  //  prepareScale6to7(interpolated);
  //  if (config.display_base_pairing) displayBasePairConnections(scale_);
  //  //displaySkips(scale_, dimension_);
  //}
  //else if (scale_ < (float)DOUBLE_HELIX_PATH) {
  //  //70 - 79
  //  prepareScale7to8(interpolated); //todo
  //  if (config.display_base_pairing) displayBasePairConnections(scale_);

  //}
  //else if (scale_ < (float)EDGES_VERTICES) {
  //  //80 - 89
  //  prepareScale8to9(interpolated);
  //}
  //else if (scale_ < (float)OBJECTS) {
  //  prepareScale9();
  //}
  //else {
  //}

}

void SEAdenitaVisualModel::prepareScale6to7(double iv, bool forSelection /*= false*/)
{

  //auto singleStrands = model_->nanorobot_->GetSingleStrands();
  //auto nucleotideEColor1 = model_->nucleotideEColor1_;

  //unsigned int nPositions = model_->nanorobot_->GetNumberOfNucleotides();
  //unsigned int nCylinders = boost::numeric_cast<unsigned int>(nPositions - singleStrands.size());

  //initArraysForDisplay(nPositions, nCylinders);

  //unsigned int index = 0;

  //for (auto & sid : singleStrands) {
  //  ANTSingleStrand* singleStrand = sid.second;

  //  if (singleStrand->chain_->isVisible()) {

  //    MSVNucleotideList nucleotides = singleStrand->nucleotides_;

  //    for (auto & nit : nucleotides) {
  //      ANTNucleotide* nucleotide = nit.second;

  //      radiiV_(index) = model_->nucleotideVRadius_;

  //      radiiE_(index) = model_->nucleotideVRadius_;

  //      flags_(index) = nucleotide->residue_->getInheritedFlags() | getInheritedFlags();
  //      nodeIndices_(index) = nucleotide->residue_->getNodeIndex();

  //      SBPosition3 min = nucleotide->GetSBSidechainCenter();
  //      SBPosition3 max = nucleotide->GetSBBackboneCenter();

  //      SBPosition3 iPos = min + iv * (max - min);

  //      positions_(index, 0) = iPos[0].getValue();
  //      positions_(index, 1) = iPos[1].getValue();
  //      positions_(index, 2) = iPos[2].getValue();

  //      SBPosition3 pos3D = nucleotide->GetSBPosition(config.use_twist);
  //      SBPosition3 pos2D = nucleotide->GetSBPosition2D();
  //      SBPosition3 pos1D = nucleotide->GetSBPosition1D();

  //      if (config.interpolate_dimensions) interpolateDimension(pos1D, pos2D, pos3D, positions_, index);

  //      if (config.show_nucleobase_text) {
  //        SBPosition3 curPos = SBPosition3(SBQuantity::picometer(positions_(index, 0)),
  //          SBQuantity::picometer(positions_(index, 1)),
  //          SBQuantity::picometer(positions_(index, 2))
  //          );
  //        displayText(curPos, nucleotide);
  //      }

  //      float minVColorR;
  //      float minVColorG;
  //      float minVColorB;
  //      float minVColorA;

  //      int stapleColorNum = singleStrand->id_ % model_->numStapleColors_;

  //      float maxVColorR = model_->stapleColors_(stapleColorNum, 0);
  //      float maxVColorG = model_->stapleColors_(stapleColorNum, 1);
  //      float maxVColorB = model_->stapleColors_(stapleColorNum, 2);
  //      float maxVColorA = model_->stapleColors_(stapleColorNum, 3);

  //      if (singleStrand->isScaffold_)
  //      {
  //        if (showMeltingTemperature_ || showGibbsFreeEnergy_) {
  //          minVColorR = propertyNucleotideColorsV_(index, 0);
  //          minVColorG = propertyNucleotideColorsV_(index, 1);
  //          minVColorB = propertyNucleotideColorsV_(index, 2);
  //          minVColorA = propertyNucleotideColorsV_(index, 3);

  //          maxVColorR = propertyNucleotideColorsV_(index, 0);
  //          maxVColorG = propertyNucleotideColorsV_(index, 1);
  //          maxVColorB = propertyNucleotideColorsV_(index, 2);
  //          maxVColorA = propertyNucleotideColorsV_(index, 3);

  //          colorsE_(index, 0) = propertyNucleotideColorsV_(index, 0);
  //          colorsE_(index, 1) = propertyNucleotideColorsV_(index, 1);
  //          colorsE_(index, 2) = propertyNucleotideColorsV_(index, 2);
  //          colorsE_(index, 3) = propertyNucleotideColorsV_(index, 3);
  //        }
  //        else {
  //          minVColorR = nucleotideEColor1(0);
  //          minVColorG = nucleotideEColor1(1);
  //          minVColorB = nucleotideEColor1(2);
  //          minVColorA = nucleotideEColor1(3);

  //          maxVColorR = nucleotideEColor1(0);
  //          maxVColorG = nucleotideEColor1(1);
  //          maxVColorB = nucleotideEColor1(2);
  //          maxVColorA = nucleotideEColor1(3);

  //          colorsE_(index, 0) = nucleotideEColor1(0);
  //          colorsE_(index, 1) = nucleotideEColor1(1);
  //          colorsE_(index, 2) = nucleotideEColor1(2);
  //          colorsE_(index, 3) = nucleotideEColor1(3);

  //        }
  //      }
  //      else
  //      {
  //        minVColorR = maxVColorR;
  //        minVColorG = maxVColorG;
  //        minVColorB = maxVColorB;
  //        minVColorA = maxVColorA;

  //        colorsE_(index, 0) = maxVColorR;
  //        colorsE_(index, 1) = maxVColorG;
  //        colorsE_(index, 2) = maxVColorB;
  //        colorsE_(index, 3) = maxVColorA;

  //        if (showMeltingTemperature_ || showGibbsFreeEnergy_) {
  //          minVColorR = propertyNucleotideColorsV_(index, 0);
  //          minVColorG = propertyNucleotideColorsV_(index, 1);
  //          minVColorB = propertyNucleotideColorsV_(index, 2);
  //          minVColorA = propertyNucleotideColorsV_(index, 3);

  //          maxVColorR = propertyNucleotideColorsV_(index, 0);
  //          maxVColorG = propertyNucleotideColorsV_(index, 1);
  //          maxVColorB = propertyNucleotideColorsV_(index, 2);
  //          maxVColorA = propertyNucleotideColorsV_(index, 3);

  //          colorsE_(index, 0) = propertyNucleotideColorsV_(index, 0);
  //          colorsE_(index, 1) = propertyNucleotideColorsV_(index, 1);
  //          colorsE_(index, 2) = propertyNucleotideColorsV_(index, 2);
  //          colorsE_(index, 3) = propertyNucleotideColorsV_(index, 3);
  //        }
  //      }

  //      colorsV_(index, 0) = minVColorR + iv * (maxVColorR - minVColorR);
  //      colorsV_(index, 1) = minVColorG + iv * (maxVColorG - minVColorG);
  //      colorsV_(index, 2) = minVColorB + iv * (maxVColorB - minVColorB);
  //      colorsV_(index, 3) = minVColorA + iv * (maxVColorA - minVColorA);

  //      highlightStrands(colorsV_, colorsE_, index, nucleotide);

  //      //strand direction
  //      if (nucleotide->end_ == End::ThreePrime) {
  //        radiiE_(index) = model_->nucleotideBaseRadius_;
  //      }

  //      ++index;
  //    }
  //  }
  //}
}

void SEAdenitaVisualModel::display() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop. This is the main function of your visual model. 
	// Implement this function to display things in SAMSON, for example thanks to the utility functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)

}

void SEAdenitaVisualModel::displayForShadow() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop in order to compute shadows. 
	// Implement this function so that your visual model can cast shadows to other objects in SAMSON, for example thanks to the utility
	// functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)

}

void SEAdenitaVisualModel::displayForSelection() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop in order to perform object picking.
	// Instead of rendering colors, your visual model is expected to write the index of a data graph node (obtained with getIndex()).
	// Implement this function so that your visual model can be selected (if you render its own index) or can be used to select other objects (if you render 
	// the other objects' indices), for example thanks to the utility functions provided by SAMSON (e.g. displaySpheresSelection, displayTrianglesSelection, etc.)

}

void SEAdenitaVisualModel::expandBounds(SBIAPosition3& bounds) const {

	// SAMSON Element generator pro tip: this function is called by SAMSON to determine the model's spatial bounds. 
	// When this function returns, the bounds interval vector should contain the visual model. 

}

void SEAdenitaVisualModel::collectAmbientOcclusion(const SBPosition3& boxOrigin, const SBPosition3& boxSize, unsigned int nCellsX, unsigned int nCellsY, unsigned int nCellsZ, float* ambientOcclusionData) {

	// SAMSON Element generator pro tip: this function is called by SAMSON to determine your model's influence on ambient occlusion.
	// Implement this function if you want your visual model to occlude other objects in ambient occlusion calculations.
	//
	// The ambientOcclusionData represents a nCellsX x nCellsY x nCellsZ grid of occlusion densities over the spatial region (boxOrigin, boxSize).
	// If your model represents geometry at position (x, y, z), then the occlusion density in corresponding grid nodes should be increased.
	//
	// Assuming x, y and z are given in length units (SBQuantity::length, SBQuantity::angstrom, etc.), the grid coordinates are:
	// SBQuantity::dimensionless xGrid = nCellsX * (x - boxOrigin.v[0]) / boxSize.v[0];
	// SBQuantity::dimensionless yGrid = nCellsY * (x - boxOrigin.v[1]) / boxSize.v[1];
	// SBQuantity::dimensionless zGrid = nCellsZ * (x - boxOrigin.v[2]) / boxSize.v[2];
	//
	// The corresponding density can be found at ambientOcclusionData[((int)zGrid.getValue() + 0)*nCellsY*nCellsX + ((int)yGrid.getValue() + 0)*nCellsX + ((int)xGrid.getValue() + 0)] (beware of grid bounds).
	// For higher-quality results, the influence of a point can be spread over neighboring grid nodes.

}

void SEAdenitaVisualModel::onBaseEvent(SBBaseEvent* baseEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle base events (e.g. when a node for which you provide a visual representation emits a base signal, such as when it is erased)

}

void SEAdenitaVisualModel::onDocumentEvent(SBDocumentEvent* documentEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle document events 

}

void SEAdenitaVisualModel::onStructuralEvent(SBStructuralEvent* documentEvent) {
	
	// SAMSON Element generator pro tip: implement this function if you need to handle structural events (e.g. when a structural node for which you provide a visual representation is updated)

}

