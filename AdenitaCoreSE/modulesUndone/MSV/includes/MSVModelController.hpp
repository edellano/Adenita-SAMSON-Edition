#pragma once
#ifndef MSVMODELCONTROLLER_H
#define MSVMODELCONTROLLER_H


#include <algorithm>
#include "SBDDataGraphNode.hpp"
#include "SBStructuralModel.hpp"
#include "SAMSON.hpp"
#include "SBChain.hpp"
#include "SBMolecule.hpp"
#include "ANTModel.hpp"
#include "MSVModel.hpp"
#include "DASBackToTheAtom.hpp"
#include "DASDaedalus.hpp"
#include "SEConfig.hpp"


class MSVModelController {
public:
	MSVModelController(const SBNodeIndexer& nodeIndexer);
	MSVModelController();
  MSVModelController(const MSVModelController& other);
	~MSVModelController();

  MSVModelController& operator=(const MSVModelController& other);

	void setUpModel();
  void loadData();
	bool connectNucleotides(ANTNucleotide* start, ANTNucleotide* end);
	bool createEdge(string inputSequence);
	bool splitSingleStrand();
	void renameSBNodes();
	bool deleteNucleotide();
  bool shiftScaffold();

	MSVModel* model_ = nullptr;
  SEConfig* configuration_;

	bool basePairingExist_;

  // testing
	std::map<std::pair<int, int>, std::map<int, std::vector<int>>> GetScaffoldToEdge();	// hard-coded function for cube
  void testCube();
  void test1BNA();

protected:
private:

	SBPointerIndexer<SBNode> atomIndexer_;
	SBPointerIndexer<SBNode> bondIndexer_;
	SBPointerIndexer<SBNode> residueIndexer_;
	SBPointerIndexer<SBNode> chainIndexer_;

	//Aux
	void configure();
  void reindexModel();
	SBPosition3 calcCenterOfMass(SBNode* sideChain1, SBNode* sideChain2);
	SBPosition3 calcCenterOfMass(SBNode* sideChain1);
	SBPosition3 getSidechainCenter(SBNode* node);
	SBPosition3	getBackBoneCenter(SBNode* node);

	void parseBasePairs(string csvFile);

	void initializeSingleStrands(ANTPart* p);
  void initializeSingleStrandsFromNanorobot();
	void initializeDoubleStrands();
};

#endif // !MSVMODELCONTROLLER_H