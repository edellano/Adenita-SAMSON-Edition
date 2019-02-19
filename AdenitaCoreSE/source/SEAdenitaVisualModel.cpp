#include "SEAdenitaVisualModel.hpp"
#include "SAMSON.hpp"
#include "ADNLogger.hpp"


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

void SEAdenitaVisualModel::init()
{
  SEAdenitaCoreSEApp* app = getAdenitaApp();
  nanorobot_ = app->GetNanorobot();

  ADNPointer<ADNPart> part = new ADNPart();

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

  setupPropertyColors();

  //orderVisibility();
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
  nCylinders_ = 0;
  positions_ = ADNArray<float>(3, nPositions);
  radiiV_ = ADNArray<float>(nPositions);
  radiiE_ = ADNArray<float>(nPositions);
  colorsV_ = ADNArray<float>(4, nPositions);
  colorsE_ = ADNArray<float>(4, nPositions);
  capData_ = ADNArray<unsigned int>(nPositions);
  flags_ = ADNArray<unsigned int>(nPositions);
  nodeIndices_ = ADNArray<unsigned int>(nPositions);


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

  auto curColors = colors_.at(REGULAR);

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
        auto baseColor = curColors->GetColor(nt);
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

  auto curColors = colors_[ColorType::REGULAR];

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
        auto baseColor = curColors->GetColor(nt);

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
  
  auto curColors = colors_[ColorType::REGULAR];

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
          auto baseColor = curColors->GetColor(nt);

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

void SEAdenitaVisualModel::setupSingleStrandColors(int index)
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

void SEAdenitaVisualModel::setupNucleotideColors(int index)
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

void SEAdenitaVisualModel::setupDoubleStrandColors(int index)
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

  displayCircularDNAConnection();

  highlightNucleotides();

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

  MSVColors * curColors = colors_[curColorType_];

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

        auto baseColor = curColors->GetColor(nt);
        colorsV_.SetRow(index, baseColor);
        
        radiiV_(index) = config.nucleotide_V_radius;
        radiiE_(index) = config.nucleotide_E_radius;

        displayLoops(nt(), index);

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

  MSVColors * curColors = colors_[curColorType_];

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

        auto baseColor = curColors->GetColor(nt);
        colorsV_.SetRow(index, baseColor);

        radiiV_(index) = config.nucleotide_V_radius;
        radiiE_(index) = config.nucleotide_E_radius;

        displayLoops(nt(), index);

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

  MSVColors * curColors = colors_[curColorType_];

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
        nodeIndices_(index) = nt->getNodeIndex();
        flags_(index) = nt->getInheritedFlags();
        radiiV_(index) = config.nucleotide_V_radius;

        if (curColorType_ == REGULAR) {

          if (ss->IsScaffold()) {
            auto color = curColors->GetColor(nt->GetStrand());
            radiiE_(index) = config.nucleotide_V_radius;
            colorsV_.SetRow(index, color);
            colorsE_.SetRow(index, color);
          }
          else {
            radiiE_(index) = config.nucleotide_E_radius;
            auto color = curColors->GetColor(nt);
            colorsV_.SetRow(index, color);
            colorsE_.SetRow(index, nucleotideEColor_);
            displayLoops(nt(), index);

          }
        }
        else if (curColorType_ == MELTTEMP || curColorType_ == GIBBS) {
          auto color = curColors->GetColor(nt);
          colorsV_.SetRow(index, color);
          if (ss->IsScaffold()) {
            colorsE_.SetRow(index, color);
            radiiE_(index) = config.nucleotide_V_radius;
          }
          else {
            colorsE_.SetRow(index, nucleotideEColor_);
            radiiE_(index) = config.nucleotide_E_radius;
          }
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

  MSVColors * curColors = colors_[curColorType_];

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

        
        if (curColorType_ == REGULAR) {
          auto color = curColors->GetColor(ss);
          colorsV_.SetRow(index, color);
        }
        else if (curColorType_ == MELTTEMP || curColorType_ == GIBBS) {
          auto color = curColors->GetColor(nt);
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

  MSVColors * curColors = colors_[curColorType_];

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

        if (curColorType_ == REGULAR) {
          auto color = curColors->GetColor(ss);
          colorsV_.SetRow(index, color);
        }
        else if (curColorType_ == MELTTEMP || curColorType_ == GIBBS) {
          auto color = curColors->GetColor(nt);
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

  MSVColors * curColors = colors_[curColorType_];
  positions_ = ADNArray<float>(3, nPositions_);
  radiiV_ = ADNArray<float>(nPositions_);
  flags_ = ADNArray<unsigned int>(nPositions_);
  colorsV_ = ADNArray<float>(4, nPositions_);
  nodeIndices_ = ADNArray<unsigned int>(nPositions_);

  unsigned int index = 0; //fix this with map

  SB_FOR(auto part, parts) {
    auto doubleStrands = part->GetDoubleStrands();
    //for now also the base segments that have loops and skips are displayed as sphere
    SB_FOR(auto doubleStrand, doubleStrands) {
      auto baseSegments = doubleStrand->GetBaseSegments();

      SB_FOR(auto baseSegment, baseSegments) {
        auto cell = baseSegment->GetCell();

        SBPosition3 pos = baseSegment->GetPosition();
        positions_(index, 0) = (float)pos.v[0].getValue();
        positions_(index, 1) = (float)pos.v[1].getValue();
        positions_(index, 2) = (float)pos.v[2].getValue();

        auto color = curColors->GetColor(baseSegment);
        colorsV_.SetRow(index, color);
        radiiV_(index) = config.base_pair_radius;

        auto type = baseSegment->GetCellType();
        if (type == CellType::SkipPair) {
          colorsV_(index, 1) = 0.0f;
          colorsV_(index, 2) = 0.0f;
          colorsV_(index, 3) = 0.3f;
        }

        if (type == CellType::LoopPair) {
          radiiV_(index) = config.base_pair_radius * 1.2f;
          colorsV_(index, 0) = 0.0f;
          colorsV_(index, 3) = 0.3f;
        }


        flags_(index) = baseSegment->getInheritedFlags();

        nodeIndices_(index) = baseSegment->getNodeIndex();

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
    
    SBPosition3 pos;
    if (scale_ == 3 || scale_ == 7) {
      pos = nt->GetBackbonePosition();
    }
    else if (scale_ == 4 || scale_ == 5 || scale_ == 6) {
      pos = nt->GetSidechainPosition();
    }
    else if (scale_ < 3 || scale_ > 7) {
      pos = nt->GetPosition();
    }

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

  if (config.debugOptions.display_nucleotide_basis) {

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
    displayBaseBairConnections(true);
  }

}

void SEAdenitaVisualModel::displayCircularDNAConnection()
{

  if (scale_ < (float)NUCLEOTIDES_BACKBONE && scale_ > (float)STAPLES_SCAFFOLD_PLAITING_BACKBONE) return;

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

void SEAdenitaVisualModel::highlightNucleotides()
{
  if (scale_ < (float)NUCLEOTIDES_BACKBONE) return;

  if (highlightType_ == GC) {

    float * colorFocus = new float[4];
    colorFocus[0] = 1.0f;
    colorFocus[1] = 0.2f;
    colorFocus[2] = 0.2f;
    colorFocus[3] = 1.0f;

    float * colorContext = new float[4];
    colorContext[0] = 0.5f;
    colorContext[1] = 0.5f;
    colorContext[2] = 0.5f;
    colorContext[3] = 1.0f;

    auto parts = nanorobot_->GetParts();

    vector<unsigned int> indicesFocus;
    vector<unsigned int> indicesContext;

    if (scale_ > (float)NUCLEOTIDES_BACKBONE && scale_ <= (float)STAPLES_SCAFFOLD_PLAITING_BACKBONE) {
      SB_FOR(auto part, parts) {
        auto singleStrands = nanorobot_->GetSingleStrands(part);
        SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
          auto nucleotides = nanorobot_->GetSingleStrandNucleotides(ss);
          SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {
            auto index = ntMap_[nt()];
            if (nt->getNucleotideType() == DNABlocks::DC || nt->getNucleotideType() == DNABlocks::DG) {
              indicesFocus.push_back(index);
            }
            else {
              indicesContext.push_back(index);
            }
          }
        }
      }
    }
    else if (scale_ == (float)DOUBLE_STRANDS){
      unsigned int index = 0;
      SB_FOR(auto part, parts) {
        auto doubleStrands = part->GetDoubleStrands();
        SB_FOR(auto doubleStrand, doubleStrands) {
          auto baseSegments = doubleStrand->GetBaseSegments();
          SB_FOR(auto baseSegment, baseSegments) {
            auto nts = baseSegment->GetNucleotides();
            SB_FOR(auto nt, nts) {
              if (nt->getNucleotideType() == DNABlocks::DC || nt->getNucleotideType() == DNABlocks::DG) {
                indicesFocus.push_back(index);
              }
              else {
                indicesContext.push_back(index);
              }
            }

            ++index;
          }
        }
      }
    }
    ADNDisplayHelper::colorCylinders(colorsV_, indicesFocus, colorFocus);
    ADNDisplayHelper::colorCylinders(colorsE_, indicesFocus, colorFocus);
    ADNDisplayHelper::colorCylinders(colorsV_, indicesContext, colorContext);
    ADNDisplayHelper::colorCylinders(colorsE_, indicesContext, colorContext);

    delete[] colorFocus;
    delete[] colorContext;
  }
  else if (highlightType_ == HighlightType::CROSSOVERS) {
    auto parts = nanorobot_->GetParts();

    SB_FOR(auto part, parts) {
      //auto xos = PICrossovers::GetCrossovers(part);

    }
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

void SEAdenitaVisualModel::displayLoops(ADNNucleotide *nt, unsigned int index)
{
  auto type = nt->GetBaseSegment()->GetCellType();
  if (type == CellType::LoopPair) {
    radiiV_(index) = radiiV_(index) * 0.7f;;
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

