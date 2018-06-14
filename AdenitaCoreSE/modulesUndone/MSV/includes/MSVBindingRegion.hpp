#pragma once
#ifndef MSVBINDINGREGION_H
#define MSVBINDINGREGION_H


#include "MSVBasePair.hpp"
#include "ANTAuxiliary.hpp"
#include "SAMSON.hpp"

class MSVBindingRegion {
public:
	MSVBindingRegion();
	MSVBindingRegion(unsigned int index);
	~MSVBindingRegion() = default;

	unsigned int selectionFlag_;

	unsigned int index_;
	float dS_;
	float dH_;
	float dG_;
	float t_;

  std::vector<ANTNucleotide *> nucleotides_;
  //ANTNucleotide * start_; //starting nucleotide in the binding region
  //ANTNucleotide * end_; //ending nucleotide in the binding region
  //unsigned int size_; //num of nucleotides in the binding region on one strand

	string leftSequence_;
	string rightSequence_;

	void initializeSequences();

	void executeNtthal(int oligo_conc, int mv, int dv);

protected:
private:

};

#endif
