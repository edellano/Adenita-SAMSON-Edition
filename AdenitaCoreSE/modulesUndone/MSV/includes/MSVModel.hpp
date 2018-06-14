#pragma once
#ifndef MSVMODEL_H
#define MSVMODEL_H

#include "SBProxy.hpp"
#include "SBDDataGraphNode.hpp"
#include "SBAtom.hpp"
#include "ANTNanorobot.hpp"
#include "ANTModel.hpp"
#include "MSVBasePair.hpp"
#include "MSVBindingRegion.hpp"

class MSVModel {
	SB_CLASS
public:
  
	MSVModel();
  ~MSVModel();

	ANTAuxiliary::ANTArray<float>                   getBaseColor(DNABlocks typ);
  ANTNucleotide*                                  getNucleotideByIndex(unsigned int index);
	ANTNucleotide*                                  getNucleotideByIndex(unsigned int index, unsigned int chain_id);
	std::vector<ANTSingleStrand*> getScaffoldStrands();
	unsigned int getSelectedNucletides(std::map<ANTSingleStrand*, std::vector<ANTNucleotide*>> &selectedNucleotides);
	ANTSingleStrand*                                getSingleStrandByNucleotide(ANTNucleotide* serachedNucleotide);
	MSVBasePair*                                    getBasePairByNucleotide(ANTNucleotide* searchedNucleotide);
	MSVBasePair*                                    getBasePairByResidue(SBResidue* serachedResidue);
	ANTNucleotide*                                  getNucleotideByResidue(SBResidue* serachedResidue);

  ANTAuxiliary::ANTArray<float>                   getAtomColor(std::string element); //get the color of atoms (i.e., O, C, N, P)
  ANTAuxiliary::ANTArray<float>                   getAtomColor(SBElement::Type type); //get the color of atoms (i.e., O, C, N, P)


  typedef std::vector<MSVBindingRegion*> MSVBindingRegionList;
  typedef std::vector<ANTSingleStrand*> MSVSingleStrandList;
  typedef std::vector<MSVBasePair*> MSVBasePairList;

	//configurations
	float	doubleHelixVRadius_;
	float	doubleHelixERadius_;
	float	atomLineRadius_;
	float	atomBallRadius_;
	float	bondBallsRadius_;
	float	nucleotideVRadius_;
  float	nucleotideBaseRadius_;
  float	nucleotideTipRadius_;
	float	singleStrandRadius_;
	float	doubleStrandRadius;
	float	vertexRadius_;
	float	edgeRadius_;

	unsigned int numAdenine_;
	unsigned int numGuanine_;
	unsigned int numCytosine_;
	unsigned int numThymine_;

	//colors
	unsigned int numElements_;
	ANTAuxiliary::ANTArray<float> atomColors_;
	unsigned int numStapleColors_;
	ANTAuxiliary::ANTArray<float> stapleColors_;
  ANTAuxiliary::ANTArray<float> nucleotideEColor1_;
  ANTAuxiliary::ANTArray<float> adenineColor_;
  ANTAuxiliary::ANTArray<float>	thymineColor_;
  ANTAuxiliary::ANTArray<float>	cytosineColor_;
  ANTAuxiliary::ANTArray<float>	guanineColor_;
  ANTAuxiliary::ANTArray<float> undefinedNucleotideColor_;
  ANTAuxiliary::ANTArray<float>	doubleHelixVColor_;
  ANTAuxiliary::ANTArray<float>	doubleHelixEColor_;
  ANTAuxiliary::ANTArray<float>	basePairingEColor_;
	unsigned int numPropertyColors_;
  ANTAuxiliary::ANTArray<float> propertyColors_;
  ANTAuxiliary::ANTArray<float>	baseSegmentColor_;



	float minMeltingTemp_;
	float	maxMeltingTemp_;
	
	float	minGibbsFreeEnergy_;
	float	maxGibbsFreeEnergy_;

	unsigned int numNucleotidesV_;
	unsigned int numNucleotidesE_;
	unsigned int selectionFlag_;

	ANTNanorobot* nanorobot_ = nullptr;
  // deprecated?
	MSVBasePairList basePairs_;
	std::map<int, int> basePairsSortedByLeft_;
	std::map<int, int> basePairsSortedByRight_;
	MSVBindingRegionList bindingRegions_;
	MSVSingleStrandList singleStrands_;
	
protected:
private:

};

SB_REGISTER_TYPE(MSVModel, "MSVModel", "1CABD0AC-80AF-595D-0DAA-F9DEC11669F9");

#endif