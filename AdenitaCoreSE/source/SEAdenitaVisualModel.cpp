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

  MSVColors * regularColors = new MSVColors();
  MSVColors * meltTempColors = new MSVColors();
  colors_[ColorType::REGULAR] = regularColors;
  colors_[ColorType::MELTTEMP] = meltTempColors;

  //setup the display properties
  nucleotideEColor_ = ADNArray<float>(4);
  nucleotideEColor_(0) = config.nucleotide_E_Color[0];
  nucleotideEColor_(1) = config.nucleotide_E_Color[1];
  nucleotideEColor_(2) = config.nucleotide_E_Color[2];
  nucleotideEColor_(3) = config.nucleotide_E_Color[3];

  auto parts = nanorobot_->GetParts();

  SB_FOR(auto part, parts) {
    part->connectBaseSignalToSlot(
      this,
      SB_SLOT(&SEAdenitaVisualModel::onBaseEvent));
  }

  SB_FOR(auto part, parts) {
    part->connectStructuralSignalToSlot(
      this,
      SB_SLOT(&SEAdenitaVisualModel::onStructuralEvent));
  }
  
  SAMSON::getActiveDocument()->connectDocumentSignalToSlot(
    this, 
    SB_SLOT(&SEAdenitaVisualModel::onDocumentEvent));
  
  changeScale(7);

  //orderVisibility();

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

void SEAdenitaVisualModel::changeDiscreteScale(int scale, bool createIndex /*= true*/)
{
  scale_ = scale;

  if (scale < (float)NUCLEOTIDES_SIDECHAIN) {
  }
  else if (scale >= (float)NUCLEOTIDES_SIDECHAIN && scale < (float)DOUBLE_STRANDS) {
    initNucleotideArraysForDisplay(createIndex);
  }
  else if (scale >= (float)DOUBLE_STRANDS) {
    initBaseSegmentArraysForDisplay(createIndex);
  }

  SAMSON::requestViewportUpdate();

}

void SEAdenitaVisualModel::changeScale(double scale, bool createIndex/* = true*/)
{
  scale_ = scale;
  if (scale < (float)NUCLEOTIDES_SIDECHAIN) {
  }
  else if (scale >= (float)NUCLEOTIDES_SIDECHAIN && scale < (float)DOUBLE_STRANDS) {
    initNucleotideArraysForDisplay(createIndex);
  }
  else if(scale >= (float)DOUBLE_STRANDS) {
    initBaseSegmentArraysForDisplay(createIndex);
  }

  prepareArraysForDisplay();


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

void SEAdenitaVisualModel::initNucleotideArraysForDisplay(bool createIndex /* = true */)
{
  auto singleStrands = nanorobot_->GetSingleStrands();

  unsigned int nPositions = nanorobot_->GetNumberOfNucleotides();
  unsigned int nCylinders = boost::numeric_cast<unsigned int>(nPositions - singleStrands.size());

  nPositions_ = nPositions;
  nCylinders_ = nCylinders;
  positions_ = ADNArray<float>(3, nPositions);
  radiiV_ = ADNArray<float>(nPositions);
  radiiE_ = ADNArray<float>(nPositions);
  colorsV_ = ADNArray<float>(4, nPositions);
  colorsE_ = ADNArray<float>(4, nPositions);
  capData_ = ADNArray<unsigned int>(nPositions);
  flags_ = ADNArray<unsigned int>(nPositions);
  nodeIndices_ = ADNArray<unsigned int>(nPositions);

  if (createIndex) {
    indices_ = getNucleotideIndices();
  }

}

void SEAdenitaVisualModel::initBaseSegmentArraysForDisplay(bool createIndex /*= true*/)
{

  unsigned int nPositions = nanorobot_->GetNumberOfBaseSegments();
  //unsigned int nCylinders = boost::numeric_cast<unsigned int>(nPositions - singleStrands.size());

  nPositions_ = nPositions;
  //nCylinders_ = nCylinders;
  positions_ = ADNArray<float>(3, nPositions);
  radiiV_ = ADNArray<float>(nPositions);
  radiiE_ = ADNArray<float>(nPositions);
  colorsV_ = ADNArray<float>(4, nPositions);
  colorsE_ = ADNArray<float>(4, nPositions);
  capData_ = ADNArray<unsigned int>(nPositions);
  flags_ = ADNArray<unsigned int>(nPositions);
  nodeIndices_ = ADNArray<unsigned int>(nPositions);

  if (createIndex) {
    indices_ = getBaseSegmentIndices();
  }

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
  unsigned int nCylinders = 0;

  ADNArray<unsigned int> indices = ADNArray<unsigned int>(nCylinders * 2);

  return indices;

}

void SEAdenitaVisualModel::prepareArraysForDisplay()
{

  SEConfig& config = SEConfig::GetInstance();

  if (nanorobot_ == nullptr) return;
  
  if (scale_ > MAX_SCALE) scale_ = MAX_SCALE;

  float interpolated = 1.0f - (floor(scale_ + 1) - scale_);

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
  else if (scale_ < (float)DOUBLE_STRANDS) {
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
  /*initArraysForDisplay(0, 0);*/
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
  ADNLogger& logger = ADNLogger::GetLogger();
  
  auto parts = nanorobot_->GetParts();

  auto colors = colors_.at(REGULAR);

  SB_FOR(auto part, parts) {

    auto singleStrands = nanorobot_->GetSingleStrands(part);

    SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {

      auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);

      SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {

        auto index = ntMap_[nt()];

        auto bbPos = nanorobot_->GetNucleotideBackbonePosition(nt);
        float minX = bbPos[0].getValue();
        float minY = bbPos[1].getValue();
        float minZ = bbPos[2].getValue();

        auto scPos = nanorobot_->GetNucleotideSidechainPosition(nt);
        float maxX = scPos[0].getValue();
        float maxY = scPos[1].getValue();
        float maxZ = scPos[2].getValue();

        positions_(index, 0) = minX + iv * (maxX - minX);
        positions_(index, 1) = minY + iv * (maxY - minY);
        positions_(index, 2) = minZ + iv * (maxZ - minZ);

        nodeIndices_(index) = nt->getNodeIndex();
        flags_(index) = nt->getInheritedFlags();
        //auto baseColor = getBaseColor(nt->getResidueType());
        auto baseColor = colors->GetColor(nt);
        colorsV_.SetRow(index, baseColor);

        colorsE_.SetRow(index, nucleotideEColor_);

        radiiV_(index) = config.nucleotide_V_radius;
        radiiE_(index) = config.nucleotide_E_radius;
        
        if (!ss->isVisible()) {
          colorsV_(index, 3) = 0.0f;
          radiiV_(index) = 0.0f;
          radiiE_(index) = 0.0f;
          colorsE_(index, 3) = 0.0f;
        }
        else if (!nt->isVisible()) {
          colorsV_(index, 3) = 0.0f;
          colorsE_(index, 3) = 0.0f;
        }

      }
      
    }
  }
}

void SEAdenitaVisualModel::prepareScale4to5(double iv, bool forSelection /*= false*/)
{
  SEConfig& config = SEConfig::GetInstance();
  ADNLogger& logger = ADNLogger::GetLogger();

  auto parts = nanorobot_->GetParts();

  float minERadius = config.nucleotide_E_radius;
  float maxERadius = config.nucleotide_V_radius;
  float intervalERadius = maxERadius - minERadius;
  float iERadius = minERadius + iv * intervalERadius;

  auto colors = colors_[ColorType::REGULAR];

  SB_FOR(auto part, parts) {

    auto singleStrands = nanorobot_->GetSingleStrands(part);

    SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {

      auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);

      SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {

        auto index = ntMap_[nt()];

        auto scPos = nanorobot_->GetNucleotideSidechainPosition(nt);

        positions_(index, 0) = nanorobot_->GetNucleotideSidechainPosition(nt)[0].getValue();
        positions_(index, 1) = nanorobot_->GetNucleotideSidechainPosition(nt)[1].getValue();
        positions_(index, 2) = nanorobot_->GetNucleotideSidechainPosition(nt)[2].getValue();

        radiiV_(index) = config.nucleotide_V_radius;
        colorsE_.SetRow(index, nucleotideEColor_);
        nodeIndices_(index) = nt->getNodeIndex();
        flags_(index) = nt->getInheritedFlags();

        //auto baseColor = getBaseColor(nt->getResidueType());
        auto baseColor = colors->GetColor(nt);

        if (ss->IsScaffold()) {
          float maxVColorR;
          float maxVColorG;
          float maxVColorB;
          float maxVColorA;

          maxVColorR = config.nucleotide_E_Color[0];
          maxVColorG = config.nucleotide_E_Color[1];
          maxVColorB = config.nucleotide_E_Color[2];
          maxVColorA = config.nucleotide_E_Color[3];

          float minVColorR = baseColor(0);
          float minVColorG = baseColor(1);
          float minVColorB = baseColor(2);
          float minVColorA = baseColor(3);

          colorsV_(index, 0) = minVColorR + iv * (maxVColorR - minVColorR);
          colorsV_(index, 1) = minVColorG + iv * (maxVColorG - minVColorG);
          colorsV_(index, 2) = minVColorB + iv * (maxVColorB - minVColorB);
          colorsV_(index, 3) = minVColorA + iv * (maxVColorA - minVColorA);

          radiiE_(index) = iERadius;

        }
        else {
          colorsV_.SetRow(index, baseColor);
          radiiE_(index) = config.nucleotide_E_radius;
        }


        if (!ss->isVisible()) {
          colorsV_(index, 3) = 0.0f;
          radiiV_(index) = 0.0f;
          radiiE_(index) = 0.0f;
          colorsE_(index, 3) = 0.0f;
        }
        else if (!nt->isVisible()) {
          colorsV_(index, 3) = 0.0f;
          colorsE_(index, 3) = 0.0f;
        }

      }

    }
  }
}

void SEAdenitaVisualModel::prepareScale5to6(double iv, bool forSelection /*= false*/)
{
  SEConfig& config = SEConfig::GetInstance();
  ADNLogger& logger = ADNLogger::GetLogger();

  auto parts = nanorobot_->GetParts();

  float minERadius = config.nucleotide_E_radius;
  float maxERadius = config.nucleotide_V_radius;
  float intervalERadius = maxERadius - minERadius;
  float iERadius = minERadius + iv * intervalERadius;
  
  auto colors = colors_[ColorType::REGULAR];

  SB_FOR(auto part, parts) {

    auto singleStrands = nanorobot_->GetSingleStrands(part);

    SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {

      auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);

      SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {

        auto index = ntMap_[nt()];

        nodeIndices_(index) = nt->getNodeIndex();
        flags_(index) = nt->getInheritedFlags();

        float minVColorR;
        float minVColorG;
        float minVColorB;
        float minVColorA;

        int stapleColorNum = ss->getNodeIndex() % config.num_staple_colors;

        float maxVColorR = config.staple_colors[stapleColorNum * 4 + 0];
        float maxVColorG = config.staple_colors[stapleColorNum * 4 + 1];
        float maxVColorB = config.staple_colors[stapleColorNum * 4 + 2];
        float maxVColorA = config.staple_colors[stapleColorNum * 4 + 3];

        positions_(index, 0) = nanorobot_->GetNucleotideSidechainPosition(nt)[0].getValue();
        positions_(index, 1) = nanorobot_->GetNucleotideSidechainPosition(nt)[1].getValue();
        positions_(index, 2) = nanorobot_->GetNucleotideSidechainPosition(nt)[2].getValue();

        radiiV_(index) = config.nucleotide_V_radius;

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

          radiiE_(index) = config.nucleotide_V_radius;

        }
        else
        {
          //auto baseColor = getBaseColor(nt->getResidueType());
          auto baseColor = colors->GetColor(nt);

          minVColorR = baseColor(0);
          minVColorG = baseColor(1);
          minVColorB = baseColor(2);
          minVColorA = baseColor(3);

          radiiE_(index) = iERadius;

          float minEColorR = config.nucleotide_E_Color[0];
          float minEColorG = config.nucleotide_E_Color[1];
          float minEColorB = config.nucleotide_E_Color[2];
          float minEColorA = config.nucleotide_E_Color[3];

          colorsE_(index, 0) = minEColorR + iv * (maxVColorR - minEColorR);
          colorsE_(index, 1) = minEColorG + iv * (maxVColorG - minEColorG);
          colorsE_(index, 2) = minEColorB + iv * (maxVColorB - minEColorB);
          colorsE_(index, 3) = minEColorA + iv * (maxVColorA - minEColorA);
        }
        
        colorsV_(index, 0) = minVColorR + iv * (maxVColorR - minVColorR);
        colorsV_(index, 1) = minVColorG + iv * (maxVColorG - minVColorG);
        colorsV_(index, 2) = minVColorB + iv * (maxVColorB - minVColorB);
        colorsV_(index, 3) = minVColorA + iv * (maxVColorA - minVColorA);


        //strand direction
        if (nanorobot_->GetNucleotideEnd(nt) == End::ThreePrime) {
          radiiE_(index) = config.nucleotide_E_radius;
        }

        if (!ss->isVisible()) {
          colorsV_(index, 3) = 0.0f;
          radiiV_(index) = 0.0f;
          radiiE_(index) = 0.0f;
          colorsE_(index, 3) = 0.0f;
        }
        else if (!nt->isVisible()) {
          colorsV_(index, 3) = 0.0f;
          colorsE_(index, 3) = 0.0f;
        }
      }
    }
  }
}

void SEAdenitaVisualModel::prepareScale6to7(double iv, bool forSelection)
{
  SEConfig& config = SEConfig::GetInstance();
  ADNLogger& logger = ADNLogger::GetLogger();

  auto parts = nanorobot_->GetParts();

  SB_FOR(auto part, parts) {

    auto singleStrands = nanorobot_->GetSingleStrands(part);

    SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {

      auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);

      SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {

        auto index = ntMap_[nt()];
        capData_(index) = 0;
        flags_(index) = nt->getInheritedFlags();
        nodeIndices_(index) = nt->getNodeIndex();

        SBPosition3 min = nanorobot_->GetNucleotideSidechainPosition(nt);
        SBPosition3 max = nanorobot_->GetNucleotideBackbonePosition(nt);

        SBPosition3 iPos = min + iv * (max - min);

        positions_(index, 0) = iPos[0].getValue();
        positions_(index, 1) = iPos[1].getValue();
        positions_(index, 2) = iPos[2].getValue();

        float minVColorR;
        float minVColorG;
        float minVColorB;
        float minVColorA;

        int stapleColorNum = ss->getNodeIndex() % config.num_staple_colors;

        float maxVColorR = config.staple_colors[stapleColorNum * 4 + 0];
        float maxVColorG = config.staple_colors[stapleColorNum * 4 + 1];
        float maxVColorB = config.staple_colors[stapleColorNum * 4 + 2];
        float maxVColorA = config.staple_colors[stapleColorNum * 4 + 3];

        if (nanorobot_->IsScaffold(ss))
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

        radiiV_(index) = config.nucleotide_V_radius;
        radiiE_(index) = config.nucleotide_V_radius;


        //strand direction
        if (nanorobot_->GetNucleotideEnd(nt) == End::ThreePrime) {
          radiiE_(index) = config.nucleotide_E_radius;
        }

        if (!ss->isVisible()) {
          colorsV_(index, 3) = 0.0f;
          radiiV_(index) = 0.0f;
          radiiE_(index) = 0.0f;
          colorsE_(index, 3) = 0.0f;
        } else if (!nt->isVisible()) {
          colorsV_(index, 3) = 0.0f;
          colorsE_(index, 3) = 0.0f;
        }
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

void SEAdenitaVisualModel::highlightFlagChanged()
{
  auto parts = nanorobot_->GetParts();

  if (scale_ < (float)NUCLEOTIDES_SIDECHAIN) {
  }
  else if (scale_ >= (float)NUCLEOTIDES_SIDECHAIN && scale_ < (float)DOUBLE_STRANDS) {
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

    unsigned int index = 0;

    SB_FOR(auto part, parts) {
      auto doubleStrands = part->GetDoubleStrands();

      SB_FOR(auto doubleStrand, doubleStrands) {
        auto baseSegments = doubleStrand->GetBaseSegments();

        SB_FOR(auto baseSegment, baseSegments) {
          
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

void SEAdenitaVisualModel::changePropertyColors(int index)
{
  if (index == MELTTEMP) {

    auto meltingTempColors = colors_.at(MELTTEMP);

    SEConfig& config = SEConfig::GetInstance();

    auto p = PIPrimer3::GetInstance();

    auto parts = nanorobot_->GetParts();
    ADNLogger& logger = ADNLogger::GetLogger();

    ADNArray<float> color = ADNArray<float>(4);
    color(0) = 1.0f;
    color(1) = 0.0f;
    color(2) = 0.0f;
    color(3) = 1.0f;

    SB_FOR(auto part, parts) {
      auto regions = p.GetBindingRegions(part);

      SB_FOR(auto region, regions) {
        auto gibbs = region->getGibbs();
        auto groupNodes = region->getGroupNodes();

        for (unsigned i = 0; i < groupNodes->size(); i++) {
          auto node = groupNodes->getReferenceTarget(i);
          ADNPointer<ADNNucleotide> nt = static_cast<ADNNucleotide*>(node);
          meltingTempColors->SetColor(color, nt);
        }
      }
    }
  }
}

void SEAdenitaVisualModel::display() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop. This is the main function of your visual model. 
	// Implement this function to display things in SAMSON, for example thanks to the utility functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)

  if (nanorobot_ == nullptr) return;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);

  int iScale = (int)scale_;

  SEConfig& config = SEConfig::GetInstance();
  ADNLogger& logger = ADNLogger::GetLogger();

  if (iScale == ALL_ATOMS_LINES) {

  }
  else if (iScale == ALL_ATOMS_STICKS) {

  }
  else if (iScale == ALL_ATOMS_BALLS) {

  }
  else if (iScale == NUCLEOTIDES_BACKBONE) {
    displayNucleotideBackbone();
  }
  else if (iScale == NUCLEOTIDES_SIDECHAIN) {
    displayNucleotideSideChain();
  }
  else if (iScale == NUCLEOTIDES_SCAFFOLD) {
    displayNucleotideScaffoldPlaiting();
  }
  else if (iScale == STAPLES_SCAFFOLD_PLAITING_SIDECHAIN) {
    displayPlatingSideChain();
  }
  else if (iScale == STAPLES_SCAFFOLD_PLAITING_BACKBONE) {
    displayPlatingBackbone();
  }
  else if (iScale == DOUBLE_STRANDS) {
    displayDoubleStrands();
  }
  else if (iScale == OBJECTS) {
    
  }
  else {

  }

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

  displayForDebugging();

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);


  //if (configuration_->display_base_pairing) {
  //  displayBasePairConnections(scale_);
  //}
}

void SEAdenitaVisualModel::displayAtomsLines()
{

}

void SEAdenitaVisualModel::displayAtomsSticks()
{

}

void SEAdenitaVisualModel::displayAtomsBalls()
{

}

void SEAdenitaVisualModel::displayNucleotideBackbone()
{
  SEConfig& config = SEConfig::GetInstance();
  ADNLogger& logger = ADNLogger::GetLogger();

  auto parts = nanorobot_->GetParts();

  auto colors = colors_[ColorType::REGULAR];

  auto conformations = nanorobot_->GetConformations();
  auto conformation = conformations[dim_ - 1];

  SB_FOR(auto part, parts) {
    auto singleStrands = nanorobot_->GetSingleStrands(part);
    SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
      auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);
      SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {
        unsigned int index = ntMap_[nt()];

        if (dim_ == 3) {
          positions_(index, 0) = nanorobot_->GetNucleotideBackbonePosition(nt)[0].getValue();
          positions_(index, 1) = nanorobot_->GetNucleotideBackbonePosition(nt)[1].getValue();
          positions_(index, 2) = nanorobot_->GetNucleotideBackbonePosition(nt)[2].getValue();
        }
        else if (dim_ == 2 || dim_ == 1) {
          SBPosition3 pos;
          //conformation->getPosition(index, pos);
          conformation->getPosition(nt()->GetBackboneCenterAtom()(), pos);

          positions_(index, 0) = pos[0].getValue();
          positions_(index, 1) = pos[1].getValue();
          positions_(index, 2) = pos[2].getValue();
        }

        capData_(index) = 0;
        colorsE_.SetRow(index, nucleotideEColor_);
        nodeIndices_(index) = nt->getNodeIndex();
        flags_(index) = nt->getInheritedFlags();

        auto baseColor = colors->GetColor(nt);
        colorsV_.SetRow(index, baseColor);
        
        radiiV_(index) = config.nucleotide_V_radius;
        radiiE_(index) = config.nucleotide_E_radius;

        //strand direction
        if (nanorobot_->GetNucleotideEnd(nt) == End::ThreePrime) {
          radiiE_(index) = config.nucleotide_E_radius;
        }

        if (!ss->isVisible()) {
          colorsV_(index, 3) = 0.0f;
          radiiV_(index) = 0.0f;
          radiiE_(index) = 0.0f;
          colorsE_(index, 3) = 0.0f;
        }
        else if (!nt->isVisible()) {
          colorsV_(index, 3) = 0.0f;
          colorsE_(index, 3) = 0.0f;
        }


      }

    }
  }
}

void SEAdenitaVisualModel::displayNucleotideSideChain()
{
  SEConfig& config = SEConfig::GetInstance();
  ADNLogger& logger = ADNLogger::GetLogger();

  auto parts = nanorobot_->GetParts();

  auto colors = colors_[ColorType::REGULAR];

  auto conformations = nanorobot_->GetConformations();
  auto conformation = conformations[dim_ - 1];

  SB_FOR(auto part, parts) {
    auto singleStrands = nanorobot_->GetSingleStrands(part);
    SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
      auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);
      SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {
        unsigned int index = ntMap_[nt()];

        if (dim_ == 3) {
          positions_(index, 0) = nanorobot_->GetNucleotideSidechainPosition(nt)[0].getValue();
          positions_(index, 1) = nanorobot_->GetNucleotideSidechainPosition(nt)[1].getValue();
          positions_(index, 2) = nanorobot_->GetNucleotideSidechainPosition(nt)[2].getValue();
        }
        else if (dim_ == 2 || dim_ == 1) {
          SBPosition3 pos;
          //conformation->getPosition(index, pos);
          conformation->getPosition(nt()->GetSidechainCenterAtom()(), pos);

          positions_(index, 0) = pos[0].getValue();
          positions_(index, 1) = pos[1].getValue();
          positions_(index, 2) = pos[2].getValue();
        }

        capData_(index) = 0;
        colorsE_.SetRow(index, nucleotideEColor_);
        nodeIndices_(index) = nt->getNodeIndex();
        flags_(index) = nt->getInheritedFlags();

        auto baseColor = colors->GetColor(nt);
        colorsV_.SetRow(index, baseColor);

        radiiV_(index) = config.nucleotide_V_radius;
        radiiE_(index) = config.nucleotide_E_radius;

        //strand direction
        if (nanorobot_->GetNucleotideEnd(nt) == End::ThreePrime) {
          radiiE_(index) = config.nucleotide_E_radius;
        }

        if (!ss->isVisible()) {
          colorsV_(index, 3) = 0.0f;
          radiiV_(index) = 0.0f;
          radiiE_(index) = 0.0f;
          colorsE_(index, 3) = 0.0f;
        }
        else if (!nt->isVisible()) {
          colorsV_(index, 3) = 0.0f;
          colorsE_(index, 3) = 0.0f;
        }
      }
    }
  }
}


void SEAdenitaVisualModel::displayNucleotideScaffoldPlaiting()
{
  SEConfig& config = SEConfig::GetInstance();
  ADNLogger& logger = ADNLogger::GetLogger();

  auto parts = nanorobot_->GetParts();

  auto colors = colors_[ColorType::REGULAR];

  auto conformations = nanorobot_->GetConformations();
  auto conformation = conformations[dim_ - 1];

  SB_FOR(auto part, parts) {
    auto singleStrands = nanorobot_->GetSingleStrands(part);
    SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
      auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);
      SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {
        unsigned int index = ntMap_[nt()];

        if (dim_ == 3) {
          positions_(index, 0) = nanorobot_->GetNucleotideSidechainPosition(nt)[0].getValue();
          positions_(index, 1) = nanorobot_->GetNucleotideSidechainPosition(nt)[1].getValue();
          positions_(index, 2) = nanorobot_->GetNucleotideSidechainPosition(nt)[2].getValue();
        }
        else if (dim_ == 2 || dim_ == 1) {
          SBPosition3 pos;
          //conformation->getPosition(index, pos);
          conformation->getPosition(nt()->GetSidechainCenterAtom()(), pos);

          positions_(index, 0) = pos[0].getValue();
          positions_(index, 1) = pos[1].getValue();
          positions_(index, 2) = pos[2].getValue();
        }

        capData_(index) = 0;
        colorsE_.SetRow(index, nucleotideEColor_);
        nodeIndices_(index) = nt->getNodeIndex();
        flags_(index) = nt->getInheritedFlags();
        radiiV_(index) = config.nucleotide_V_radius;

        if (ss->IsScaffold()) {
          colorsV_(index, 0) = config.nucleotide_E_Color[0];
          colorsV_(index, 1) = config.nucleotide_E_Color[1];
          colorsV_(index, 2) = config.nucleotide_E_Color[2];
          colorsV_(index, 3) = config.nucleotide_E_Color[3];

          radiiE_(index) = config.nucleotide_V_radius;
        }
        else {
          auto baseColor = colors->GetColor(nt);
          colorsV_.SetRow(index, baseColor);
          radiiE_(index) = config.nucleotide_E_radius;
        }
        //strand direction
        if (nanorobot_->GetNucleotideEnd(nt) == End::ThreePrime) {
          radiiE_(index) = config.nucleotide_E_radius;
        }

        if (!ss->isVisible()) {
          colorsV_(index, 3) = 0.0f;
          radiiV_(index) = 0.0f;
          radiiE_(index) = 0.0f;
          colorsE_(index, 3) = 0.0f;
        }
        else if (!nt->isVisible()) {
          colorsV_(index, 3) = 0.0f;
          colorsE_(index, 3) = 0.0f;
        }

      }
    }
  }
}

void SEAdenitaVisualModel::displayPlatingSideChain()
{
  SEConfig& config = SEConfig::GetInstance();
  auto parts = nanorobot_->GetParts();

  auto conformations = nanorobot_->GetConformations();
  auto conformation = conformations[dim_ - 1];

  auto colors = colors_.at(REGULAR);

  SB_FOR(auto part, parts) {
    auto singleStrands = nanorobot_->GetSingleStrands(part);
    SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
      auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);
      SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {

        unsigned int index = ntMap_[nt()];
        capData_(index) = 0;
        flags_(index) = nt->getInheritedFlags();
        nodeIndices_(index) = nt->getNodeIndex();
        
        if (dim_ == 3) {
          positions_(index, 0) = nanorobot_->GetNucleotideSidechainPosition(nt)[0].getValue();
          positions_(index, 1) = nanorobot_->GetNucleotideSidechainPosition(nt)[1].getValue();
          positions_(index, 2) = nanorobot_->GetNucleotideSidechainPosition(nt)[2].getValue();
        }
        else if (dim_ == 2 || dim_ == 1) {
          SBPosition3 pos;
          //conformation->getPosition(index, pos);
          conformation->getPosition(nt()->GetSidechainCenterAtom()(), pos);
          
          positions_(index, 0) = pos[0].getValue();
          positions_(index, 1) = pos[1].getValue();
          positions_(index, 2) = pos[2].getValue();
        }

        
       
        if (nanorobot_->IsScaffold(ss))
        {
          colorsV_(index, 0) = config.nucleotide_E_Color[0];
          colorsV_(index, 1) = config.nucleotide_E_Color[1];
          colorsV_(index, 2) = config.nucleotide_E_Color[2];
          colorsV_(index, 3) = config.nucleotide_E_Color[3];
        }
        else
        {
          auto color = colors->GetColor(ss);
          colorsV_.SetRow(index, color);

        }

        colorsE_(index, 0) = colorsV_(index, 0);
        colorsE_(index, 1) = colorsV_(index, 1);
        colorsE_(index, 2) = colorsV_(index, 2);
        colorsE_(index, 3) = colorsV_(index, 3);

        radiiV_(index) = config.nucleotide_V_radius;
        radiiE_(index) = config.nucleotide_V_radius;

        //strand direction
        if (nanorobot_->GetNucleotideEnd(nt) == End::ThreePrime) {
          radiiE_(index) = config.nucleotide_E_radius;
        }

        if (!ss->isVisible()) {
          colorsV_(index, 3) = 0.0f;
          radiiV_(index) = 0.0f;
          radiiE_(index) = 0.0f;
          colorsE_(index, 3) = 0.0f;
        }
        else if (!nt->isVisible()) {
          colorsV_(index, 3) = 0.0f;
          colorsE_(index, 3) = 0.0f;
        }

      }
    }
  }
}

void SEAdenitaVisualModel::displayPlatingBackbone()
{
  SEConfig& config = SEConfig::GetInstance();
  auto parts = nanorobot_->GetParts();

  auto conformations = nanorobot_->GetConformations();
  auto conformation = conformations[dim_ - 1];

  auto colors = colors_.at(MELTTEMP);

  SB_FOR(auto part, parts) {
    auto singleStrands = nanorobot_->GetSingleStrands(part);
    SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
      auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);
      SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {
        unsigned int index = ntMap_[nt()];

        capData_(index) = 0;
        flags_(index) = nt->getInheritedFlags();
        nodeIndices_(index) = nt->getNodeIndex();

        if (dim_ == 3) {
          positions_(index, 0) = nanorobot_->GetNucleotideBackbonePosition(nt)[0].getValue();
          positions_(index, 1) = nanorobot_->GetNucleotideBackbonePosition(nt)[1].getValue();
          positions_(index, 2) = nanorobot_->GetNucleotideBackbonePosition(nt)[2].getValue();
        }
        else if (dim_ == 2 || dim_ == 1) {
          SBPosition3 pos;
          conformation->getPosition(nt()->GetBackboneCenterAtom()(), pos);

          positions_(index, 0) = pos[0].getValue();
          positions_(index, 1) = pos[1].getValue();
          positions_(index, 2) = pos[2].getValue();
        }

        if (nanorobot_->IsScaffold(ss))
        {
          colorsV_(index, 0) = config.nucleotide_E_Color[0];
          colorsV_(index, 1) = config.nucleotide_E_Color[1];
          colorsV_(index, 2) = config.nucleotide_E_Color[2];
          colorsV_(index, 3) = config.nucleotide_E_Color[3];
        }
        else
        {
          auto color = colors->GetColor(ss);
          colorsV_.SetRow(index, color);
        }

        colorsE_(index, 0) = colorsV_(index, 0);
        colorsE_(index, 1) = colorsV_(index, 1);
        colorsE_(index, 2) = colorsV_(index, 2);
        colorsE_(index, 3) = colorsV_(index, 3);

        radiiV_(index) = config.nucleotide_V_radius;
        radiiE_(index) = config.nucleotide_V_radius;

        //strand direction
        if (nanorobot_->GetNucleotideEnd(nt) == End::ThreePrime) {
          radiiE_(index) = config.nucleotide_E_radius;
        }

        if (!ss->isVisible()) {
          colorsV_(index, 3) = 0.0f;
          radiiV_(index) = 0.0f;
          radiiE_(index) = 0.0f;
          colorsE_(index, 3) = 0.0f;
        }
        else if (!nt->isVisible()) {
          colorsV_(index, 3) = 0.0f;
          colorsE_(index, 3) = 0.0f;
        }

      }
    }
  }

}

void SEAdenitaVisualModel::displayDoubleStrands()
{
  SEConfig& config = SEConfig::GetInstance();
  ADNLogger& logger = ADNLogger::GetLogger();

  auto parts = nanorobot_->GetParts();
  auto colors = colors_.at(REGULAR);

  positions_ = ADNArray<float>(3, nPositions_);
  radiiV_ = ADNArray<float>(nPositions_);
  flags_ = ADNArray<unsigned int>(nPositions_);
  colorsV_ = ADNArray<float>(4, nPositions_);
  nodeIndices_ = ADNArray<unsigned int>(nPositions_);
  capData_ = ADNArray<unsigned int>(nPositions_);

  unsigned int index = 0; //fix this with map

  SB_FOR(auto part, parts) {
    auto doubleStrands = part->GetDoubleStrands();

    SB_FOR(auto doubleStrand, doubleStrands) {
      auto baseSegments = doubleStrand->GetBaseSegments();

      SB_FOR(auto baseSegment, baseSegments) {
        auto cell = baseSegment->GetCell();

        if (cell->GetType() == BasePair) {
          SBPosition3 pos = baseSegment->GetPosition();
          positions_(index, 0) = (float)pos.v[0].getValue();
          positions_(index, 1) = (float)pos.v[1].getValue();
          positions_(index, 2) = (float)pos.v[2].getValue();
        }

        auto color = colors->GetColor(baseSegment);
        colorsV_.SetRow(index, color);

        radiiV_(index) = config.base_pair_radius;

        capData_(index) = 1;

        flags_(index) = baseSegment->getInheritedFlags();

        if (!doubleStrand->isVisible()) {
          colorsV_(index, 3) = 0.0f;
          radiiV_(index) = 0.0f;
        }
        else if (!baseSegment->isVisible()) {
          colorsV_(index, 3) = 0.0f;
        }

        ++index;

      }
    }
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

  if (nanorobot_ == nullptr) return;

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

void SEAdenitaVisualModel::displayBaseBairConnections(bool onlySelected)
{
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
    
    positions(index, 0) = nanorobot_->GetNucleotideBackbonePosition(nt)[0].getValue();
    positions(index, 1) = nanorobot_->GetNucleotideBackbonePosition(nt)[1].getValue();
    positions(index, 2) = nanorobot_->GetNucleotideBackbonePosition(nt)[2].getValue();

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

    if (!nt->isSelected() && !pair->isSelected()) {
      radii(index) = 0;
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
  auto parts = nanorobot_->GetParts();

  if (config.display_nucleotide_basis) {

    SB_FOR(auto part, parts) {
      auto singleStrands = nanorobot_->GetSingleStrands(part);
      SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
        auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);
        SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {
          if (nt->isSelected()) {
            ADNDisplayHelper::displayBaseVectors(nt);
          }
        }
      }
    }
  }
  if (config.display_base_pairing) {
    displayBaseBairConnections(true);
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

  if (baseEvent->getType() == SBBaseEvent::SelectionFlagChanged || baseEvent->getType() == SBBaseEvent::HighlightingFlagChanged){
    highlightFlagChanged();
  }

  if (baseEvent->getType() == SBBaseEvent::VisibilityFlagChanged) {
    changeScale(scale_, false);
  }



}

void SEAdenitaVisualModel::onDocumentEvent(SBDocumentEvent* documentEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle document events 

}

void SEAdenitaVisualModel::onStructuralEvent(SBStructuralEvent* structuralEvent) {
	
	//// SAMSON Element generator pro tip: implement this function if you need to handle structural events (e.g. when a structural node for which you provide a visual representation is updated)
  //ADNLogger& logger = ADNLogger::GetLogger();
  // changeScale(scale_);
  //if (structuralEvent->getType() == SBStructuralEvent::MobilityFlagChanged) {
  //  SAMSON::requestViewportUpdate();
  //}

}

