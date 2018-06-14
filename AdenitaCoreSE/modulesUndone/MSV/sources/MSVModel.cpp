#include "MSVModel.hpp"	

MSVModel::MSVModel() {
	selectionFlag_ = ANTAuxiliary::Unselected;

  // initialize colors
  nucleotideEColor1_ = ANTAuxiliary::ANTArray<float>(4);
  nucleotideEColor1_(0) = 141.0f / 255.0f;
  nucleotideEColor1_(1) = 160.0f / 255.0f;
  nucleotideEColor1_(2) = 203.0f / 255.0f;
  nucleotideEColor1_(3) = 1.0f;

  adenineColor_ = ANTAuxiliary::ANTArray<float>(4);
  adenineColor_(0) = 102.0f / 255.0f;
  adenineColor_(1) = 194.0f / 255.0f;
  adenineColor_(2) = 165.0f / 255.0f;
  adenineColor_(3) = 1.0f;

  thymineColor_ = ANTAuxiliary::ANTArray<float>(4);
  thymineColor_(0) = 252.0f / 255.0f;
  thymineColor_(1) = 141.0f / 255.0f;
  thymineColor_(2) = 98.0f / 255.0f;
  thymineColor_(3) = 1.0f;

  guanineColor_ = ANTAuxiliary::ANTArray<float>(4);
  guanineColor_(0) = 231.0f / 255.0f;
  guanineColor_(1) = 138.0f / 255.0f;
  guanineColor_(2) = 195.0f / 255.0f;
  guanineColor_(3) = 1.0f;

  cytosineColor_ = ANTAuxiliary::ANTArray<float>(4);
  cytosineColor_(0) = 229.0f / 255.0f;
  cytosineColor_(1) = 196.0f / 255.0f;
  cytosineColor_(2) = 148.0f / 255.0f;
  cytosineColor_(3) = 1.0f;

  undefinedNucleotideColor_ = ANTAuxiliary::ANTArray<float>(4);
  undefinedNucleotideColor_(0) = 1.0f;
  undefinedNucleotideColor_(1) = 1.0f;
  undefinedNucleotideColor_(2) = 1.0f;
  undefinedNucleotideColor_(3) = 1.0f;

  basePairingEColor_ = ANTAuxiliary::ANTArray<float>(4);
  basePairingEColor_(0) = 255.0f / 255.0f;
  basePairingEColor_(1) = 255.0f / 255.0f;
  basePairingEColor_(2) = 255.0f / 255.0f;
  basePairingEColor_(3) = 1.0f;

  doubleHelixVColor_ = ANTAuxiliary::ANTArray<float>(4);
  doubleHelixVColor_(0) = 0.0f / 255.0f;
  doubleHelixVColor_(1) = 110.0f / 255.0f;
  doubleHelixVColor_(2) = 220.0f / 255.0f;
  doubleHelixVColor_(3) = 1.0f;

  doubleHelixEColor_ = ANTAuxiliary::ANTArray<float>(4);
  doubleHelixEColor_(0) = 0.0f / 255.0f;
  doubleHelixEColor_(1) = 110.0f / 255.0f;
  doubleHelixEColor_(2) = 220.0f / 255.0f;
  doubleHelixEColor_(3) = 1.0f;

  numElements_ = 6;
  atomColors_ = ANTAuxiliary::ANTArray<float>(4, numElements_);
  //O
  atomColors_(0, 0) = 0.9f;
  atomColors_(0, 1) = 0.1f;
  atomColors_(0, 2) = 0.0f;
  atomColors_(0, 3) = 1.0f;

  //C
  atomColors_(1, 0) = 0.15f;
  atomColors_(1, 1) = 0.15f;
  atomColors_(1, 2) = 0.15f;
  atomColors_(1, 3) = 1.0f;

  //N
  atomColors_(2, 0) = 0.1f;
  atomColors_(2, 1) = 0.1f;
  atomColors_(2, 2) = 0.9f;
  atomColors_(2, 3) = 1.0f;

  //P
  atomColors_(3, 0) = 0.9f;
  atomColors_(3, 1) = 0.5f;
  atomColors_(3, 2) = 0.0f;
  atomColors_(3, 3) = 1.0f;

  //Fm
  atomColors_(4, 0) = 0.5f;
  atomColors_(4, 1) = 0.9f;
  atomColors_(4, 2) = 0.0f;
  atomColors_(4, 3) = 1.0f;

  //Es
  atomColors_(5, 0) = 0.9f;
  atomColors_(5, 1) = 0.0f;
  atomColors_(5, 2) = 0.1f;
  atomColors_(5, 3) = 1.0f;

  //DASStaple colors
  /*numStapleColors_ = 8;
  stapleColors_ = ANTAuxiliary::ANTArray<float>(4, numStapleColors_);
  stapleColors_(0, 0) = 228 / 255.0f;
  stapleColors_(0, 1) = 26 / 255.0f;
  stapleColors_(0, 2) = 28 / 255.0f;
  stapleColors_(0, 3) = 1.0f;

  stapleColors_(1, 0) = 55 / 255.0f;
  stapleColors_(1, 1) = 126 / 255.0f;
  stapleColors_(1, 2) = 184 / 255.0f;
  stapleColors_(1, 3) = 1.0f;

  stapleColors_(2, 0) = 77 / 255.0f;
  stapleColors_(2, 1) = 175 / 255.0f;
  stapleColors_(2, 2) = 74 / 255.0f;
  stapleColors_(2, 3) = 1.0f;

  stapleColors_(3, 0) = 152 / 255.0f;
  stapleColors_(3, 1) = 78 / 255.0f;
  stapleColors_(3, 2) = 163 / 255.0f;
  stapleColors_(3, 3) = 1.0f;

  stapleColors_(4, 0) = 255 / 255.0f;
  stapleColors_(4, 1) = 127 / 255.0f;
  stapleColors_(4, 2) = 0 / 255.0f;
  stapleColors_(4, 3) = 1.0f;

  stapleColors_(5, 0) = 255 / 255.0f;
  stapleColors_(5, 1) = 255 / 255.0f;
  stapleColors_(5, 2) = 51 / 255.0f;
  stapleColors_(5, 3) = 1.0f;

  stapleColors_(6, 0) = 166 / 255.0f;
  stapleColors_(6, 1) = 86 / 255.0f;
  stapleColors_(6, 2) = 40 / 255.0f;
  stapleColors_(6, 3) = 1.0f;

  stapleColors_(7, 0) = 247 / 255.0f;
  stapleColors_(7, 1) = 129 / 255.0f;
  stapleColors_(7, 2) = 191 / 255.0f;
  stapleColors_(7, 3) = 1.0f;*/

  //different set of colors (set 3)

  numStapleColors_ = 12;
  int stColorb[] = { 166, 206, 227 };
  int stColora[] = { 31, 120, 180 };
  int stColor9[] = { 178, 223, 138 };
  int stColor8[] = { 51, 160, 44 };
  int stColor7[] = { 251, 154, 153 };
  int stColor6[] = { 227, 26, 28 };
  int stColor5[] = { 253, 191, 111 };
  int stColor4[] = { 255, 127, 0 };
  int stColor3[] = { 202, 178, 214 };
  int stColor2[] = { 106, 61, 154 };
  int stColor1[] = { 255, 255, 153 };
  int stColor0[] = { 177, 89, 40 };
  
  vector<int*> stColors;
  stColors.push_back(stColorb);
  stColors.push_back(stColora);
  stColors.push_back(stColor9);
  stColors.push_back(stColor8);
  stColors.push_back(stColor7);
  stColors.push_back(stColor6);
  stColors.push_back(stColor5);
  stColors.push_back(stColor4);
  stColors.push_back(stColor3);
  stColors.push_back(stColor2);
  stColors.push_back(stColor1);
  stColors.push_back(stColor0);

  stapleColors_ = ANTAuxiliary::ANTArray<float>(4, numStapleColors_);
  for (unsigned int i = 0; i < numStapleColors_; i++) {
    stapleColors_(i, 0) = stColors[i][0] / 255.0f;
    stapleColors_(i, 1) = stColors[i][1] / 255.0f;
    stapleColors_(i, 2) = stColors[i][2] / 255.0f;
    stapleColors_(i, 3) = 1.0f;
  }


  numPropertyColors_ = 3;
  propertyColors_ = ANTAuxiliary::ANTArray<float>(4, numPropertyColors_);
  propertyColors_(0, 0) = 252.0f / 255.0f;
  propertyColors_(0, 1) = 141.0f / 255.0f;
  propertyColors_(0, 2) = 89.0f / 255.0f;
  propertyColors_(0, 3) = 1.0f;

  propertyColors_(1, 0) = 255.0f / 255.0f;
  propertyColors_(1, 1) = 255.0f / 255.0f;
  propertyColors_(1, 2) = 191.0f / 255.0f;
  propertyColors_(1, 3) = 1.0f;

  propertyColors_(2, 0) = 145.0f / 255.0f;
  propertyColors_(2, 1) = 207.0f / 255.0f;
  propertyColors_(2, 2) = 96.0f / 255.0f;
  propertyColors_(2, 3) = 1.0f;

  //Geometric Properties of Visual Model
  nucleotideVRadius_ = 160.0f;
  nucleotideBaseRadius_ = nucleotideVRadius_ / 5;
  nucleotideTipRadius_ = nucleotideBaseRadius_ * 0.4f;
  singleStrandRadius_ = 160.0f;
  vertexRadius_ = 200.0f;
  edgeRadius_ = 1000.0f;
  atomLineRadius_ = 15.0f;
  atomBallRadius_ = 35.0f;
  bondBallsRadius_ = 30.0f;
  minMeltingTemp_ = 25.0f;
  maxMeltingTemp_ = 80.0f;

  minGibbsFreeEnergy_ = -10000.0f;
  maxGibbsFreeEnergy_ = 0.0f;

  doubleHelixVRadius_ = 250.0f;
  doubleHelixERadius_ = 250.0f;

  numAdenine_ = 10;
  numGuanine_ = 11;
  numCytosine_ = 8;
  numThymine_ = 9;

  baseSegmentColor_ = ANTAuxiliary::ANTArray<float>(4);
  //baseSegmentColor_(0) = 230 / 255.0f;
  //baseSegmentColor_(1) = 140 / 255.0f;
  //baseSegmentColor_(2) = 80 / 255.0f;
  //baseSegmentColor_(3) = 1.0f;

  baseSegmentColor_(0) = 0.8f;
  baseSegmentColor_(1) = 0.8f;
  baseSegmentColor_(2) = 0.8f;
  baseSegmentColor_(3) = 1.0f;

}

ANTAuxiliary::ANTArray<float> MSVModel::getAtomColor(std::string element) {

  auto color = ANTAuxiliary::ANTArray<float>(1, 4);

  if (element == "O") {
    color(0) = atomColors_(0, 0);
    color(1) = atomColors_(0, 1);
    color(2) = atomColors_(0, 2);
    color(3) = atomColors_(0, 3);
  }
  else if (element == "C") {
    color(0) = atomColors_(1, 0);
    color(1) = atomColors_(1, 1);
    color(2) = atomColors_(1, 2);
    color(3) = atomColors_(1, 3);
  }
  else if (element == "N") {
    color(0) = atomColors_(2, 0);
    color(1) = atomColors_(2, 1);
    color(2) = atomColors_(2, 2);
    color(3) = atomColors_(2, 3);
  }
  else if (element == "P") {
    color(0) = atomColors_(3, 0);
    color(1) = atomColors_(3, 1);
    color(2) = atomColors_(3, 2);
    color(3) = atomColors_(3, 3);
  }
  else if (element == "Fm") {
    color(0) = atomColors_(4, 0);
    color(1) = atomColors_(4, 1);
    color(2) = atomColors_(4, 2);
    color(3) = atomColors_(4, 3);
  }
  else if (element == "Es") {
    color(0) = atomColors_(5, 0);
    color(1) = atomColors_(5, 1);
    color(2) = atomColors_(5, 2);
    color(3) = atomColors_(5, 3);
  }
  else {
    color(0) = 0.0f;
    color(1) = 0.0f;
    color(2) = 0.0f;
    color(3) = 0.0f;
  }

  return color;
}

ANTAuxiliary::ANTArray<float> MSVModel::getAtomColor(SBElement::Type type)
{
  auto color = ANTAuxiliary::ANTArray<float>(1, 4);

  if (type == SBElement::Oxygen) {
    color(0) = atomColors_(0, 0);
    color(1) = atomColors_(0, 1);
    color(2) = atomColors_(0, 2);
    color(3) = atomColors_(0, 3);
  }
  else if (type == SBElement::Carbon) {
    color(0) = atomColors_(1, 0);
    color(1) = atomColors_(1, 1);
    color(2) = atomColors_(1, 2);
    color(3) = atomColors_(1, 3);
  }
  else if (type == SBElement::Nitrogen) {
    color(0) = atomColors_(2, 0);
    color(1) = atomColors_(2, 1);
    color(2) = atomColors_(2, 2);
    color(3) = atomColors_(2, 3);
  }
  else if (type == SBElement::Phosphorus) {
    color(0) = atomColors_(3, 0);
    color(1) = atomColors_(3, 1);
    color(2) = atomColors_(3, 2);
    color(3) = atomColors_(3, 3);
  }
  else {
    color(0) = 0.0f;
    color(1) = 0.0f;
    color(2) = 0.0f;
    color(3) = 0.0f;
  }

  return color;
}

MSVModel::~MSVModel() {
  //We don't need to delete nanorobot from here.
  for (auto & bit : bindingRegions_) {
    delete &bit;
  }
}

ANTAuxiliary::ANTArray<float> MSVModel::getBaseColor(DNABlocks baseSymbol) {
  ANTAuxiliary::ANTArray<float> color;

	if (baseSymbol == DNABlocks::DA_) {
		//adenine
		color = ANTAuxiliary::ANTArray<float>(adenineColor_);
	}
  else if (baseSymbol == DNABlocks::DT_) {
		//thymine
		color = ANTAuxiliary::ANTArray<float>(thymineColor_);
	}
  else if (baseSymbol == DNABlocks::DG_) {
		//guanine
    color = ANTAuxiliary::ANTArray<float>(guanineColor_);
	}
  else if (baseSymbol == DNABlocks::DC_) {
		//cytosine
		color = ANTAuxiliary::ANTArray<float>(cytosineColor_);
	}
	else {
    color = ANTAuxiliary::ANTArray<float>(undefinedNucleotideColor_);
	}

	return color;
}

ANTNucleotide* MSVModel::getNucleotideByIndex(unsigned int index) {
  ANTNucleotide* result = nullptr;

  for (auto sit = singleStrands_.begin(); sit != singleStrands_.end(); ++sit) {
    ANTSingleStrand* singleStrand = *sit;
    ANTNucleotideList nucleotides = singleStrand->nucleotides_;
    if (nucleotides.find(index) != nucleotides.end()) {
      result = nucleotides.at(index);
      return result;
    }
  }
  return result;
}

ANTNucleotide* MSVModel::getNucleotideByIndex(unsigned int index, unsigned int chain_id) {
	ANTNucleotide* result = nullptr;
  ANTSingleStrand* singleStrand = singleStrands_.at(chain_id);
	ANTNucleotideList nucleotides = singleStrand->nucleotides_;
  if (nucleotides.find(index) != nucleotides.end()) {
    result = nucleotides.at(index);
	}
	return result;
}

std::vector<ANTSingleStrand*> MSVModel::getScaffoldStrands() {
	return nanorobot_->GetScaffolds();
}

unsigned int MSVModel::getSelectedNucletides(std::map<ANTSingleStrand*, std::vector<ANTNucleotide*>> &selectedNucleotides) {
  selectedNucleotides = this->nanorobot_->GetSelectedNucleotides();
  unsigned int num = 0;
  for (auto &p : selectedNucleotides) {
    num += boost::numeric_cast<unsigned int>(p.second.size());
  }
  return num;
}

ANTSingleStrand* MSVModel::getSingleStrandByNucleotide(ANTNucleotide* searchedNucleotide) {
  return searchedNucleotide->strand_;
}

MSVBasePair* MSVModel::getBasePairByNucleotide(ANTNucleotide* searchedNucleotide) {
  MSVBasePairList basepairs = this->basePairs_;

  for (unsigned int i = 0; i < basepairs.size(); i++) {
    MSVBasePair* bp = basepairs[i];

    ANTNucleotide* left = bp->leftNucleotide_;
    ANTNucleotide* right = bp->rightNucleotide_;

    if (left == searchedNucleotide || right == searchedNucleotide) {
      return bp;
    }
  }

	return nullptr;
}

MSVBasePair* MSVModel::getBasePairByResidue(SBResidue* searchedResidue) {
	MSVBasePairList basepairs = this->basePairs_;

	for (unsigned int i = 0; i < basepairs.size(); i++) {
		MSVBasePair* bp = basepairs[i];

		ANTNucleotide* left = bp->leftNucleotide_;
		ANTNucleotide* right = bp->rightNucleotide_;

		if (left->residue_ == searchedResidue || right->residue_ == searchedResidue) {
			return bp;
		}
	}

	return nullptr;
}

ANTNucleotide* MSVModel::getNucleotideByResidue(SBResidue* searchedResidue) {
  for (auto sit = singleStrands_.begin(); sit != singleStrands_.end(); sit++) {
		ANTSingleStrand* singleStrand = *sit;
		ANTNucleotideList nucleotides = singleStrand->nucleotides_;

		for (auto nit = nucleotides.begin(); nit != nucleotides.end(); nit++) {
			ANTNucleotide* nucleotide = nit->second;
			SBPointer<SBResidue> node = nucleotide->residue_;

			if (node == searchedResidue) {
				return nucleotide;
			}
		}
	}

	return nullptr;
}
