#include "SEAdenitaVisualModel.hpp"
#include "SAMSON.hpp"
#include "ADNLogger.hpp"


SEAdenitaVisualModel::SEAdenitaVisualModel() {

  // SAMSON Element generator pro tip: this default constructor is called when unserializing the node, so it should perform all default initializations.
	

}

SEAdenitaVisualModel::SEAdenitaVisualModel(const SBNodeIndexer& nodeIndexer) {

	// SAMSON Element generator pro tip: implement this function if you want your visual model to be applied to a set of data graph nodes.
	// You might want to connect to various signals and handle the corresponding events. For example, if your visual model represents a sphere positioned at
	// the center of mass of a group of atoms, you might want to connect to the atoms' base signals (e.g. to update the center of mass when an atom is erased) and
	// the atoms' structural signals (e.g. to update the center of mass when an atom is moved).

  SEAdenitaCoreSEApp* app = getAdenitaApp();
  nanorobot_ = app->GetNanorobot();

  SEConfig& config = SEConfig::GetInstance();

  //setup the display properties
  nucleotideEColor_ = ADNArray<float>(4);
  nucleotideEColor_(0) = config.nucleotide_E_Color[0];
  nucleotideEColor_(1) = config.nucleotide_E_Color[1];
  nucleotideEColor_(2) = config.nucleotide_E_Color[2];
  nucleotideEColor_(3) = config.nucleotide_E_Color[3];

  adenineColor_ = ADNArray<float>(4);
  adenineColor_(0) = config.adenine_color[0];
  adenineColor_(1) = config.adenine_color[1];
  adenineColor_(2) = config.adenine_color[2];
  adenineColor_(3) = config.adenine_color[3];

  thymineColor_ = ADNArray<float>(4);
  thymineColor_(0) = config.thymine_color[0];
  thymineColor_(1) = config.thymine_color[1];
  thymineColor_(2) = config.thymine_color[2];
  thymineColor_(3) = config.thymine_color[3];

  cytosineColor_ = ADNArray<float>(4);
  cytosineColor_(0) = config.cytosine_color[0];
  cytosineColor_(1) = config.cytosine_color[1];
  cytosineColor_(2) = config.cytosine_color[2];
  cytosineColor_(3) = config.cytosine_color[3];

  guanineColor_ = ADNArray<float>(4);
  guanineColor_(0) = config.guanine_color[0];
  guanineColor_(1) = config.guanine_color[1];
  guanineColor_(2) = config.guanine_color[2];
  guanineColor_(3) = config.guanine_color[3];


  SBNodeIndexer residues;
  SB_FOR(SBNode* node, nodeIndexer)
    node->getNodes(residues, SBNode::IsType(SBNode::Residue));

  SB_FOR(SBNode* node, residues) {
    SBResidue* residue = static_cast<SBResidue*>(node);

    //connect signals
    residue->connectBaseSignalToSlot(
      this,
      SB_SLOT(&SEAdenitaVisualModel::onBaseEvent));

    residue->connectStructuralSignalToSlot(
      this, // the pointer to the app
      SB_SLOT(&SEAdenitaVisualModel::onStructuralEvent) // the slot
      );


  }




  changeScale(6);
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

void SEAdenitaVisualModel::changeScale(double scale)
{
  scale_ = scale;

  prepareArraysForDisplay();

  SAMSON::requestViewportUpdate();
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
  auto singleStrands = nanorobot_->GetSingleStrands();
  unsigned int nPositions = nanorobot_->GetNumberOfNucleotides();
  unsigned int nCylinders = boost::numeric_cast<unsigned int>(nPositions - singleStrands.size());

  ADNArray<unsigned int> indices = ADNArray<unsigned int>(nCylinders * 2);
  
  std::map<ADNNucleotide*, unsigned int> ntMap;

  unsigned int index = 0;
  //this init can be optimized in the future
  SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
    auto nucleotides = ss->GetNucleotides();

    SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {

      ntMap.insert(make_pair(nt(), index));
      ++index;
    }
  }

  size_t sumNumEdges = 0;

  SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
   
    auto nucleotides = ss->GetNucleotides();

    ADNPointer<ADNNucleotide> cur = ss->GetFivePrime();

    size_t curNCylinders = nucleotides.size() - 1;
    ADNArray<unsigned int> curIndices = ADNArray<unsigned int>(2 * curNCylinders);

    unsigned int j = 0;

    while (cur->GetNext() != nullptr) {

      unsigned int curIndex;
      //nucleotides.getIndex(cur(), curIndex);
      curIndex = ntMap[cur()];

      unsigned int nextIndex;
      //nucleotides.getIndex(cur->GetNext()(), nextIndex);
      nextIndex = ntMap[cur->GetNext()()];

      curIndices(2 * j) = curIndex;
      curIndices(2 * j + 1) = nextIndex;
      j++;

      cur = cur->GetNext();
    }

    for (int k = 0; k < curNCylinders * 2; ++k) {
      indices(sumNumEdges + k) = curIndices(k);
    }

    sumNumEdges += (2 * curNCylinders);

  }

  return indices;

}

void SEAdenitaVisualModel::prepareArraysForDisplay()
{

  SEConfig& config = SEConfig::GetInstance();

  if (nanorobot_ == nullptr) return;
  
  //if (!allScalesInitialized_) {
  //  return;
  //}

  //if (config.detect_possible_crossovers && config.display_possible_crossovers) {
  //  displayPossibleCrossovers();
  //}

  //if (config.display_nucleotide_basis) {
  //  displayNucleotideBasis();
  //  return;
  //}

  //scale_ += animation_step_size;

  if (scale_ > MAX_SCALE) scale_ = MAX_SCALE;

  float interpolated = 1.0f - (ceil(scale_) - scale_);

  if (scale_ < (float)ALL_ATOMS_STICKS) {
    //0 - 9
    prepareScale0to1(interpolated);
    
  }
  else if (scale_ < (float)ALL_ATOMS_BALLS) {
    //10 - 19
    //displayScale1to2(interpolated);
  }
  else if (scale_ < (float)NUCLEOTIDES_BACKBONE) {
    //20 - 29
    //if (config.show_atomic_scales) {
    //  prepareScale2to3(interpolated);
    //}
  }
  else if (scale_ < (float)NUCLEOTIDES_SIDECHAIN) {
    //30 - 39
    prepareScale3to4(interpolated);
    //if (config.display_base_pairing) displayBasePairConnections(scale_);
  }
  else if (scale_ < (float)NUCLEOTIDES_SCAFFOLD) {
    //40 - 49
    prepareScale4to5(interpolated);
    //if (config.display_base_pairing) displayBasePairConnections(scale_);
    //displaySkips(scale_, dimension_);
  }
  else if (scale_ < (float)STAPLES_SCAFFOLD_PLAITING_SIDECHAIN) {
    //50 - 59
    prepareScale5to6(interpolated);
    //if (config.display_base_pairing) displayBasePairConnections(scale_);
    //displaySkips(scale_, dimension_);
  }
  else if (scale_ < (float)STAPLES_SCAFFOLD_PLAITING_BACKBONE) {
    //60 - 69
    prepareScale6to7(interpolated);
    //if (config.display_base_pairing) displayBasePairConnections(scale_);
  }
  else if (scale_ < (float)DOUBLE_HELIX_PATH) {
    //70 - 79
    prepareScale7to8(interpolated); //todo
    //if (config.display_base_pairing) displayBasePairConnections(scale_);

  }
  else if (scale_ < (float)EDGES_VERTICES) {
    //80 - 89
    prepareScale8to9(interpolated);
  }
  else if (scale_ < (float)OBJECTS) {
    prepareScale9();
  }
  else {
  }

}

void SEAdenitaVisualModel::prepareScale0to1(double iv, bool forSelection /*= false*/)
{
  initArraysForDisplay(0, 0);
}

void SEAdenitaVisualModel::prepareScale1to2(double iv, bool forSelection /*= false*/)
{

}

void SEAdenitaVisualModel::prepareScale2to3(double iv, bool forSelection /*= false*/)
{

}

void SEAdenitaVisualModel::prepareScale3to4(double iv, bool forSelection /*= false*/)
{
  SEConfig& config = SEConfig::GetInstance();

  auto singleStrands = nanorobot_->GetSingleStrands();

  unsigned int nPositions = nanorobot_->GetNumberOfNucleotides();
  unsigned int nCylinders = boost::numeric_cast<unsigned int>(nPositions - singleStrands.size());

  initArraysForDisplay(nPositions, nCylinders);

  unsigned int index = 0;

  SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
    
    if (ss->isVisible()) {

      auto nucleotides = ss->GetNucleotides();

      SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {

        float minX = nt->GetBackbonePosition()[0].getValue();
        float minY = nt->GetBackbonePosition()[1].getValue();
        float minZ = nt->GetBackbonePosition()[2].getValue();
        
        float maxX = nt->GetSidechainPosition()[0].getValue();
        float maxY = nt->GetSidechainPosition()[1].getValue();
        float maxZ = nt->GetSidechainPosition()[2].getValue();

        positions_(index, 0) = minX + iv * (maxX - minX);
        positions_(index, 1) = minY + iv * (maxY - minY);
        positions_(index, 2) = minZ + iv * (maxZ - minZ);

        nodeIndices_(index) = nt->getNodeIndex();
        flags_(index) = nt->getInheritedFlags() | getInheritedFlags();
        auto baseColor = getBaseColor(nt->getResidueType());
        colorsV_.SetRow(index, baseColor);

        colorsE_.SetRow(index, nucleotideEColor_);

        radiiV_(index) = config.nucleotide_V_radius;
        radiiE_(index) = config.nucleotide_E_radius;

        SBPosition3 pos3D = nt->GetPosition();
        /*SBPosition3 pos2D = nucleotide->GetSBPosition2D();
        SBPosition3 pos1D = nucleotide->GetSBPosition1D();*/

        //if (configuration_->interpolate_dimensions) interpolateDimension(pos1D, pos2D, pos3D, positions_, index);

        /*if (configuration_->show_nucleobase_text) {
          SBPosition3 curPos = SBPosition3(SBQuantity::picometer(positions_(index, 0)),
            SBQuantity::picometer(positions_(index, 1)),
            SBQuantity::picometer(positions_(index, 2))
            );
          displayText(curPos, nucleotide);
        }

        highlightStrands(colorsV_, colorsV_, index, nucleotide);*/
        ++index;
      }
    }
  }
}

void SEAdenitaVisualModel::prepareScale4to5(double iv, bool forSelection /*= false*/)
{

}

void SEAdenitaVisualModel::prepareScale5to6(double iv, bool forSelection /*= false*/)
{

}

void SEAdenitaVisualModel::prepareScale6to7(double iv, bool forSelection)
{
  SEConfig& config = SEConfig::GetInstance();

  auto singleStrands = nanorobot_->GetSingleStrands();

  unsigned int nPositions = nanorobot_->GetNumberOfNucleotides();
  unsigned int nCylinders = boost::numeric_cast<unsigned int>(nPositions - singleStrands.size());

  initArraysForDisplay(nPositions, nCylinders);

  unsigned int index = 0;

  SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {

    if (ss->isVisible()) {

      auto nucleotides = ss->GetNucleotides();

      SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) { 
        
        radiiV_(index) = config.nucleotide_V_radius;

        radiiE_(index) = config.nucleotide_V_radius;

        flags_(index) = nt->getInheritedFlags() | getInheritedFlags();
        nodeIndices_(index) = nt->getNodeIndex();

        SBPosition3 min = nt->GetSidechainPosition();
        SBPosition3 max = nt->GetBackbonePosition();

        SBPosition3 iPos = min + iv * (max - min);

        positions_(index, 0) = iPos[0].getValue();
        positions_(index, 1) = iPos[1].getValue();
        positions_(index, 2) = iPos[2].getValue();

        SBPosition3 pos3D = nt->GetPosition();
        //SBPosition3 pos2D = nucleotide->GetSBPosition2D();
        //SBPosition3 pos1D = nucleotide->GetSBPosition1D();

        //if (config.interpolate_dimensions) interpolateDimension(pos1D, pos2D, pos3D, positions_, index);

        float minVColorR;
        float minVColorG;
        float minVColorB;
        float minVColorA;

        int stapleColorNum = ss->getNodeIndex() % config.num_staple_colors;

        float maxVColorR = config.staple_colors[stapleColorNum * 4 + 0];
        float maxVColorG = config.staple_colors[stapleColorNum * 4 + 1];
        float maxVColorB = config.staple_colors[stapleColorNum * 4 + 2];
        float maxVColorA = config.staple_colors[stapleColorNum * 4 + 3];

        if (ss->IsScaffold())
        {
          minVColorR = config.nucleotide_E_Color[0];
          minVColorG = config.nucleotide_E_Color[1];
          minVColorB = config.nucleotide_E_Color[2];
          minVColorA = config.nucleotide_E_Color[3];

          maxVColorR = config.nucleotide_E_Color[0];
          maxVColorG = config.nucleotide_E_Color[1];
          maxVColorB = config.nucleotide_E_Color[2];
          maxVColorA = config.nucleotide_E_Color[3];

          colorsE_(index, 0) = config.nucleotide_E_Color[0];
          colorsE_(index, 1) = config.nucleotide_E_Color[1];
          colorsE_(index, 2) = config.nucleotide_E_Color[2];
          colorsE_(index, 3) = config.nucleotide_E_Color[3];

        }
        else
        {
          minVColorR = maxVColorR;
          minVColorG = maxVColorG;
          minVColorB = maxVColorB;
          minVColorA = maxVColorA;

          colorsE_(index, 0) = maxVColorR;
          colorsE_(index, 1) = maxVColorG;
          colorsE_(index, 2) = maxVColorB;
          colorsE_(index, 3) = maxVColorA;
        }

        colorsV_(index, 0) = minVColorR + iv * (maxVColorR - minVColorR);
        colorsV_(index, 1) = minVColorG + iv * (maxVColorG - minVColorG);
        colorsV_(index, 2) = minVColorB + iv * (maxVColorB - minVColorB);
        colorsV_(index, 3) = minVColorA + iv * (maxVColorA - minVColorA);

        //highlightStrands(colorsV_, colorsE_, index, nucleotide);

        //strand direction
        if (nt->GetEnd() == End::ThreePrime) {
          radiiE_(index) = config.nucleotide_E_radius;
        }

        //nt = nt->GetNext();
        ++index;
      }
    }
  }

}

void SEAdenitaVisualModel::prepareScale7to8(double iv, bool forSelection /*= false*/)
{

}

void SEAdenitaVisualModel::prepareScale8to9(double iv, bool forSelection /*= false*/)
{

}

void SEAdenitaVisualModel::prepareScale9(bool forSelection /*= false*/)
{

}

SEAdenitaCoreSEApp* SEAdenitaVisualModel::getAdenitaApp() const
{
  return static_cast<SEAdenitaCoreSEApp*>(SAMSON::getApp(SBCContainerUUID("85DB7CE6-AE36-0CF1-7195-4A5DF69B1528"), SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));
}

ADNArray<float> SEAdenitaVisualModel::getBaseColor(SBResidue::ResidueType baseSymbol)
{
  ADNArray<float> color;

  if (baseSymbol == SBResidue::ResidueType::DA) {
    //adenine
    color = adenineColor_;
  }
  else if (baseSymbol == SBResidue::ResidueType::DT) {
    //thymine
    color = thymineColor_;
  }
  else if (baseSymbol == SBResidue::ResidueType::DG) {
    //guanine
    color = guanineColor_;
  }
  else if (baseSymbol == SBResidue::ResidueType::DC) {
    //cytosine
    color = cytosineColor_;
  }
  else {
    color = nucleotideEColor_;
  }

  return color;
}

void SEAdenitaVisualModel::display() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop. This is the main function of your visual model. 
	// Implement this function to display things in SAMSON, for example thanks to the utility functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)

  if (nanorobot_ == nullptr) return;

  SAMSON::displaySpheres(
    nPositions_,
    positions_.GetArray(),
    radiiV_.GetArray(),
    colorsV_.GetArray(),
    flags_.GetArray());

  if (nCylinders_ > 0) {
    SAMSON::displayCylinders(
      nCylinders_,
      nPositions_,
      indices_.GetArray(),
      positions_.GetArray(),
      radiiE_.GetArray(),
      nullptr,
      colorsE_.GetArray(),
      flags_.GetArray());
  }

  //if (configuration_->display_base_pairing) {
  //  displayBasePairConnections(scale_);
  //}
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

  if (nanorobot_ == nullptr) return;

  SAMSON::displaySpheresSelection(
    nPositions_,
    positions_.GetArray(),
    radiiV_.GetArray(),
    nodeIndices_.GetArray()
    );
  if (nCylinders_ > 0) {
    SAMSON::displayCylindersSelection(
      nCylinders_,
      nPositions_,
      indices_.GetArray(),
      positions_.GetArray(),
      radiiE_.GetArray(),
      nullptr,
      nodeIndices_.GetArray());
  }
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

  if (baseEvent->getType() == SBBaseEvent::SelectionFlagChanged || baseEvent->getType() == SBBaseEvent::HighlightingFlagChanged) {
    //flags have to be changed
    changeScale(scale_);
  }
}

void SEAdenitaVisualModel::onDocumentEvent(SBDocumentEvent* documentEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle document events 

}

void SEAdenitaVisualModel::onStructuralEvent(SBStructuralEvent* documentEvent) {
	
	// SAMSON Element generator pro tip: implement this function if you need to handle structural events (e.g. when a structural node for which you provide a visual representation is updated)

}

