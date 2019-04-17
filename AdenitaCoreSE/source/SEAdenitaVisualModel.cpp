#include "SEAdenitaVisualModel.hpp"
#include "SAMSON.hpp"
#include "ADNLogger.hpp"
#include "PICrossovers.hpp"


SEAdenitaVisualModel::SEAdenitaVisualModel() {

  // SAMSON Element generator pro tip: this default constructor is called when unserializing the node, so it should perform all default initializations.
	
  init();
}

SEAdenitaVisualModel::SEAdenitaVisualModel(const SBNodeIndexer& nodeIndexer) {

  // SAMSON Element generator pro tip: implement this function if you want your visual model to be applied to a set of data graph nodes.
  // You might want to connect to various signals and handle the corresponding events. For example, if your visual model represents a sphere positioned at
  // the center of mass of a group of atoms, you might want to connect to the atoms' base signals (e.g. to update the center of mass when an atom is erased) and
  // the atoms' structural signals (e.g. to update the center of mass when an atom is moved).

  SEAdenitaCoreSEApp* app = getAdenitaApp();
  
  app->FromDatagraph();

  init();

}

SEAdenitaVisualModel::~SEAdenitaVisualModel() {
  ADNLogger& logger = ADNLogger::GetLogger();
  logger.Log(QString("SEAdenitaVisualModel got destroyed!"));
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

float SEAdenitaVisualModel::getScale()
{
  return scale_;
}

void SEAdenitaVisualModel::changeScale(double scale, bool createIndex/* = true*/)
{
  scale_ = scale;

  prepareTransition();

  SAMSON::requestViewportUpdate();
}

void SEAdenitaVisualModel::changeDimension(int dimension)
{
  dim_ = dimension;
  ADNLogger& logger = ADNLogger::GetLogger();
  /*
    auto parts = nanorobot_->GetParts();
    auto conformations = nanorobot_->GetConformations();
    logger.Log(QString("num conformations"));
    logger.Log(QString::number(conformations.size()));

    auto conformation = conformations[dim_ - 1];
    SB_FOR(auto part, parts) {

      auto singleStrands = nanorobot_->GetSingleStrands(part);
      SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
        auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);
        SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {
          auto index = ntMap_[nt()];

          SBPosition3 pos;
          conformation->getPosition(index, pos);

          positions_(index, 0) = pos[0].getValue();
          positions_(index, 1) = pos[1].getValue();
          positions_(index, 2) = pos[2].getValue();
        }
      }
    }*/

  SAMSON::requestViewportUpdate();
}

void SEAdenitaVisualModel::changeVisibility(double layer)
{
  auto parts = nanorobot_->GetParts();
  SEConfig& config = SEConfig::GetInstance();

  SB_FOR(auto part, parts) {

    auto singleStrands = nanorobot_->GetSingleStrands(part);
    SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
      auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);
      auto ssDist = sortedSingleStrandsByDist_[ss()];

      SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {

        auto index = ntMap_[nt()];

        if (index >= nPositions_) return;
        auto ntDist = sortedNucleotidesByDist_[nt()];
        if (ssDist > layer) {
          radiiV_(index) = 0;
          radiiE_(index) = 0;
        }
        else {
          radiiV_(index) = config.nucleotide_V_radius;
          radiiE_(index) = config.nucleotide_V_radius;
        }
        if (ntDist > layer) {
          //nt->setVisibilityFlag(false);
          colorsV_(index, 3) = 0.0f;
          colorsE_(index, 3) = 0.0f;
          if (true) {
            if (ss->IsScaffold()) {
              radiiV_(index) = 0;
              radiiE_(index) = 0;
            }
          }
        }
        else {
          //nt->setVisibilityFlag(true);
          colorsV_(index, 3) = 1.0f;
          colorsE_(index, 3) = 1.0f;
        }
        
      }
    }
  }

  SAMSON::requestViewportUpdate();

}

void SEAdenitaVisualModel::init()
{
  SEAdenitaCoreSEApp* app = getAdenitaApp();
  nanorobot_ = app->GetNanorobot();

  SEConfig& config = SEConfig::GetInstance();

  MSVColors * regularColors = new MSVColors();
  MSVColors * meltTempColors = new MSVColors();
  MSVColors * gibbsColors = new MSVColors();
  colors_[ColorType::REGULAR] = regularColors;
  colors_[ColorType::MELTTEMP] = meltTempColors;
  colors_[ColorType::GIBBS] = gibbsColors;

  //setup the display properties
  nucleotideEColor_ = ADNArray<float>(4);
  nucleotideEColor_(0) = config.nucleotide_E_Color[0];
  nucleotideEColor_(1) = config.nucleotide_E_Color[1];
  nucleotideEColor_(2) = config.nucleotide_E_Color[2];
  nucleotideEColor_(3) = config.nucleotide_E_Color[3];

  update();

  setupPropertyColors();

  //orderVisibility();
}

void SEAdenitaVisualModel::initAtoms(bool createIndex /*= true*/)
{

  unsigned int nPositions = 0;

  auto parts = nanorobot_->GetParts();
  SB_FOR(auto part, parts) {
    nPositions += part->GetNumberOfAtoms();
  }

  unsigned int nCylinders = 0;

  nPositionsAtom_ = nPositions;
  nCylindersAtom_ = nCylinders;
  positionsAtom_ = ADNArray<float>(3, nPositions);
  radiiVAtom_ = ADNArray<float>(nPositions);
  radiiEAtom_ = ADNArray<float>(nPositions);
  colorsVAtom_ = ADNArray<float>(4, nPositions);
  colorsEAtom_ = ADNArray<float>(4, nPositions);
  flagsAtom_ = ADNArray<unsigned int>(nPositions);
  nodeIndicesAtom_ = ADNArray<unsigned int>(nPositions);
  
  if (createIndex) {
    indicesAtom_ = getAtomIndices();
  }
}

void SEAdenitaVisualModel::update()
{
  auto parts = nanorobot_->GetParts();

  SB_FOR(auto part, parts) {
    part->connectBaseSignalToSlot(
      this,
      SB_SLOT(&SEAdenitaVisualModel::onBaseEvent));
  }

  //SB_FOR(auto part, parts) {
  //  part->connectStructuralSignalToSlot(
  //    this,
  //    SB_SLOT(&SEAdenitaVisualModel::onStructuralEvent));
  //}

  //SAMSON::getActiveDocument()->connectDocumentSignalToSlot(
  //  this,
  //  SB_SLOT(&SEAdenitaVisualModel::onDocumentEvent));

  initAtoms(true);
  initNucleotidesAndSingleStrands(true);
  initDoubleStrands(true);

  prepareDiscreteScales();

  changeScale(scale_);

}

void SEAdenitaVisualModel::initNucleotidesAndSingleStrands(bool createIndex /* = true */)
{
  auto singleStrands = nanorobot_->GetSingleStrands();

  unsigned int nPositions = nanorobot_->GetNumberOfNucleotides();
  unsigned int nCylinders = boost::numeric_cast<unsigned int>(nPositions - singleStrands.size());

  nPositionsNt_ = nPositions;
  nCylindersNt_ = nCylinders;
  positionsNt_ = ADNArray<float>(3, nPositions);
  radiiVNt_ = ADNArray<float>(nPositions);
  radiiENt_ = ADNArray<float>(nPositions);
  colorsVNt_ = ADNArray<float>(4, nPositions);
  colorsENt_ = ADNArray<float>(4, nPositions);
  flagsNt_ = ADNArray<unsigned int>(nPositions);
  nodeIndicesNt_ = ADNArray<unsigned int>(nPositions);

  colorsVSS_ = ADNArray<float>(4, nPositions);
  colorsESS_ = ADNArray<float>(4, nPositions);
  radiiVSS_ = ADNArray<float>(nPositions);
  radiiESS_ = ADNArray<float>(nPositions);
  
  if (createIndex) {
    indicesNt_ = getNucleotideIndices();
  }

}

void SEAdenitaVisualModel::initDoubleStrands(bool createIndex /*= true*/)
{

  unsigned int nPositions = nanorobot_->GetNumberOfBaseSegments();

  nPositionsDS_ = nPositions;
  positionsDS_ = ADNArray<float>(3, nPositions);
  radiiVDS_ = ADNArray<float>(nPositions);
  colorsVDS_ = ADNArray<float>(4, nPositions);
  flagsDS_ = ADNArray<unsigned int>(nPositions);
  nodeIndicesDS_ = ADNArray<unsigned int>(nPositions);

  bsMap_.clear();

  auto parts = nanorobot_->GetParts();

  unsigned int index = 0;

  SB_FOR(auto part, parts) {
    auto doubleStrands = nanorobot_->GetDoubleStrands(part);
    SB_FOR(ADNPointer<ADNDoubleStrand> ds, doubleStrands) {
      auto baseSegments = ds->GetBaseSegments();
      SB_FOR(ADNPointer<ADNBaseSegment> bs, baseSegments) {
        bsMap_.insert(make_pair(bs(), index));
        ++index;
      }
    }
  }
  
  if (createIndex) {
    /*indicesDS_ = getBaseSegmentIndices();*/
  }
}

ADNArray<unsigned int> SEAdenitaVisualModel::getAtomIndices()
{

  ADNArray<unsigned int> indices = ADNArray<unsigned int>(0);

  ntMap_.clear();

  unsigned int index = 0;

  auto parts = nanorobot_->GetParts();

  SB_FOR(auto part, parts) {
    auto singleStrands = nanorobot_->GetSingleStrands(part);
    SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
      auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);
      SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {
        auto atoms = nt->GetAtoms();
        SB_FOR(ADNPointer<ADNAtom> a, atoms) {
          atomMap_.insert(make_pair(a(), index));
          ++index;
        }
      }
    }
  }

  return indices;
}

ADNArray<unsigned int> SEAdenitaVisualModel::getNucleotideIndices()
{
  ADNLogger& logger = ADNLogger::GetLogger();

  auto singleStrands = nanorobot_->GetSingleStrands();

  unsigned int nPositions = nanorobot_->GetNumberOfNucleotides();
  unsigned int nCylinders = boost::numeric_cast<unsigned int>(nPositions - singleStrands.size());

  ADNArray<unsigned int> indices = ADNArray<unsigned int>(nCylinders * 2);
  
  ntMap_.clear();

  unsigned int index = 0;
  //this init can be optimized in the future

  auto parts = nanorobot_->GetParts();

  SB_FOR(auto part, parts) {
    auto signelStrands = nanorobot_->GetSingleStrands(part);
    SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
      auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);
      SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {
        ntMap_.insert(make_pair(nt(), index));
        ++index;
      }
    }
  }

  size_t sumNumEdges = 0;

  SB_FOR(auto part, parts) {
    auto singleStrands = nanorobot_->GetSingleStrands(part);
    SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
      auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);
      ADNPointer<ADNNucleotide> cur = nanorobot_->GetSingleStrandFivePrime(ss);
      size_t curNCylinders = nucleotides.size() - 1;
      ADNArray<unsigned int> curIndices = ADNArray<unsigned int>(2 * curNCylinders);

      //looping using the next_ member variable of nucleotides
      unsigned int j = 0;
      while (nanorobot_->GetNucleotideNext(cur) != nullptr) {
        unsigned int curIndex;
        curIndex = ntMap_[cur()];
        //nucleotides.getIndex(cur(), curIndex);
        unsigned int nextIndex;
        auto next = nanorobot_->GetNucleotideNext(cur)();
        nextIndex = ntMap_[next];
        //nucleotides.getIndex(next, nextIndex);

        curIndices(2 * j) = curIndex;
        curIndices(2 * j + 1) = nextIndex;
        j++;

        cur = nanorobot_->GetNucleotideNext(cur);
      }

      //for (int j = 0; j < nucleotides.size() - 1; ++j) {
      //  auto cur = nucleotides[j];
      //  unsigned int curIndex;
      //  curIndex = ntMap[cur];
      //  unsigned int nextIndex;
      //  auto next = nucleotides[j + 1];
      //  nextIndex = ntMap[next];

      //  curIndices(2 * j) = curIndex;
      //  curIndices(2 * j + 1) = nextIndex;

      //}

      for (int k = 0; k < curNCylinders * 2; ++k) {
        indices(sumNumEdges + k) = curIndices(k);
      }

      sumNumEdges += (2 * curNCylinders);
    }
  }


  return indices;

}


ADNArray<unsigned int> SEAdenitaVisualModel::getBaseSegmentIndices()
{
  //not clear yet whether the indices are needed
  //for now this function is just used to construct the bsMap_

  ADNLogger& logger = ADNLogger::GetLogger();
  auto parts = nanorobot_->GetParts();


  unsigned int nDs = bsMap_.size();
  
  unsigned int nPositions = nanorobot_->GetNumberOfBaseSegments();
  unsigned int nCylinders = boost::numeric_cast<unsigned int>(nPositions - nDs);
 



  ADNArray<unsigned int> indices = ADNArray<unsigned int>(nCylinders * 2);

  return indices;
  

}


void SEAdenitaVisualModel::prepareTransition()
{

  SEConfig& config = SEConfig::GetInstance();

  if (nanorobot_ == nullptr) return;
  
  if (scale_ > OBJECTS) scale_ = OBJECTS;

  float interpolated = 1.0f - (floor(scale_ + 1) - scale_);

  if (scale_ < (float)ATOMS_STICKS) {
    //prepareSticksToBalls(interpolated);
  }
  else if (scale_ < (float)ATOMS_BALLS) {
  }
  else if (scale_ < (float)NUCLEOTIDES) {
    prepareBallsToNucleotides(interpolated);
  }
  else if (scale_ < (float)SINGLE_STRANDS) {
    prepareNucleotidesToSingleStrands(interpolated);
  }
  else if (scale_ < (float)DOUBLE_STRANDS) {
    prepareSingleStrandsToDoubleStrands(interpolated);
  }
  else if (scale_ < (float)OBJECTS) {
    prepareDoubleStrandsToObjects(interpolated);
  } 

}

void SEAdenitaVisualModel::prepareDiscreteScales()
{
  prepareAtoms();
  prepareNucleotides();
  prepareSingleStrands();
  prepareDoubleStrands();
}

void SEAdenitaVisualModel::displayTransition(bool forSelection)
{
  if (forSelection) {
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

    SAMSON::displaySpheresSelection(
      nPositions_,
      positions_.GetArray(),
      radiiV_.GetArray(),
      nodeIndices_.GetArray()
    );
  }
  else {
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

    SAMSON::displaySpheres(
      nPositions_,
      positions_.GetArray(),
      radiiV_.GetArray(),
      colorsV_.GetArray(),
      flags_.GetArray());

    displayCircularDNAConnection();
    if (showBasePairing_) displayBasePairConnections(false);
    if (highlightType_ == TAGGED) displayTags();
    displayForDebugging();
  }
}

void SEAdenitaVisualModel::prepareSticksToBalls(double iv)
{
  
}

void SEAdenitaVisualModel::prepareBallsToNucleotides(double iv)
{
  nPositions_ = nPositionsAtom_;
  nCylinders_ = nCylindersAtom_;

  positions_ = ADNArray<float>(3, nPositions_);
  radiiV_ = ADNArray<float>(nPositions_);
  radiiE_ = ADNArray<float>(0);
  colorsV_ = ADNArray<float>(4, nPositions_);
  colorsE_ = ADNArray<float>(0);
  //flags_ = ADNArray<unsigned int>(nPositions_);
  //nodeIndices_ = ADNArray<unsigned int>(nPositions_);
  indices_ = ADNArray<unsigned int>(0);

  //positions_ = positionsAtom_;
  //radiiV_ = radiiVAtom_;
  //radiiE_ = radiiEAtom_;
  flags_ = flagsAtom_;
  nodeIndices_ = nodeIndicesAtom_;
  //indices_ = indicesAtom_;
  //colorsV_ = colorsVAtom_;
  //colorsE_ = colorsEAtom_;

  for (auto it = ntMap_.begin(); it != ntMap_.end(); it++)
  {
    auto nt = it->first;
    auto atoms = nt->GetAtoms();
    SB_FOR(ADNPointer<ADNAtom> a, atoms) {
      auto indexAtom = atomMap_[a()];
      auto indexNt = it->second;

      positions_(indexAtom, 0) = positionsAtom_(indexAtom, 0) + iv * (positionsNt_(indexNt, 0) - positionsAtom_(indexAtom, 0));
      positions_(indexAtom, 1) = positionsAtom_(indexAtom, 1) + iv * (positionsNt_(indexNt, 1) - positionsAtom_(indexAtom, 1));
      positions_(indexAtom, 2) = positionsAtom_(indexAtom, 2) + iv * (positionsNt_(indexNt, 2) - positionsAtom_(indexAtom, 2));
      
      colorsV_(indexAtom, 0) = colorsVAtom_(indexAtom, 0) + iv * (colorsVNt_(indexNt, 0) - colorsVAtom_(indexAtom, 0));
      colorsV_(indexAtom, 1) = colorsVAtom_(indexAtom, 1) + iv * (colorsVNt_(indexNt, 1) - colorsVAtom_(indexAtom, 1));
      colorsV_(indexAtom, 2) = colorsVAtom_(indexAtom, 2) + iv * (colorsVNt_(indexNt, 2) - colorsVAtom_(indexAtom, 2));
      colorsV_(indexAtom, 3) = colorsVAtom_(indexAtom, 3) + iv * (colorsVNt_(indexNt, 3) - colorsVAtom_(indexAtom, 3));

      radiiV_(indexAtom) = radiiVAtom_(indexAtom) + iv * (radiiVNt_(indexNt) - radiiVAtom_(indexAtom));
    }
  }
}

void SEAdenitaVisualModel::prepareNucleotidesToSingleStrands(double iv)
{
  nPositions_ = nPositionsNt_;
  nCylinders_ = nCylindersNt_;

  //positions_ = ADNArray<float>(3, nPositions_);
  //radiiV_ = ADNArray<float>(nPositions_);
  radiiE_ = ADNArray<float>(nPositions_);
  colorsV_ = ADNArray<float>(4, nPositions_);
  //colorsE_ = ADNArray<float>(4, nPositions_);
  //flags_ = ADNArray<unsigned int>(nPositions_);
  //nodeIndices_ = ADNArray<unsigned int>(nPositions_);
  //indices_ = ADNArray<unsigned int>(nCylinders_ * 2);

  positions_ = positionsNt_;
  radiiV_ = radiiVNt_;
  flags_ = flagsNt_;
  nodeIndices_ = nodeIndicesNt_;
  indices_ = indicesNt_;
  
  for (int index = 0; index < nPositions_; ++index) {
    radiiE_(index) = radiiENt_(index) + iv * (radiiESS_(index) - radiiENt_(index));
    colorsV_(index, 0) = colorsVNt_(index, 0) + iv * (colorsVSS_(index, 0) - colorsVNt_(index, 0));
    colorsV_(index, 1) = colorsVNt_(index, 1) + iv * (colorsVSS_(index, 1) - colorsVNt_(index, 1));
    colorsV_(index, 2) = colorsVNt_(index, 2) + iv * (colorsVSS_(index, 2) - colorsVNt_(index, 2));
    colorsV_(index, 3) = colorsVNt_(index, 3) + iv * (colorsVSS_(index, 3) - colorsVNt_(index, 3));
  }

  colorsE_ = colorsV_;


}

void SEAdenitaVisualModel::prepareSingleStrandsToDoubleStrands(double iv)
{

  nPositions_ = nPositionsNt_;
  nCylinders_ = nCylindersNt_;

  positions_ = ADNArray<float>(3, nPositions_);
  radiiV_ = ADNArray<float>(nPositions_);
  radiiE_ = ADNArray<float>(nPositions_);
  colorsV_ = ADNArray<float>(4, nPositions_);
  //colorsE_ = ADNArray<float>(4, nPositions_);
  //flags_ = ADNArray<unsigned int>(nPositions_);
  //nodeIndices_ = ADNArray<unsigned int>(nPositions_);
  //indices_ = ADNArray<unsigned int>(nCylinders_ * 2);


  //positions_ = positionsNt_;
  //radiiV_ = radiiVSS_;
  //radiie_ = radiiESS_;
  flags_ = flagsNt_;
  nodeIndices_ = nodeIndicesNt_;
  indices_ = indicesNt_;
  //colorsV_ = colorsVNt_;
  colorsE_ = colorsESS_;

  for (auto it = ntMap_.begin(); it != ntMap_.end(); it++)
  {
    auto nt = it->first;
    auto bs = nt->GetBaseSegment();
    auto indexSS = it->second;
    auto indexDS = bsMap_[bs()];

    radiiV_(indexSS) = radiiVSS_(indexSS) + iv * (radiiVDS_(indexDS) - radiiVSS_(indexSS));
    radiiE_(indexSS) = radiiESS_(indexSS) - iv * radiiESS_(indexSS);

    positions_(indexSS, 0) = positionsNt_(indexSS, 0) + iv * (positionsDS_(indexDS, 0) - positionsNt_(indexSS, 0));
    positions_(indexSS, 1) = positionsNt_(indexSS, 1) + iv * (positionsDS_(indexDS, 1) - positionsNt_(indexSS, 1));
    positions_(indexSS, 2) = positionsNt_(indexSS, 2) + iv * (positionsDS_(indexDS, 2) - positionsNt_(indexSS, 2));

    colorsV_(indexSS, 0) = colorsVSS_(indexSS, 0) + iv * (colorsVDS_(indexDS, 0) - colorsVSS_(indexSS, 0));
    colorsV_(indexSS, 1) = colorsVSS_(indexSS, 1) + iv * (colorsVDS_(indexDS, 1) - colorsVSS_(indexSS, 1));
    colorsV_(indexSS, 2) = colorsVSS_(indexSS, 2) + iv * (colorsVDS_(indexDS, 2) - colorsVSS_(indexSS, 2));
    colorsV_(indexSS, 3) = colorsVSS_(indexSS, 3) + iv * (colorsVDS_(indexDS, 3) - colorsVSS_(indexSS, 3));

  }

}

void SEAdenitaVisualModel::prepareDoubleStrandsToObjects(double iv)
{
  nPositions_ = nPositionsDS_;
  nCylinders_ = 0;

  positions_ = positionsDS_;
  radiiV_ = radiiVDS_;
  flags_ = flagsDS_;
  nodeIndices_ = nodeIndicesDS_;
  colorsV_ = colorsVDS_;
}

void SEAdenitaVisualModel::highlightFlagChanged()
{
  auto parts = nanorobot_->GetParts();

  if (scale_ < (float)NUCLEOTIDES) {
  }
  else if (scale_ >= (float)NUCLEOTIDES && scale_ < (float)DOUBLE_STRANDS) {
    SB_FOR(auto part, parts) {
      auto singleStrands = nanorobot_->GetSingleStrands(part);
      SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
        auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);
        SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {
          auto index = ntMap_[nt()];
          flags_(index) = nt->getInheritedFlags();
        }
      }
    }
  }
  else if (scale_ >= (float)DOUBLE_STRANDS) {
    SB_FOR(auto part, parts) {
      auto doubleStrands = part->GetDoubleStrands();
      SB_FOR(auto doubleStrand, doubleStrands) {
        auto baseSegments = doubleStrand->GetBaseSegments();
        SB_FOR(auto baseSegment, baseSegments) {
          auto index = bsMap_[baseSegment];
          flags_(index) = baseSegment->getInheritedFlags();
          ++index;
        }
      }
    }
  }

  SAMSON::requestViewportUpdate();

}

SEAdenitaCoreSEApp* SEAdenitaVisualModel::getAdenitaApp() const
{
  return static_cast<SEAdenitaCoreSEApp*>(SAMSON::getApp(SBCContainerUUID("85DB7CE6-AE36-0CF1-7195-4A5DF69B1528"), SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));
}

void SEAdenitaVisualModel::orderVisibility()
{

  unsigned int order = 1;

  SEConfig& config = SEConfig::GetInstance();
  ADNLogger& logger = ADNLogger::GetLogger();
  
  auto parts = nanorobot_->GetParts();

  std::vector<pair<ADNNucleotide*, float>> nucleotidesSorted;
  std::vector<pair<ADNSingleStrand*, float>> singleStrandsSorted;

  //ordered by
  if (order == 1) {
    SB_FOR(auto part, parts) {
      auto scaffolds = nanorobot_->GetScaffolds(part);

      if (scaffolds.size() == 0) return;
        
      SB_FOR(ADNPointer<ADNSingleStrand> ss, scaffolds) {
        auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);
        SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {
          auto pair = nanorobot_->GetNucleotidePair(nt);
          nucleotidesSorted.push_back(make_pair(nt(), float(nt->getNodeIndex())));
          if (pair != nullptr)
            nucleotidesSorted.push_back(make_pair(pair(), float(nt->getNodeIndex()))); //the staple nucleotide should get the same order as the scaffold nucleotide
        }
      }

      auto singleStrands = nanorobot_->GetSingleStrands(part);
      SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
        auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);
        unsigned int minIdx = UINT_MAX;
        if (nanorobot_->IsScaffold(ss)) {
          minIdx = 0;
        }
        else {
          SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {
            auto pair = nanorobot_->GetNucleotidePair(nt);
            if (pair != nullptr) {
              unsigned int idx = pair->getNodeIndex();
              if (idx < minIdx) minIdx = idx;
            }
          }
        }

        singleStrandsSorted.push_back(std::make_pair(ss(), boost::numeric_cast<float>(minIdx)));
      }
      
    }
  }
  else if (order == 1) {
    SBPosition3 center;
    SB_FOR(auto part, parts) {
      auto singleStrands = nanorobot_->GetSingleStrands(part);
      SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
        auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);
        SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {
          center += nanorobot_->GetNucleotidePosition(nt);
        }
      }
    }

    center /= nanorobot_->GetNumberOfNucleotides();

    SB_FOR(auto part, parts) {
      auto singleStrands = nanorobot_->GetSingleStrands(part);
      SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
        auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);
        SBPosition3 strandPosition;
        float minDist = FLT_MAX;
        SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {
          SBPosition3 diff = nanorobot_->GetNucleotidePosition(nt) - center;
          float dist = diff.norm().getValue();
          nucleotidesSorted.push_back(make_pair(nt(), dist));

          if (dist < minDist) minDist = dist;

        }

        singleStrandsSorted.push_back(std::make_pair(ss(), minDist));

      }
    }
  }
  else if (order == 2) {

  }

  if (nucleotidesSorted.size() == 0 || singleStrandsSorted.size() == 0) return;

  sort(nucleotidesSorted.begin(), nucleotidesSorted.end(), [=](std::pair<ADNNucleotide*, float>& a, std::pair<ADNNucleotide*, float>& b)
  {
    return a.second < b.second;
  });

  sort(singleStrandsSorted.begin(), singleStrandsSorted.end(), [=](std::pair<ADNSingleStrand*, float>& a, std::pair<ADNSingleStrand*, float>& b)
  {
    return a.second < b.second;
  });

  sortedNucleotidesByDist_.clear();
  sortedSingleStrandsByDist_.clear();

  float max = nucleotidesSorted.back().second;

  for (int i = 0; i < nucleotidesSorted.size(); i++) {
    sortedNucleotidesByDist_.insert(make_pair(nucleotidesSorted[i].first, nucleotidesSorted[i].second / max));
    //logger.Log(nucleotidesSorted[i].second);
  }

  for (int i = 0; i < singleStrandsSorted.size(); i++) {
    sortedSingleStrandsByDist_.insert(make_pair(singleStrandsSorted[i].first, singleStrandsSorted[i].second / max));
    //logger.Log(singleStrandsSorted[i].second);
  }
}

void SEAdenitaVisualModel::setSingleStrandColors(int index)
{
  auto regularColors = colors_[ColorType::REGULAR];

  if (index == 0) {
    regularColors->SetStandardSingleStrandColorScheme();
  }
  else if (index == 1) {

    ADNArray<float> pastel12 = ADNArray<float>(4, 12);

    pastel12(0, 0) = 141 / 255.0f;
    pastel12(0, 1) = 211 / 255.0f;
    pastel12(0, 2) = 199 / 255.0f;
    pastel12(0, 3) = 1;

    pastel12(1, 0) = 255 / 255.0f;
    pastel12(1, 1) = 255 / 255.0f;
    pastel12(1, 2) = 179 / 255.0f;
    pastel12(1, 3) = 1;

    pastel12(2, 0) = 190 / 255.0f;
    pastel12(2, 1) = 186 / 255.0f;
    pastel12(2, 2) = 218 / 255.0f;
    pastel12(2, 3) = 1;

    pastel12(3, 0) = 251 / 255.0f;
    pastel12(3, 1) = 128 / 255.0f;
    pastel12(3, 2) = 114 / 255.0f;
    pastel12(3, 3) = 1;

    pastel12(4, 0) = 128 / 255.0f;
    pastel12(4, 1) = 177 / 255.0f;
    pastel12(4, 2) = 211 / 255.0f;
    pastel12(4, 3) = 1;

    pastel12(5, 0) = 253 / 255.0f;
    pastel12(5, 1) = 180 / 255.0f;
    pastel12(5, 2) = 98 / 255.0f;
    pastel12(5, 3) = 1;

    pastel12(6, 0) = 179 / 255.0f;
    pastel12(6, 1) = 222 / 255.0f;
    pastel12(6, 2) = 105 / 255.0f;
    pastel12(6, 3) = 1;

    pastel12(7, 0) = 252 / 255.0f;
    pastel12(7, 1) = 205 / 255.0f;
    pastel12(7, 2) = 229 / 255.0f;
    pastel12(7, 3) = 1;

    pastel12(8, 0) = 217 / 255.0f;
    pastel12(8, 1) = 217 / 255.0f;
    pastel12(8, 2) = 217 / 255.0f;
    pastel12(8, 3) = 1;

    pastel12(9, 0) = 188 / 255.0f;
    pastel12(9, 1) = 128 / 255.0f;
    pastel12(9, 2) = 189 / 255.0f;
    pastel12(9, 3) = 1;

    pastel12(10, 0) = 204 / 255.0f;
    pastel12(10, 1) = 235 / 255.0f;
    pastel12(10, 2) = 197 / 255.0f;
    pastel12(10, 3) = 1;

    pastel12(11, 0) = 255 / 255.0f;
    pastel12(11, 1) = 237 / 255.0f;
    pastel12(11, 2) = 111 / 255.0f;
    pastel12(11, 3) = 1;

    regularColors->SetSingleStrandColorScheme(pastel12);
  }
  else if (index == 2) {

    ADNArray<float> dark8 = ADNArray<float>(4, 8);

    dark8(0, 0) = 102 / 255.0f;
    dark8(0, 1) = 102 / 255.0f;
    dark8(0, 2) = 102 / 255.0f;
    dark8(0, 3) = 1;

    dark8(1, 0) = 27  / 255.0f;
    dark8(1, 1) = 158 / 255.0f;
    dark8(1, 2) = 119 / 255.0f;
    dark8(1, 3) = 1;

    dark8(2, 0) = 217 / 255.0f;
    dark8(2, 1) = 95 / 255.0f;
    dark8(2, 2) = 2 / 255.0f;
    dark8(2, 3) = 1;

    dark8(3, 0) = 117 / 255.0f;
    dark8(3, 1) = 112 / 255.0f;
    dark8(3, 2) = 179 / 255.0f;
    dark8(3, 3) = 1;

    dark8(4, 0) = 231 / 255.0f;
    dark8(4, 1) = 41 / 255.0f;
    dark8(4, 2) = 138 / 255.0f;
    dark8(4, 3) = 1;

    dark8(5, 0) = 102 / 255.0f;
    dark8(5, 1) = 166 / 255.0f;
    dark8(5, 2) = 30 / 255.0f;
    dark8(5, 3) = 1;

    dark8(6, 0) = 230 / 255.0f;
    dark8(6, 1) = 171 / 255.0f;
    dark8(6, 2) = 2 / 255.0f;
    dark8(6, 3) = 1;

    dark8(7, 0) = 166 / 255.0f;
    dark8(7, 1) = 118 / 255.0f;
    dark8(7, 2) = 29 / 255.0f;
    dark8(7, 3) = 1;
    
    regularColors->SetSingleStrandColorScheme(dark8);

  }
  else if (index == 3) {

    ADNArray<float> pastel8 = ADNArray<float>(4, 8);

    pastel8(0, 0) = 179 / 255.0f;
    pastel8(0, 1) = 226 / 255.0f;
    pastel8(0, 2) = 205 / 255.0f;
    pastel8(0, 3) = 1;

    pastel8(1, 0) = 253 / 255.0f;
    pastel8(1, 1) = 205 / 255.0f;
    pastel8(1, 2) = 172 / 255.0f;
    pastel8(1, 3) = 1;

    pastel8(2, 0) = 203 / 255.0f;
    pastel8(2, 1) = 213 / 255.0f;
    pastel8(2, 2) = 232 / 255.0f;
    pastel8(2, 3) = 1;

    pastel8(3, 0) = 244 / 255.0f;
    pastel8(3, 1) = 202 / 255.0f;
    pastel8(3, 2) = 228 / 255.0f;
    pastel8(3, 3) = 1;

    pastel8(4, 0) = 230 / 255.0f;
    pastel8(4, 1) = 245 / 255.0f;
    pastel8(4, 2) = 201 / 255.0f;
    pastel8(4, 3) = 1;

    pastel8(5, 0) = 255 / 255.0f;
    pastel8(5, 1) = 242 / 255.0f;
    pastel8(5, 2) = 174 / 255.0f;
    pastel8(5, 3) = 1;

    pastel8(6, 0) = 241 / 255.0f;
    pastel8(6, 1) = 226 / 255.0f;
    pastel8(6, 2) = 204 / 255.0f;
    pastel8(6, 3) = 1;

    pastel8(7, 0) = 204 / 255.0f; 
    pastel8(7, 1) = 204 / 255.0f; 
    pastel8(7, 2) = 204 / 255.0f; 
    pastel8(7, 3) = 1;

    regularColors->SetSingleStrandColorScheme(pastel8);

  }

  
}

void SEAdenitaVisualModel::setNucleotideColors(int index)
{
  auto regularColors = colors_[ColorType::REGULAR];

  if (index == 0) {
    regularColors->SetStandardNucleotideColorScheme();
  }
  else if (index == 1) {

    ADNArray<float> dark4 = ADNArray<float>(4, 4);

    dark4(0, 0) = 27 / 255.0f;  
    dark4(0, 1) = 158 / 255.0f;
    dark4(0, 2) = 119 / 255.0f;
    dark4(0, 3) = 1;

    dark4(1, 0) = 217 / 255.0f;
    dark4(1, 1) = 95 / 255.0f;
    dark4(1, 2) = 2 / 255.0f;
    dark4(1, 3) = 1;

    dark4(2, 0) = 117 / 255.0f;
    dark4(2, 1) = 112 / 255.0f;
    dark4(2, 2) = 179 / 255.0f;
    dark4(2, 3) = 1;

    dark4(3, 0) = 231 / 255.0f;
    dark4(3, 1) = 41 / 255.0f;
    dark4(3, 2) = 138 / 255.0f;
    dark4(3, 3) = 1;

    regularColors->SetNucleotideColorScheme(dark4);

  }
  else if (index == 2) {

    ADNArray<float> pastel4 = ADNArray<float>(4, 4);
    
    pastel4(0, 0) = 166 / 255.0f; 
    pastel4(0, 1) = 206 / 255.0f;
    pastel4(0, 2) = 227 / 255.0f;
    pastel4(0, 3) = 1;

    pastel4(1, 0) = 31 / 255.0f;
    pastel4(1, 1) = 120 / 255.0f;
    pastel4(1, 2) = 180 / 255.0f;
    pastel4(1, 3) = 1;

    pastel4(2, 0) = 178 / 255.0f;
    pastel4(2, 1) = 223 / 255.0f;
    pastel4(2, 2) = 138 / 255.0f;
    pastel4(2, 3) = 1;

    pastel4(3, 0) = 51 / 255.0f;
    pastel4(3, 1) = 160 / 255.0f;
    pastel4(3, 2) = 44 / 255.0f;
    pastel4(3, 3) = 1;

    regularColors->SetNucleotideColorScheme(pastel4);

  }
  else if (index == 3) {

    ADNArray<float> pastel2 = ADNArray<float>(4, 4);

    pastel2(0, 0) = 127 / 255.0f; 
    pastel2(0, 1) = 201 / 255.0f; 
    pastel2(0, 2) = 127 / 255.0f; 
    pastel2(0, 3) = 1;

    pastel2(1, 0) = 253 / 255.0f;
    pastel2(1, 1) = 192 / 255.0f;
    pastel2(1, 2) = 134 / 255.0f;
    pastel2(1, 3) = 1;

    pastel2(2, 0) = 253 / 255.0f;
    pastel2(2, 1) = 192 / 255.0f;
    pastel2(2, 2) = 134 / 255.0f;
    pastel2(2, 3) = 1;

    pastel2(3, 0) = 127 / 255.0f;
    pastel2(3, 1) = 201 / 255.0f;
    pastel2(3, 2) = 127 / 255.0f;
    pastel2(3, 3) = 1;

    regularColors->SetNucleotideColorScheme(pastel2);

  }
}

void SEAdenitaVisualModel::setDoubleStrandColors(int index)
{
  auto regularColors = colors_[ColorType::REGULAR];

  if (index == 0) {
    regularColors->SetStandardDoubleStrandColorScheme();
  }
  else if (index == 1) {
    ADNArray<float> pastel2 = ADNArray<float>(4, 2);

    pastel2(0, 0) = 179 / 255.0f;
    pastel2(0, 1) = 226 / 255.0f;
    pastel2(0, 2) = 205 / 255.0f;
    pastel2(0, 3) = 1.0f;

    pastel2(1, 0) = 253 / 255.0f;
    pastel2(1, 1) = 205 / 255.0f;
    pastel2(1, 2) = 172 / 255.0f;
    pastel2(1, 3) = 1.0f;

    regularColors->SetDoubleStrandColorScheme(pastel2);

  }
  else if (index == 2) {
    ADNArray<float> pastel4 = ADNArray<float>(4, 4);

    pastel4(0, 0) = 141 / 255.0f;
    pastel4(0, 1) = 211 / 255.0f;
    pastel4(0, 2) = 199 / 255.0f;
    pastel4(0, 3) = 1.0f;

    pastel4(1, 0) = 255 / 255.0f;
    pastel4(1, 1) = 255 / 255.0f;
    pastel4(1, 2) = 179 / 255.0f;
    pastel4(1, 3) = 1.0f;

    pastel4(2, 0) = 190 / 255.0f;
    pastel4(2, 1) = 186 / 255.0f;
    pastel4(2, 2) = 218 / 255.0f;
    pastel4(2, 3) = 1.0f;

    pastel4(3, 0) = 251 / 255.0f;
    pastel4(3, 1) = 128 / 255.0f;
    pastel4(3, 2) = 114 / 255.0f;
    pastel4(3, 3) = 1.0f;

    regularColors->SetDoubleStrandColorScheme(pastel4);

  }
  else if (index == 3) {
    ADNArray<float> pastel6 = ADNArray<float>(4, 6);

    pastel6(0, 0) = 251 / 255.0f; 
    pastel6(0, 1) = 180 / 255.0f; 
    pastel6(0, 2) = 174 / 255.0f; 
    pastel6(0, 3) = 1.0f;

    pastel6(1, 0) = 179 / 255.0f; 
    pastel6(1, 1) = 205 / 255.0f; 
    pastel6(1, 2) = 227 / 255.0f; 
    pastel6(1, 3) = 1.0f;

    pastel6(2, 0) = 204 / 255.0f;
    pastel6(2, 1) = 235 / 255.0f;
    pastel6(2, 2) = 197 / 255.0f;
    pastel6(2, 3) = 1.0f;

    pastel6(3, 0) = 222 / 255.0f;
    pastel6(3, 1) = 203 / 255.0f;
    pastel6(3, 2) = 228 / 255.0f;
    pastel6(3, 3) = 1.0f;

    pastel6(4, 0) = 254 / 255.0f; 
    pastel6(4, 1) = 217 / 255.0f; 
    pastel6(4, 2) = 166 / 255.0f; 
    pastel6(4, 3) = 1.0f;

    pastel6(5, 0) = 255 / 255.0f; 
    pastel6(5, 1) = 255 / 255.0f; 
    pastel6(5, 2) = 204 / 255.0f; 
    pastel6(5, 3) = 1.0f;

    regularColors->SetDoubleStrandColorScheme(pastel6);

  }
  else if (index == 4) {
    ADNArray<float> pastel12 = ADNArray<float>(4, 12);

    pastel12(0, 0) = 141 / 255.0f;
    pastel12(0, 1) = 211 / 255.0f;
    pastel12(0, 2) = 199 / 255.0f;
    pastel12(0, 3) = 1;

    pastel12(1, 0) = 255 / 255.0f;
    pastel12(1, 1) = 255 / 255.0f;
    pastel12(1, 2) = 179 / 255.0f;
    pastel12(1, 3) = 1;

    pastel12(2, 0) = 190 / 255.0f;
    pastel12(2, 1) = 186 / 255.0f;
    pastel12(2, 2) = 218 / 255.0f;
    pastel12(2, 3) = 1;

    pastel12(3, 0) = 251 / 255.0f;
    pastel12(3, 1) = 128 / 255.0f;
    pastel12(3, 2) = 114 / 255.0f;
    pastel12(3, 3) = 1;

    pastel12(4, 0) = 128 / 255.0f;
    pastel12(4, 1) = 177 / 255.0f;
    pastel12(4, 2) = 211 / 255.0f;
    pastel12(4, 3) = 1;

    pastel12(5, 0) = 253 / 255.0f;
    pastel12(5, 1) = 180 / 255.0f;
    pastel12(5, 2) = 98 / 255.0f;
    pastel12(5, 3) = 1;

    pastel12(6, 0) = 179 / 255.0f;
    pastel12(6, 1) = 222 / 255.0f;
    pastel12(6, 2) = 105 / 255.0f;
    pastel12(6, 3) = 1;

    pastel12(7, 0) = 252 / 255.0f;
    pastel12(7, 1) = 205 / 255.0f;
    pastel12(7, 2) = 229 / 255.0f;
    pastel12(7, 3) = 1;

    pastel12(8, 0) = 217 / 255.0f;
    pastel12(8, 1) = 217 / 255.0f;
    pastel12(8, 2) = 217 / 255.0f;
    pastel12(8, 3) = 1;

    pastel12(9, 0) = 188 / 255.0f;
    pastel12(9, 1) = 128 / 255.0f;
    pastel12(9, 2) = 189 / 255.0f;
    pastel12(9, 3) = 1;

    pastel12(10, 0) = 204 / 255.0f;
    pastel12(10, 1) = 235 / 255.0f;
    pastel12(10, 2) = 197 / 255.0f;
    pastel12(10, 3) = 1;

    pastel12(11, 0) = 255 / 255.0f;
    pastel12(11, 1) = 237 / 255.0f;
    pastel12(11, 2) = 111 / 255.0f;
    pastel12(11, 3) = 1;

    regularColors->SetDoubleStrandColorScheme(pastel12);

  }

}

void SEAdenitaVisualModel::setupPropertyColors()
{
  ADNArray<float> purpleBlueYellow = ADNArray<float>(4, 3);
  purpleBlueYellow(0, 0) = 68.0f / 255.0f;
  purpleBlueYellow(0, 1) = 3.0f / 255.0f;
  purpleBlueYellow(0, 2) = 84.0f / 255.0f;
  purpleBlueYellow(0, 3) = 1.0f;

  purpleBlueYellow(1, 0) = 38.0f / 255.0f;
  purpleBlueYellow(1, 1) = 148.0f / 255.0f;
  purpleBlueYellow(1, 2) = 139.0f / 255.0f;
  purpleBlueYellow(1, 3) = 1.0f;

  purpleBlueYellow(2, 0) = 252.0f / 255.0f;
  purpleBlueYellow(2, 1) = 230.0f / 255.0f;
  purpleBlueYellow(2, 2) = 59.0f / 255.0f;
  purpleBlueYellow(2, 3) = 1.0f;

  propertyColorSchemes_.push_back(purpleBlueYellow);

  ADNArray<float> divergingPurpleOrange = ADNArray<float>(4, 4);
  divergingPurpleOrange(0, 0) = 94 / 255.0f;
  divergingPurpleOrange(0, 1) = 60 / 255.0f;
  divergingPurpleOrange(0, 2) = 153 / 255.0f;
  divergingPurpleOrange(0, 3) = 1.0f;

  divergingPurpleOrange(1, 0) = 178 / 255.0f;
  divergingPurpleOrange(1, 1) = 171 / 255.0f;
  divergingPurpleOrange(1, 2) = 210 / 255.0f;
  divergingPurpleOrange(1, 3) = 1.0f;

  divergingPurpleOrange(2, 0) = 253 / 255.0f;
  divergingPurpleOrange(2, 1) = 184 / 255.0f;
  divergingPurpleOrange(2, 2) = 99 / 255.0f;
  divergingPurpleOrange(2, 3) = 1.0f;

  divergingPurpleOrange(3, 0) = 230 / 255.0f;
  divergingPurpleOrange(3, 1) = 97 / 255.0f;
  divergingPurpleOrange(3, 2) = 1 / 255.0f;
  divergingPurpleOrange(3, 3) = 1.0f;

  propertyColorSchemes_.push_back(divergingPurpleOrange);

  //this is a color scheme that is especially hard for red-green blind people to distinguish differences
  //our PI is red-green blind
  ADNArray<float> magicColors = ADNArray<float>(4, 4);
  magicColors(0, 0) = 0 / 255.0f;
  magicColors(0, 1) = 255 / 255.0f;
  magicColors(0, 2) = 0 / 255.0f;
  magicColors(0, 3) = 1.0f;

  magicColors(1, 0) = 255 / 255.0f;
  magicColors(1, 1) = 255 / 255.0f;
  magicColors(1, 2) = 0 / 255.0f;
  magicColors(1, 3) = 1.0f;

  magicColors(2, 0) = 255 / 255.0f;
  magicColors(2, 1) = 150 / 255.0f;
  magicColors(2, 2) = 0 / 255.0f;
  magicColors(2, 3) = 1.0f;

  magicColors(3, 0) = 255 / 255.0f;
  magicColors(3, 1) = 0 / 255.0f;
  magicColors(3, 2) = 0 / 255.0f;
  magicColors(3, 3) = 1.0f;

  propertyColorSchemes_.push_back(magicColors);
}

void SEAdenitaVisualModel::changePropertyColors(int propertyIdx, int colorSchemeIdx)
{
  curColorType_ = static_cast<ColorType>(propertyIdx);

  if (propertyIdx == MELTTEMP || propertyIdx == GIBBS) {
    auto meltingTempColors = colors_.at(MELTTEMP);
    auto gibbsColors = colors_.at(GIBBS);

    SEConfig& config = SEConfig::GetInstance();

    auto p = PIPrimer3::GetInstance();

    auto parts = nanorobot_->GetParts();
    ADNLogger& logger = ADNLogger::GetLogger();

    
    SB_FOR(auto part, parts) {
      auto regions = p.GetBindingRegions(part);

      SB_FOR(auto region, regions) {
        auto mt = region->getTemp();
        auto gibbs = region->getGibbs();
        auto groupNodes = region->getGroupNodes();

        for (unsigned i = 0; i < groupNodes->size(); i++) {
          auto node = groupNodes->getReferenceTarget(i);
          ADNPointer<ADNNucleotide> nt = static_cast<ADNNucleotide*>(node);
          auto baseSegment = nt->GetBaseSegment();

          ADNArray<float> meltTempColor = calcPropertyColor(colorSchemeIdx, config.min_melting_temp, config.max_melting_temp, mt);
          meltingTempColors->SetColor(meltTempColor, nt);
          meltingTempColors->SetColor(meltTempColor, baseSegment);

          ADNArray<float> gibbsColor = calcPropertyColor(colorSchemeIdx, config.min_gibbs_free_energy, config.max_gibbs_free_energy, gibbs);
          gibbsColors->SetColor(gibbsColor, nt);
          gibbsColors->SetColor(gibbsColor, baseSegment);


        }
      }
    }
  }
}

void SEAdenitaVisualModel::changeHighlight(int highlightIdx)
{
  if (highlightIdx == 0) {
    highlightType_ = HighlightType::NONE;
  }
  else if (highlightIdx == 1) {
    highlightType_ = HighlightType::CROSSOVERS;
  }
  else if (highlightIdx == 2) {
    highlightType_ = HighlightType::GC;
  }
  else if (highlightIdx == 3) {
    highlightType_ = HighlightType::TAGGED;
  }

  prepareDiscreteScales();
  prepareTransition();
  highlightNucleotides();
  SAMSON::requestViewportUpdate();

}

void SEAdenitaVisualModel::display() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop. This is the main function of your visual model. 
	// Implement this function to display things in SAMSON, for example thanks to the utility functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)

  //ADNLogger& logger = ADNLogger::GetLogger();

  auto ed = SAMSON::getActiveEditor();
  /*logger.Log(ed->getName());*/
  if (ed->getName() == "SEERotation" || ed->getName() == "SEETranslation") {
    prepareDiscreteScales();
  }

  displayTransition(false);
  
}


void SEAdenitaVisualModel::prepareNucleotides()
{
  SEConfig& config = SEConfig::GetInstance();
  ADNLogger& logger = ADNLogger::GetLogger();

  auto parts = nanorobot_->GetParts();

  MSVColors * curColors = colors_[curColorType_];

  SB_FOR(auto part, parts) {
    auto singleStrands = nanorobot_->GetSingleStrands(part);
    SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
      auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);
      SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {
        unsigned int index = ntMap_[nt()];

        positionsNt_(index, 0) = nanorobot_->GetNucleotideBackbonePosition(nt)[0].getValue();
        positionsNt_(index, 1) = nanorobot_->GetNucleotideBackbonePosition(nt)[1].getValue();
        positionsNt_(index, 2) = nanorobot_->GetNucleotideBackbonePosition(nt)[2].getValue();

        colorsENt_.SetRow(index, nucleotideEColor_);
        nodeIndicesNt_(index) = nt->getNodeIndex();
        flagsNt_(index) = nt->getInheritedFlags();

        auto baseColor = curColors->GetColor(nt);
        colorsVNt_.SetRow(index, baseColor);
        radiiVNt_(index) = config.nucleotide_V_radius;
        radiiENt_(index) = config.nucleotide_E_radius;

        auto type = nt->GetBaseSegment()->GetCellType();
        if (type == CellType::LoopPair) {
          radiiVNt_(index) = radiiVNt_(index) * 0.7f;;
        }

        //strand direction
        if (nanorobot_->GetNucleotideEnd(nt) == End::ThreePrime) {
          radiiENt_(index) = config.nucleotide_E_radius;
        }

        if (!ss->isVisible()) {
          colorsVNt_(index, 3) = 0.0f;
          radiiVNt_(index) = 0.0f;
          radiiENt_(index) = 0.0f;
          colorsENt_(index, 3) = 0.0f;
        }
        else if (!nt->isVisible()) {
          colorsVNt_(index, 3) = 0.0f;
          colorsENt_(index, 3) = 0.0f;
        }
      }
    }
  }
}

void SEAdenitaVisualModel::prepareSingleStrands()
{
  SEConfig& config = SEConfig::GetInstance();
  auto parts = nanorobot_->GetParts();
  
  MSVColors * curColors = colors_[curColorType_];

  SB_FOR(auto part, parts) {
    auto singleStrands = nanorobot_->GetSingleStrands(part);
    SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
      auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);
      SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {

        unsigned int index = ntMap_[nt()];

        if (curColorType_ == REGULAR) {
          auto color = curColors->GetColor(ss);
          colorsVSS_.SetRow(index, color);
        }
        else if (curColorType_ == MELTTEMP || curColorType_ == GIBBS) {
          auto color = curColors->GetColor(ss);
          colorsVSS_.SetRow(index, color);
        }

        colorsESS_(index, 0) = colorsVSS_(index, 0);
        colorsESS_(index, 1) = colorsVSS_(index, 1);
        colorsESS_(index, 2) = colorsVSS_(index, 2);
        colorsESS_(index, 3) = colorsVSS_(index, 3);

        radiiVSS_(index) = config.nucleotide_V_radius;

        radiiESS_(index) = config.nucleotide_V_radius;

        //strand direction
        if (nanorobot_->GetNucleotideEnd(nt) == End::ThreePrime) {
          radiiESS_(index) = config.nucleotide_E_radius;
        }

        if (!ss->isVisible()) {
          radiiVSS_(index) = 0.0f;
          radiiESS_(index) = 0.0f;
        }
      }
    }
  }
}

void SEAdenitaVisualModel::prepareDoubleStrands()
{
  SEConfig& config = SEConfig::GetInstance();
  ADNLogger& logger = ADNLogger::GetLogger();

  auto parts = nanorobot_->GetParts();

  MSVColors * curColors = colors_[curColorType_];
  positionsDS_ = ADNArray<float>(3, nPositionsDS_);
  radiiVDS_ = ADNArray<float>(nPositionsDS_);
  flagsDS_ = ADNArray<unsigned int>(nPositionsDS_);
  colorsVDS_ = ADNArray<float>(4, nPositionsDS_);
  nodeIndicesDS_ = ADNArray<unsigned int>(nPositionsDS_);
  
  SB_FOR(auto part, parts) {
    auto doubleStrands = part->GetDoubleStrands();
    //for now also the base segments that have loops and skips are displayed as sphere
    SB_FOR(auto doubleStrand, doubleStrands) {
      auto baseSegments = doubleStrand->GetBaseSegments();
      SB_FOR(auto baseSegment, baseSegments) {
        auto index = bsMap_[baseSegment];
        auto cell = baseSegment->GetCell();

        SBPosition3 pos = baseSegment->GetPosition();
        positionsDS_(index, 0) = (float)pos.v[0].getValue();
        positionsDS_(index, 1) = (float)pos.v[1].getValue();
        positionsDS_(index, 2) = (float)pos.v[2].getValue();

        auto color = curColors->GetColor(baseSegment);
        colorsVDS_.SetRow(index, color);
        radiiVDS_(index) = config.base_pair_radius;

        auto type = baseSegment->GetCellType();
        if (type == CellType::SkipPair) {
          colorsVDS_(index, 1) = 0.0f;
          colorsVDS_(index, 2) = 0.0f;
          colorsVDS_(index, 3) = 0.3f;
        }

        if (type == CellType::LoopPair) {
          radiiVDS_(index) = config.base_pair_radius * 1.2f;
          colorsVDS_(index, 0) = 0.0f;
          colorsVDS_(index, 3) = 0.3f;
        }

        flagsDS_(index) = baseSegment->getInheritedFlags();

        nodeIndicesDS_(index) = baseSegment->getNodeIndex();

        if (!doubleStrand->isVisible()) {
          colorsVDS_(index, 3) = 0.0f;
          radiiVDS_(index) = 0.0f;
        }
        else if (!baseSegment->isVisible()) {
          colorsVDS_(index, 3) = 0.0f;
        }

        ++index;

      }
    }
  }
}

void SEAdenitaVisualModel::displayNucleotides(bool forSelection)
{

  if (forSelection) {
    if (nCylindersNt_ > 0) {
      SAMSON::displayCylindersSelection(
        nCylindersNt_,
        nPositionsNt_,
        indicesNt_.GetArray(),
        positionsNt_.GetArray(),
        radiiENt_.GetArray(),
        nullptr,
        nodeIndicesNt_.GetArray());
    }

    SAMSON::displaySpheresSelection(
      nPositionsNt_,
      positionsNt_.GetArray(),
      radiiVNt_.GetArray(),
      nodeIndicesNt_.GetArray()
    );
  }
  else {
    if (nCylindersNt_ > 0) {
      SAMSON::displayCylinders(
        nCylindersNt_,
        nPositionsNt_,
        indicesNt_.GetArray(),
        positionsNt_.GetArray(),
        radiiENt_.GetArray(),
        nullptr,
        colorsENt_.GetArray(),
        flagsNt_.GetArray());
    }

    SAMSON::displaySpheres(
      nPositionsNt_,
      positionsNt_.GetArray(),
      radiiVNt_.GetArray(),
      colorsVNt_.GetArray(),
      flagsNt_.GetArray());
  }
}

void SEAdenitaVisualModel::displaySingleStrands(bool forSelection)
{
  if (forSelection) {
    if (nCylindersNt_ > 0) {
      SAMSON::displayCylindersSelection(
        nCylindersNt_,
        nPositionsNt_,
        indicesNt_.GetArray(),
        positionsNt_.GetArray(),
        radiiESS_.GetArray(),
        nullptr,
        nodeIndicesNt_.GetArray());
    }

    SAMSON::displaySpheresSelection(
      nPositionsNt_,
      positionsNt_.GetArray(),
      radiiVSS_.GetArray(),
      nodeIndicesNt_.GetArray()
    );
  }
  else {
    if (nCylindersNt_ > 0) {
      SAMSON::displayCylinders(
        nCylindersNt_,
        nPositionsNt_,
        indicesNt_.GetArray(),
        positionsNt_.GetArray(),
        radiiESS_.GetArray(),
        nullptr,
        colorsESS_.GetArray(),
        flagsNt_.GetArray());
    }

    SAMSON::displaySpheres(
      nPositionsNt_,
      positionsNt_.GetArray(),
      radiiVSS_.GetArray(),
      colorsVSS_.GetArray(),
      flagsNt_.GetArray());
  }
}

void SEAdenitaVisualModel::displayDoubleStrands(bool forSelection)
{

  if (forSelection) {
    SAMSON::displaySpheresSelection(
      nPositionsDS_,
      positionsDS_.GetArray(),
      radiiVDS_.GetArray(),
      nodeIndicesDS_.GetArray()
    );
  }
  else {
    SAMSON::displaySpheres(
      nPositionsDS_,
      positionsDS_.GetArray(),
      radiiVDS_.GetArray(),
      colorsVDS_.GetArray(),
      flagsDS_.GetArray());
  }
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

  displayTransition(true);

}

void SEAdenitaVisualModel::displayBasePairConnections(bool onlySelected)
{
  if (scale_ < NUCLEOTIDES && scale_ > SINGLE_STRANDS) return;

  SEConfig& config = SEConfig::GetInstance();

  auto baseColors = colors_.at(REGULAR);
  auto parts = nanorobot_->GetParts();

  unsigned int numPairedNts = 0;
  std::map<ADNNucleotide*, unsigned int> ntMap;

  //determine how many nucleotides have pairs 

  SB_FOR(auto part, parts) {
    auto singleStrands = nanorobot_->GetSingleStrands(part);
    SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
      auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);
      SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {
        if (nt->GetPair() != nullptr) {
          ntMap.insert(make_pair(nt(), numPairedNts));
          ++numPairedNts;
        }
      }
    }
  }

  unsigned int nPositions = boost::numeric_cast<unsigned int>(ntMap.size());
  unsigned int nCylinders = nPositions / 2;

  ADNArray<float> positions = ADNArray<float>(3, nPositions);
  ADNArray<unsigned int> indices = ADNArray<unsigned int>(nCylinders * 2);
  ADNArray<float> radii = ADNArray<float>(nPositions);
  ADNArray<float> colors = ADNArray<float>(4, nPositions);
  ADNArray<unsigned int> flags = ADNArray<unsigned int>(nPositions);

  unsigned int j = 0;
  std::vector<unsigned int> registerIndices;
  for (auto &p : ntMap) {
    ADNPointer<ADNNucleotide> nt = p.first;
    ADNPointer<ADNNucleotide> pair = nt->GetPair();
    unsigned int index = p.second;
    
    SBPosition3 pos = nt->GetBackbonePosition();
   
    positions(index, 0) = pos[0].getValue();
    positions(index, 1) = pos[1].getValue();
    positions(index, 2) = pos[2].getValue();

    radii(index) = config.nucleotide_E_radius;
    colors.SetRow(index, baseColors->GetColor(nt));
    flags(index) = 0;

    if (std::find(registerIndices.begin(), registerIndices.end(), ntMap[pair()]) == registerIndices.end()) {
      // we only need to insert the indices once per pair
      indices(2 * j) = index;
      indices(2 * j + 1) = ntMap[pair()];
      registerIndices.push_back(index);
      
      ++j;
    }

    if (onlySelected) {
      if (!nt->isSelected() && !pair->isSelected()) {
        radii(index) = 0;
      }
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

void SEAdenitaVisualModel::displayForDebugging()
{
  SEConfig& config = SEConfig::GetInstance();

  if (config.debugOptions.display_nucleotide_basis) {
    auto parts = nanorobot_->GetParts();

    SB_FOR(auto part, parts) {
      auto singleStrands = nanorobot_->GetSingleStrands(part);
      SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
        auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);
        SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {
          if (nt->isSelected()) {
            ADNDisplayHelper::displayBaseVectors(nt, scale_);
          }
        }
      }
    }
  }
  if (config.debugOptions.display_base_pairing) {
    displayBasePairConnections(true);
  }


}

void SEAdenitaVisualModel::displayCircularDNAConnection()
{

  if (scale_ < (float)NUCLEOTIDES || scale_ > (float)SINGLE_STRANDS) return;

  auto parts = nanorobot_->GetParts();

  SB_FOR(auto part, parts) {
    auto singleStrands = nanorobot_->GetSingleStrands(part);
    SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
      if (ss->IsCircular()) {

        SEConfig& config = SEConfig::GetInstance();

        auto startNt = ss->GetFivePrime();
        auto endNt = ss->GetThreePrime();
        int startIdx = ntMap_[startNt()];
        int endIdx = ntMap_[endNt()];

        float * startPos = new float[3];
        startPos[0] = positions_(startIdx, 0);
        startPos[1] = positions_(startIdx, 1);
        startPos[2] = positions_(startIdx, 2);
        
        float * endPos = new float[3];
        endPos[0] = positions_(endIdx, 0);
        endPos[1] = positions_(endIdx, 1);
        endPos[2] = positions_(endIdx, 2);

        float * color = new float[4];
        color[0] = colorsE_(endIdx, 0);
        color[1] = colorsE_(endIdx, 1);
        color[2] = colorsE_(endIdx, 2);
        color[3] = colorsE_(endIdx, 3);
        
        auto radius = radiiE_(startIdx);

        ADNDisplayHelper::displayDirectedCylinder(startPos, endPos, color, radius);
        
        delete[] startPos;
        delete[] endPos;
        delete[] color;

      }
    }
  }
}

void SEAdenitaVisualModel::displayTags()
{
  //tagged nucleotides should be saved in a list
  auto parts = nanorobot_->GetParts();

  SB_FOR(auto part, parts) {
    auto singleStrands = nanorobot_->GetSingleStrands(part);
    SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
      auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);
      SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {
        ADNDisplayHelper::displayText(nt->GetBackbonePosition(), nt->getTag());
      }
    }
  }
}

void SEAdenitaVisualModel::prepareAtoms()
{
  SEConfig& config = SEConfig::GetInstance();
  ADNLogger& logger = ADNLogger::GetLogger();
  MSVColors * curColors = colors_[curColorType_];

  auto parts = nanorobot_->GetParts();

  SB_FOR(auto part, parts) {
    auto singleStrands = nanorobot_->GetSingleStrands(part);
    SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
      auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);
      SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {
        auto atoms = nt->GetAtoms();
        SB_FOR(ADNPointer<ADNAtom> a, atoms) {

          auto index = atomMap_[a()];

          positionsAtom_(index, 0) = a->GetPosition()[0].getValue();
          positionsAtom_(index, 1) = a->GetPosition()[1].getValue();
          positionsAtom_(index, 2) = a->GetPosition()[2].getValue();

          auto color = curColors->GetColor(a);
          colorsVAtom_.SetRow(index, color);

          nodeIndicesAtom_(index) = a->getNodeIndex();
          flagsAtom_(index) = a->getInheritedFlags();

          radiiVAtom_(index) = a->getVanDerWaalsRadius().getValue();

          if (!ss->isVisible()) {
            colorsVAtom_(index, 3) = 0.0f;
            radiiVAtom_(index) = 0.0f;
          }
          else if (!nt->isVisible()) {
            radiiVAtom_(index) = 0.0f;
            colorsVAtom_(index, 3) = 0.0f;
          }
          /*else if (!a->isVisible()) {
            colorsVAtom_(index, 3) = 0.0f;
            radiiVAtom_(index) = 0.0f;
          }*/
        }
      }
    }
  }
      
}

void SEAdenitaVisualModel::highlightNucleotides()
{
  if (scale_ < (float)NUCLEOTIDES) return;
  if (highlightType_ == NONE) return;

  float * colorHighlight = new float[4];
  colorHighlight[0] = 0.2f;
  colorHighlight[1] = 0.8f;
  colorHighlight[2] = 0.2f;
  colorHighlight[3] = 1.0f;

  if (highlightType_ == GC) {

    auto parts = nanorobot_->GetParts();

    vector<unsigned int> indicesHighlight;
    vector<unsigned int> indicesContext;

    if (scale_ >= (float)NUCLEOTIDES && scale_ <= (float)SINGLE_STRANDS) {
      //deemphasize the context with grey color

      for (auto p : ntMap_) {
        auto index = p.second;
        indicesContext.push_back(index);
      }

      SB_FOR(auto part, parts) {
        auto singleStrands = nanorobot_->GetSingleStrands(part);
        SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
          auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);
          SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {
            auto index = ntMap_[nt()];
            if (nt->getNucleotideType() == DNABlocks::DC || nt->getNucleotideType() == DNABlocks::DG) {
              indicesHighlight.push_back(index);
            }
          }
        }
      }
    }
    else if (scale_ == (float)DOUBLE_STRANDS) {
      for (auto p : bsMap_) {
        auto index = p.second;
        indicesContext.push_back(index);
      }
      SB_FOR(auto part, parts) {
        auto doubleStrands = part->GetDoubleStrands();
        SB_FOR(auto doubleStrand, doubleStrands) {
          auto baseSegments = doubleStrand->GetBaseSegments();
          SB_FOR(auto baseSegment, baseSegments) {
            auto index = bsMap_[baseSegment];
            auto nts = baseSegment->GetNucleotides();
            SB_FOR(auto nt, nts) {
              if (nt->getNucleotideType() == DNABlocks::DC || nt->getNucleotideType() == DNABlocks::DG) {
                indicesHighlight.push_back(index);
              }
              else {
                indicesContext.push_back(index);
              }
            }

          }
        }
      }
    }
    ADNDisplayHelper::deemphasizeCylinderColors(colorsV_, indicesContext, 0.5f, 0.5f, 0.5f, 1.0f);
    ADNDisplayHelper::deemphasizeCylinderColors(colorsE_, indicesContext, 0.5f, 0.5f, 0.5f, 1.0f);
    ADNDisplayHelper::replaceCylinderColors(colorsV_, indicesHighlight, colorHighlight);
    ADNDisplayHelper::replaceCylinderColors(colorsE_, indicesHighlight, colorHighlight);

    delete[] colorHighlight;
  }
  else if (highlightType_ == HighlightType::CROSSOVERS) {
    auto parts = nanorobot_->GetParts();
    vector<unsigned int> indicesHighlight;
    vector<unsigned int> indicesContext;

    if (scale_ >= (float)NUCLEOTIDES && scale_ <= (float)SINGLE_STRANDS) {
      for (auto p : ntMap_) {
        auto index = p.second;
        indicesContext.push_back(index);
      }
      SB_FOR(auto part, parts) {
        auto xos = PICrossovers::GetCrossovers(part);
        for (auto xo : xos) {
          auto startNt = xo.first;
          auto endNt = xo.second;

          auto startIdx = ntMap_[startNt()];
          auto endIdx = ntMap_[endNt()];

          indicesHighlight.push_back(startIdx);
          indicesHighlight.push_back(endIdx);
        }

      }
    }
    else if (scale_ == (float)DOUBLE_STRANDS) {
      for (auto p : bsMap_) {
        auto index = p.second;
        indicesContext.push_back(index);
      }
      SB_FOR(auto part, parts) {
        auto xos = PICrossovers::GetCrossovers(part);
        for (auto xo : xos) {
          auto startNt = xo.first;
          auto endNt = xo.second;
          auto startBs = startNt->GetBaseSegment();
          auto endBs = endNt->GetBaseSegment();

          auto startIdx = bsMap_[startBs()];
          auto endIdx = bsMap_[endBs()];

          indicesHighlight.push_back(startIdx);
          indicesHighlight.push_back(endIdx);
        }
      }
    }
    else if (highlightType_ == HighlightType::CROSSOVERS) {
      auto parts = nanorobot_->GetParts();
      vector<unsigned int> indicesHighlight;
      vector<unsigned int> indicesContext;

      if (scale_ >= (float)NUCLEOTIDES && scale_ <= (float)SINGLE_STRANDS) {
        for (auto p : ntMap_) {
          auto index = p.second;
          indicesContext.push_back(index);
        }
        SB_FOR(auto part, parts) {
          auto xos = PICrossovers::GetCrossovers(part);
          for (auto xo : xos) {
            auto startNt = xo.first;
            auto endNt = xo.second;

            auto startIdx = ntMap_[startNt()];
            auto endIdx = ntMap_[endNt()];

            indicesHighlight.push_back(startIdx);
            indicesHighlight.push_back(endIdx);
          }

        }
      }
      else if (scale_ == (float)DOUBLE_STRANDS) {
        for (auto p : bsMap_) {
          auto index = p.second;
          indicesContext.push_back(index);
        }
        SB_FOR(auto part, parts) {
          auto xos = PICrossovers::GetCrossovers(part);
          for (auto xo : xos) {
            auto startNt = xo.first;
            auto endNt = xo.second;
            auto startBs = startNt->GetBaseSegment();
            auto endBs = endNt->GetBaseSegment();

            auto startIdx = bsMap_[startBs()];
            auto endIdx = bsMap_[endBs()];

            indicesHighlight.push_back(startIdx);
            indicesHighlight.push_back(endIdx);
          }
        }
      }
      ADNDisplayHelper::deemphasizeCylinderColors(colorsV_, indicesContext, 0.5f, 0.5f, 0.5f, 1.0f);
      ADNDisplayHelper::deemphasizeCylinderColors(colorsE_, indicesContext, 0.5f, 0.5f, 0.5f, 1.0f);

      ADNDisplayHelper::replaceCylinderColors(colorsV_, indicesHighlight, colorHighlight);
      ADNDisplayHelper::replaceCylinderColors(colorsE_, indicesHighlight, colorHighlight);

      delete[] colorHighlight;
    }

  }
  else if (highlightType_ == TAGGED) {

    colorHighlight[0] = 1.0f;
    colorHighlight[1] = 0.1f;
    colorHighlight[2] = 0.2f;
    colorHighlight[3] = 1.0f;

    auto parts = nanorobot_->GetParts();

    vector<unsigned int> indicesHighlight;
    vector<unsigned int> indicesContext;

    if (scale_ >= (float)NUCLEOTIDES && scale_ <= (float)SINGLE_STRANDS) {

      for (auto p : ntMap_) {
        auto index = p.second;
        indicesContext.push_back(index);
      }

      SB_FOR(auto part, parts) {
        auto singleStrands = nanorobot_->GetSingleStrands(part);
        SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
          auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);
          SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {
            auto index = ntMap_[nt()];
            if (nt->hasTag()) {
              indicesHighlight.push_back(index);
            }
          }
        }
      }
    }
    else if (scale_ == (float)DOUBLE_STRANDS) {
      for (auto p : bsMap_) {
        auto index = p.second;
        indicesContext.push_back(index);
      }
      SB_FOR(auto part, parts) {
        auto doubleStrands = part->GetDoubleStrands();
        SB_FOR(auto doubleStrand, doubleStrands) {
          auto baseSegments = doubleStrand->GetBaseSegments();
          SB_FOR(auto baseSegment, baseSegments) {
            auto index = bsMap_[baseSegment];
            auto nts = baseSegment->GetNucleotides();
            SB_FOR(auto nt, nts) {
              if (nt->hasTag()) {
                indicesHighlight.push_back(index);
              }
              else {
                indicesContext.push_back(index);
              }
            }
          }
        }
      }
    }
    ADNDisplayHelper::deemphasizeCylinderColors(colorsV_, indicesContext, 0.5f, 0.5f, 0.5f, 1.0f);
    ADNDisplayHelper::deemphasizeCylinderColors(colorsE_, indicesContext, 0.5f, 0.5f, 0.5f, 1.0f);
    ADNDisplayHelper::replaceCylinderColors(colorsV_, indicesHighlight, colorHighlight);
    ADNDisplayHelper::replaceCylinderColors(colorsE_, indicesHighlight, colorHighlight);

    delete[] colorHighlight;
  }
}

ADNArray<float> SEAdenitaVisualModel::calcPropertyColor(int colorSchemeIdx, float min, float max, float val) {

  ADNArray<float> color = ADNArray<float>(4);

  auto colorScheme = propertyColorSchemes_[colorSchemeIdx];

  if (val == FLT_MAX) { //if region is unbound
    color(0) = colorScheme(0, 0);
    color(1) = colorScheme(0, 1);
    color(2) = colorScheme(0, 2);
    color(3) = colorScheme(0, 3);

    return color;
  }

  auto numColors = colorScheme.GetNumElements();

  int idx1;
  int idx2;
  float fractBetween = 0;

  //Y = (X - A) / (B - A) * (D - C) + C
  double mappedVal = ADNAuxiliary::mapRange(val, min, max, 0, 1);

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

  color(0) = (colorScheme(idx2, 0) - colorScheme(idx1, 0)) * fractBetween + colorScheme(idx1, 0);
  color(1) = (colorScheme(idx2, 1) - colorScheme(idx1, 1)) * fractBetween + colorScheme(idx1, 1);
  color(2) = (colorScheme(idx2, 2) - colorScheme(idx1, 2)) * fractBetween + colorScheme(idx1, 2);
  color(3) = 1.0f;

  return color;
}


void SEAdenitaVisualModel::showBasePairing(bool show)
{
  showBasePairing_ = show;
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

  if (baseEvent->getType() == SBBaseEvent::HighlightingFlagChanged){
    highlightFlagChanged();
  }

  if (baseEvent->getType() == SBBaseEvent::SelectionFlagChanged) {
    highlightFlagChanged();
  }

  if (baseEvent->getType() == SBBaseEvent::VisibilityFlagChanged) {
    changeScale(scale_, false);
  }

  if (baseEvent->getType() == SBBaseEvent::MaterialChanged) {
    prepareDiscreteScales();
    changeScale(scale_, false);
  }

  //ADNLogger& logger = ADNLogger::GetLogger();
  //logger.Log(QString("onBaseEvent"));

}

void SEAdenitaVisualModel::onDocumentEvent(SBDocumentEvent* documentEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle document events 


}

void SEAdenitaVisualModel::onStructuralEvent(SBStructuralEvent* structuralEvent) {
	
	//// SAMSON Element generator pro tip: implement this function if you need to handle structural events (e.g. when a structural node for which you provide a visual representation is updated)
  //ADNLogger& logger = ADNLogger::GetLogger();
  //logger.Log(QString("onStructuralEvent"));
  //if (structuralEvent->getType() == SBStructuralEvent::MobilityFlagChanged) {
  //  prepareArraysNoTranstion();
  //  //ADNLogger& logger = ADNLogger::GetLogger();
  //  //logger.Log(QString("MobilityFlagChanged"));
  //}

 //logger.Log(structuralEvent->getTypeString(structuralEvent->getType()));
}

