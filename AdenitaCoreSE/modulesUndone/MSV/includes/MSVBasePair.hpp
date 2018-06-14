#pragma once

#include "ANTModel.hpp"

class MSVBasePair {
public:
	MSVBasePair(SBNode* leftNode, SBNode* rightNode);
	MSVBasePair();
	~MSVBasePair();

	unsigned int selectionFlag_;

	unsigned int index_;
	float t_;
	float dG_;
	ANTNucleotide* leftNucleotide_; //currently for scaffold
	ANTNucleotide* rightNucleotide_; //currently for staple

	MSVBasePair* prev_;
	MSVBasePair* next_;

protected:
private:

};
