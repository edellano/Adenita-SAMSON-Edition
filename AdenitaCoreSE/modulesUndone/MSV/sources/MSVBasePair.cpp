#include "MSVBasePair.hpp"	

MSVBasePair::MSVBasePair() {

}

MSVBasePair::MSVBasePair(SBNode* leftSidechain, SBNode* rightSidechain) {
	leftNucleotide_ = new ANTNucleotide(leftSidechain);
	rightNucleotide_ = new ANTNucleotide(rightSidechain);
}

MSVBasePair::~MSVBasePair() {

}