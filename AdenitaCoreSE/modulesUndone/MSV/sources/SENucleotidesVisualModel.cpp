#include "SENucleotidesVisualModel.hpp"
#include "SAMSON.hpp"


SENucleotidesVisualModel::SENucleotidesVisualModel() {

  // SAMSON Element generator pro tip: this default constructor is called when unserializing the node, so it should perform all default initializations.

}

SENucleotidesVisualModel::SENucleotidesVisualModel(const SBNodeIndexer& nodeIndexer) {

  // SAMSON Element generator pro tip: implement this function if you want your visual model to be applied to a set of data graph nodes.
  // You might want to connect to various signals and handle the corresponding events. For example, if your visual model represents a sphere positioned at
  // the center of mass of a group of atoms, you might want to connect to the atoms' base signals (e.g. to update the center of mass when an atom is erased) and
  // the atoms' structural signals (e.g. to update the center of mass when an atom is moved).

  SBNodeIndexer temporaryAtomIndexer;
  SB_FOR(SBNode* node, nodeIndexer)
    node->getNodes(temporaryAtomIndexer, SBNode::IsType(SBNode::Atom));

  SB_FOR(SBNode* node, temporaryAtomIndexer) {
    atomIndexer_.addReferenceTarget(node);
    SBAtom* atom = static_cast<SBAtom*>(node);

    node->setVisibilityFlag(false);

    node->connectBaseSignalToSlot(
      this,
      SB_SLOT(&SENucleotidesVisualModel::onBaseEvent));

    atom->connectStructuralSignalToSlot(
      this, // the pointer to the app
      SB_SLOT(&SENucleotidesVisualModel::onStructuralEvent) // the slot
      );
  }

  SBNodeIndexer temporaryBondIndexer;
  SB_FOR(SBNode* node, nodeIndexer)
    node->getNodes(temporaryBondIndexer, SBNode::IsType(SBNode::Bond));

  SB_FOR(SBNode* node, temporaryBondIndexer) {
    bondIndexer_.addReferenceTarget(node);
    node->setVisibilityFlag(false);
  }

  SBNodeIndexer temporaryResidueIndexer;
  SB_FOR(SBNode* node, nodeIndexer)
    node->getNodes(temporaryResidueIndexer, SBNode::IsType(SBNode::Residue));

  SB_FOR(SBNode* node, temporaryResidueIndexer) {
    SBResidue* residue = static_cast<SBResidue*>(node);

    residue->connectBaseSignalToSlot(
      this,
      SB_SLOT(&SENucleotidesVisualModel::onBaseEvent));

    residue->connectStructuralSignalToSlot(
      this, // the pointer to the app
      SB_SLOT(&SENucleotidesVisualModel::onStructuralEvent) // the slot
      );

    residueIndexer_.addReferenceTarget(node);

  }

  SBNodeIndexer temporaryChainIndexer;
  SB_FOR(SBNode* node, nodeIndexer)
    node->getNodes(temporaryChainIndexer, SBNode::IsType(SBNode::Chain));

  SB_FOR(SBNode* node, temporaryChainIndexer) {
    chainIndexer_.addReferenceTarget(node);
    node->connectBaseSignalToSlot(
      this,
      SB_SLOT(&SENucleotidesVisualModel::onBaseEvent));
    /*node->connectDocumentSignalToSlot(
    this,
    SB_SLOT(&SENucleotidesVisualModel::onDocumentEvent));*/

  }

  configure();
}

void SENucleotidesVisualModel::createThis() //todo rename to initialize
{
  create();
  SAMSON::getActiveLayer()->addChild(this);
  SAMSON::getActiveDocument()->addChild(this);
}

void SENucleotidesVisualModel::initialize() {
  time_t startTime = time(0);

  if (configuration_->use_atomic_details) {
    initAtomVs();
    startTime = ANTAuxiliary::logPassedTime(startTime, "initAtomVs()");
    initAtomEs();
    startTime = ANTAuxiliary::logPassedTime(startTime, "initAtomEs()");
  }

  init1DSorting();
  startTime = ANTAuxiliary::logPassedTime(startTime, "init1DSorting()");
  crossovers_ = DASAlgorithms::DetectCrossovers(*(model_->nanorobot_));
  if (configuration_->detect_possible_crossovers) {
    startTime = ANTAuxiliary::logPassedTime(startTime, "DetectCrossovers()");
    possibleCrossovers_ = DASAlgorithms::DetectPossibleCrossovers(*(model_->nanorobot_), configuration_->crossover_angle_threshold, configuration_->crossover_distance_threshold);

  }
  startTime = ANTAuxiliary::logPassedTime(startTime, "DetectPossibleCrossovers()");

  allScalesInitialized_ = true;

}

void SENucleotidesVisualModel::changeScale(double scale) {
  scale_ = scale;
  SAMSON::requestViewportUpdate();
}

void SENucleotidesVisualModel::bindingRegionPropertiesChanged(int index) {
  ANTAuxiliary::log(QString::number(index));
  if (index == 0) {
    //staples colors
    showMeltingTemperature_ = false;
    showGibbsFreeEnergy_ = false;
    showConsecutivesGs_ = false;
  }
  else if (index == 1) {
    //melting temps
    showMeltingTemperature_ = true;
    showGibbsFreeEnergy_ = false;
    showConsecutivesGs_ = false;
    createBindingRegions();
  }
  else if (index == 2) {
    //gibbs free energy
    showMeltingTemperature_ = false;
    showGibbsFreeEnergy_ = true;
    showConsecutivesGs_ = false;
    createBindingRegions();
  }
  else if (index == 3) {
    // check for >4 consecutive G
    showMeltingTemperature_ = false;
    showGibbsFreeEnergy_ = false;
    showConsecutivesGs_ = true;
    checkConsecutiveGs(4);  // todo: add number of consecutive Gs as setting
  }
}

void SENucleotidesVisualModel::setBindingRegionParameters(int oligo_conc, int mv, int dv) {
  oligo_conc_ = oligo_conc;
  mv_ = mv;
  dv_ = dv;

  createBindingRegions();

  SAMSON::requestViewportUpdate();
}


void SENucleotidesVisualModel::changeOverlay(std::vector<string> texts, std::vector<SBPosition3> positions) {
  overlayTexts_ = texts;
  overlayPositions_ = positions;
}

void SENucleotidesVisualModel::configure() {

  configuration_ = new SEConfig();

  oligo_conc_ = 100;
  mv_ = 5;
  dv_ = 16;

  allScalesInitialized_ = false;
  overlayTextColor_ = new float[4];
  overlayTextColor_[0] = 1.0f;
  overlayTextColor_[1] = 1.0f;
  overlayTextColor_[2] = 1.0f;
  overlayTextColor_[3] = 1.0f;


}

ANTAuxiliary::ANTArray<float> SENucleotidesVisualModel::getPropertyColor(float min, float max, float val) {

  ANTAuxiliary::ANTArray<float> color = ANTAuxiliary::ANTArray<float>(4);

  if (val == FLT_MAX) { //if region is unbound
    color(0) = 1.0f;
    color(1) = 0.0f;
    color(2) = 0.0f;
    color(3) = 1.0f;

    return color;
  }

  unsigned int numColors = model_->numPropertyColors_;
  auto colors = model_->propertyColors_;

  int idx1;
  int idx2;
  float fractBetween = 0;

  //Y = (X - A) / (B - A) * (D - C) + C
  double mappedVal = ANTAuxiliary::mapRange(val, min, max, 0, 1);

  if (mappedVal <= 0) {
    idx1 = idx2 = 0;
  }
  else if (mappedVal >= 1) {
    idx1 = idx2 = numColors - 1;
  }
  else {

    mappedVal = mappedVal * (numColors - 1);
    idx1 = int(mappedVal);
    idx2 = idx1 + 1;
    fractBetween = mappedVal - float(idx1);
  }

  color(0) = (colors(idx2, 0) - colors(idx1, 0)) * fractBetween + colors(idx1, 0);
  color(1) = (colors(idx2, 1) - colors(idx1, 1)) * fractBetween + colors(idx1, 1);
  color(2) = (colors(idx2, 2) - colors(idx1, 2)) * fractBetween + colors(idx1, 2);
  color(3) = 1.0f;

  return color;
}

void SENucleotidesVisualModel::initAtomVs() {
  //std::string msg = " initAtomVs";
  //ANTAuxiliary::log(msg);
  numAtomV_ = atomIndexer_.size();

  atomVPositions3D_ = ANTAuxiliary::ANTArray<float>(3, numAtomV_);
  atomVPositions2D_ = ANTAuxiliary::ANTArray<float>(3, numAtomV_);
  atomVPositions1D_ = ANTAuxiliary::ANTArray<float>(3, numAtomV_);
  atomVRadii_ = ANTAuxiliary::ANTArray<float>(numAtomV_);
  atomVColorData_ = ANTAuxiliary::ANTArray<float>(4, numAtomV_);
  atomVFlagData_ = ANTAuxiliary::ANTArray<unsigned int>(numAtomV_);
  atomVNodeIndexData_ = ANTAuxiliary::ANTArray<unsigned int>(numAtomV_);
  atomNucleotideVColorData_ = ANTAuxiliary::ANTArray<float>(4, numAtomV_);

  auto singleStrands = model_->nanorobot_->GetSingleStrands();

  unsigned int atomIndex = 0;
  for (auto & sid : singleStrands) {
    ANTSingleStrand* singleStrand = sid.second;
    MSVNucleotideList nucleotides = singleStrand->nucleotides_;

    for (auto & nit : nucleotides) {
      ANTNucleotide* nucleotide = nit.second;

      SBNodeIndexer atoms;
      nucleotide->residue_->getNodes(atoms, SBNode::IsType(SBNode::Atom));

      auto nucleotideColor = model_->getBaseColor(nucleotide->type_);

      int atomInNucleotide = 0;
      SB_FOR(SBNode* node, atoms) {

        SBAtom* curSBAtom = static_cast<SBAtom*>(node);
        SBPosition3 position2D = nucleotide->GetSBPosition2D();
        SBPosition3 position1D = nucleotide->GetSBPosition1D();
        if (nucleotide->atomList_.size() > 0) {
          ANTAtom* curANTAtom = static_cast<ANTAtom*>(nucleotide->atomList_[atomInNucleotide]);
          position2D = curANTAtom->position2D_;
          position1D = curANTAtom->position1D_;
          //atommap
          atomMap_.insert(AtomMap::value_type(curSBAtom, curANTAtom));
        }

        SBPosition3 position = curSBAtom->getPosition();

        make1DVertical(position1D);

        atomVPositions3D_(atomIndex, 0) = (float)position.v[0].getValue();
        atomVPositions3D_(atomIndex, 1) = (float)position.v[1].getValue();
        atomVPositions3D_(atomIndex, 2) = (float)position.v[2].getValue();

        atomVPositions2D_(atomIndex, 0) = (float)position2D.v[0].getValue();
        atomVPositions2D_(atomIndex, 1) = (float)position2D.v[1].getValue();
        atomVPositions2D_(atomIndex, 2) = (float)position2D.v[2].getValue();

        atomVPositions1D_(atomIndex, 0) = (float)position1D.v[0].getValue();
        atomVPositions1D_(atomIndex, 1) = (float)position1D.v[1].getValue();
        atomVPositions1D_(atomIndex, 2) = (float)position1D.v[2].getValue();

        atomVRadii_(atomIndex) = (float)curSBAtom->getVanDerWaalsRadius().getValue();

        std::string atomElement = curSBAtom->getElementSymbol();
        auto color = model_->getAtomColor(atomElement);

        atomVColorData_.SetRow(atomIndex, color);

        atomVFlagData_(atomIndex) = curSBAtom->getInheritedFlags() | getInheritedFlags();
        atomVNodeIndexData_(atomIndex) = curSBAtom->getNodeIndex();

        atomNucleotideVColorData_.SetRow(atomIndex, nucleotideColor);

        atomIndices_.insert(AtomIndices::value_type(atomIndex, curSBAtom));

        atomIndex++;
        atomInNucleotide++;
      }

    }
  }

  //msg = " finish initAtomVs";
  //ANTAuxiliary::log(msg);
}

void SENucleotidesVisualModel::initAtomEs()
{
  //some of this is actually with the atomVposition, but its how samson fore
  numAtomE_ = bondIndexer_.size();

  atomEPositions_ = ANTAuxiliary::ANTArray<float>(3, numAtomE_ * 4);
  atomEPositions2D_ = ANTAuxiliary::ANTArray<float>(3, numAtomE_ * 4);
  atomEPositions1D_ = ANTAuxiliary::ANTArray<float>(3, numAtomE_ * 4);
  atomEFlagData_ = ANTAuxiliary::ANTArray<unsigned int>(numAtomE_ * 4);
  atomEIndexData_ = ANTAuxiliary::ANTArray<unsigned int>(numAtomE_ * 4); // before: dim_=2
  atomEColorData_ = ANTAuxiliary::ANTArray<float>(4, numAtomE_ * 4);

  std::vector<unsigned int> indices;


  for (unsigned int i = 0; i < numAtomE_; i++) {

    indices.push_back(i);
    indices.push_back(numAtomE_ + i);

    SBBond* curBond = static_cast<SBBond*>(bondIndexer_[i]);

    SBAtom* leftAtom = curBond->getLeftAtom();

    std::string leftElement = leftAtom->getElementSymbol();

    SBPosition3 leftAtomPosition = leftAtom->getPosition();

    atomEPositions_(i, 0) = (float)leftAtomPosition.v[0].getValue();
    atomEPositions_(i, 1) = (float)leftAtomPosition.v[1].getValue();
    atomEPositions_(i, 2) = (float)leftAtomPosition.v[2].getValue();

    ANTAtom* leftANTAtom = atomMap_.left.at(leftAtom);
    SBPosition3 position2D = leftANTAtom->position2D_;
    atomEPositions2D_(i, 0) = (float)position2D.v[0].getValue();
    atomEPositions2D_(i, 1) = (float)position2D.v[1].getValue();
    atomEPositions2D_(i, 2) = (float)position2D.v[2].getValue();

    SBPosition3 position1D = leftANTAtom->position1D_;
    make1DVertical(position1D);
    atomEPositions1D_(i, 0) = (float)position1D.v[0].getValue();
    atomEPositions1D_(i, 1) = (float)position1D.v[1].getValue();
    atomEPositions1D_(i, 2) = (float)position1D.v[2].getValue();

    auto color = model_->getAtomColor(leftElement);

    atomEColorData_(i, 0) = color(0);
    atomEColorData_(i, 1) = color(1);
    atomEColorData_(i, 2) = color(2);
    atomEColorData_(i, 3) = color(3);

    atomEFlagData_(i) = curBond->getInheritedFlags() | getInheritedFlags();
  }

  for (unsigned int i = numAtomE_; i < numAtomE_ * 2; i++) {
    SBBond* curBond = static_cast<SBBond*>(bondIndexer_[i - numAtomE_]);

    SBAtom* leftAtom = curBond->getLeftAtom();
    SBAtom* rightAtom = curBond->getRightAtom();
    std::string leftElement = leftAtom->getElementSymbol();

    SBPosition3 centerPosition = curBond->getMidPoint();

    atomEPositions_(i, 0) = (float)centerPosition.v[0].getValue();
    atomEPositions_(i, 1) = (float)centerPosition.v[1].getValue();
    atomEPositions_(i, 2) = (float)centerPosition.v[2].getValue();

    ANTAtom* leftANTAtom = atomMap_.left.at(leftAtom);
    ANTAtom* rightANTAtom = atomMap_.left.at(rightAtom);
    SBPosition3 position2D = (leftANTAtom->position2D_ + rightANTAtom->position2D_) / 2;
    atomEPositions2D_(i, 0) = (float)position2D.v[0].getValue();
    atomEPositions2D_(i, 1) = (float)position2D.v[1].getValue();
    atomEPositions2D_(i, 2) = (float)position2D.v[2].getValue();

    SBPosition3 position1D = (leftANTAtom->position1D_ + rightANTAtom->position1D_) / 2;
    make1DVertical(position1D);
    atomEPositions1D_(i, 0) = (float)position1D.v[0].getValue();
    atomEPositions1D_(i, 1) = (float)position1D.v[1].getValue();
    atomEPositions1D_(i, 2) = (float)position1D.v[2].getValue();

    auto color = model_->getAtomColor(leftElement);

    atomEColorData_(i, 0) = color(0);
    atomEColorData_(i, 1) = color(1);
    atomEColorData_(i, 2) = color(2);
    atomEColorData_(i, 3) = color(3);

    atomEFlagData_(i) = curBond->getInheritedFlags() | getInheritedFlags();
  }

  for (unsigned int i = numAtomE_ * 2; i < numAtomE_ * 3; i++) {

    indices.push_back(i);
    indices.push_back(numAtomE_ + i);

    SBBond* curBond = static_cast<SBBond*>(bondIndexer_[i - numAtomE_ * 2]);

    SBAtom* leftAtom = curBond->getLeftAtom();
    SBAtom* rightAtom = curBond->getRightAtom();
    std::string rightElement = rightAtom->getElementSymbol();

    SBPosition3 centerPosition = curBond->getMidPoint();

    atomEPositions_(i, 0) = (float)centerPosition.v[0].getValue();
    atomEPositions_(i, 1) = (float)centerPosition.v[1].getValue();
    atomEPositions_(i, 2) = (float)centerPosition.v[2].getValue();

    auto color = model_->getAtomColor(rightElement);

    ANTAtom* leftANTAtom = atomMap_.left.at(leftAtom);
    ANTAtom* rightANTAtom = atomMap_.left.at(rightAtom);
    SBPosition3 position2D = (leftANTAtom->position2D_ + rightANTAtom->position2D_) / 2;
    atomEPositions2D_(i, 0) = (float)position2D.v[0].getValue();
    atomEPositions2D_(i, 1) = (float)position2D.v[1].getValue();
    atomEPositions2D_(i, 2) = (float)position2D.v[2].getValue();

    SBPosition3 position1D = (leftANTAtom->position1D_ + rightANTAtom->position1D_) / 2;
    make1DVertical(position1D);
    atomEPositions1D_(i, 0) = (float)position1D.v[0].getValue();
    atomEPositions1D_(i, 1) = (float)position1D.v[1].getValue();
    atomEPositions1D_(i, 2) = (float)position1D.v[2].getValue();

    atomEColorData_(i, 0) = color(0);
    atomEColorData_(i, 1) = color(1);
    atomEColorData_(i, 2) = color(2);
    atomEColorData_(i, 3) = color(3);

    atomEFlagData_(i) = curBond->getInheritedFlags() | getInheritedFlags();
  }

  for (unsigned int i = numAtomE_ * 3; i < numAtomE_ * 4; i++) {
    SBBond* curBond = static_cast<SBBond*>(bondIndexer_[i - numAtomE_ * 3]);

    SBAtom* rightAtom = curBond->getRightAtom();

    SBPosition3 rightAtomPosition = rightAtom->getPosition();

    atomEPositions_(i, 0) = (float)rightAtomPosition.v[0].getValue();
    atomEPositions_(i, 1) = (float)rightAtomPosition.v[1].getValue();
    atomEPositions_(i, 2) = (float)rightAtomPosition.v[2].getValue();

    std::string rightElement = rightAtom->getElementSymbol();

    auto color = model_->getAtomColor(rightElement);

    ANTAtom* rightANTAtom = atomMap_.left.at(rightAtom);
    SBPosition3 position2D = rightANTAtom->position2D_;
    atomEPositions2D_(i, 0) = (float)position2D.v[0].getValue();
    atomEPositions2D_(i, 1) = (float)position2D.v[1].getValue();
    atomEPositions2D_(i, 2) = (float)position2D.v[2].getValue();

    SBPosition3 position1D = rightANTAtom->position1D_;
    make1DVertical(position1D);
    atomEPositions1D_(i, 0) = (float)position1D.v[0].getValue();
    atomEPositions1D_(i, 1) = (float)position1D.v[1].getValue();
    atomEPositions1D_(i, 2) = (float)position1D.v[2].getValue();

    atomEColorData_(i, 0) = color(0);
    atomEColorData_(i, 1) = color(1);
    atomEColorData_(i, 2) = color(2);
    atomEColorData_(i, 3) = color(3);

    atomEFlagData_(i) = curBond->getInheritedFlags() | getInheritedFlags();
  }

  for (unsigned int i = 0; i < indices.size(); i++) {
    atomEIndexData_(i) = indices[i];
  }

}

void SENucleotidesVisualModel::createBindingRegions() {

  auto singleStrands = model_->nanorobot_->GetSingleStrands();
  auto parts = model_->nanorobot_->GetParts();

  MSVModel::MSVBindingRegionList bindingRegions;

  for (auto &sid : singleStrands) {
    ANTSingleStrand * singleStrand = sid.second;
    ANTNucleotide* nt = singleStrand->fivePrime_;
    int regionSize = 0;
    unsigned int numRegions = 0;

    MSVBindingRegion * region;
    while (nt != nullptr) {
      if (regionSize == 0) {
        region = new MSVBindingRegion(numRegions);
        bindingRegions.push_back(region);
        ++numRegions;
      }

      bool endOfRegion = true;

      auto * st_cur = nt->pair_;
      auto * sc_next = nt->next_;

      if (sc_next != nullptr && st_cur != nullptr && st_cur->prev_ != nullptr) {
        if (sc_next->pair_ == st_cur->prev_) {
          endOfRegion = false;
        }
      }

      region->nucleotides_.push_back(nt);

      ++regionSize;

      if (endOfRegion) {
        regionSize = 0;
      }
      nt = nt->next_;

    }
  }

  std::map<ANTNucleotide*, float> meltingTempNtMap;
  std::map<ANTNucleotide*, float> gibbsFreeNtMap;

  for (auto bindingRegion : bindingRegions) {

    bindingRegion->initializeSequences();
    bindingRegion->executeNtthal(oligo_conc_, mv_, dv_);

    for (auto & nt : bindingRegion->nucleotides_) {
      meltingTempNtMap.insert(make_pair(nt, bindingRegion->t_));
      gibbsFreeNtMap.insert(make_pair(nt, bindingRegion->dG_));
    }

  }

  float min_t = FLT_MAX;
  float max_t = FLT_MIN;

  float min_dG = FLT_MAX;
  float max_dG = FLT_MIN;

  for (auto bindingRegion : bindingRegions) {
    float t = bindingRegion->t_;
    if (t < min_t) min_t = t;
    if (t > max_t) max_t = t;

    float dG = bindingRegion->dG_;
    if (dG < min_dG) min_dG = dG;
    if (dG > max_dG) max_dG = dG;
  }

  unsigned int nPositions = model_->nanorobot_->GetNumberOfNucleotides();

  propertyNucleotideColorsV_ = ANTAuxiliary::ANTArray<float>(4, nPositions);

  unsigned int index = 0;

  for (auto & sid : singleStrands) {
    ANTSingleStrand* singleStrand = sid.second;
    ANTNucleotideList nucleotides = singleStrand->nucleotides_;

    for (auto & nid : nucleotides) {
      ANTNucleotide * nt = nid.second;
      ANTAuxiliary::ANTArray<float> propertyColor;

      if (showMeltingTemperature_) {
        float val = meltingTempNtMap[nt];
        propertyColor = getPropertyColor(configuration_->min_melting_temp, configuration_->max_melting_temp, val);
      }
      else if (showGibbsFreeEnergy_) {
        float val = gibbsFreeNtMap[nt];
        propertyColor = getPropertyColor(configuration_->min_gibbs_free_energy, configuration_->min_gibbs_free_energy, val);
      }

      propertyNucleotideColorsV_(index, 0) = propertyColor(0);
      propertyNucleotideColorsV_(index, 1) = propertyColor(1);
      propertyNucleotideColorsV_(index, 2) = propertyColor(2);
      propertyNucleotideColorsV_(index, 3) = propertyColor(3);

      ++index;
    }
  }

  propertyJointColorsV_ = ANTAuxiliary::ANTArray<float>(4, model_->nanorobot_->GetNumberOfJoints());
  std::map<ANTJoint*, unsigned int> jointMap;

  unsigned int jid = 0;

  for (auto &part : parts) {
    for (auto const &joint : part.second->GetSegmentJoints()) {
      jointMap.insert(std::make_pair(joint, jid));
      ++jid;
    }

    for (auto const &bid : part.second->GetBaseSegments()) {
      ANTBaseSegment* bs = bid.second;

      ANTNucleotide* nt = bs->GetNucleotide();
      ANTAuxiliary::ANTArray<float> propertyColor;

      unsigned int jid1 = jointMap[bs->source_];
      unsigned int jid2 = jointMap[bs->target_];

      if (showMeltingTemperature_) {
        float val = meltingTempNtMap[nt];
        propertyColor = getPropertyColor(configuration_->min_melting_temp, configuration_->max_melting_temp, val);
      }
      else if (showGibbsFreeEnergy_) {
        float val = gibbsFreeNtMap[nt];
        propertyColor = getPropertyColor(configuration_->min_gibbs_free_energy, configuration_->min_gibbs_free_energy, val);
      }

      propertyJointColorsV_(jid1, 0) = propertyColor(0);
      propertyJointColorsV_(jid1, 1) = propertyColor(1);
      propertyJointColorsV_(jid1, 2) = propertyColor(2);
      propertyJointColorsV_(jid1, 3) = propertyColor(3);

      propertyJointColorsV_(jid2, 0) = propertyColor(0);
      propertyJointColorsV_(jid2, 1) = propertyColor(1);
      propertyJointColorsV_(jid2, 2) = propertyColor(2);
      propertyJointColorsV_(jid2, 3) = propertyColor(3);

    }
  }
}

void SENucleotidesVisualModel::checkConsecutiveGs(int num) {
  auto singleStrands = model_->nanorobot_->GetSingleStrands();
  unsigned int nPositions = model_->nanorobot_->GetNumberOfNucleotides();

  propertyNucleotideColorsV_ = ANTAuxiliary::ANTArray<float>(4, nPositions);

  int counter = 0;
  unsigned int index = 0;
  for (auto &sid : singleStrands) {
    ANTSingleStrand * singleStrand = sid.second;
    ANTNucleotide* nt = singleStrand->fivePrime_;

    DNABlocks prevType = DNABlocks::DG_;
    while (nt != nullptr) {
      DNABlocks type = nt->type_;

      if (singleStrand->isScaffold_ == false) {
        if (type == DNABlocks::DG_ && type == prevType) {
          counter += 1;
        }
        else {
          if (counter > num - 1) {
            // change colors of previous Gs block
            unsigned int k = index - 1;
            for (int j = 0; j <= counter; ++j) {
              // red
              propertyNucleotideColorsV_(k, 0) = 1.0f;
              propertyNucleotideColorsV_(k, 1) = 0.0f;
              propertyNucleotideColorsV_(k, 2) = 0.0f;
              propertyNucleotideColorsV_(k, 3) = 1.0f;

              --k;
            }
          }
          counter = 0;
        }
      }

      // set default color
      ANTAuxiliary::ANTArray<float> propertyColor = model_->getBaseColor(type);

      propertyNucleotideColorsV_(index, 0) = propertyColor(0);
      propertyNucleotideColorsV_(index, 1) = propertyColor(1);
      propertyNucleotideColorsV_(index, 2) = propertyColor(2);
      propertyNucleotideColorsV_(index, 3) = propertyColor(3);

      ++index;
      prevType = type;
      nt = nt->next_;
    }
  }
}

void SENucleotidesVisualModel::init1DSorting() {
  // default sort is regarding single strand id

  std::vector<std::pair<ANTSingleStrand*, double>> gcSort;  // gc content sort
  std::vector<std::pair<ANTSingleStrand*, int>> lengthSort;  // sequence length
                                                             //std::vector<std::pair<ANTSingleStrand*, double>> meltingTempSort;  // melting temperature sort
                                                             //std::vector<std::pair<ANTSingleStrand*, double>> gibbsSort;  // gibbs free energy sort

  auto singleStrands = model_->nanorobot_->GetSingleStrands();
  //MSVModel::MSVBindingRegionList bRegions = model_->bindingRegions_;

  //std::map<ANTSingleStrand*, double> totalMeltingTemp;
  //std::map<ANTSingleStrand*, double> totalGibbsEnerg;

  //for (auto &reg : bindingRegions_) {
  //  std::vector<ANTNucleotide*> nucleotides = reg->nucleotides_;
  //  for (auto &nt : nucleotides) {
  //    ANTSingleStrand* ss = nt->strand_;
  //    totalMeltingTemp[ss] += reg->t_;
  //    totalGibbsEnerg[ss] += reg->dG_;
  //  }
  //}

  for (auto &ss_pair : singleStrands) {
    ANTSingleStrand* ss = ss_pair.second;
    double gcCont = ss->GetGCContent();
    int length = boost::numeric_cast<int>(ss->nucleotides_.size());

    //double meltTemp = totalMeltingTemp[ss] / length;
    //double gibbsEnerg = totalGibbsEnerg[ss] / length;

    gcSort.push_back(std::make_pair(ss, gcCont));
    lengthSort.push_back(std::make_pair(ss, length));
    //meltingTempSort.push_back(std::make_pair(ss, meltTemp));
    //gibbsSort.push_back(std::make_pair(ss, gibbsEnerg));
  }


  // sorting
  sort(gcSort.begin(), gcSort.end(), [=](std::pair<ANTSingleStrand*, double>& a, std::pair<ANTSingleStrand*, double>& b) {
    return a.second < b.second;
  });
  sort(lengthSort.begin(), lengthSort.end(), [=](std::pair<ANTSingleStrand*, int>& a, std::pair<ANTSingleStrand*, int>& b) {
    return a.second < b.second;
  });
  //sort(meltingTempSort.begin(), meltingTempSort.end(), [=](std::pair<ANTSingleStrand*, double>& a, std::pair<ANTSingleStrand*, double>& b) {
  //  return a.second < b.second;
  //});
  //sort(gibbsSort.begin(), gibbsSort.end(), [=](std::pair<ANTSingleStrand*, double>& a, std::pair<ANTSingleStrand*, double>& b) {
  //  return a.second < b.second;  // lowest are related to highest melting temperature
  //});

  gcSortValues_ = gcSort;
  lengthSortValues_ = lengthSort;
  //meltingTempSortValues_ = meltingTempSort;
  //gibbsEnergSortValues_ = gibbsSort;

  int i = 0;
  for (auto & ent : lengthSort) {
    auto singleStrand = ent.first;
    lengthMap_.insert(make_pair(singleStrand, i));
    i++;
  }
  i = 0;
  for (auto & ent : gcSort) {
    auto singleStrand = ent.first;
    gcMap_.insert(make_pair(singleStrand, i));
    i++;
  }
  /*i = 0;
  for (auto & ent : meltingTempSort) {
  auto singleStrand = ent.first;
  meltingTempMap_.insert(make_pair(singleStrand, i));
  i++;
  }
  i = 0;
  for (auto & ent : gibbsSort) {
  auto singleStrand = ent.first;
  gibbsEnergMap_.insert(make_pair(singleStrand, i));
  i++;
  }*/
}

void SENucleotidesVisualModel::changeDimension(double dimension) {
  dimension_ = 4.0f - dimension; //flip dimension
  SAMSON::requestViewportUpdate();
}

void SENucleotidesVisualModel::setSorting(int index) {
  sorting_index_ = ANTAuxiliary::SortingType(index);
  sort1DPos();
}

void SENucleotidesVisualModel::setOverlay(int index) {
  //auto singleStrands = model_->nanorobot_->GetSingleStrands();
  //std::vector<std::pair<ANTNucleotide*, ANTNucleotide*>> crossovers;
  //int crossoverValue = 0;
  //if (index == ANTAuxiliary::OverlayType::Crossovers) {
  //  crossovers = crossovers_;
  //  crossoverValue = 1;
  //}
  //else if (index == ANTAuxiliary::OverlayType::PossibleCrossovers) {
  //  crossovers = possibleCrossovers_;
  //  crossoverValue = 1;
  //}

  //for (auto n : crossovers) {
  //  ANTNucleotide * leftNucleotide = n.first;
  //  ANTNucleotide * rightNucleotide = n.second;

  //  int leftIndex = ntMap_.right.at(leftNucleotide);
  //  int rightIndex = ntMap_.right.at(rightNucleotide);

  //  nucleotideVFlagData_(leftIndex) = crossoverValue;
  //  nucleotideVFlagData_(rightIndex) = crossoverValue;
  //  nucleotideEFlagData_(leftIndex) = crossoverValue;
  //  nucleotideEFlagData_(rightIndex) = crossoverValue;

  //  singleStrandVFlagData_(leftIndex) = crossoverValue;
  //  singleStrandVFlagData_(rightIndex) = crossoverValue;
  //  singleStrandEFlagData_(leftIndex) = crossoverValue;
  //  singleStrandEFlagData_(rightIndex) = crossoverValue;

  //  if (leftNucleotide->strand_->isScaffold_) {
  //    int dhLeftIndex = scaffoldMap_.right.at(leftNucleotide);
  //    doubleHelixVFlagData_(dhLeftIndex) = crossoverValue;
  //    doubleHelixEFlagData_(dhLeftIndex) = crossoverValue;
  //  }
  //  if (rightNucleotide->strand_->isScaffold_) {
  //    int dhRightIndex = scaffoldMap_.right.at(rightNucleotide);
  //    doubleHelixVFlagData_(dhRightIndex) = crossoverValue;
  //    doubleHelixEFlagData_(dhRightIndex) = crossoverValue;
  //  }

  //  //atomic details
  //  if (leftNucleotide->atomList_.size() > 0) {
  //    for (auto &p : leftNucleotide->atomList_) {
  //      ANTAtom* atom = p.second;
  //      SBAtom* sbAtom = atomMap_.right.at(atom);
  //      atomVFlagData_(atomIndices_.right.at(sbAtom)) = crossoverValue;
  //    }
  //  }
  //  if (rightNucleotide->atomList_.size() > 0) {
  //    for (auto &p : rightNucleotide->atomList_) {
  //      ANTAtom* atom = p.second;
  //      SBAtom* sbAtom = atomMap_.right.at(atom);
  //      atomVFlagData_(atomIndices_.right.at(sbAtom)) = crossoverValue;
  //    }
  //  }
  //}
}

void SENucleotidesVisualModel::rotateX(double theta)
{
  ANTAuxiliary::log(string("rotate around x-axis: "), false);
  ANTAuxiliary::log(QString::number(theta));
  ublas::vector<double> baseX(3);
  baseX[0] = 1;
  baseX[1] = 0;
  baseX[2] = 0;

  rotate(baseX, theta);
}

void SENucleotidesVisualModel::rotateY(double theta)
{
  ANTAuxiliary::log(string("rotateY"));
  ANTAuxiliary::log(QString::number(theta));
  ublas::vector<double> baseY(3);
  baseY[0] = 0;
  baseY[1] = 1;
  baseY[2] = 0;

  rotate(baseY, theta);

}

void SENucleotidesVisualModel::rotateZ(double theta)
{
  ANTAuxiliary::log(string("rotateZ"));
  ANTAuxiliary::log(QString::number(theta));
  ublas::vector<double> baseZ(3);
  baseZ[0] = 0;
  baseZ[1] = 0;
  baseZ[2] = 1;

  rotate(baseZ, theta);
}

void SENucleotidesVisualModel::rotate(ublas::vector<double> dir, double theta)
{

  //rotDir_ = dir;
  //rotMat_ = ANTVectorMath::MakeRotationMatrix(dir, theta);
  //rotAngle_ = theta;

  //auto singleStrands = model_->nanorobot_->GetSelectedSingleStrands(); //todo get the selected part

  ////get center
  //SBPosition3 center;
  //int n = 0;
  //for (auto & singleStrand : singleStrands) {
  //  //ANTSingleStrand* singleStrand = sid.second;
  //  MSVNucleotideList nucleotides = singleStrand->nucleotides_;

  //  for (auto & nit : nucleotides) {
  //    ANTNucleotide* nucleotide = nit.second;
  //    center += nucleotide->GetSBPosition();
  //    ++n;
  //  }
  //}

  //center /= n;

  //for (auto & singleStrand : singleStrands) {
  //  //ANTSingleStrand* singleStrand = sid.second;
  //  MSVNucleotideList nucleotides = singleStrand->nucleotides_;

  //  for (auto & nit : nucleotides) {
  //    ANTNucleotide* nucleotide = nit.second;
  //    unsigned int index = ntMap_.right.at(nucleotide);

  //    ublas::vector<double> backbonePos(3);
  //    backbonePos[0] = nucleotide->GetSBBackboneCenter()[0].getValue() - center[0].getValue();
  //    backbonePos[1] = nucleotide->GetSBBackboneCenter()[1].getValue() - center[1].getValue();
  //    backbonePos[2] = nucleotide->GetSBBackboneCenter()[2].getValue() - center[2].getValue();

  //    ublas::vector<double> sideChainPos(3);
  //    sideChainPos[0] = nucleotide->GetSBSidechainCenter()[0].getValue() - center[0].getValue();
  //    sideChainPos[1] = nucleotide->GetSBSidechainCenter()[1].getValue() - center[1].getValue();
  //    sideChainPos[2] = nucleotide->GetSBSidechainCenter()[2].getValue() - center[2].getValue();

  //    ublas::vector<double> newBackbonePos = ublas::prod(rotMat_, backbonePos);
  //    ublas::vector<double> newSideChainPos = ublas::prod(rotMat_, sideChainPos);

  //    //visual model
  //    //refactoring: this has to be deleted since the visual model should work without initialization
  //    nucleotideBackboneVPositions_(index, 0) = newBackbonePos[0] + center[0].getValue();
  //    nucleotideBackboneVPositions_(index, 1) = newBackbonePos[1] + center[1].getValue();
  //    nucleotideBackboneVPositions_(index, 2) = newBackbonePos[2] + center[2].getValue();

  //    nucleotideSideChainVPositions_(index, 0) = newSideChainPos[0] + center[0].getValue();
  //    nucleotideSideChainVPositions_(index, 1) = newSideChainPos[1] + center[1].getValue();
  //    nucleotideSideChainVPositions_(index, 2) = newSideChainPos[2] + center[2].getValue();

  //  }
  //}
}

void SENucleotidesVisualModel::applyRotation()
{
  auto singleStrands = model_->nanorobot_->GetSelectedSingleStrands(); //todo get the selected part
  std::vector<ANTSingleStrand*> scaffs;

  //get center
  SBPosition3 center;

  int n = 0;
  for (auto & singleStrand : singleStrands) {
    //ANTSingleStrand* singleStrand = sid.second;
    MSVNucleotideList nucleotides = singleStrand->nucleotides_;

    for (auto & nit : nucleotides) {
      ANTNucleotide* nucleotide = nit.second;
      center += nucleotide->GetSBPosition();
      ++n;
    }
  }

  center /= n;
  ublas::vector<double> uCenter(3);
  uCenter[0] = center[0].getValue();
  uCenter[1] = center[1].getValue();
  uCenter[2] = center[2].getValue();

  unsigned int atomIndex = 0;
  for (auto & singleStrand : singleStrands) {
    //ANTSingleStrand* singleStrand = sid.second;
    if (singleStrand->isScaffold_) scaffs.push_back(singleStrand);

    MSVNucleotideList nucleotides = singleStrand->nucleotides_;

    for (auto & nit : nucleotides) {
      ANTNucleotide* nucleotide = nit.second;
      ublas::vector<double> backbonePos(3);
      backbonePos[0] = nucleotide->GetSBBackboneCenter()[0].getValue() - center[0].getValue();
      backbonePos[1] = nucleotide->GetSBBackboneCenter()[1].getValue() - center[1].getValue();
      backbonePos[2] = nucleotide->GetSBBackboneCenter()[2].getValue() - center[2].getValue();

      ublas::vector<double> sideChainPos(3);
      sideChainPos[0] = nucleotide->GetSBSidechainCenter()[0].getValue() - center[0].getValue();
      sideChainPos[1] = nucleotide->GetSBSidechainCenter()[1].getValue() - center[1].getValue();
      sideChainPos[2] = nucleotide->GetSBSidechainCenter()[2].getValue() - center[2].getValue();

      ublas::vector<double> nucleotidePos(3);
      nucleotidePos[0] = nucleotide->GetSBPosition()[0].getValue() - center[0].getValue();
      nucleotidePos[1] = nucleotide->GetSBPosition()[1].getValue() - center[1].getValue();
      nucleotidePos[2] = nucleotide->GetSBPosition()[2].getValue() - center[2].getValue();

      ublas::vector<double> newBackbonePos = ublas::prod(rotMat_, backbonePos);
      ublas::vector<double> newSideChainPos = ublas::prod(rotMat_, sideChainPos);
      ublas::vector<double> newNucleotidePos = ublas::prod(rotMat_, nucleotidePos);

      newBackbonePos += uCenter;
      newSideChainPos += uCenter;
      newNucleotidePos += uCenter;

      nucleotide->SetBackboneCenter(newBackbonePos);
      nucleotide->SetSidechainCenter(newSideChainPos);
      nucleotide->SetPosition(newNucleotidePos);

      SBNodeIndexer atoms;
      nucleotide->residue_->getNodes(atoms, SBNode::IsType(SBNode::Atom));

      int atomInNucleotide = 0;
      SB_FOR(SBNode* node, atoms) {
        SBAtom* atom = static_cast<SBAtom*>(node);
        SBPosition3 atomPos = atom->getPosition();
        atomPos -= center;

        ublas::vector<double> newAtomPos(3);
        newAtomPos[0] = atomPos[0].getValue();
        newAtomPos[1] = atomPos[1].getValue();
        newAtomPos[2] = atomPos[2].getValue();

        newAtomPos = ublas::prod(rotMat_, newAtomPos);

        SBPosition3 newSBAtomPos = SBPosition3(SBQuantity::picometer(newAtomPos[0]),
          SBQuantity::picometer(newAtomPos[1]), SBQuantity::picometer(newAtomPos[2]));

        newSBAtomPos += center;

        atom->setPosition(newSBAtomPos);

        atomVPositions3D_(atomIndex, 0) = newSBAtomPos[0].getValue();
        atomVPositions3D_(atomIndex, 1) = newSBAtomPos[1].getValue();
        atomVPositions3D_(atomIndex, 2) = newSBAtomPos[2].getValue();
        atomIndex++;

        if (nucleotide->atomList_.size() > 0) {
          ANTAtom* curANTAtom = static_cast<ANTAtom*>(nucleotide->atomList_[atomInNucleotide]);
          curANTAtom->SetPosition(atom->getPosition());
          atomInNucleotide++;
        }
      }
    }
  }

  auto joints = model_->nanorobot_->GetSegmentJoints();
  for (auto &j : joints) {

    SBPosition3 jointPos = j->position_;

    jointPos -= center;

    ublas::vector<double> newJointPos(3);
    newJointPos[0] = jointPos[0].getValue();
    newJointPos[1] = jointPos[1].getValue();
    newJointPos[2] = jointPos[2].getValue();

    newJointPos = ublas::prod(rotMat_, newJointPos);

    newJointPos += uCenter;

    SBPosition3 newSBJointPos = SBPosition3(SBQuantity::picometer(newJointPos[0]),
      SBQuantity::picometer(newJointPos[1]), SBQuantity::picometer(newJointPos[2]));

    j->position_ = newSBJointPos;

  }

  for (unsigned int i = 0; i < numAtomE_ * 4; i++) {

    ublas::vector<double> atomEPos(3);
    atomEPos[0] = atomEPositions_(i, 0);
    atomEPos[1] = atomEPositions_(i, 1);
    atomEPos[2] = atomEPositions_(i, 2);

    atomEPos -= uCenter;

    atomEPos = ublas::prod(rotMat_, atomEPos);

    atomEPos += uCenter;

    atomEPositions_(i, 0) = atomEPos[0];
    atomEPositions_(i, 1) = atomEPos[1];
    atomEPositions_(i, 2) = atomEPos[2];
  }

}

void SENucleotidesVisualModel::translate(double x, double y, double z)
{

  //auto singleStrands = model_->nanorobot_->GetSelectedSingleStrands();

  //SBPosition3 translation = SBPosition3(SBQuantity::picometer(x * 100),
  //  SBQuantity::picometer(y * 100),
  //  SBQuantity::picometer(z * 100));

  //unsigned int atomIndex = 0;

  //for (auto & singleStrand : singleStrands) {
  //  //ANTSingleStrand* singleStrand = sid.second;
  //  MSVNucleotideList nucleotides = singleStrand->nucleotides_;
  //  
  //  for (auto & nit : nucleotides) {
  //    ANTNucleotide* nucleotide = nit.second;
  //    unsigned int index = ntMap_.right.at(nucleotide);

  //    //visual model
  //    //refactoring: simply delete
  //    nucleotideBackboneVPositions_(index, 0) = (float)nucleotide->GetVectorBackboneCenter()[0] + translation[0].getValue();
  //    nucleotideBackboneVPositions_(index, 1) = (float)nucleotide->GetVectorBackboneCenter()[1] + translation[1].getValue();
  //    nucleotideBackboneVPositions_(index, 2) = (float)nucleotide->GetVectorBackboneCenter()[2] + translation[2].getValue();

  //    nucleotideSideChainVPositions_(index, 0) = (float)nucleotide->GetVectorSidechainCenter()[0] + translation[0].getValue();
  //    nucleotideSideChainVPositions_(index, 1) = (float)nucleotide->GetVectorSidechainCenter()[1] + translation[1].getValue();
  //    nucleotideSideChainVPositions_(index, 2) = (float)nucleotide->GetVectorSidechainCenter()[2] + translation[2].getValue();
  //    
  //    /*if (nucleotide->pair_ != nullptr) {
  //        
  //      SBPosition3 center1 = nucleotide->GetSBPosition();
  //      SBPosition3 center2 = nucleotide->pair_->GetSBPosition();
  //      SBPosition3 doubleHelixCenter = (center1 + center2) / 2;
  //      doubleHelixCenter += translation;
  //      doubleHelixVPositions_.insert(std::make_pair(nucleotide, doubleHelixCenter));
  //    }
  //    else {
  //      SBPosition3 doubleHelixCenter = nucleotide->GetSBSidechainCenter();
  //      doubleHelixCenter += translation;
  //      doubleHelixVPositions_.insert(std::make_pair(nucleotide, doubleHelixCenter));
  //    }

  //    SBNodeIndexer atoms;
  //    nucleotide->residue_->getNodes(atoms, SBNode::IsType(SBNode::Atom));
  //    SB_FOR(SBNode* node, atoms) {
  //      SBAtom* atom = static_cast<SBAtom*>(node);
  //      SBPosition3 atomPos = atom->getPosition();
  //      atomVPositions3D_(atomIndex, 0) = (float)atomPos.v[0].getValue() + translation[0].getValue();
  //      atomVPositions3D_(atomIndex, 1) = (float)atomPos.v[1].getValue() + translation[1].getValue();
  //      atomVPositions3D_(atomIndex, 2) = (float)atomPos.v[2].getValue() + translation[2].getValue();
  //      atomIndex++;
  //    }*/
  //  }
  //}
}

void SENucleotidesVisualModel::setPossibleCrossovers() {
  possibleCrossovers_ = DASAlgorithms::DetectPossibleCrossovers(*(model_->nanorobot_), configuration_->crossover_angle_threshold, configuration_->crossover_distance_threshold);
}

void SENucleotidesVisualModel::setCrossovers() {
  crossovers_ = DASAlgorithms::DetectCrossovers(*(model_->nanorobot_));
}

void SENucleotidesVisualModel::applyTranslation(double x, double y, double z)
{
  auto singleStrands = model_->nanorobot_->GetSelectedSingleStrands(); //todo get the selected part
  std::vector<ANTSingleStrand*> scaffs;

  ANTAuxiliary::log(string("translation"), false);
  ANTAuxiliary::log(QString::number(x), false);
  ANTAuxiliary::log(QString::number(y), false);
  ANTAuxiliary::log(QString::number(z));

  SBPosition3 translation = SBPosition3(SBQuantity::picometer(x * 100),
    SBQuantity::picometer(y * 100),
    SBQuantity::picometer(z * 100));

  int atomIndex = 0;

  for (auto & singleStrand : singleStrands) {
    //ANTSingleStrand* singleStrand = sid.second;

    if (singleStrand->isScaffold_) scaffs.push_back(singleStrand);

    MSVNucleotideList nucleotides = singleStrand->nucleotides_;

    for (auto & nit : nucleotides) {
      ANTNucleotide* nucleotide = nit.second;

      SBPosition3 newBackbonePos = nucleotide->GetSBBackboneCenter() + translation;
      SBPosition3 newSideChainPos = nucleotide->GetSBSidechainCenter() + translation;
      SBPosition3 newNucleotidePos = nucleotide->GetSBPosition() + translation;

      nucleotide->SetBackboneCenter(newBackbonePos);
      nucleotide->SetSidechainCenter(newSideChainPos);
      nucleotide->SetPosition(newNucleotidePos);

      SBNodeIndexer atoms;
      nucleotide->residue_->getNodes(atoms, SBNode::IsType(SBNode::Atom));

      /*     int atomInNucleotide = 0;
      SB_FOR(SBNode* node, atoms) {
      SBAtom* atom = static_cast<SBAtom*>(node);
      SBPosition3 atomPos = atom->getPosition();
      atom->setPosition(atomPos + translation);
      atomVPositions3D_(atomIndex, 0) = (float)atomPos.v[0].getValue() + translation[0].getValue();
      atomVPositions3D_(atomIndex, 1) = (float)atomPos.v[1].getValue() + translation[1].getValue();
      atomVPositions3D_(atomIndex, 2) = (float)atomPos.v[2].getValue() + translation[2].getValue();
      atomIndex++;

      if (nucleotide->atomList_.size() > 0) {
      ANTAtom* curANTAtom = static_cast<ANTAtom*>(nucleotide->atomList_[atomInNucleotide]);
      curANTAtom->SetPosition(atom->getPosition());
      atomInNucleotide++;
      }
      }*/
    }
  }


  //doubleHelixVPositions_.clear();

  //for (auto & singleStrand : scaffs) {

  //    MSVNucleotideList nucleotides = singleStrand->nucleotides_;

  //    for (auto & nit : nucleotides) {
  //      ANTNucleotide* nucleotide = nit.second;

  //      if (nucleotide->pair_ != nullptr) {

  //        SBPosition3 center1 = nucleotide->GetSBPosition();
  //        SBPosition3 center2 = nucleotide->pair_->GetSBPosition();
  //        SBPosition3 doubleHelixCenter = (center1 + center2) / 2;
  //        doubleHelixVPositions_.insert(std::make_pair(nucleotide, doubleHelixCenter));
  //      }
  //      else {
  //        SBPosition3 doubleHelixCenter = nucleotide->GetSBSidechainCenter();
  //        doubleHelixVPositions_.insert(std::make_pair(nucleotide, doubleHelixCenter));
  //      }
  //    }
  //}

  auto joints = model_->nanorobot_->GetSegmentJoints();
  for (auto &j : joints) {
    j->position_ = j->position_ + translation;
  }
  /*
  for (unsigned int i = 0; i < numAtomE_ * 4; i++) {
  atomEPositions_(i, 0) = atomEPositions_(i, 0) + translation[0].getValue();
  atomEPositions_(i, 1) = atomEPositions_(i, 1) + translation[1].getValue();
  atomEPositions_(i, 2) = atomEPositions_(i, 2) + translation[2].getValue();
  }*/
}

SENucleotidesVisualModel::SENucleotidesVisualModel(std::vector<SBDDataGraphNode*>& nodeVector) {

  // SAMSON Element generator pro tip: implement this function if you want your visual model to be applied to ALL_ATOMS_STICKS set of data graph nodes.
  // You might want to connect to various signals and handle the corresponding events. For example, if your visual model represents ALL_ATOMS_STICKS sphere positioned at
  // the center of mass of ALL_ATOMS_STICKS group of atoms, you might want to connect to the atoms' base signals (e.g. to update the center of mass when an atom is erased) and
  // the atoms' structural signals (e.g. to update the center of mass when an atom is moved).

}

SENucleotidesVisualModel::~SENucleotidesVisualModel() {

  // SAMSON Element generator pro tip: disconnect from signals you might have connected to.

  if (model_ != nullptr) delete model_; // binding regions and nts should be deleted along with model

  delete overlayTextColor_;
  //delete configuration_;
}


bool SENucleotidesVisualModel::isSerializable() const {

  // SAMSON Element generator pro tip: serialization is used in SAMSON to e.g. save documents, copy nodes, etc. 
  // Please refer to the SDK documentation for more information.
  // Modify the line below to "return true;" if you want this visual model be serializable (hence copyable, savable, etc.)

  return false;

}

void SENucleotidesVisualModel::serialize(SBCSerializer* serializer, const SBNodeIndexer& nodeIndexer, const SBVersionNumber& sdkVersionNumber, const SBVersionNumber& classVersionNumber) const {

  SBMVisualModel::serialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);

  // SAMSON Element generator pro tip: serialization is used in SAMSON to e.g. save documents, copy nodes, etc. 
  // Please refer to the SDK documentation for more information.
  // Complete this function to serialize your visual model.

}

void SENucleotidesVisualModel::unserialize(SBCSerializer* serializer, const SBNodeIndexer& nodeIndexer, const SBVersionNumber& sdkVersionNumber, const SBVersionNumber& classVersionNumber) {

  SBMVisualModel::unserialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);

  // SAMSON Element generator pro tip: serialization is used in SAMSON to e.g. save documents, copy nodes, etc. 
  // Please refer to the SDK documentation for more information.
  // Complete this function to unserialize your visual model.

}

void SENucleotidesVisualModel::eraseImplementation() {

  // SAMSON Element generator pro tip: modify this function when you need to perform special tasks when your visual model is erased (e.g. disconnect from nodes you are connected to).
  // Important: this function must be undoable (i.e. only call undoable functions or add an undo command to the undo stack)

  // this is a hack to workaround the selection after modification bug
  //ntMap_.clear();
}


void SENucleotidesVisualModel::displayText(SBPosition3 pos, ANTNucleotide * n) {

  string text;
  text.push_back(residue_names_.left.at(n->type_));

  SAMSON::displayText(
    text,
    pos,
    QFont(QString("Helvetica"), 60),
    overlayTextColor_);

}


void SENucleotidesVisualModel::displayScale0to1(double iv, bool forSelection) {
  ANTAuxiliary::ANTArray<float> iAtomVRadii = ANTAuxiliary::ANTArray<float>(numAtomV_);
  ANTAuxiliary::ANTArray<float> iAtomERadii = ANTAuxiliary::ANTArray<float>(numAtomE_ * 4);
  ANTAuxiliary::ANTArray<float> iAtomVPositions = ANTAuxiliary::ANTArray<float>(3, numAtomV_);
  ANTAuxiliary::ANTArray<float> iAtomEPositions = ANTAuxiliary::ANTArray<float>(3, numAtomE_ * 4);

  float minVRadius = model_->atomLineRadius_;
  float maxVRadius = model_->bondBallsRadius_;
  float intervalVRadius = maxVRadius - minVRadius;
  float iVRadius = minVRadius + iv * intervalVRadius;

  float minERadius = model_->atomLineRadius_;
  float maxERadius = model_->bondBallsRadius_;
  float intervalERadius = maxERadius - minERadius;
  float iERadius = minERadius + iv * intervalERadius;

  for (unsigned int i = 0; i < numAtomV_; i++) {
    iAtomVRadii(i) = iVRadius;

    iAtomVPositions(i, 0) = atomVPositions3D_(i, 0);
    iAtomVPositions(i, 1) = atomVPositions3D_(i, 1);
    iAtomVPositions(i, 2) = atomVPositions3D_(i, 2);

    if (configuration_->interpolate_dimensions) interpolateAtomVPosDimension(iAtomVPositions, i);
  }

  for (unsigned int i = 0; i < numAtomE_ * 4; i++) {
    iAtomERadii(i) = iERadius;

    iAtomEPositions(i, 0) = atomEPositions_(i, 0);
    iAtomEPositions(i, 1) = atomEPositions_(i, 1);
    iAtomEPositions(i, 2) = atomEPositions_(i, 2);

    if (configuration_->interpolate_dimensions) interpolateAtomEPosDimension(iAtomEPositions, i);
  }

  if (!forSelection) {
    SAMSON::displaySpheres(
      numAtomV_,
      iAtomVPositions.GetArray(),
      iAtomVRadii.GetArray(),
      atomVColorData_.GetArray(),
      atomVFlagData_.GetArray()
      );

    SAMSON::displayCylinders(
      numAtomE_ * 2, //prolly not correct, todo
      numAtomE_ * 4,
      atomEIndexData_.GetArray(),
      iAtomEPositions.GetArray(),
      iAtomERadii.GetArray(),
      nullptr,
      atomEColorData_.GetArray(),
      atomEFlagData_.GetArray());
  }
  else {
    SAMSON::displaySpheresSelection(
      numAtomV_,
      iAtomVPositions.GetArray(),
      iAtomVRadii.GetArray(),
      atomVNodeIndexData_.GetArray()
      );

    SAMSON::displayCylindersSelection(
      numAtomE_ * 2,
      numAtomE_ * 4,
      atomEIndexData_.GetArray(),
      atomEPositions_.GetArray(),
      iAtomERadii.GetArray(),
      nullptr,
      atomVNodeIndexData_.GetArray());
  }

}


void SENucleotidesVisualModel::displayScale1to2(double iv, bool forSelection) {
  ANTAuxiliary::ANTArray<float> iAtomVRadii = ANTAuxiliary::ANTArray<float>(numAtomV_);
  ANTAuxiliary::ANTArray<float> iAtomERadii = ANTAuxiliary::ANTArray<float>(numAtomE_ * 4); //todo
  ANTAuxiliary::ANTArray<float> iAtomVPositions = ANTAuxiliary::ANTArray<float>(3, numAtomV_);
  ANTAuxiliary::ANTArray<float> iAtomEPositions = ANTAuxiliary::ANTArray<float>(3, numAtomE_ * 4);

  float minERadius = model_->bondBallsRadius_;
  float maxERadius = 0;
  float intervalERadius = maxERadius - minERadius;
  float iERadius = minERadius + iv * intervalERadius;

  for (unsigned int i = 0; i < numAtomV_; i++)
  {
    float minVRadius = model_->atomBallRadius_;
    float maxVRadius = atomVRadii_(i);

    float intervalVRadius = maxVRadius - minVRadius;
    float iVRadius = minVRadius + iv * intervalVRadius;

    iAtomVRadii(i) = iVRadius;

    iAtomVPositions(i, 0) = atomVPositions3D_(i, 0);
    iAtomVPositions(i, 1) = atomVPositions3D_(i, 1);
    iAtomVPositions(i, 2) = atomVPositions3D_(i, 2);

    if (configuration_->interpolate_dimensions) interpolateAtomVPosDimension(iAtomVPositions, i);
  }

  for (unsigned int i = 0; i < numAtomE_ * 4; i++)
  {
    iAtomERadii(i) = iERadius;

    iAtomEPositions(i, 0) = atomEPositions_(i, 0);
    iAtomEPositions(i, 1) = atomEPositions_(i, 1);
    iAtomEPositions(i, 2) = atomEPositions_(i, 2);

    if (configuration_->interpolate_dimensions) interpolateAtomEPosDimension(iAtomEPositions, i);
  }

  if (!forSelection) {
    SAMSON::displaySpheres(
      numAtomV_,
      iAtomVPositions.GetArray(),
      iAtomVRadii.GetArray(),
      atomVColorData_.GetArray(),
      atomVFlagData_.GetArray()
      );

    SAMSON::displayCylinders(
      numAtomE_ * 2,
      numAtomE_ * 4,
      atomEIndexData_.GetArray(),
      iAtomEPositions.GetArray(),
      iAtomERadii.GetArray(),
      nullptr,
      atomEColorData_.GetArray(),
      atomEFlagData_.GetArray());
  }
  else {
    SAMSON::displaySpheresSelection(
      numAtomV_,
      iAtomVPositions.GetArray(),
      iAtomVRadii.GetArray(),
      atomVNodeIndexData_.GetArray()
      );

    SAMSON::displayCylindersSelection(
      numAtomE_ * 2,
      numAtomE_ * 4,
      atomEIndexData_.GetArray(),
      atomEPositions_.GetArray(),
      iAtomERadii.GetArray(),
      nullptr,
      atomVNodeIndexData_.GetArray());
  }

}

void SENucleotidesVisualModel::displayScale2to3(double iv, bool forSelection)
{
  unsigned int numNucleotidesV = model_->nanorobot_->GetNumberOfNucleotides();
  unsigned int numNucleotidesE = boost::numeric_cast<unsigned int>(numNucleotidesV - model_->nanorobot_->GetSingleStrands().size());

  ANTAuxiliary::ANTArray<float> iAtomVPositions = ANTAuxiliary::ANTArray<float>(3, numAtomV_);
  ANTAuxiliary::ANTArray<float> iAtomVColorData = ANTAuxiliary::ANTArray<float>(4, numAtomV_);
  ANTAuxiliary::ANTArray<float> iAtomVRadii = ANTAuxiliary::ANTArray<float>(numAtomV_);

  ANTAuxiliary::ANTArray<float> iNucleotideVPositions = ANTAuxiliary::ANTArray<float>(3, numNucleotidesV);
  ANTAuxiliary::ANTArray<float> iNucleotideERadii = ANTAuxiliary::ANTArray<float>(numNucleotidesV);

  float maxVRadius = model_->singleStrandRadius_;
  float minERadius = 0;
  float maxERadius = model_->nucleotideBaseRadius_;
  float intervalERadius = maxERadius - minERadius;
  float iERadius = minERadius + iv * intervalERadius;

  auto singleStrands = model_->nanorobot_->GetSingleStrands();

  int i = 0;
  unsigned int index = 0;
  for (auto & sid : singleStrands) {
    ANTSingleStrand* singleStrand = sid.second;
    MSVNucleotideList nucleotides = singleStrand->nucleotides_;

    for (auto & nit : nucleotides) {
      ANTNucleotide* nucleotide = nit.second;
      //unsigned int index = ntMap_.right.at(nucleotide);

      SBPosition3 pos2D = nucleotide->GetSBPosition2D();
      SBPosition3 pos1D = nucleotide->GetSBPosition1D();

      //approx backbone center with first atom position
      SBPosition3 cPos1D = pos1D;
      if (nucleotide->atomList_.size() > 0) {
        ANTAtom* a = static_cast<ANTAtom*>(nucleotide->atomList_[0]);
        cPos1D = a->position1D_;
      }
      auto temp = cPos1D[1];
      cPos1D[1] = cPos1D[0];
      cPos1D[0] = temp;

      if (dimension_ >= 1 && dimension_ < 2) {
        float dimInterpolator = dimension_ - 1;

        SBPosition3 dimInterpolatedVal = pos1D + dimInterpolator * (pos2D - pos1D);
        SBPosition3 dimInterpolatedVal2 = cPos1D + dimInterpolator * (pos2D - cPos1D);

        SBPosition3 finalInterpolatedVal = dimInterpolatedVal2 + iv * (dimInterpolatedVal - dimInterpolatedVal2);

        iNucleotideVPositions(index, 0) = (float)finalInterpolatedVal.v[0].getValue();
        iNucleotideVPositions(index, 1) = (float)finalInterpolatedVal.v[1].getValue();
        iNucleotideVPositions(index, 2) = (float)finalInterpolatedVal.v[2].getValue();
      }
      else if (dimension_ >= 2 && dimension_ <= 3) {
        float dimInterpolator = dimension_ - 2;

        SBPosition3 sideChainPos = nucleotide->GetSBSidechainCenter(configuration_->use_twist);
        SBPosition3 backbonePos = nucleotide->GetSBBackboneCenter(configuration_->use_twist);
        SBPosition3 interpolatedVal = sideChainPos + iv * (backbonePos - sideChainPos);
        SBPosition3 finalInterpolatedVal = pos2D + dimInterpolator * (interpolatedVal - pos2D);

        iNucleotideVPositions(index, 0) = (float)finalInterpolatedVal.v[0].getValue();
        iNucleotideVPositions(index, 1) = (float)finalInterpolatedVal.v[1].getValue();
        iNucleotideVPositions(index, 2) = (float)finalInterpolatedVal.v[2].getValue();
      }

      if (configuration_->show_nucleobase_text) {
        SBPosition3 curPos = SBPosition3(SBQuantity::picometer(iNucleotideVPositions(index, 0)),
          SBQuantity::picometer(iNucleotideVPositions(index, 1)),
          SBQuantity::picometer(iNucleotideVPositions(index, 2))
          );
        displayText(curPos, nucleotide);
      }

      iNucleotideERadii(index) = iERadius;

      //atoms 

      SBNodeIndexer atoms;
      nucleotide->residue_->getNodes(atoms, SBNode::IsType(SBNode::Atom));

      int atomInNucleotide = 0;

      SB_FOR(SBNode* node, atoms) {

        SBPosition3 position3D = nucleotide->GetSBPosition(configuration_->use_twist);
        SBPosition3 position2D = nucleotide->GetSBPosition2D();
        SBPosition3 position1D = nucleotide->GetSBPosition1D();
        if (nucleotide->atomList_.size() > 0) {
          ANTAtom* curANTAtom = static_cast<ANTAtom*>(nucleotide->atomList_[atomInNucleotide]);
          position3D = curANTAtom->position_;
          position2D = curANTAtom->position2D_;
          position1D = curANTAtom->position1D_;
        }

        if (configuration_->interpolate_dimensions) {

          if (dimension_ >= 1 && dimension_ < 2) {
            float dimInterpolator = dimension_ - 1;

            SBPosition3 vPosition1D;
            vPosition1D[0] = position1D[1];
            vPosition1D[1] = position1D[0];
            vPosition1D[2] = position1D[2];

            SBPosition3 dimInterpolatedVal = vPosition1D + dimInterpolator * (position2D - vPosition1D);

            float aMinX = (float)dimInterpolatedVal.v[0].getValue();
            float aMinY = (float)dimInterpolatedVal.v[1].getValue();
            float aMinZ = (float)dimInterpolatedVal.v[2].getValue();

            iAtomVPositions(i, 0) = aMinX + iv * (iNucleotideVPositions(index, 0) - aMinX);
            iAtomVPositions(i, 1) = aMinY + iv * (iNucleotideVPositions(index, 1) - aMinY);
            iAtomVPositions(i, 2) = aMinZ + iv * (iNucleotideVPositions(index, 2) - aMinZ);

            //float tempX = iAtomVPositions(i, 0);
            //float tempY = iAtomVPositions(i, 1);

            //iAtomVPositions(i, 0) = tempY;
            //iAtomVPositions(i, 1) = tempX;
          }
          else if (dimension_ >= 2 && dimension_ <= 3) {
            float dimInterpolator = dimension_ - 2;

            SBPosition3 dimInterpolatedVal = position2D + dimInterpolator * (position3D - position2D);

            float aMinX = (float)dimInterpolatedVal.v[0].getValue();
            float aMinY = (float)dimInterpolatedVal.v[1].getValue();
            float aMinZ = (float)dimInterpolatedVal.v[2].getValue();

            iAtomVPositions(i, 0) = aMinX + iv * (iNucleotideVPositions(index, 0) - aMinX);
            iAtomVPositions(i, 1) = aMinY + iv * (iNucleotideVPositions(index, 1) - aMinY);
            iAtomVPositions(i, 2) = aMinZ + iv * (iNucleotideVPositions(index, 2) - aMinZ);
          }

          float minVColorR = atomVColorData_(i, 0);
          float minVColorG = atomVColorData_(i, 1);
          float minVColorB = atomVColorData_(i, 2);
          float minVColorA = atomVColorData_(i, 3);

          float maxVColorR = atomNucleotideVColorData_(i, 0);
          float maxVColorG = atomNucleotideVColorData_(i, 1);
          float maxVColorB = atomNucleotideVColorData_(i, 2);
          float maxVColorA = atomNucleotideVColorData_(i, 3);

          iAtomVColorData(i, 0) = minVColorR + iv * (maxVColorR - minVColorR);
          iAtomVColorData(i, 1) = minVColorG + iv * (maxVColorG - minVColorG);
          iAtomVColorData(i, 2) = minVColorB + iv * (maxVColorB - minVColorB);
          iAtomVColorData(i, 3) = minVColorA + iv * (maxVColorA - minVColorA);

          float minVRadius = atomVRadii_(i);
          float intervalVRadius = maxVRadius - minVRadius;
          float iVRadius = minVRadius + iv * intervalVRadius;

          iAtomVRadii(i) = iVRadius;

        }
        //temp disabled
        //highlightAtoms(iAtomVColorData, nucleotideEColorData_, i, index, nucleotide);

        ++i;
        ++atomInNucleotide;

      }

      ++index;
    }

  }

  //temp disabled
  //if (!forSelection) {
  //  SAMSON::displaySpheres(
  //    numAtomV_,
  //    iAtomVPositions.GetArray(),
  //    iAtomVRadii.GetArray(),
  //    iAtomVColorData.GetArray(),
  //    atomVFlagData_.GetArray()
  //  );

  //  SAMSON::displayCylinders(
  //    numNucleotidesE,
  //    numNucleotidesV,
  //    nucleotideEIndexData_.GetArray(),
  //    iNucleotideVPositions.GetArray(),
  //    iNucleotideERadii.GetArray(),
  //    nullptr,
  //    nucleotideEColorData_.GetArray(),
  //    nucleotideEFlagData_.GetArray());
  //}
  //else {
  //  SAMSON::displaySpheresSelection(
  //    numAtomV_,
  //    iAtomVPositions.GetArray(),
  //    iAtomVRadii.GetArray(),
  //    atomVNodeIndexData_.GetArray()
  //    );
  //}

}


void SENucleotidesVisualModel::displayScale3to4(double iv, bool forSelection)
{
  auto singleStrands = model_->nanorobot_->GetSingleStrands();

  unsigned int nPositions = model_->nanorobot_->GetNumberOfNucleotides();
  unsigned int nCylinders = boost::numeric_cast<unsigned int>(nPositions - singleStrands.size());

  ANTAuxiliary::ANTArray<float> positions = ANTAuxiliary::ANTArray<float>(3, nPositions);
  ANTAuxiliary::ANTArray<float> radiiV = ANTAuxiliary::ANTArray<float>(nPositions);
  ANTAuxiliary::ANTArray<float> radiiE = ANTAuxiliary::ANTArray<float>(nPositions);
  ANTAuxiliary::ANTArray<float> colorsV = ANTAuxiliary::ANTArray<float>(4, nPositions);
  ANTAuxiliary::ANTArray<float> colorsE = ANTAuxiliary::ANTArray<float>(4, nPositions);
  ANTAuxiliary::ANTArray<unsigned int> flags = ANTAuxiliary::ANTArray<unsigned int>(nPositions);
  ANTAuxiliary::ANTArray<unsigned int> nodeIndices = ANTAuxiliary::ANTArray<unsigned int>(nPositions);
  ANTAuxiliary::ANTArray<unsigned int> indices = getNucleotideIndices();

  unsigned int index = 0;

  for (auto & sid : singleStrands) {
    ANTSingleStrand* singleStrand = sid.second;
    if (singleStrand->chain_->isVisible()) {
      MSVNucleotideList nucleotides = singleStrand->nucleotides_;

      for (auto & nit : nucleotides) {
        ANTNucleotide* nucleotide = nit.second;

        float minX = nucleotide->GetSBBackboneCenter()[0].getValue();
        float minY = nucleotide->GetSBBackboneCenter()[1].getValue();
        float minZ = nucleotide->GetSBBackboneCenter()[2].getValue();

        float maxX = nucleotide->GetSBSidechainCenter()[0].getValue();
        float maxY = nucleotide->GetSBSidechainCenter()[1].getValue();
        float maxZ = nucleotide->GetSBSidechainCenter()[2].getValue();

        positions(index, 0) = minX + iv * (maxX - minX);
        positions(index, 1) = minY + iv * (maxY - minY);
        positions(index, 2) = minZ + iv * (maxZ - minZ);

        nodeIndices(index) = nucleotide->residue_->getNodeIndex();
        flags(index) = nucleotide->residue_->getInheritedFlags() | getInheritedFlags();
        auto baseColor = model_->getBaseColor(nucleotide->type_);
        colorsV.SetRow(index, baseColor);

        auto colorE = model_->nucleotideEColor1_;
        colorsE.SetRow(index, colorE);

        radiiV(index) = getNucleotideRadius(nucleotide);
        radiiE(index) = model_->nucleotideBaseRadius_;

        SBPosition3 pos3D = nucleotide->GetSBPosition(configuration_->use_twist);
        SBPosition3 pos2D = nucleotide->GetSBPosition2D();
        SBPosition3 pos1D = nucleotide->GetSBPosition1D();

        if (configuration_->interpolate_dimensions) interpolateDimension(pos1D, pos2D, pos3D, positions, index);

        if (configuration_->show_nucleobase_text) {
          SBPosition3 curPos = SBPosition3(SBQuantity::picometer(positions(index, 0)),
            SBQuantity::picometer(positions(index, 1)),
            SBQuantity::picometer(positions(index, 2))
            );
          displayText(curPos, nucleotide);
        }

        highlightStrands(colorsV, colorsV, index, nucleotide);
        ++index;
      }
    }
  }

  if (!forSelection) {
    SAMSON::displaySpheres(
      nPositions,
      positions.GetArray(),
      radiiV.GetArray(),
      colorsV.GetArray(),
      flags.GetArray());

    SAMSON::displayCylinders(
      nCylinders,
      nPositions,
      indices.GetArray(),
      positions.GetArray(),
      radiiE.GetArray(),
      nullptr,
      colorsE.GetArray(),
      flags.GetArray());

  }
  else {
    SAMSON::displaySpheresSelection(
      nPositions,
      positions.GetArray(),
      radiiV.GetArray(),
      nodeIndices.GetArray());

    SAMSON::displayCylindersSelection(
      nCylinders,
      nPositions,
      indices.GetArray(),
      positions.GetArray(),
      radiiV.GetArray(),
      nullptr,
      nodeIndices.GetArray());

  }
}


void SENucleotidesVisualModel::displayScale4to5(double iv, bool forSelection) {

  auto singleStrands = model_->nanorobot_->GetSingleStrands();

  unsigned int nPositions = model_->nanorobot_->GetNumberOfNucleotides();
  unsigned int nCylinders = boost::numeric_cast<unsigned int>(nPositions - singleStrands.size());

  ANTAuxiliary::ANTArray<float> positions = ANTAuxiliary::ANTArray<float>(3, nPositions);
  ANTAuxiliary::ANTArray<float> radiiV = ANTAuxiliary::ANTArray<float>(nPositions);
  ANTAuxiliary::ANTArray<float> radiiE = ANTAuxiliary::ANTArray<float>(nPositions);
  ANTAuxiliary::ANTArray<float> colorsV = ANTAuxiliary::ANTArray<float>(4, nPositions);
  ANTAuxiliary::ANTArray<float> colorsE = ANTAuxiliary::ANTArray<float>(4, nPositions);
  ANTAuxiliary::ANTArray<unsigned int> flags = ANTAuxiliary::ANTArray<unsigned int>(nPositions);
  ANTAuxiliary::ANTArray<unsigned int> nodeIndices = ANTAuxiliary::ANTArray<unsigned int>(nPositions);
  ANTAuxiliary::ANTArray<unsigned int> indices = getNucleotideIndices();

  float minERadius = model_->nucleotideBaseRadius_;
  float maxERadius = model_->nucleotideVRadius_;
  float intervalERadius = maxERadius - minERadius;
  float iERadius = minERadius + iv * intervalERadius;

  unsigned int index = 0;

  for (auto & sid : singleStrands) {
    ANTSingleStrand* singleStrand = sid.second;
    if (singleStrand->chain_->isVisible()) {

      MSVNucleotideList nucleotides = singleStrand->nucleotides_;

      for (auto & nit : nucleotides) {
        ANTNucleotide* nucleotide = nit.second;

        flags(index) = nucleotide->residue_->getInheritedFlags() | getInheritedFlags();
        nodeIndices(index) = nucleotide->residue_->getNodeIndex();

        positions(index, 0) = nucleotide->GetSBSidechainCenter()[0].getValue();
        positions(index, 1) = nucleotide->GetSBSidechainCenter()[1].getValue();
        positions(index, 2) = nucleotide->GetSBSidechainCenter()[2].getValue();

        SBPosition3 pos3D = nucleotide->GetSBPosition(configuration_->use_twist);
        SBPosition3 pos2D = nucleotide->GetSBPosition2D();
        SBPosition3 pos1D = nucleotide->GetSBPosition1D();

        if (configuration_->interpolate_dimensions) interpolateDimension(pos1D, pos2D, pos3D, positions, index);

        if (configuration_->show_nucleobase_text) {
          SBPosition3 curPos = SBPosition3(
            SBQuantity::picometer(positions(index, 0)),
            SBQuantity::picometer(positions(index, 1)),
            SBQuantity::picometer(positions(index, 2))
            );
          displayText(curPos, nucleotide);
        }

        radiiV(index) = getNucleotideRadius(nucleotide);

        colorsE.SetRow(index, model_->nucleotideEColor1_);

        auto baseColor = model_->getBaseColor(nucleotide->type_);

        if (singleStrand->isScaffold_) {
          float maxVColorR;
          float maxVColorG;
          float maxVColorB;
          float maxVColorA;

          if (showMeltingTemperature_ || showGibbsFreeEnergy_) {
            maxVColorR = propertyNucleotideColorsV_(index, 0);
            maxVColorG = propertyNucleotideColorsV_(index, 1); 
            maxVColorB = propertyNucleotideColorsV_(index, 2);
            maxVColorA = propertyNucleotideColorsV_(index, 3);

            colorsE(index, 0) = propertyNucleotideColorsV_(index, 0);
            colorsE(index, 1) = propertyNucleotideColorsV_(index, 1);
            colorsE(index, 2) = propertyNucleotideColorsV_(index, 2);
            colorsE(index, 3) = propertyNucleotideColorsV_(index, 3);
          }
          else {
            maxVColorR = model_->nucleotideEColor1_(0);
            maxVColorG = model_->nucleotideEColor1_(1);
            maxVColorB = model_->nucleotideEColor1_(2);
            maxVColorA = model_->nucleotideEColor1_(3);
          }

          float minVColorR = baseColor(0);
          float minVColorG = baseColor(1);
          float minVColorB = baseColor(2);
          float minVColorA = baseColor(3);

          colorsV(index, 0) = minVColorR + iv * (maxVColorR - minVColorR);
          colorsV(index, 1) = minVColorG + iv * (maxVColorG - minVColorG);
          colorsV(index, 2) = minVColorB + iv * (maxVColorB - minVColorB);
          colorsV(index, 3) = minVColorA + iv * (maxVColorA - minVColorA);

          radiiE(index) = iERadius;

        }
        else {
          if (showConsecutivesGs_) {
            baseColor = propertyNucleotideColorsV_.GetRow(index);
          }

          colorsV.SetRow(index, baseColor);

          radiiE(index) = model_->nucleotideBaseRadius_;

        }

        highlightStrands(colorsV, colorsE, index, nucleotide);

        //strand direction
        if (nucleotide->end_ == End::ThreePrime) {
          radiiE(index) = model_->nucleotideBaseRadius_;
        }

        ++index;
      }
    }
  }

  if (!forSelection) {
    SAMSON::displaySpheres(
      nPositions,
      positions.GetArray(),
      radiiV.GetArray(),
      colorsV.GetArray(),
      flags.GetArray());

    SAMSON::displayCylinders(
      nCylinders,
      nPositions,
      indices.GetArray(),
      positions.GetArray(),
      radiiE.GetArray(),
      nullptr,
      colorsE.GetArray(),
      flags.GetArray());
  }
  else {
    SAMSON::displaySpheresSelection(
      nPositions,
      positions.GetArray(),
      radiiV.GetArray(),
      nodeIndices.GetArray());

    SAMSON::displayCylindersSelection(
      nCylinders,
      nPositions,
      indices.GetArray(),
      positions.GetArray(),
      radiiV.GetArray(),
      nullptr,
      nodeIndices.GetArray());

  }

}

void SENucleotidesVisualModel::displayScale5to6(double iv, bool forSelection)
{
  auto singleStrands = model_->nanorobot_->GetSingleStrands();

  unsigned int nPositions = model_->nanorobot_->GetNumberOfNucleotides();
  unsigned int nCylinders = boost::numeric_cast<unsigned int>(nPositions - singleStrands.size());

  ANTAuxiliary::ANTArray<float> radiiV = ANTAuxiliary::ANTArray<float>(nPositions);
  ANTAuxiliary::ANTArray<float> radiiE = ANTAuxiliary::ANTArray<float>(nPositions);
  ANTAuxiliary::ANTArray<float> colorsV = ANTAuxiliary::ANTArray<float>(4, nPositions);
  ANTAuxiliary::ANTArray<float> colorsE = ANTAuxiliary::ANTArray<float>(4, nPositions);
  ANTAuxiliary::ANTArray<float> positions = ANTAuxiliary::ANTArray<float>(3, nPositions);
  ANTAuxiliary::ANTArray<unsigned int> flags = ANTAuxiliary::ANTArray<unsigned int>(nPositions);
  ANTAuxiliary::ANTArray<unsigned int> nodeIndices = ANTAuxiliary::ANTArray<unsigned int>(nPositions);
  ANTAuxiliary::ANTArray<unsigned int> indices = getNucleotideIndices();

  float minERadius = model_->nucleotideBaseRadius_;
  float maxERadius = model_->singleStrandRadius_;
  float intervalERadius = maxERadius - minERadius;
  float iERadius = minERadius + iv * intervalERadius;

  unsigned int index = 0;

  for (auto & sid : singleStrands) {
    ANTSingleStrand* singleStrand = sid.second;
    if (singleStrand->chain_->isVisible()) {
      MSVNucleotideList nucleotides = singleStrand->nucleotides_;
      for (auto & nit : nucleotides) {
        ANTNucleotide* nucleotide = nit.second;

        flags(index) = nucleotide->residue_->getInheritedFlags() | getInheritedFlags();
        nodeIndices(index) = nucleotide->residue_->getNodeIndex();

        float minVColorR;
        float minVColorG;
        float minVColorB;
        float minVColorA;

        int stapleColorNum = singleStrand->id_ % model_->numStapleColors_;

        float maxVColorR = model_->stapleColors_(stapleColorNum, 0);
        float maxVColorG = model_->stapleColors_(stapleColorNum, 1);
        float maxVColorB = model_->stapleColors_(stapleColorNum, 2);
        float maxVColorA = model_->stapleColors_(stapleColorNum, 3);

        positions(index, 0) = nucleotide->GetSBSidechainCenter()[0].getValue();
        positions(index, 1) = nucleotide->GetSBSidechainCenter()[1].getValue();
        positions(index, 2) = nucleotide->GetSBSidechainCenter()[2].getValue();

        radiiV(index) = getNucleotideRadius(nucleotide);

        if (singleStrand->isScaffold_)
        {

          minVColorR = model_->nucleotideEColor1_(0);
          minVColorG = model_->nucleotideEColor1_(1);
          minVColorB = model_->nucleotideEColor1_(2);
          minVColorA = model_->nucleotideEColor1_(3);

          maxVColorR = model_->nucleotideEColor1_(0);
          maxVColorG = model_->nucleotideEColor1_(1);
          maxVColorB = model_->nucleotideEColor1_(2);
          maxVColorA = model_->nucleotideEColor1_(3);

          colorsE(index, 0) = model_->nucleotideEColor1_(0);
          colorsE(index, 1) = model_->nucleotideEColor1_(1);
          colorsE(index, 2) = model_->nucleotideEColor1_(2);
          colorsE(index, 3) = model_->nucleotideEColor1_(3);

          radiiE(index) = model_->nucleotideVRadius_;

          if (showMeltingTemperature_ || showGibbsFreeEnergy_) {
            minVColorR = propertyNucleotideColorsV_(index, 0);
            minVColorG = propertyNucleotideColorsV_(index, 1);
            minVColorB = propertyNucleotideColorsV_(index, 2);
            minVColorA = propertyNucleotideColorsV_(index, 3);
          }
        }
        else
        {
          auto baseColor = model_->getBaseColor(nucleotide->type_);

          minVColorR = baseColor(0);
          minVColorG = baseColor(1);
          minVColorB = baseColor(2);
          minVColorA = baseColor(3);

          radiiE(index) = iERadius;

          float minEColorR = model_->nucleotideEColor1_(0);
          float minEColorG = model_->nucleotideEColor1_(1);
          float minEColorB = model_->nucleotideEColor1_(2);
          float minEColorA = model_->nucleotideEColor1_(3);

          colorsE(index, 0) = minEColorR + iv * (maxVColorR - minEColorR);
          colorsE(index, 1) = minEColorG + iv * (maxVColorG - minEColorG);
          colorsE(index, 2) = minEColorB + iv * (maxVColorB - minEColorB);
          colorsE(index, 3) = minEColorA + iv * (maxVColorA - minEColorA);
        }

        if (showMeltingTemperature_ || showGibbsFreeEnergy_) {

          maxVColorR = propertyNucleotideColorsV_(index, 0);
          maxVColorG = propertyNucleotideColorsV_(index, 1);
          maxVColorB = propertyNucleotideColorsV_(index, 2);
          maxVColorA = propertyNucleotideColorsV_(index, 3);

          colorsE(index, 0) = propertyNucleotideColorsV_(index, 0);
          colorsE(index, 1) = propertyNucleotideColorsV_(index, 1);
          colorsE(index, 2) = propertyNucleotideColorsV_(index, 2);
          colorsE(index, 3) = propertyNucleotideColorsV_(index, 3);

        }

        colorsV(index, 0) = minVColorR + iv * (maxVColorR - minVColorR);
        colorsV(index, 1) = minVColorG + iv * (maxVColorG - minVColorG);
        colorsV(index, 2) = minVColorB + iv * (maxVColorB - minVColorB);
        colorsV(index, 3) = minVColorA + iv * (maxVColorA - minVColorA);

        SBPosition3 pos3D = nucleotide->GetSBSidechainCenter(configuration_->use_twist);
        SBPosition3 pos2D = nucleotide->GetSBPosition2D();
        SBPosition3 pos1D = nucleotide->GetSBPosition1D();

        if (configuration_->interpolate_dimensions) interpolateDimension(pos1D, pos2D, pos3D, positions, index);

        if (configuration_->show_nucleobase_text) {
          SBPosition3 curPos = SBPosition3(SBQuantity::picometer(positions(index, 0)),
            SBQuantity::picometer(positions(index, 1)),
            SBQuantity::picometer(positions(index, 2))
            );
          displayText(curPos, nucleotide);
        }

        highlightStrands(colorsV, colorsE, index, nucleotide);

        //strand direction
        if (nucleotide->end_ == End::ThreePrime) {
          radiiE(index) = model_->nucleotideBaseRadius_;
        }

        ++index;
      }
    }
  }

  if (!forSelection) {
    SAMSON::displaySpheres(
      nPositions,
      positions.GetArray(),
      radiiV.GetArray(),
      colorsV.GetArray(),
      flags.GetArray());

    SAMSON::displayCylinders(
      nCylinders,
      nPositions,
      indices.GetArray(),
      positions.GetArray(),
      radiiE.GetArray(),
      nullptr,
      colorsE.GetArray(),
      flags.GetArray());
  }
  else {
    SAMSON::displaySpheresSelection(
      nPositions,
      positions.GetArray(),
      radiiV.GetArray(),
      nodeIndices.GetArray());

    SAMSON::displayCylindersSelection(
      nCylinders,
      nPositions,
      indices.GetArray(),
      positions.GetArray(),
      radiiV.GetArray(),
      nullptr,
      nodeIndices.GetArray());

  }

}


void SENucleotidesVisualModel::displayScale6to7(double iv, bool forSelection) {

  auto singleStrands = model_->nanorobot_->GetSingleStrands();

  unsigned int nPositions = model_->nanorobot_->GetNumberOfNucleotides();
  unsigned int nCylinders = boost::numeric_cast<unsigned int>(nPositions - singleStrands.size());
  auto nucleotideEColor1 = model_->nucleotideEColor1_;

  ANTAuxiliary::ANTArray<float> colorsV = ANTAuxiliary::ANTArray<float>(4, nPositions);
  ANTAuxiliary::ANTArray<float> colorsE = ANTAuxiliary::ANTArray<float>(4, nPositions);
  ANTAuxiliary::ANTArray<float> positions = ANTAuxiliary::ANTArray<float>(3, nPositions);
  ANTAuxiliary::ANTArray<float> radiiV = ANTAuxiliary::ANTArray<float>(nPositions);
  ANTAuxiliary::ANTArray<float> radiiE = ANTAuxiliary::ANTArray<float>(nPositions);
  ANTAuxiliary::ANTArray<unsigned int> flags = ANTAuxiliary::ANTArray<unsigned int>(nPositions);
  ANTAuxiliary::ANTArray<unsigned int> nodeIndices = ANTAuxiliary::ANTArray<unsigned int>(nPositions);
  ANTAuxiliary::ANTArray<unsigned int> indices = getNucleotideIndices();
  
  unsigned int index = 0;

  for (auto & sid : singleStrands) {
    ANTSingleStrand* singleStrand = sid.second;

    if (singleStrand->chain_->isVisible()) {

      MSVNucleotideList nucleotides = singleStrand->nucleotides_;

      for (auto & nit : nucleotides) {
        ANTNucleotide* nucleotide = nit.second;

        radiiV(index) = model_->nucleotideVRadius_;

        radiiE(index) = model_->nucleotideVRadius_;

        flags(index) = nucleotide->residue_->getInheritedFlags() | getInheritedFlags();
        nodeIndices(index) = nucleotide->residue_->getNodeIndex();

        SBPosition3 min = nucleotide->GetSBSidechainCenter();
        SBPosition3 max = nucleotide->GetSBBackboneCenter();

        SBPosition3 iPos = min + iv * (max - min);

        positions(index, 0) = iPos[0].getValue();
        positions(index, 1) = iPos[1].getValue();
        positions(index, 2) = iPos[2].getValue();

        SBPosition3 pos3D = nucleotide->GetSBPosition(configuration_->use_twist);
        SBPosition3 pos2D = nucleotide->GetSBPosition2D();
        SBPosition3 pos1D = nucleotide->GetSBPosition1D();

        if (configuration_->interpolate_dimensions) interpolateDimension(pos1D, pos2D, pos3D, positions, index);

        if (configuration_->show_nucleobase_text) {
          SBPosition3 curPos = SBPosition3(SBQuantity::picometer(positions(index, 0)),
            SBQuantity::picometer(positions(index, 1)),
            SBQuantity::picometer(positions(index, 2))
            );
          displayText(curPos, nucleotide);
        }

        float minVColorR;
        float minVColorG;
        float minVColorB;
        float minVColorA;

        int stapleColorNum = singleStrand->id_ % model_->numStapleColors_;

        float maxVColorR = model_->stapleColors_(stapleColorNum, 0);
        float maxVColorG = model_->stapleColors_(stapleColorNum, 1);
        float maxVColorB = model_->stapleColors_(stapleColorNum, 2);
        float maxVColorA = model_->stapleColors_(stapleColorNum, 3);

        if (singleStrand->isScaffold_)
        {
          if (showMeltingTemperature_ || showGibbsFreeEnergy_) {
            minVColorR = propertyNucleotideColorsV_(index, 0);
            minVColorG = propertyNucleotideColorsV_(index, 1);
            minVColorB = propertyNucleotideColorsV_(index, 2);
            minVColorA = propertyNucleotideColorsV_(index, 3);

            maxVColorR = propertyNucleotideColorsV_(index, 0);
            maxVColorG = propertyNucleotideColorsV_(index, 1);
            maxVColorB = propertyNucleotideColorsV_(index, 2);
            maxVColorA = propertyNucleotideColorsV_(index, 3);

            colorsE(index, 0) = propertyNucleotideColorsV_(index, 0);
            colorsE(index, 1) = propertyNucleotideColorsV_(index, 1);
            colorsE(index, 2) = propertyNucleotideColorsV_(index, 2);
            colorsE(index, 3) = propertyNucleotideColorsV_(index, 3);
          }
          else {
            minVColorR = nucleotideEColor1(0);
            minVColorG = nucleotideEColor1(1);
            minVColorB = nucleotideEColor1(2);
            minVColorA = nucleotideEColor1(3);

            maxVColorR = nucleotideEColor1(0);
            maxVColorG = nucleotideEColor1(1);
            maxVColorB = nucleotideEColor1(2);
            maxVColorA = nucleotideEColor1(3);

            colorsE(index, 0) = nucleotideEColor1(0);
            colorsE(index, 1) = nucleotideEColor1(1);
            colorsE(index, 2) = nucleotideEColor1(2);
            colorsE(index, 3) = nucleotideEColor1(3);

          }
        }
        else
        {
          minVColorR = maxVColorR;
          minVColorG = maxVColorG;
          minVColorB = maxVColorB;
          minVColorA = maxVColorA;

          colorsE(index, 0) = maxVColorR;
          colorsE(index, 1) = maxVColorG;
          colorsE(index, 2) = maxVColorB;
          colorsE(index, 3) = maxVColorA;

          if (showMeltingTemperature_ || showGibbsFreeEnergy_) {
            minVColorR = propertyNucleotideColorsV_(index, 0);
            minVColorG = propertyNucleotideColorsV_(index, 1);
            minVColorB = propertyNucleotideColorsV_(index, 2);
            minVColorA = propertyNucleotideColorsV_(index, 3);

            maxVColorR = propertyNucleotideColorsV_(index, 0);
            maxVColorG = propertyNucleotideColorsV_(index, 1);
            maxVColorB = propertyNucleotideColorsV_(index, 2);
            maxVColorA = propertyNucleotideColorsV_(index, 3);

            colorsE(index, 0) = propertyNucleotideColorsV_(index, 0);
            colorsE(index, 1) = propertyNucleotideColorsV_(index, 1);
            colorsE(index, 2) = propertyNucleotideColorsV_(index, 2);
            colorsE(index, 3) = propertyNucleotideColorsV_(index, 3);
          }
        }

        colorsV(index, 0) = minVColorR + iv * (maxVColorR - minVColorR);
        colorsV(index, 1) = minVColorG + iv * (maxVColorG - minVColorG);
        colorsV(index, 2) = minVColorB + iv * (maxVColorB - minVColorB);
        colorsV(index, 3) = minVColorA + iv * (maxVColorA - minVColorA);

        highlightStrands(colorsV, colorsE, index, nucleotide);

        //strand direction
        if (nucleotide->end_ == End::ThreePrime) {
          radiiE(index) = model_->nucleotideBaseRadius_;
        }

        ++index;
      }
    }
  }

  if (!forSelection) {
    SAMSON::displaySpheres(
      nPositions,
      positions.GetArray(),
      radiiV.GetArray(),
      colorsV.GetArray(),
      flags.GetArray());

    SAMSON::displayCylinders(
      nCylinders,
      nPositions,
      indices.GetArray(),
      positions.GetArray(),
      radiiE.GetArray(),
      nullptr,
      colorsE.GetArray(),
      flags.GetArray());
  }
  else {
    SAMSON::displaySpheresSelection(
      nPositions,
      positions.GetArray(),
      radiiV.GetArray(),
      nodeIndices.GetArray());

    SAMSON::displayCylindersSelection(
      nCylinders,
      nPositions,
      indices.GetArray(),
      positions.GetArray(),
      radiiE.GetArray(),
      nullptr,
      nodeIndices.GetArray());

  }

}

void SENucleotidesVisualModel::displayScale7to8(double iv, bool forSelection)
{
  auto singleStrands = model_->nanorobot_->GetSingleStrands();

  unsigned int nPositions = model_->nanorobot_->GetNumberOfNucleotides();
  unsigned int nCylinders = boost::numeric_cast<unsigned int>(nPositions - singleStrands.size());

  ANTAuxiliary::ANTArray<float> radiiV = ANTAuxiliary::ANTArray<float>(nPositions);
  ANTAuxiliary::ANTArray<float> colorsV = ANTAuxiliary::ANTArray<float>(4, nPositions);
  ANTAuxiliary::ANTArray<float> positions = ANTAuxiliary::ANTArray<float>(3, nPositions);
  ANTAuxiliary::ANTArray<unsigned int> flags = ANTAuxiliary::ANTArray<unsigned int>(nPositions);
  ANTAuxiliary::ANTArray<unsigned int> nodeIndices = ANTAuxiliary::ANTArray<unsigned int>(nPositions);
  ANTAuxiliary::ANTArray<unsigned int> indices = getNucleotideIndices();

  unsigned int index = 0;

  float minVRadius = model_->singleStrandRadius_;
  float maxVRadius = model_->doubleHelixVRadius_;
  float intervalVRadius = maxVRadius - minVRadius;
  float iVRadius = minVRadius + iv * intervalVRadius;
  float iVRadiusStaples = minVRadius + iv * (maxVRadius);

  float minERadius = model_->singleStrandRadius_;
  float maxERadius = model_->doubleHelixERadius_;
  float intervalERadius = maxERadius - minERadius;
  float iERadius = minERadius + iv * intervalERadius;

  for (auto & sid : singleStrands) {
    ANTSingleStrand* singleStrand = sid.second;
    MSVNucleotideList nucleotides = singleStrand->nucleotides_;

    for (auto & nit : nucleotides) {
      ANTNucleotide* nucleotide = nit.second;

      radiiV(index) = iVRadius;
      flags(index) = nucleotide->residue_->getInheritedFlags() | getInheritedFlags();
      nodeIndices(index) = nucleotide->residue_->getNodeIndex();

      int stapleColorNum = singleStrand->id_ % model_->numStapleColors_;

      float minVColorR = model_->stapleColors_(stapleColorNum, 0);
      float minVColorG = model_->stapleColors_(stapleColorNum, 1);
      float minVColorB = model_->stapleColors_(stapleColorNum, 2);
      float minVColorA = model_->stapleColors_(stapleColorNum, 3);

      float maxVColorR = model_->doubleHelixVColor_(0);
      float maxVColorG = model_->doubleHelixVColor_(1);
      float maxVColorB = model_->doubleHelixVColor_(2);
      float maxVColorA = model_->doubleHelixVColor_(3);

      float minX = nucleotide->GetSBSidechainCenter()[0].getValue();
      float minY = nucleotide->GetSBSidechainCenter()[1].getValue();
      float minZ = nucleotide->GetSBSidechainCenter()[2].getValue();

      SBPosition3 left = nucleotide->GetSBPosition();
      SBPosition3 right;
      if (nucleotide->pair_ != nullptr) {
        right = nucleotide->pair_->GetSBPosition();
      }
      else {
        right = left;
      }
      SBPosition3 mid = (left + right) / 2;

      float maxX = mid[0].getValue();
      float maxY = mid[1].getValue();
      float maxZ = mid[2].getValue();

      if (singleStrand->isScaffold_) {
        //non paired nucleotides stay where they are and are highlighted

        if (nucleotide->pair_ != nullptr) {

          minVColorR = model_->nucleotideEColor1_(0);
          minVColorG = model_->nucleotideEColor1_(1);
          minVColorB = model_->nucleotideEColor1_(2);
          minVColorA = model_->nucleotideEColor1_(3);

        }
        else {
          minVColorR = 1.0f;
          minVColorG = 0.0f;
          minVColorB = 0.0f;
          minVColorA = 1.0f;

          maxVColorR = minVColorR;
          maxVColorG = minVColorG;
          maxVColorB = minVColorB;
          maxVColorA = minVColorA;
        }

      }
      else {
        ANTNucleotide* scaffoldNucleotide = nucleotide->pair_;

        if (scaffoldNucleotide != nullptr && scaffoldNucleotide->strand_->isScaffold_) {

        }
        else {
          //non paired nucleotides stay where they are and are highlighted
          maxX = minX;
          maxY = minY;
          maxZ = minZ;

          minVColorR = 1.0f;
          minVColorG = 0.0f;
          minVColorB = 0.0f;
          minVColorA = 1.0f;

          maxVColorR = minVColorR;
          maxVColorG = minVColorG;
          maxVColorB = minVColorB;
          maxVColorA = minVColorA;
        }
      }

      if (showMeltingTemperature_ || showGibbsFreeEnergy_) {
        minVColorR = propertyNucleotideColorsV_(index, 0);
        minVColorG = propertyNucleotideColorsV_(index, 1);
        minVColorB = propertyNucleotideColorsV_(index, 2);
        minVColorA = propertyNucleotideColorsV_(index, 3);

        maxVColorR = propertyNucleotideColorsV_(index, 0);
        maxVColorG = propertyNucleotideColorsV_(index, 1);
        maxVColorB = propertyNucleotideColorsV_(index, 2);
        maxVColorA = propertyNucleotideColorsV_(index, 3);
      }

      colorsV(index, 0) = minVColorR + iv * (maxVColorR - minVColorR);
      colorsV(index, 1) = minVColorG + iv * (maxVColorG - minVColorG);
      colorsV(index, 2) = minVColorB + iv * (maxVColorB - minVColorB);
      colorsV(index, 3) = minVColorA + iv * (maxVColorA - minVColorA);

      SBPosition3 pairPos3D = nucleotide->GetSBBackboneCenter(configuration_->use_twist);
      SBPosition3 pairPos2D = nucleotide->GetSBPosition2D();
      SBPosition3 pairPos1D = nucleotide->GetSBPosition1D();
      if (nucleotide->pair_ != nullptr) {
        pairPos3D = nucleotide->pair_->GetSBBackboneCenter(configuration_->use_twist);
        pairPos2D = nucleotide->pair_->GetSBPosition2D();
        pairPos1D = nucleotide->pair_->GetSBPosition1D();
      }
      SBPosition3 pos3D = nucleotide->GetSBBackboneCenter(configuration_->use_twist);
      SBPosition3 pos2D = nucleotide->GetSBPosition2D();
      SBPosition3 pos1D = nucleotide->GetSBPosition1D();

      if (configuration_->interpolate_dimensions) {
        //allow for backbone and sidechain positions
        if (dimension_ >= 1 && dimension_ < 2) {
          float dimInterpolator = dimension_ - 1;

          SBPosition3 interpolatedVal = pos1D + dimInterpolator * (pos2D - pos1D);
          SBPosition3 pairInterpolatedVal = pairPos1D + dimInterpolator * (pairPos2D - pairPos1D);
          SBPosition3 center = (interpolatedVal + pairInterpolatedVal) / 2;

          SBPosition3 finalInterpolatedVal = interpolatedVal + iv * (center - interpolatedVal);

          positions(index, 0) = (float)interpolatedVal.v[0].getValue();
          positions(index, 1) = (float)interpolatedVal.v[1].getValue();
          positions(index, 2) = (float)interpolatedVal.v[2].getValue();
        }
        else if (dimension_ >= 2 && dimension_ <= 3) {
          float dimInterpolator = dimension_ - 2;

          SBPosition3 interpolatedVal = pos2D + dimInterpolator * (pos3D - pos2D);
          SBPosition3 pairInterpolatedVal = pairPos2D + dimInterpolator * (pairPos3D - pairPos2D);
          SBPosition3 center = (interpolatedVal + pairInterpolatedVal) / 2;

          SBPosition3 finalInterpolatedVal = interpolatedVal + iv * (center - interpolatedVal);

          positions(index, 0) = (float)finalInterpolatedVal.v[0].getValue();
          positions(index, 1) = (float)finalInterpolatedVal.v[1].getValue();
          positions(index, 2) = (float)finalInterpolatedVal.v[2].getValue();
        }
      }

      if (configuration_->show_nucleobase_text) {
        SBPosition3 curPos = SBPosition3(SBQuantity::picometer(positions(index, 0)),
          SBQuantity::picometer(positions(index, 1)),
          SBQuantity::picometer(positions(index, 2))
          );
        displayText(curPos, nucleotide);
      }

      highlightStrands(colorsV, colorsV, index, nucleotide);

      ++index;
    }
  }

  if (!forSelection) {
    SAMSON::displaySpheres(
      nPositions,
      positions.GetArray(),
      radiiV.GetArray(),
      colorsV.GetArray(),
      flags.GetArray());

    SAMSON::displayCylinders(
      nCylinders,
      nPositions,
      indices.GetArray(),
      positions.GetArray(),
      radiiV.GetArray(),
      nullptr,
      colorsV.GetArray(),
      flags.GetArray());
  }
  else {
    SAMSON::displaySpheresSelection(
      nPositions,
      positions.GetArray(),
      radiiV.GetArray(),
      nodeIndices.GetArray());

    SAMSON::displayCylindersSelection(
      nCylinders,
      nPositions,
      indices.GetArray(),
      positions.GetArray(),
      radiiV.GetArray(),
      nullptr,
      nodeIndices.GetArray());

  }

}


void SENucleotidesVisualModel::displayScale8to9(double iv, bool forSelection) {

  auto scaffs = model_->nanorobot_->GetScaffolds();

  std::map<ANTNucleotide*, unsigned int> ntMap;
  unsigned int index = 0;
  for (auto &s : scaffs) {
    for (auto &nid : s->nucleotides_) {
      ANTNucleotide * nt = nid.second;
      ntMap.insert(make_pair(nt, index));
      ++index;
    }
  }

  unsigned int nPositions = boost::numeric_cast<unsigned int>(ntMap.size());
  unsigned int nCylinders = boost::numeric_cast<unsigned int>(nPositions - scaffs.size());

  ANTAuxiliary::ANTArray<float> positions = ANTAuxiliary::ANTArray<float>(3, nPositions);
  ANTAuxiliary::ANTArray<float> radiiV = ANTAuxiliary::ANTArray<float>(nPositions);
  ANTAuxiliary::ANTArray<float> colorsV = ANTAuxiliary::ANTArray<float>(4, nPositions);
  ANTAuxiliary::ANTArray<unsigned int> flags = ANTAuxiliary::ANTArray<unsigned int>(nPositions);
  ANTAuxiliary::ANTArray<unsigned int> nodeIndices = ANTAuxiliary::ANTArray<unsigned int>(nPositions);
  ANTAuxiliary::ANTArray<unsigned int> indices = ANTAuxiliary::ANTArray<unsigned int>(nCylinders * 2);

  unsigned int j = 0;
  for (auto &s : scaffs) {
    ANTNucleotide* cur = s->fivePrime_;

    while (cur->next_ != nullptr) {
      unsigned int curIndex = ntMap[cur];
      unsigned int nextIndex = ntMap[cur->next_];

      indices(2 * j) = curIndex;
      indices(2 * j + 1) = nextIndex;
      cur = cur->next_;
      j++;
    }
  }

  float minVRadius = model_->doubleHelixVRadius_;
  float maxVRadius = model_->edgeRadius_;
  float intervalVRadius = maxVRadius - minVRadius;
  float iVRadius = minVRadius + iv * intervalVRadius;

  float minVColorR = configuration_->double_helix_V_color[0];
  float minVColorG = configuration_->double_helix_V_color[1];
  float minVColorB = configuration_->double_helix_V_color[2];
  float minVColorA = configuration_->double_helix_V_color[3];

  float maxVColorR = model_->baseSegmentColor_(0);
  float maxVColorG = model_->baseSegmentColor_(1);
  float maxVColorB = model_->baseSegmentColor_(2);
  float maxVColorA = model_->baseSegmentColor_(3);

  unsigned int i = 0;

  for (auto &scaff : scaffs) {
    for (auto const &nit : scaff->nucleotides_) {
      ANTNucleotide* nucleotide = nit.second;
      unsigned int index = i;

      radiiV(index) = iVRadius;

      colorsV(index, 0) = minVColorR + iv * (maxVColorR - minVColorR);
      colorsV(index, 1) = minVColorG + iv * (maxVColorG - minVColorG);
      colorsV(index, 2) = minVColorB + iv * (maxVColorB - minVColorB);
      colorsV(index, 3) = minVColorA + iv * (maxVColorA - minVColorA);

      flags(index) = nucleotide->residue_->getInheritedFlags() | getInheritedFlags();
      nodeIndices(index) = nucleotide->residue_->getNodeIndex();

      if (showMeltingTemperature_ || showGibbsFreeEnergy_ ) {
        colorsV(index, 0) = propertyNucleotideColorsV_(index, 0);
        colorsV(index, 1) = propertyNucleotideColorsV_(index, 1);
        colorsV(index, 2) = propertyNucleotideColorsV_(index, 2);
        colorsV(index, 3) = propertyNucleotideColorsV_(index, 3);
      }

      ANTBaseSegment * bs = model_->nanorobot_->GetBaseSegment(nucleotide);
      SBPosition3 bsPos3D;
      SBPosition3 bsPos2D;
      SBPosition3 bsPos1D;
      if (bs != nullptr) {
        bsPos3D = bs->GetSBPosition();
        bsPos2D = bs->GetSBPosition2D();
        bsPos1D = bs->GetSBPosition1D();
      }
      else {
        bsPos3D = nucleotide->GetSBPosition();
        bsPos2D = nucleotide->GetSBPosition2D();
        bsPos1D = nucleotide->GetSBPosition1D();
      }

      SBPosition3 nPos3D = nucleotide->GetSBPosition();
      SBPosition3 nPos2D = nucleotide->GetSBPosition2D();
      SBPosition3 nPos1D = nucleotide->GetSBPosition1D();

      if (configuration_->interpolate_dimensions) {

        if (dimension_ >= 1 && dimension_ < 2) {
          float dimInterpolator = dimension_ - 1;

          SBPosition3 vBsPos1D = bsPos1D;
          vBsPos1D[0] = bsPos1D[1];
          vBsPos1D[1] = bsPos1D[0];

          SBPosition3 nDimInterpolated = nPos1D + dimInterpolator * (nPos2D - nPos1D);
          SBPosition3 bsDimInterpolatedVal = vBsPos1D + dimInterpolator * (bsPos2D - vBsPos1D);
          SBPosition3 posScaleInterpolatedVal = nDimInterpolated + iv * (bsDimInterpolatedVal - nDimInterpolated);

          positions(index, 0) = (float)posScaleInterpolatedVal.v[0].getValue();
          positions(index, 1) = (float)posScaleInterpolatedVal.v[1].getValue();
          positions(index, 2) = (float)posScaleInterpolatedVal.v[2].getValue();
        }
        else if (dimension_ >= 2 && dimension_ <= 3) {
          float dimInterpolator = dimension_ - 2;

          SBPosition3 left = nucleotide->GetSBPosition();
          SBPosition3 right;
          if (nucleotide->pair_ != nullptr) {
            right = nucleotide->pair_->GetSBPosition();
          }
          else {
            right = left;
          }
          SBPosition3 mid = (left + right) / 2;

          float minX = mid[0].getValue();
          float minY = mid[1].getValue();
          float minZ = mid[2].getValue();

          float maxX = (float)bsPos3D.v[0].getValue();
          float maxY = (float)bsPos3D.v[1].getValue();
          float maxZ = (float)bsPos3D.v[2].getValue();

          //testing
          if (bs == nullptr) {
            positions(index, 0) = maxX;
            positions(index, 1) = maxY;
            positions(index, 2) = maxZ;
          }

          float dimMinX = (float)nPos2D.v[0].getValue();
          float dimMinY = (float)nPos2D.v[1].getValue();
          float dimMinZ = (float)nPos2D.v[2].getValue();

          float dimMaxX = minX;
          float dimMaxY = minY;
          float dimMaxZ = minZ;

          positions(index, 0) = dimMinX + dimInterpolator * (dimMaxX - dimMinX);
          positions(index, 1) = dimMinY + dimInterpolator * (dimMaxY - dimMinY);
          positions(index, 2) = dimMinZ + dimInterpolator * (dimMaxZ - dimMinZ);
        }
      }
      else {
      }

      ++i;
    }
  }

  if (!forSelection) {
    SAMSON::displaySpheres(
      nPositions,
      positions.GetArray(),
      radiiV.GetArray(),
      colorsV.GetArray(),
      flags.GetArray());

    SAMSON::displayCylinders(
      nCylinders,
      nPositions,
      indices.GetArray(),
      positions.GetArray(),
      radiiV.GetArray(),
      nullptr,
      colorsV.GetArray(),
      flags.GetArray());
  }
  else {
    SAMSON::displaySpheresSelection(
      nPositions,
      positions.GetArray(),
      radiiV.GetArray(),
      nodeIndices.GetArray());

    SAMSON::displayCylindersSelection(
      nCylinders,
      nPositions,
      indices.GetArray(),
      positions.GetArray(),
      radiiV.GetArray(),
      nullptr,
      nodeIndices.GetArray());

  }
}


void SENucleotidesVisualModel::displayScale9(bool forSelection) {
  ANTNanorobot* nanorobot = model_->nanorobot_;
  auto parts = nanorobot->GetParts();

  unsigned int nCylinders = boost::numeric_cast<unsigned int>(nanorobot->GetNumberOfBaseSegments());
  unsigned int nPositions = boost::numeric_cast<unsigned int>(nanorobot->GetNumberOfJoints());

  ANTAuxiliary::ANTArray<float> positions = ANTAuxiliary::ANTArray<float>(3, nPositions);
  ANTAuxiliary::ANTArray<float> radiiV = ANTAuxiliary::ANTArray<float>(nPositions);
  ANTAuxiliary::ANTArray<unsigned int> flags = ANTAuxiliary::ANTArray<unsigned int>(nPositions);
  ANTAuxiliary::ANTArray<float> colorsV = ANTAuxiliary::ANTArray<float>(4, nPositions);
  ANTAuxiliary::ANTArray<unsigned int> nodeIndices = ANTAuxiliary::ANTArray<unsigned int>(nPositions);
  ANTAuxiliary::ANTArray<unsigned int> indices = ANTAuxiliary::ANTArray<unsigned int>(nCylinders * 2);
  ANTAuxiliary::ANTArray<unsigned int> capData = ANTAuxiliary::ANTArray<unsigned int>(nPositions);

  //todo use local index for visual model

  unsigned int index = 0;

  std::map<ANTJoint*, unsigned int> jointMap;

  unsigned int jid = 0;

  for (auto &part : parts) {
    for (auto const &ent : part.second->GetBaseSegments()) {
      ANTBaseSegment* bs = ent.second;

      ANTCell* cell = bs->GetCell();
      ANTNucleotide* nt = nullptr;
      if (cell->GetType() == BasePair) {
        ANTBasePair* bp = static_cast<ANTBasePair*>(bs->GetCell());
        nt = bp->GetLeftNucleotide();
        if (nt == nullptr) nt = bp->GetRightNucleotide();
      }
      else if (cell->GetType() == LoopPair) {
        ANTLoopPair* lp = static_cast<ANTLoopPair*>(bs->GetCell());
        ANTLoop* loop = lp->GetLeftLoop();
        if (loop == nullptr) {
          loop = lp->GetRightLoop();
        }
        nt = loop->startNt_;
      }

      ANTJoint* source = bs->source_;
      ANTJoint* target = bs->target_;
      if (jointMap.find(source) == jointMap.end()) {
        jointMap.insert(std::make_pair(source, jid));
        SBPosition3 pos3D = source->GetSBPosition();

        positions(jid, 0) = (float)pos3D.v[0].getValue();
        positions(jid, 1) = (float)pos3D.v[1].getValue();
        positions(jid, 2) = (float)pos3D.v[2].getValue();

        colorsV(jid, 0) = model_->baseSegmentColor_(0);
        colorsV(jid, 1) = model_->baseSegmentColor_(1);
        colorsV(jid, 2) = model_->baseSegmentColor_(2);
        colorsV(jid, 3) = model_->baseSegmentColor_(3);

        radiiV(jid) = model_->edgeRadius_;

        capData(jid) = 1;

        SBPosition3 pos2D = source->position2D_;
        SBPosition3 pos1D = source->position1D_;

        make1DVertical(pos1D); //has to be here, because there is not init function for edges and vertices
        if (configuration_->interpolate_dimensions) interpolateDimension(pos1D, pos2D, pos3D, positions, jid);

        ++jid;
      }
      jointMap.insert(std::make_pair(target, jid));
      SBPosition3 pos3D = target->GetSBPosition();

      positions(jid, 0) = (float)pos3D.v[0].getValue();
      positions(jid, 1) = (float)pos3D.v[1].getValue();
      positions(jid, 2) = (float)pos3D.v[2].getValue();

      colorsV(jid, 0) = model_->baseSegmentColor_(0);
      colorsV(jid, 1) = model_->baseSegmentColor_(1);
      colorsV(jid, 2) = model_->baseSegmentColor_(2);
      colorsV(jid, 3) = model_->baseSegmentColor_(3);

      radiiV(jid) = model_->edgeRadius_;

      capData(jid) = 1;

      SBPosition3 pos2D = target->position2D_;
      SBPosition3 pos1D = target->position1D_;

      make1DVertical(pos1D); //has to be here, because there is not init function for edges and vertices
      if (configuration_->interpolate_dimensions) interpolateDimension(pos1D, pos2D, pos3D, positions, jid);

      ++jid;
      
      if (nt != nullptr) {
        unsigned int jid1 = jointMap[source];
        unsigned int jid2 = jointMap[target];

        indices(2 * index + 0) = jid1;
        indices(2 * index + 1) = jid2;

        nodeIndices(jid1) = nt->residue_->getNodeIndex();
        nodeIndices(jid2) = nt->residue_->getNodeIndex();

        flags(jid1) = nt->residue_->getInheritedFlags() | getInheritedFlags();
        flags(jid2) = nt->residue_->getInheritedFlags() | getInheritedFlags();

        if (showMeltingTemperature_ || showGibbsFreeEnergy_) {

          colorsV(jid1, 0) = propertyJointColorsV_(jid1, 0);
          colorsV(jid1, 1) = propertyJointColorsV_(jid1, 1);
          colorsV(jid1, 2) = propertyJointColorsV_(jid1, 2);
          colorsV(jid1, 3) = propertyJointColorsV_(jid1, 3);

          colorsV(jid2, 0) = propertyJointColorsV_(jid2, 0);
          colorsV(jid2, 1) = propertyJointColorsV_(jid2, 1);
          colorsV(jid2, 2) = propertyJointColorsV_(jid2, 2);
          colorsV(jid2, 3) = propertyJointColorsV_(jid2, 3);
        }
      }
      ++index;
    }
  }

  if (!forSelection) {
    //SAMSON::displaySpheres(
    //  nPositions,
    //  positions.GetArray(),
    //  radiiV.GetArray(),
    //  colorsV.GetArray(),
    //  flags.GetArray());

    SAMSON::displayCylinders(
      nCylinders,
      nPositions,
      indices.GetArray(),
      positions.GetArray(),
      radiiV.GetArray(),
      capData.GetArray(),
      colorsV.GetArray(),
      flags.GetArray());
  }
  else {
    //SAMSON::displaySpheresSelection(
    //  nPositions,
    //  positions.GetArray(),
    //  radiiV.GetArray(),
    //  nodeIndices.GetArray());

    SAMSON::displayCylindersSelection(
      nCylinders,
      nPositions,
      indices.GetArray(),
      positions.GetArray(),
      radiiV.GetArray(),
      nullptr,
      nodeIndices.GetArray());
  }
}


void SENucleotidesVisualModel::displayText(SBPosition3 pos, string text)
{
  SAMSON::displayText(
    text,
    pos,
    QFont(QString("Helvetica"), 60),
    overlayTextColor_);
}

void SENucleotidesVisualModel::displayBasePairConnections(float scale)
{
  //weird bug. only part of the bps are depicted. it seems random.
  float nucleotideERadius = model_->nucleotideBaseRadius_;

  float iv = 1.0f - (ceil(scale) - scale);

  auto singleStrands = model_->nanorobot_->GetSingleStrands();

  unsigned int numPairedNts = 0;
  std::map<ANTNucleotide*, unsigned int> ntMap;
  //determine how many nucleotides have pairs 
  for (auto & sid : singleStrands) {
    ANTSingleStrand* singleStrand = sid.second;
    MSVNucleotideList nucleotides = singleStrand->nucleotides_;

    for (auto & nit : nucleotides) {
      ANTNucleotide* nucleotide = nit.second;
      if (nucleotide->pair_ != nullptr) {
        ntMap.insert(make_pair(nucleotide, numPairedNts));
        ++numPairedNts;
      }
    }
  }

  unsigned int nPositions = boost::numeric_cast<unsigned int>(ntMap.size());
  unsigned int nCylinders = nPositions / 2;

  ANTAuxiliary::ANTArray<float> positions = ANTAuxiliary::ANTArray<float>(3, nPositions);
  ANTAuxiliary::ANTArray<unsigned int> indices = ANTAuxiliary::ANTArray<unsigned int>(nCylinders * 2);
  ANTAuxiliary::ANTArray<float> radii = ANTAuxiliary::ANTArray<float>(nPositions);
  ANTAuxiliary::ANTArray<float> colors = ANTAuxiliary::ANTArray<float>(4, nPositions);
  ANTAuxiliary::ANTArray<unsigned int> flags = ANTAuxiliary::ANTArray<unsigned int>(nPositions);

  unsigned int posIndex = 0;
  std::vector<unsigned int> registerIndices;
  for (auto &p: ntMap) {
    ANTNucleotide* ntPaired = p.first;
    int here = ntPaired->id_;
    unsigned int numNtPaired = p.second;
    colors.SetRow(numNtPaired, model_->getBaseColor(ntPaired->type_));
    SBPosition3 iPosStart;
    SBPosition3 iPosEnd;

    if (scale >= 3 && scale < 4) {
      iPosStart = ntPaired->GetSBBackboneCenter();
      iPosEnd = ntPaired->GetSBSidechainCenter();
    }
    else if (scale >= 4 && scale < 6) {
      iPosStart = ntPaired->GetSBSidechainCenter();
      iPosEnd = ntPaired->GetSBSidechainCenter();
    }
    else if (scale >= 6 && scale < 7) {
      iPosStart = ntPaired->GetSBSidechainCenter();
      iPosEnd = ntPaired->GetSBBackboneCenter();
    }
    else if (scale >= 7 && scale < 8) {
      iPosStart = ntPaired->GetSBBackboneCenter();
      iPosEnd = (ntPaired->GetSBBackboneCenter() + ntPaired->pair_->GetSBBackboneCenter()) / 2;
    }

    SBPosition3 iPos = iPosStart + iv * (iPosEnd - iPosStart);

    positions(numNtPaired, 0) = iPos[0].getValue();
    positions(numNtPaired, 1) = iPos[1].getValue();
    positions(numNtPaired, 2) = iPos[2].getValue();

    radii(numNtPaired) = nucleotideERadius;
    flags(numNtPaired) = 0;

    if (std::find(registerIndices.begin(), registerIndices.end(), ntMap[ntPaired->pair_]) == registerIndices.end()) {
      // we only need to insert the indices once per pair
      indices(2 * posIndex) = numNtPaired;
      indices(2 * posIndex + 1) = ntMap[ntPaired->pair_];
      registerIndices.push_back(numNtPaired);
      ++posIndex;
    }
  }

  SAMSON::displayCylinders(
    nCylinders,
    nPositions,
    indices.GetArray(),
    positions.GetArray(),
    radii.GetArray(),
    nullptr,
    colors.GetArray(),
    flags.GetArray()
  );
}

void SENucleotidesVisualModel::displayPossibleCrossovers(bool selection) {
  if (scale_ > 7.5) return;

  int numCrossovers = boost::numeric_cast<int>(possibleCrossovers_.size());

  unsigned int nCylinders = 2 * numCrossovers * 2;
  unsigned int nPositions = 2 * nCylinders;
  unsigned int* indexData = new unsigned int[nPositions];
  float *positionData = new float[3 * nPositions];
  float *radiusData = new float[nPositions];
  unsigned int *capData = new unsigned int[nPositions];
  float *colorData = new float[4 * nPositions];
  unsigned int *flagData = new unsigned int[nPositions];
  unsigned int *nodeIndexData = new unsigned int[nPositions];

  ANTAuxiliary::ANTArray<float> start_color = ANTAuxiliary::ANTArray<float>(4);
  ANTAuxiliary::ANTArray<float> end_color = ANTAuxiliary::ANTArray<float>(4);

  int i = 0;
  for (auto xo : possibleCrossovers_) {

    ANTNucleotide * leftNucleotide = xo.first;
    ANTNucleotide * rightNucleotide = xo.second;

    int leftColorNum = leftNucleotide->strand_->id_ % model_->numStapleColors_;
    int rightColorNum = rightNucleotide->strand_->id_ % model_->numStapleColors_;

    start_color(0) = model_->stapleColors_(leftColorNum, 0);
    start_color(1) = model_->stapleColors_(leftColorNum, 1);
    start_color(2) = model_->stapleColors_(leftColorNum, 2);
    start_color(3) = model_->stapleColors_(leftColorNum, 3);

    end_color(0) = model_->stapleColors_(rightColorNum, 0);
    end_color(1) = model_->stapleColors_(rightColorNum, 1);
    end_color(2) = model_->stapleColors_(rightColorNum, 2);
    end_color(3) = model_->stapleColors_(rightColorNum, 3);

    SBPosition3 xo_start;
    SBPosition3 xo_end;

    if (scale_ > 3.5f && scale_ < 6.5f) {
      xo_start = leftNucleotide->GetSBSidechainCenter();
      xo_end = rightNucleotide->GetSBSidechainCenter();
    }
    else {
      xo_start = leftNucleotide->GetSBBackboneCenter();
      xo_end = rightNucleotide->GetSBBackboneCenter();
    }

    //outgoing
    if (leftNucleotide->residue_->isVisible()) {
      SBPosition3 out_start = xo_end - xo_start;
      SBPosition3 out_end = xo_end - xo_start;
      out_start *= 0.2;
      out_end *= 0.45;
      out_start += xo_start;
      out_end += xo_start;
      //MSVDisplayHelper::displayArrow(out_start, out_end, leftNucleotide->residue_->getNodeIndex(), end_color.GetArray(), true);

      SBPosition3 shaft = out_end - out_start;
      shaft *= 0.55f;
      shaft += out_start;

      positionData[12 * i + 0] = out_start.v[0].getValue();
      positionData[12 * i + 1] = out_start.v[1].getValue();
      positionData[12 * i + 2] = out_start.v[2].getValue();
      positionData[12 * i + 3] = shaft.v[0].getValue();
      positionData[12 * i + 4] = shaft.v[1].getValue();
      positionData[12 * i + 5] = shaft.v[2].getValue();
      positionData[12 * i + 6] = shaft.v[0].getValue();
      positionData[12 * i + 7] = shaft.v[1].getValue();
      positionData[12 * i + 8] = shaft.v[2].getValue();
      positionData[12 * i + 9] = out_end.v[0].getValue();
      positionData[12 * i + 10] = out_end.v[1].getValue();
      positionData[12 * i + 11] = out_end.v[2].getValue();

      indexData[4 * i + 0] = 4 * i + 0;
      indexData[4 * i + 1] = 4 * i + 1;
      indexData[4 * i + 2] = 4 * i + 2;
      indexData[4 * i + 3] = 4 * i + 3;

      radiusData[4 * i + 0] = 60;
      radiusData[4 * i + 1] = 60;
      radiusData[4 * i + 2] = 100;
      radiusData[4 * i + 3] = 0;

      capData[4 * i + 0] = 1;
      capData[4 * i + 1] = 1;
      capData[4 * i + 2] = 1;
      capData[4 * i + 3] = 1;

      nodeIndexData[4 * i + 0] = leftNucleotide->residue_->getNodeIndex();
      nodeIndexData[4 * i + 1] = leftNucleotide->residue_->getNodeIndex();
      nodeIndexData[4 * i + 2] = leftNucleotide->residue_->getNodeIndex();
      nodeIndexData[4 * i + 3] = leftNucleotide->residue_->getNodeIndex();

      colorData[16 * i + 0] = end_color(0);
      colorData[16 * i + 1] = end_color(1);
      colorData[16 * i + 2] = end_color(2);
      colorData[16 * i + 3] = end_color(3);
      colorData[16 * i + 4] = end_color(0);
      colorData[16 * i + 5] = end_color(1);
      colorData[16 * i + 6] = end_color(2);
      colorData[16 * i + 7] = end_color(3);
      colorData[16 * i + 8] = end_color(0);
      colorData[16 * i + 9] = end_color(1);
      colorData[16 * i + 10] = end_color(2);
      colorData[16 * i + 11] = end_color(3);
      colorData[16 * i + 12] = end_color(0);
      colorData[16 * i + 13] = end_color(1);
      colorData[16 * i + 14] = end_color(2);
      colorData[16 * i + 15] = end_color(3);

      flagData[4 * i + 0] = leftNucleotide->residue_->getInheritedFlags() | getInheritedFlags();
      flagData[4 * i + 1] = leftNucleotide->residue_->getInheritedFlags() | getInheritedFlags();
      flagData[4 * i + 2] = leftNucleotide->residue_->getInheritedFlags() | getInheritedFlags();
      flagData[4 * i + 3] = leftNucleotide->residue_->getInheritedFlags() | getInheritedFlags();

      ++i;

      //ANTAuxiliary::log(QString::number(leftNucleotide->residue_->getNodeIndex()));
    }


    //incoming
    if (rightNucleotide->residue_->isVisible()) {
      SBPosition3 in_start = xo_end - xo_start;
      SBPosition3 in_end = xo_end - xo_start;
      in_start *= 0.55;
      in_end *= 0.8;
      in_start += xo_start;
      in_end += xo_start;
      //MSVDisplayHelper::displayArrow(in_end, in_start, rightNucleotide->residue_->getNodeIndex(), start_color.GetArray());
      SBPosition3 shaft = in_end - in_start;
      shaft *= 0.55f;
      shaft += in_start;

      positionData[12 * i + 0] = in_start.v[0].getValue();
      positionData[12 * i + 1] = in_start.v[1].getValue();
      positionData[12 * i + 2] = in_start.v[2].getValue();
      positionData[12 * i + 3] = shaft.v[0].getValue();
      positionData[12 * i + 4] = shaft.v[1].getValue();
      positionData[12 * i + 5] = shaft.v[2].getValue();
      positionData[12 * i + 6] = shaft.v[0].getValue();
      positionData[12 * i + 7] = shaft.v[1].getValue();
      positionData[12 * i + 8] = shaft.v[2].getValue();
      positionData[12 * i + 9] = in_end.v[0].getValue();
      positionData[12 * i + 10] = in_end.v[1].getValue();
      positionData[12 * i + 11] = in_end.v[2].getValue();

      indexData[4 * i + 0] = 4 * i + 0;
      indexData[4 * i + 1] = 4 * i + 1;
      indexData[4 * i + 2] = 4 * i + 2;
      indexData[4 * i + 3] = 4 * i + 3;

      radiusData[4 * i + 0] = 60;
      radiusData[4 * i + 1] = 60;
      radiusData[4 * i + 2] = 100;
      radiusData[4 * i + 3] = 0;

      capData[4 * i + 0] = 1;
      capData[4 * i + 1] = 1;
      capData[4 * i + 2] = 1;
      capData[4 * i + 3] = 1;

      nodeIndexData[4 * i + 0] = rightNucleotide->residue_->getNodeIndex();
      nodeIndexData[4 * i + 1] = rightNucleotide->residue_->getNodeIndex();
      nodeIndexData[4 * i + 2] = rightNucleotide->residue_->getNodeIndex();
      nodeIndexData[4 * i + 3] = rightNucleotide->residue_->getNodeIndex();

      colorData[16 * i + 0] = start_color(0);
      colorData[16 * i + 1] = start_color(1);
      colorData[16 * i + 2] = start_color(2);
      colorData[16 * i + 3] = start_color(3);
      colorData[16 * i + 4] = start_color(0);
      colorData[16 * i + 5] = start_color(1);
      colorData[16 * i + 6] = start_color(2);
      colorData[16 * i + 7] = start_color(3);
      colorData[16 * i + 8] = start_color(0);
      colorData[16 * i + 9] = start_color(1);
      colorData[16 * i + 10] = start_color(2);
      colorData[16 * i + 11] = start_color(3);
      colorData[16 * i + 12] = start_color(0);
      colorData[16 * i + 13] = start_color(1);
      colorData[16 * i + 14] = start_color(2);
      colorData[16 * i + 15] = start_color(3);

      flagData[4 * i + 0] = rightNucleotide->residue_->getInheritedFlags() | getInheritedFlags();
      flagData[4 * i + 1] = rightNucleotide->residue_->getInheritedFlags() | getInheritedFlags();
      flagData[4 * i + 2] = rightNucleotide->residue_->getInheritedFlags() | getInheritedFlags();
      flagData[4 * i + 3] = rightNucleotide->residue_->getInheritedFlags() | getInheritedFlags();
      ++i;

    }


  }

  if (!selection) {
    SAMSON::displayCylinders(nCylinders, nPositions, indexData, positionData, radiusData, capData, colorData, flagData);
  }
  else {
    SAMSON::displayCylindersSelection(nCylinders, nPositions, indexData, positionData, radiusData, capData, nodeIndexData);
  }
  delete[] indexData;
  delete[] radiusData;
  delete[] capData;
  delete[] colorData;
  delete[] nodeIndexData;
  delete[] flagData;

}

void SENucleotidesVisualModel::displayNucleotideBasis() {
  auto singleStrands = model_->nanorobot_->GetSingleStrands();
  unsigned int numNucleotidesV = model_->nanorobot_->GetNumberOfNucleotides();
  unsigned int numNucleotidesE = boost::numeric_cast<unsigned int>(numNucleotidesV - singleStrands.size());

  for (auto & sid : singleStrands) {
    ANTSingleStrand* singleStrand = sid.second;
    MSVNucleotideList nucleotides = singleStrand->nucleotides_;

    for (auto & nit : nucleotides) {
      ANTNucleotide* nucleotide = nit.second;

      SBVector3 e1 = ANTVectorMath::CreateSBVector(nucleotide->e1_);
      SBVector3 e2 = ANTVectorMath::CreateSBVector(nucleotide->e2_);
      SBVector3 e3 = ANTVectorMath::CreateSBVector(nucleotide->e3_);

      float e1_c[] = { 0, 0, 1, 1 };
      float e2_c[] = { 1, 0, 0, 1 };
      float e3_c[] = { 0, 1, 0, 1 };

      MSVDisplayHelper::displayVector(e1, nucleotide->GetSBBackboneCenter(), e1_c, 200);
      MSVDisplayHelper::displayVector(e2, nucleotide->GetSBBackboneCenter(), e2_c, 200);
      MSVDisplayHelper::displayVector(e3, nucleotide->GetSBBackboneCenter(), e3_c, 200);
    }
  }

  //temp disabled
  //SAMSON::displayCylinders(
  //  numNucleotidesE,
  //  numNucleotidesV,
  //  nucleotideEIndexData_.GetArray(),
  //  nucleotideSideChainVPositions_.GetArray(),
  //  nucleotideERadii_.GetArray(),
  //  nullptr,
  //  nucleotideEColorData_.GetArray(),
  //  nucleotideVFlagData_.GetArray());
}

void SENucleotidesVisualModel::displaySkips(float scale, float dimension)
{
  ANTNanorobot* nanorobot = model_->nanorobot_;
  auto parts = nanorobot->GetParts();

  std::vector<ANTNucleotide*> skips;

  for (auto &part : parts) {

    for (auto const &ent : part.second->GetBaseSegments()) {

      ANTBaseSegment* bs = ent.second;

      if (bs->GetCell()->GetType() == CellType::SkipPair) {
        ANTSkipPair * sp = static_cast<ANTSkipPair*>(bs->GetCell());
        ANTNucleotide* left = sp->GetLeftSkip();
        ANTNucleotide* right = sp->GetRightSkip();
        if (left != nullptr) skips.push_back(left);
        if (right != nullptr) skips.push_back(right);
      }
    }
  }

  unsigned int nPositions = boost::numeric_cast<unsigned int>(skips.size());
  //ANTAuxiliary::log(QString("numSkips (vm) "), false);
  //ANTAuxiliary::log(QString::number(nPositions), false);

  ANTAuxiliary::ANTArray<float> positions = ANTAuxiliary::ANTArray<float>(3, nPositions);
  ANTAuxiliary::ANTArray<float> radiiV = ANTAuxiliary::ANTArray<float>(nPositions);
  ANTAuxiliary::ANTArray<unsigned int> flags = ANTAuxiliary::ANTArray<unsigned int>(nPositions);
  ANTAuxiliary::ANTArray<float> colorsV = ANTAuxiliary::ANTArray<float>(4, nPositions);

  for (unsigned int index = 0; index < nPositions; index++) {

    ANTNucleotide* skip = skips[index];
    SBPosition3 pos;

    if (dimension > 1 && dimension <= 1.5) {
      pos = skip->GetSBPosition1D();
    }
    else if (dimension > 1.5 && dimension <= 2.5) {
      pos = skip->GetSBPosition2D();
    }
    else if (dimension > 2.5 && dimension <= 3) {
      pos = skip->GetSBPosition();
    }

    positions(index, 0) = pos[0].getValue();
    positions(index, 1) = pos[1].getValue();
    positions(index, 2) = pos[2].getValue();

    colorsV(index, 0) = 1.0f;
    colorsV(index, 1) = 0.2f;
    colorsV(index, 2) = 0.5f;
    colorsV(index, 3) = 1.0f;

    radiiV(index) = 200;
    flags(index) = 0;

  }

  SAMSON::displaySpheres(
    nPositions,
    positions.GetArray(),
    radiiV.GetArray(),
    colorsV.GetArray(),
    flags.GetArray());


}

void SENucleotidesVisualModel::interpolateDimension(SBPosition3 pos1D, SBPosition3 pos2D, SBPosition3 pos3D, ANTAuxiliary::ANTArray<float> & positions, int index)
{
  //allow for backbone and sidechain positions
  if (dimension_ >= 1 && dimension_ < 2) {
    float interpolator = dimension_ - 1;

    float minX = (float)pos1D.v[0].getValue();
    float minY = (float)pos1D.v[1].getValue();
    float minZ = (float)pos1D.v[2].getValue();

    float maxX = (float)pos2D.v[0].getValue();
    float maxY = (float)pos2D.v[1].getValue();
    float maxZ = (float)pos2D.v[2].getValue();

    positions(index, 0) = minX + interpolator * (maxX - minX);
    positions(index, 1) = minY + interpolator * (maxY - minY);
    positions(index, 2) = minZ + interpolator * (maxZ - minZ);
  }
  else if (dimension_ >= 2 && dimension_ <= 3) {
    float interpolator = dimension_ - 2;

    float minX = (float)pos2D.v[0].getValue();
    float minY = (float)pos2D.v[1].getValue();
    float minZ = (float)pos2D.v[2].getValue();

    float maxX = positions(index, 0);
    float maxY = positions(index, 1);
    float maxZ = positions(index, 2);

    positions(index, 0) = minX + interpolator * (maxX - minX);
    positions(index, 1) = minY + interpolator * (maxY - minY);
    positions(index, 2) = minZ + interpolator * (maxZ - minZ);
  }
  else {

  }
}

void SENucleotidesVisualModel::interpolateAtomVPosDimension(ANTAuxiliary::ANTArray<float> & positions, int index)
{

  if (dimension_ >= 1 && dimension_ < 2) {
    float interpolator = dimension_ - 1;

    float minX = atomVPositions1D_(index, 0);
    float minY = atomVPositions1D_(index, 1);
    float minZ = atomVPositions1D_(index, 2);

    float maxX = atomVPositions2D_(index, 0);
    float maxY = atomVPositions2D_(index, 1);
    float maxZ = atomVPositions2D_(index, 2);

    positions(index, 0) = minX + interpolator * (maxX - minX);
    positions(index, 1) = minY + interpolator * (maxY - minY);
    positions(index, 2) = minZ + interpolator * (maxZ - minZ);
  }
  else if (dimension_ >= 2 && dimension_ <= 3) {
    float interpolator = dimension_ - 2;

    float minX = atomVPositions2D_(index, 0);
    float minY = atomVPositions2D_(index, 1);
    float minZ = atomVPositions2D_(index, 2);

    float maxX = positions(index, 0);
    float maxY = positions(index, 1);
    float maxZ = positions(index, 2);

    positions(index, 0) = minX + interpolator * (maxX - minX);
    positions(index, 1) = minY + interpolator * (maxY - minY);
    positions(index, 2) = minZ + interpolator * (maxZ - minZ);
  }
  else {
    //positions(index, 0) = (float)pos3D.v[0].getValue();
    //positions(index, 1) = (float)pos3D.v[1].getValue();
    //positions(index, 2) = (float)pos3D.v[2].getValue();
  }
}

void SENucleotidesVisualModel::interpolateAtomEPosDimension(ANTAuxiliary::ANTArray<float> & positions, int index)
{

  if (dimension_ >= 1 && dimension_ < 2) {
    float interpolator = dimension_ - 1;

    float minX = atomEPositions1D_(index, 0);
    float minY = atomEPositions1D_(index, 1);
    float minZ = atomEPositions1D_(index, 2);

    float maxX = atomEPositions2D_(index, 0);
    float maxY = atomEPositions2D_(index, 1);
    float maxZ = atomEPositions2D_(index, 2);

    positions(index, 0) = minX + interpolator * (maxX - minX);
    positions(index, 1) = minY + interpolator * (maxY - minY);
    positions(index, 2) = minZ + interpolator * (maxZ - minZ);
  }
  else if (dimension_ >= 2 && dimension_ <= 3) {
    float interpolator = dimension_ - 2;

    float minX = atomEPositions2D_(index, 0);
    float minY = atomEPositions2D_(index, 1);
    float minZ = atomEPositions2D_(index, 2);

    float maxX = positions(index, 0);
    float maxY = positions(index, 1);
    float maxZ = positions(index, 2);

    positions(index, 0) = minX + interpolator * (maxX - minX);
    positions(index, 1) = minY + interpolator * (maxY - minY);
    positions(index, 2) = minZ + interpolator * (maxZ - minZ);
  }
  else {
    //positions(index, 0) = (float)pos3D.v[0].getValue();
    //positions(index, 1) = (float)pos3D.v[1].getValue();
    //positions(index, 2) = (float)pos3D.v[2].getValue();
  }
}


void SENucleotidesVisualModel::sort1DPos() {
  auto strands = model_->nanorobot_->GetSingleStrands();

  int counter = 0;
  for (auto &ss : strands) {
    ANTSingleStrand* singleStrand = ss.second;

    int sortedId = counter;
    if (sorting_index_ == ANTAuxiliary::StrandID) {
      // id of strands
      sortedId = counter;
    }
    else if (sorting_index_ == ANTAuxiliary::Length) {
      // gc content
      sortedId = lengthMap_.at(singleStrand);
    }
    else if (sorting_index_ == ANTAuxiliary::GCCont) {
      // melting temp
      sortedId = gcMap_.at(singleStrand);
    }
    else if (sorting_index_ == ANTAuxiliary::MeltTemp) {
      //asdf
      // gibbs free energy
      sortedId = meltingTempMap_.at(singleStrand);
    }
    else if (sorting_index_ == ANTAuxiliary::Gibbs) {
      // length
      sortedId = gibbsEnergMap_.at(singleStrand);
    }

    SBQuantity::nanometer x_pos(sortedId * ANTConstants::DH_DIAMETER - strands.size());
    for (auto &n : singleStrand->nucleotides_) {
      ANTNucleotide* nt = n.second;
      SBPosition3 pos = nt->GetSBPosition1D();
      SBPosition3 t_vec = SBPosition3();
      t_vec[1] = x_pos - pos[0];
      pos[0] = x_pos;
      nt->SetPosition1D(pos);

      for (auto &a : nt->atomList_) {
        ANTAtom* at = a.second;
        at->position1D_ += t_vec;
      }
    }
    counter++;
  }
  initAtomVs();
  initAtomEs();
  SAMSON::requestViewportUpdate();
}

void SENucleotidesVisualModel::make1DVertical(SBPosition3 & pos1D)
{
  SBPosition3 temp_pos = pos1D;
  pos1D[1] = temp_pos[0];
  pos1D[0] = temp_pos[1];
}



void SENucleotidesVisualModel::display() {

  bool show_nucleobase_text = configuration_->show_nucleobase_text;
  bool show_overlay = configuration_->show_overlay;
  int animation_step_size = configuration_->animation_step_size;
  float min_melting_temp = configuration_->min_melting_temp;


  if (model_ == nullptr) return;

  if (!allScalesInitialized_) {
    return;
  }

  if (show_overlay) displayOverlay();

  if (configuration_->detect_possible_crossovers && configuration_->display_possible_crossovers) {
    displayPossibleCrossovers();
  }

  if (configuration_->display_nucleotide_basis) {
    displayNucleotideBasis();
    return;
  }

  scale_ += animation_step_size;

  if (scale_ > 9.9) scale_ = 9.9;

  float interpolated = 1.0f - (ceil(scale_) - scale_);

  if (scale_ < (float)ALL_ATOMS_STICKS) {
    //0 - 9
    if (configuration_->use_atomic_details) {
      displayScale0to1(interpolated);
    }
  }
  else if (scale_ < (float)ALL_ATOMS_BALLS) {
    //10 - 19
    if (configuration_->use_atomic_details) {
      displayScale1to2(interpolated);
    }
  }
  else if (scale_ < (float)NUCLEOTIDES_BACKBONE) {
    //20 - 29
    if (configuration_->use_atomic_details) {
      displayScale2to3(interpolated);
    }
  }
  else if (scale_ < (float)NUCLEOTIDES_SIDECHAIN) {
    //30 - 39
    displayScale3to4(interpolated);
    if (configuration_->display_base_pairing) displayBasePairConnections(scale_);
    //displaySkips(scale_, dimension_);
  }
  else if (scale_ < (float)NUCLEOTIDES_SCAFFOLD) {
    //40 - 49
    displayScale4to5(interpolated);
    if (configuration_->display_base_pairing) displayBasePairConnections(scale_);
    //displaySkips(scale_, dimension_);
  }
  else if (scale_ < (float)STAPLES_SCAFFOLD_PLAITING_SIDECHAIN) {
    //50 - 59
    displayScale5to6(interpolated);
    if (configuration_->display_base_pairing) displayBasePairConnections(scale_);
    //displaySkips(scale_, dimension_);
  }
  else if (scale_ < (float)STAPLES_SCAFFOLD_PLAITING_BACKBONE) {
    //60 - 69
    displayScale6to7(interpolated);
    if (configuration_->display_base_pairing) displayBasePairConnections(scale_);
    //displaySkips(scale_, dimension_);
  }
  else if (scale_ < (float)DOUBLE_HELIX_PATH) {
    //70 - 79
    displayScale7to8(interpolated); //todo
    if (configuration_->display_base_pairing) displayBasePairConnections(scale_);

  }
  else if (scale_ < (float)EDGES_VERTICES) {
    //80 - 89
    displayScale8to9(interpolated);
  }
  else if (scale_ < (float)OBJECTS) {
    displayScale9();

    /*auto parts = model_->nanorobot_->GetParts();
    for (auto &pid : parts) {
    ANTPart * part = pid.second;
    MSVDisplayHelper::displayPolyhedron(part);
    }*/
  }
  else {
  }


}

void SENucleotidesVisualModel::displayForShadow() {

  // SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop in order to compute shadows. 
  // Implement this function so that your visual model can cast shadows to other objects in SAMSON, for example thanks to the utility
  // functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)

}

void SENucleotidesVisualModel::displayForSelection() {

  // SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop in order to perform object picking.
  // Instead of rendering colors, your visual model is expected to write the index of a data graph node (obtained with getIndex()).
  // Implement this function so that your visual model can be selected (if you render its own index) or can be used to select other objects (if you render 
  // the other objects' indices), for example thanks to the utility functions provided by SAMSON (e.g. displaySpheresSelection, displayTrianglesSelection, etc.)

  if (model_ == nullptr) return;

  if (scale_ >= 9.9f) scale_ = 9.9f;

  float interpolated = 1.0f - (ceil(scale_) - scale_);

  if (configuration_->display_possible_crossovers) displayPossibleCrossovers(true);

  if (scale_ < (float)ALL_ATOMS_STICKS) {
    //0 - 9
    //displayScale0to1(interpolated, true);
  }
  else if (scale_ < (float)ALL_ATOMS_BALLS) {
    //10 - 19
    //displayScale1to2(interpolated, true);
  }
  else if (scale_ < (float)NUCLEOTIDES_BACKBONE) {
    //20 - 29
    //displayScale2to3(interpolated, true);
  }
  else if (scale_ < (float)NUCLEOTIDES_SIDECHAIN) {
    //30 - 39
    displayScale3to4(interpolated, true);
    //displayBasePairConnections();

  }
  else if (scale_ < (float)NUCLEOTIDES_SCAFFOLD) {
    //40 - 49
    displayScale4to5(interpolated, true);
    //displayBasePairConnections();
  }
  else if (scale_ < (float)STAPLES_SCAFFOLD_PLAITING_SIDECHAIN) {
    //50 - 59
    displayScale5to6(interpolated, true);
    //displayBasePairConnections();
  }
  else if (scale_ < (float)STAPLES_SCAFFOLD_PLAITING_BACKBONE) {
    //60 - 69
    displayScale6to7(interpolated, true);
    //displayBasePairConnections();
  }
  else if (scale_ < (float)DOUBLE_HELIX_PATH) {
    //70 - 79
    displayScale7to8(interpolated, true); //todo
  }
  else if (scale_ < (float)EDGES_VERTICES) {
    //80 - 89
    displayScale8to9(interpolated, true);
  }
  else if (scale_ < (float)OBJECTS) {
    displayScale9(true);
  }
  else {

  }
}

void SENucleotidesVisualModel::displayOverlay()
{
  for (int i = 0; i < overlayTexts_.size(); i++) {
    SAMSON::displayText(overlayTexts_[i], overlayPositions_[i], QFont(), overlayTextColor_);

  }

}

void SENucleotidesVisualModel::expandBounds(SBIAPosition3& bounds) const {

  // SAMSON Element generator pro tip: this function is called by SAMSON to determine the model's spatial bounds. 
  // When this function returns, the bounds interval vector should contain the visual model. 

}

void SENucleotidesVisualModel::collectAmbientOcclusion(const SBPosition3& boxOrigin, const SBPosition3& boxSize, unsigned int nCellsX, unsigned int nCellsY, unsigned int nCellsZ, float* ambientOcclusionData) {

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

float SENucleotidesVisualModel::getNucleotideRadius(ANTNucleotide* nt)
{
  ANTCell* cell = nt->GetBaseSegment()->GetCell();
  if (cell->GetType() == CellType::BasePair) {
    return model_->nucleotideVRadius_;
  }
  else if (cell->GetType() == CellType::LoopPair) {
    return model_->nucleotideVRadius_ / 2;
  }
  else if (cell->GetType() == CellType::SkipPair) {
    return model_->nucleotideBaseRadius_;
  }
  else {
    return model_->nucleotideVRadius_;
  }
}

void SENucleotidesVisualModel::onBaseEvent(SBBaseEvent* baseEvent) {

  // SAMSON Element generator pro tip: implement this function if you need to handle base events (e.g. when a node for which you provide a visual representation emits a base signal, such as when it is erased)
  //std::string message = baseEvent->getTypeString(baseEvent->getType()) + " baseEvent";
  //auto eventType = baseEvent->getType();
  //if (!eventType == SBBaseEvent::VisibilityFlagChanged) {
  //  ANTAuxiliary::log(message);
  //}

  if (baseEvent->getType() == SBBaseEvent::SelectionFlagChanged || baseEvent->getType() == SBBaseEvent::HighlightingFlagChanged) {
    //if (ntMap_.size() > 0) { // this is a hack to workaround the selection after modification bug
    //  int atomIndex = 0;
    //  int numResiduesSelected = 0;
    //  int numResidues = residueIndexer_.size();

    //  for (unsigned int i = 0; i < residueIndexer_.size(); i++) {
    //    ANTNucleotide* nt = ntMap_.left.at(i);
    //    nucleotideVFlagData_(i) = nt->residue_->getInheritedFlags() | getInheritedFlags();
    //    nucleotideEFlagData_(i) = nt->residue_->getInheritedFlags() | getInheritedFlags();

    //    //if (scale_ <= Scale::NUCLEOTIDES_BACKBONE) {
    //      SBNodeIndexer atoms;
    //      nt->residue_->getNodes(atoms, SBNode::IsType(SBNode::Atom));
    //      SB_FOR(SBNode* node, atoms) {
    //        //SBAtom* atom = static_cast<SBAtom*>(node);
    //        atomVFlagData_(atomIndex) = nucleotideVFlagData_(i);
    //        ++atomIndex;
    //      }
    //      if (nt->residue_->isSelected()) numResiduesSelected++;
    //    //}
    //  }

    //  if (numResiduesSelected > 6) {
    //    strandsHighlighted_ = true;
    //  }
    //  else {
    //    strandsHighlighted_ = false;

    //  }
    //}
  }

  //for creating crossovers
  if (configuration_->display_possible_crossovers) {
    //return; // currently deactivated since not working
    if (baseEvent->getType() == SBBaseEvent::SelectionFlagChanged) {

      //todo replace this with signals of QObject

      //get the selected nucleotide (which is the origin of the arrow for possible xover)
      ANTNucleotide* firstNucleotide = nullptr;
      ANTNucleotide* secondNucleotide = nullptr;

      std::map<ANTSingleStrand*, std::vector<ANTNucleotide*>> selectedNucleotides;
      unsigned int numSelected = model_->getSelectedNucletides(selectedNucleotides);

      if (numSelected != 1) return;

      firstNucleotide = selectedNucleotides.begin()->second.at(0);

      //get the second nucleotide
      for (auto & xo : possibleCrossovers_) {
        if (xo.first == firstNucleotide) {
          secondNucleotide = xo.second;
        }
        else if (xo.second == firstNucleotide) {
          secondNucleotide = xo.first;
        }
      }

      //only continue when both strands are part of possible crossover location
      if (firstNucleotide == nullptr || secondNucleotide == nullptr) {
        return;
      }

      ANTSingleStrand* firstSingleStrand = firstNucleotide->strand_;
      ANTSingleStrand* secondSingleStrand = secondNucleotide->strand_;

      //break first strand (this should be using the modelcontroller code. refactoring needed)
      SBPointer<SBChain> oldFirstChain = firstSingleStrand->chain_;

      auto first_chain_ids = model_->nanorobot_->BreakStrand(firstNucleotide, firstNucleotide->next_);
      SBPointer<SBChain> new_firstChain1 = first_chain_ids.first->chain_;
      SBPointer<SBChain> new_firstChain2 = first_chain_ids.second->chain_;
      new_firstChain1->create();
      new_firstChain2->create();

      SAMSON::beginHolding("Splitting strand");

      oldFirstChain->getParent()->addChild(new_firstChain1());
      oldFirstChain->getParent()->addChild(new_firstChain2());
      oldFirstChain->getParent()->removeChild(oldFirstChain());
      SAMSON::endHolding();

      //break second strand (this should be using the modelcontroller code. refactoring needed)
      SBPointer<SBChain> oldSecondChain = secondSingleStrand->chain_;

      // store pointers to nucleotides for easy merging later
      ANTNucleotide* connectStart = firstNucleotide;
      ANTNucleotide* connectEnd = secondNucleotide;

      auto second_chain_ids = model_->nanorobot_->BreakStrand(secondNucleotide->prev_, secondNucleotide);
      SBPointer<SBChain> new_secondChain1 = second_chain_ids.first->chain_;
      SBPointer<SBChain> new_secondChain2 = second_chain_ids.second->chain_;
      new_secondChain1->create();
      new_secondChain2->create();

      SAMSON::beginHolding("Splitting strand");

      oldSecondChain->getParent()->addChild(new_secondChain1());
      oldSecondChain->getParent()->addChild(new_secondChain2());
      oldSecondChain->getParent()->removeChild(oldSecondChain());
      SAMSON::endHolding();

      //merge the two strands

      ANTNucleotide* tmp;

      if (connectStart->end_ == End::ThreePrime && connectEnd->end_ == End::FivePrime) {
        //this is fine
      }
      else if (connectStart->end_ == End::FivePrime && connectEnd->end_ == End::ThreePrime) {
        //switch
        tmp = connectStart;
        connectStart = connectEnd;
        connectEnd = tmp;
      }
      else {
        std::string msg = "Connection between two nucleotides failed due to wrong directionality.";
        ANTAuxiliary::log(msg);
        return;  //this should not happen if the possibleCrossover map is correct
      }

      ANTSingleStrand* startSingleStrand = model_->getSingleStrandByNucleotide(connectStart);
      ANTSingleStrand* endSingleStrand = model_->getSingleStrandByNucleotide(connectEnd);

      // merge strands in ANTModel
      ANTSingleStrand* mergedChain = model_->nanorobot_->MergeStrands(startSingleStrand, endSingleStrand);

      SBPointer<SBChain> endChain = endSingleStrand->chain_;
      SBPointer<SBChain> startChain = startSingleStrand->chain_;

      SAMSON::beginHolding("Connecting nucleotides");
      // add new chain
      mergedChain->CreateSBChain(configuration_->use_atomic_details);
      SBPointer<SBChain> newChain = mergedChain->chain_;
      newChain->create();
      startChain->getParent()->addChild(newChain());
      // delete old chains
      endChain->getParent()->removeChild(endChain());
      startChain->getParent()->removeChild(startChain());
      // we don't delete ANTSingleStrand since we have reused 
      // the ANTNucleotide objects and they would be deleted.

      //made newly created atoms invisible
      SBNodeIndexer nodeIndexer;
      SAMSON::getActiveDocument()->getNodes(nodeIndexer);

      SB_FOR(SBNode* node, nodeIndexer) {
        if (node->getType() == SBNode::Atom) {
          SBAtom * a = static_cast<SBAtom*>(node);
          a->setVisibilityFlag(false);
        }
      }

      SAMSON::endHolding();

    }
  }

  if (baseEvent->getType() == SBBaseEvent::VisibilityFlagChanged) {

  }
}

void SENucleotidesVisualModel::onDocumentEvent(SBDocumentEvent* documentEvent) {

  // SAMSON Element generator pro tip: implement this function if you need to handle document events 

  std::string message = documentEvent->getTypeString(documentEvent->getType()) + " documentEvent";
  ANTAuxiliary::log(message);

  if (documentEvent->getType() == SBDocumentEvent::SelectionChanged) {

  }

  //if (documentEvent->getType() == SBDocumentEvent::DocumentRemoved) {c
  //  std::string message = "DocumentRemoved";
  //  ANTAuxiliary::log(message);
  //}

  //if (documentEvent->getType() == SBDocumentEvent::ActiveDocumentChanged) {
  //  std::string message = "ActiveDocumentChanged";
  //  ANTAuxiliary::log(message);
  //}

  //if (documentEvent->getType() == SBDocumentEvent::ActiveModelChanged) {
  //  std::string message = "ActiveModelChanged";
  //  ANTAuxiliary::log(message);
  //}

}

void SENucleotidesVisualModel::onStructuralEvent(SBStructuralEvent* structuralEvent) {

  // SAMSON Element generator pro tip: implement this function if you need to handle structural events (e.g. when a structural node for which you provide a visual representation is updated)
  //std::string message = structuralEvent->getTypeString(structuralEvent->getType()) + " structuralEvent";
  //ANTAuxiliary::log(message);

}

void SENucleotidesVisualModel::getNodes(SBNodeIndexer& nodeIndexer, const SBNodePredicate& selectionPredicate, const SBNodePredicate& visitPredicate) {
  if (!visitPredicate(this)) return;
  if (selectionPredicate(this)) nodeIndexer.push_back(this);

  SB_FOR(SBNode* node, atomIndexer_)
    node->getNodes(nodeIndexer, selectionPredicate, visitPredicate);
}

//=== Auxiliary functions ===

void SENucleotidesVisualModel::setMSVModel(MSVModel* model) {
  model_ = model;

}

void SENucleotidesVisualModel::highlightStrands(ANTAuxiliary::ANTArray<float> & colorsV, ANTAuxiliary::ANTArray<float> & colorsE, int index, ANTNucleotide* n)
{
  if (strandsHighlighted_) {
    if (!n->residue_->isHighlighted()) {
      colorsV(index, 0) = 0.6f;
      colorsV(index, 1) = 0.6f;
      colorsV(index, 2) = 0.6f;
      colorsV(index, 3) = 1.0f;

      colorsE(index, 0) = 0.6f;
      colorsE(index, 1) = 0.6f;
      colorsE(index, 2) = 0.6f;
      colorsE(index, 3) = 1.0f;
    }
  }
}

void SENucleotidesVisualModel::highlightAtoms(ANTAuxiliary::ANTArray<float> & colorsV, ANTAuxiliary::ANTArray<float> & colorsE, int aIndex, int nIndex, ANTNucleotide* n)
{
  if (strandsHighlighted_) {

    if (!n->residue_->isHighlighted()) {
      SBNodeIndexer atoms;
      n->residue_->getNodes(atoms, SBNode::IsType(SBNode::Atom));
      SB_FOR(SBNode* node, atoms) {
        colorsV(aIndex, 0) = 0.6f;
        colorsV(aIndex, 1) = 0.6f;
        colorsV(aIndex, 2) = 0.6f;
        colorsV(aIndex, 3) = 1.0f;
      }

      colorsE(nIndex, 0) = 0.6f;
      colorsE(nIndex, 1) = 0.6f;
      colorsE(nIndex, 2) = 0.6f;
      colorsE(nIndex, 3) = 1.0f;

    }
  }
}

ANTAuxiliary::ANTArray<unsigned int> SENucleotidesVisualModel::getNucleotideIndices()
{
  auto singleStrands = model_->nanorobot_->GetSingleStrands();
  unsigned int nPositions = model_->nanorobot_->GetNumberOfNucleotides();
  unsigned int nCylinders = boost::numeric_cast<unsigned int>(nPositions - singleStrands.size());

  ANTAuxiliary::ANTArray<unsigned int> indices = ANTAuxiliary::ANTArray<unsigned int>(nCylinders * 2);

  std::map<ANTNucleotide*, unsigned int> ntMap;

  unsigned int index = 0;
  //this init can be optimized in the future
  for (auto & sid : singleStrands) {
    ANTSingleStrand* singleStrand = sid.second;
    MSVNucleotideList nucleotides = singleStrand->nucleotides_;

    for (auto & nid : nucleotides) {
      ANTNucleotide* nt = nid.second;

      ntMap.insert(make_pair(nt, index));
      ++index;
    }
  }

  size_t sumNumEdges = 0;

  for (auto & sid : singleStrands) {
    ANTSingleStrand* singleStrand = sid.second;
    MSVNucleotideList nucleotides = singleStrand->nucleotides_;

    ANTNucleotide* cur = singleStrand->fivePrime_;

    size_t curNCylinders = nucleotides.size() - 1;
    ANTAuxiliary::ANTArray<unsigned int> curIndices = ANTAuxiliary::ANTArray<unsigned int>(2 * curNCylinders);

    unsigned int j = 0;

    while (cur->next_ != nullptr) {

      unsigned int curIndex = ntMap[cur];
      unsigned int nextIndex = ntMap[cur->next_];
      curIndices(2 * j) = curIndex;
      curIndices(2 * j + 1) = nextIndex;
      j++;

      cur = cur->next_;
    }

    for (int k = 0; k < curNCylinders * 2; ++k) {
      indices(sumNumEdges + k) = curIndices(k);
    }

    sumNumEdges += (2 * curNCylinders);

  }

  return indices;
}
