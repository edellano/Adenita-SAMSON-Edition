#include "MSVModelController.hpp"	

MSVModelController::MSVModelController(const SBNodeIndexer& nodeIndexer) : model_{ nullptr } {
	SBNodeIndexer temporaryAtomIndexer;
	SB_FOR(SBNode* node, nodeIndexer)
		node->getNodes(temporaryAtomIndexer, SBNode::IsType(SBNode::Atom));

	SB_FOR(SBNode* node, temporaryAtomIndexer) {
		atomIndexer_.addReferenceTarget(node);
		node->setVisibilityFlag(false);
	}

	SBNodeIndexer temporaryBondIndexer;
	SB_FOR(SBNode* node, nodeIndexer)
		node->getNodes(temporaryBondIndexer, SBNode::IsType(SBNode::Bond));

	SB_FOR(SBNode* node, temporaryBondIndexer)
		bondIndexer_.addReferenceTarget(node);

	SBNodeIndexer temporaryResidueIndexer;
	SB_FOR(SBNode* node, nodeIndexer)
		node->getNodes(temporaryResidueIndexer, SBNode::IsType(SBNode::Residue));

	SB_FOR(SBNode* node, temporaryResidueIndexer)
		residueIndexer_.addReferenceTarget(node);

	SBNodeIndexer temporaryChainIndexer;
	SB_FOR(SBNode* node, nodeIndexer)
		node->getNodes(temporaryChainIndexer, SBNode::IsType(SBNode::Chain));

	SB_FOR(SBNode* node, temporaryChainIndexer)
		chainIndexer_.addReferenceTarget(node);

	configure();
}

MSVModelController::MSVModelController() : model_{nullptr} {
	
}

MSVModelController::MSVModelController(const MSVModelController & other) {
  *this = other;
}

MSVModelController::~MSVModelController() {
  if (model_ != nullptr) delete model_;
  //delete configuration_;
}

MSVModelController & MSVModelController::operator=(const MSVModelController & other) {
  if (this != &other) {
    model_ = other.model_;
    //configuration_ = other.configuration_;
  }
  return *this;
}


void MSVModelController::configure() {
  configuration_ = new SEConfig();
	basePairingExist_ = true;
}


void MSVModelController::setUpModel() {
	if (model_ != nullptr) {
		return;
	}
  else {
    model_ = new MSVModel();
  }
}

void MSVModelController::loadData() {
  auto strands = model_->nanorobot_->GetSingleStrands();
  if (strands.size() > 0) {
    initializeSingleStrandsFromNanorobot();
  }
}

bool MSVModelController::connectNucleotides(ANTNucleotide* start, ANTNucleotide* end) {
	return true;
}

bool MSVModelController::createEdge(string inputSequence) {
	std::map<ANTSingleStrand*, std::vector<ANTNucleotide*>> selectedNucleotides;
	unsigned int numSelected = model_->getSelectedNucletides(selectedNucleotides);
  std::string message = "number of nucleotides selected: ";
	ANTAuxiliary::log(message, false);
	ANTAuxiliary::log(QString::number(numSelected));
	
	if (numSelected != 2) return false;

  ANTNucleotide* connectStart = selectedNucleotides.begin()->second.at(0);
  ANTNucleotide* connectEnd = selectedNucleotides.rbegin()->second.at(0);
  // if we join the scaffold ends it will break since there won't be a 5' end and a 3' end
  //if (selectedNucleotides.size() == 1) {
  //  // both nucleotides belong to the scaffold
  //  connectEnd = selectedNucleotides.begin()->second.at(1);
  //}
	
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
		return false;
	}

	ANTSingleStrand* startSingleStrand = model_->getSingleStrandByNucleotide(connectStart);
	ANTSingleStrand* endSingleStrand = model_->getSingleStrandByNucleotide(connectEnd);

  // merge strands in ANTModel
  ANTSingleStrand* mergedChain = model_->nanorobot_->MergeStrands(startSingleStrand, endSingleStrand, inputSequence);
  // recalculate positions
  if (inputSequence.size() > 0) {
    DASBackToTheAtom btta = DASBackToTheAtom();
    ANTPart& part = *(model_->nanorobot_->GetPart(mergedChain));
    btta.SetNucleotidesPostions(part, *mergedChain);
    /*if (configuration_->use_atomic_details) {
      btta.SetAllAtomsPostions(origami, ss);
    }*/
  }

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

  SAMSON::endHolding();
	return true;
}

bool MSVModelController::splitSingleStrand() {
	std::map<ANTSingleStrand*, std::vector<ANTNucleotide*>> selectedNucleotides;
	unsigned int numSelected = model_->getSelectedNucletides(selectedNucleotides);

	if (numSelected != 1) return false;

	ANTNucleotide* cur = selectedNucleotides.begin()->second.at(0);

  ANTSingleStrand* leftSingleStrand = cur->strand_;
  SBPointer<SBChain> oldChain = leftSingleStrand->chain_;
  
  if (cur->next_ == nullptr) return false;

  auto chain_ids = model_->nanorobot_->BreakStrand(cur, cur->next_);
  SBPointer<SBChain> new_chain1 = chain_ids.first->chain_;
  SBPointer<SBChain> new_chain2 = chain_ids.second->chain_;
  new_chain1->create();
  new_chain2->create();
	
  SAMSON::beginHolding("Splitting strand");

	oldChain->getParent()->addChild(new_chain1());
  oldChain->getParent()->addChild(new_chain2());
  oldChain->getParent()->removeChild(oldChain());
  SAMSON::endHolding();

  return true;
}


void MSVModelController::renameSBNodes() {
	SBNodeIndexer nodeIndexer;
	SAMSON::getActiveDocument()->getNodes(nodeIndexer);

	SBNodeIndexer residues;
	SB_FOR(SBNode* node, nodeIndexer)
		node->getNodes(residues, SBNode::IsType(SBNode::Residue));

	int i = 0;
	SB_FOR(SBNode* node, residues) {
		SBResidue* residue = static_cast<SBResidue*>(node);
		SBSideChain* sidechain = residue->getSideChain();
		SBBackbone* backbone = residue->getBackbone();

		string resName = residue->getResidueTypeString() + " " + to_string(i);
		residue->setName(resName);

		sidechain->setName(residue->getName() + " " + "Side-chain");
		backbone->setName(residue->getName() + " " + "Backbone");

		i++;
	}

}

bool MSVModelController::deleteNucleotide() {
	std::map<ANTSingleStrand*,  std::vector<ANTNucleotide*>> selectedNucleotides;
	unsigned int numSelected = model_->getSelectedNucletides(selectedNucleotides);

	//if (numSelected != 1) return false; 
  if (numSelected < 1) return false;

  if (numSelected == 1) {
    for (auto &p : selectedNucleotides) {
      ANTSingleStrand* ss = p.first;
      std::vector<ANTNucleotide*> delNt = p.second;

      if (delNt.size() == ss->nucleotides_.size()) {
        // delete entire strand
        SAMSON::beginHolding("deleting strand");
        SBPointer<SBChain> chain = ss->chain_;
        if (chain != nullptr) {
          chain->getParent()->removeChild(chain());
        }
        model_->nanorobot_->DeleteStrand(ss);
        SAMSON::endHolding();
      }
      else {
        // todo
        // for the moment we just delete first nt of selection
        ANTNucleotide* cur = delNt[0];
        ANTSingleStrand* leftSingleStrand = cur->strand_;

        SBPointer<SBChain> oldChain = leftSingleStrand->chain_;
        auto chain_ids = model_->nanorobot_->DeleteNucleotide(cur);

        SAMSON::beginHolding("deleting nucleotide");

        if (chain_ids.first != nullptr && chain_ids.second != nullptr) {
          SBPointer<SBChain> new_chain1 = chain_ids.first->chain_;
          SBPointer<SBChain> new_chain2 = chain_ids.second->chain_;
          new_chain1->create();
          new_chain2->create();
          // deleting nucleotide has created two new strands
          oldChain->getParent()->addChild(new_chain1());
          oldChain->getParent()->addChild(new_chain2());
          oldChain->getParent()->removeChild(oldChain());
        }
        else if (chain_ids.first != nullptr) {
          SBPointer<SBChain> new_chain1 = chain_ids.first->chain_;
          new_chain1->create();
          auto* parent = oldChain->getParent();
          oldChain->getParent()->removeChild(oldChain());
          parent->addChild(new_chain1());
        }
        SAMSON::endHolding();
      }
    }
  }
  else if (numSelected > 1) {
    model_->nanorobot_->DeleteNucleotides(selectedNucleotides);
  }

  return true;
}

void MSVModelController::initializeDoubleStrands() {

}

void MSVModelController::initializeSingleStrands(ANTPart* p) {

	MSVModel::MSVSingleStrandList& singleStrands = model_->singleStrands_;

	int nucleotideIndex = 0;
  std::pair<int, size_t> max_strand = std::make_pair(-1, 0);
	for (unsigned int i = 0; i < chainIndexer_.size(); i++) {
		SBNode* chain = chainIndexer_[i];
		ANTSingleStrand * singleStrand = new ANTSingleStrand();
		singleStrand->chain_ = static_cast<SBChain*>(chain);
		singleStrand->chainName_ = singleStrand->chain_->getName();
		singleStrand->id_ = stoi(singleStrand->chainName_);
		singleStrands.push_back(singleStrand);
    p->AddSingleStrand(singleStrand);

		SBNodeIndexer residues;
		chain->getNodes(residues, SBNode::IsType(SBNode::Residue));
		SBNodeIndexer backbones;
		chain->getNodes(backbones, SBNode::IsType(SBNode::Backbone));
		SBNodeIndexer sidechains;
		chain->getNodes(sidechains, SBNode::IsType(SBNode::SideChain));
		
		//add nodes to singlestrands according to the chain index
		map<int, SBResidue*> residueMap;
		for (unsigned int j = 0; j < residues.size(); j++) {
			SBResidue* residue = static_cast<SBResidue*>(residues[j]);
      //ANTAuxiliary::log("residue->getName");
      //ANTAuxiliary::logPtr(quintptr(&residue->getName()));
			string residueName = residue->getName();
      //ANTAuxiliary::log("residueName");
      //ANTAuxiliary::logPtr(quintptr(&residueName));
			int rSpaceIndex = boost::numeric_cast<int>(residueName.find_last_of(" "));
			string residueIndex = residueName.substr(rSpaceIndex, residueName.size());
			//Auxiliary::log("residueIndex", false);

			int index = atoi(residueIndex.c_str());
			//Auxiliary::log(QString::number(index));
      residueMap.insert(make_pair(index, residue));
		}

		int j = 0;
    ANTNucleotide* nt_prev = nullptr;
		for (auto const &ent : residueMap) {
			SBResidue* residue = static_cast<SBResidue*>(ent.second);
			
			SBNodeIndexer backboneList;
			residue->getNodes(backboneList, SBNode::IsType(SBNode::Backbone));
			SBNode* backbone = backboneList[0];

			SBNodeIndexer sidechainList;
			residue->getNodes(sidechainList, SBNode::IsType(SBNode::SideChain));
			SBNode* sidechain = sidechainList[0];

			ANTNucleotide * nucleotide = new ANTNucleotide(residue, backbone, sidechain);
      nucleotide->id_ = nucleotideIndex;
			singleStrand->nucleotides_.insert(std::make_pair(nucleotide->id_, nucleotide));

      nucleotide->SetBackboneCenter(this->calcCenterOfMass(backbone));
      nucleotide->SetSidechainCenter(this->calcCenterOfMass(sidechain));
      nucleotide->SetPosition(this->calcCenterOfMass(residue));

      char inputBaseSymbol = ANTAuxiliary::getBaseSymbol(sidechain);
      nucleotide->type_ = residue_names_.right.at(inputBaseSymbol);

      nucleotide->prev_ = nt_prev;
      if (nt_prev != nullptr) nt_prev->next_ = nucleotide;
			nucleotideIndex++;
			j++;
      nt_prev = nucleotide;
		}

		//set nucleotide as threePrime and fivePrime ends
    singleStrand->SetNucleotidesEnd();
    size_t num_nts = singleStrand->nucleotides_.size();
    if (num_nts > max_strand.second) max_strand = std::make_pair(singleStrand->id_, num_nts);
	}
  p->SetScaffold(max_strand.first);
	model_->numNucleotidesV_ += nucleotideIndex;
	model_->numNucleotidesE_ = model_->numNucleotidesV_; //todo
}

void MSVModelController::initializeSingleStrandsFromNanorobot() {

  std::map<int, ANTPart*> parts = model_->nanorobot_->GetParts();

  for (auto &p : parts) {
    SBPointer<SBStructuralModel> model = p.second->CreateStructuralModel(configuration_->use_atomic_details);
    // re-index
    reindexModel();
    SAMSON::beginHolding("Add model");
    model->create();
    SAMSON::getActiveLayer()->addChild(model());
    SAMSON::endHolding();
  }

  model_->numNucleotidesV_ = boost::numeric_cast<unsigned int>(model_->nanorobot_->GetNucleotides().size());
  model_->numNucleotidesE_ = model_->numNucleotidesV_; //todo
}

SBPosition3 MSVModelController::calcCenterOfMass(SBNode* sideChain1, SBNode* sideChain2) {
	SBPosition3 center;
	SBNodeIndexer atoms1, atoms2;

	sideChain1->getNodes(atoms1, SBNode::IsType(SBNode::Atom));
	sideChain2->getNodes(atoms2, SBNode::IsType(SBNode::Atom));

	int numAtoms = 0;

	SB_FOR(SBNode* node, atoms1) {
		SBAtom* curAtom = static_cast<SBAtom*>(node);
		SBPosition3 curAtomPos = curAtom->getPosition();

		//todo discard H
		center += curAtomPos;
		numAtoms++;
	}

	SB_FOR(SBNode* node, atoms2) {
		SBAtom* curAtom = static_cast<SBAtom*>(node);
		SBPosition3 curAtomPos = curAtom->getPosition();

		//todo discard H
		center += curAtomPos;
		numAtoms++;
		//center = curAtomPos;
	}

	center /= numAtoms;

	return center;
}

SBPosition3 MSVModelController::calcCenterOfMass(SBNode* sideChain1) {
	return ANTVectorMath::CalculateCM(sideChain1);
}

SBPosition3 MSVModelController::getSidechainCenter(SBNode* node) {
	SBPosition3 pos;

	SBNodeIndexer atoms;
	node->getNodes(atoms, SBNode::IsType(SBNode::Atom));
	
	SB_FOR(SBNode* n, atoms) {
		SBAtom* atom = static_cast<SBAtom*>(n);
		string name = atom->getName();
		SBElement::Type element = atom->getElementType();
		if (element == SBElement::Type::Einsteinium) {
			pos = atom->getPosition();
		}
	}
	
	return pos;
}

SBPosition3 MSVModelController::getBackBoneCenter(SBNode* node) {
	SBPosition3 pos;

	SBNodeIndexer atoms;
	node->getNodes(atoms, SBNode::IsType(SBNode::Atom));

	SB_FOR(SBNode* n, atoms) {
		SBAtom* atom = static_cast<SBAtom*>(n);
		string name = atom->getName();
		SBElement::Type element = atom->getElementType();
		if (element == SBElement::Type::Fermium) {
			pos = atom->getPosition();
		}
	}

	return pos;
}

// deprecated?
void MSVModelController::parseBasePairs(string csvFile) {
	//todo remove msvbasepair?

	QFile file(csvFile.c_str());
	if (!file.open(QIODevice::ReadOnly)) {
    std::string message = "error loading csv file";
		ANTAuxiliary::log(message);
		return;
	}

	typedef pair<QString, int> BaseID; //chain and id of nucleotide in the chain
	typedef pair<BaseID, BaseID> BP;
	std::vector<BP> bps;

	QStringList tokens;
	int chainIndex = 0; //the order in csv is different than in pdb
	while (!file.atEnd()) {
		QByteArray line = file.readLine();
		QList<QByteArray> lineTokens = line.split(';');
		QString stapleChainName = lineTokens.at(0);
		QString stapleChainId = lineTokens.at(1);
		QString scaffoldChainName = lineTokens.at(2);
		QString scaffoldChainId = lineTokens.at(3);

		BaseID baseStaple = make_pair(stapleChainName, stapleChainId.toInt());
		BaseID baseScaffold = make_pair(scaffoldChainName, (scaffoldChainId.split("\n")[0]).toInt());

		BP basePair = make_pair(baseStaple, baseScaffold);
		bps.push_back(basePair);
	}
	file.close();

	SBNode* scaffoldChainNode = this->chainIndexer_[0]; //todo

	SBNodeIndexer scaffoldChains;
	scaffoldChainNode->getNodes(scaffoldChains, SBNode::IsType(SBNode::SideChain));

	unsigned int startOfStaplesSideChainIndex = scaffoldChains.size();

	for (int i = 0; i < bps.size(); i++) {

		unsigned int scaffoldSideChainMatlabID = bps[i].second.second;
		unsigned int scaffoldSideChainID = scaffoldSideChainMatlabID - 1;
		unsigned int stapleSideChainID = startOfStaplesSideChainIndex + i;

		if (scaffoldSideChainID >= 0 && scaffoldSideChainID < scaffoldChains.size()) {
      MSVBasePair* bp = new MSVBasePair();
			ANTNucleotide* leftNucleotide = model_->getNucleotideByIndex(scaffoldSideChainID);
			ANTNucleotide* rightNucleotide = model_->getNucleotideByIndex(stapleSideChainID);

			leftNucleotide->pair_ = rightNucleotide;
			rightNucleotide->pair_ = leftNucleotide;

      bp->index_ = i;
      bp->leftNucleotide_ = leftNucleotide;
      bp->rightNucleotide_ = rightNucleotide;

      model_->basePairs_.push_back(bp);
      model_->basePairsSortedByLeft_.insert(make_pair(bp->leftNucleotide_->id_, bp->index_));
      model_->basePairsSortedByRight_.insert(make_pair(bp->rightNucleotide_->id_, bp->index_));
		}
	}
}

bool MSVModelController::shiftScaffold() {
  std::map<ANTSingleStrand*, std::vector<ANTNucleotide*>> selectedNucleotides;
  unsigned int numSelected = model_->getSelectedNucletides(selectedNucleotides);

  if (numSelected != 1) return false;

  ANTNucleotide* cur = selectedNucleotides.begin()->second.at(0);
  if (!cur->strand_->isScaffold_) return false;
  
  ANTSingleStrand* ss = cur->strand_;
  auto parent = ss->chain_->getParent();
  model_->nanorobot_->ShiftScaffold(cur);

  SAMSON::beginHolding("shifting scaffold");

  parent->removeChild(ss->chain_());
  SBPointer<SBChain> chain = ss->CreateSBChain();
  chain->create();
  parent->addChild(chain());

  SAMSON::endHolding();

  return true;
}

std::map<std::pair<int, int>, std::map<int, std::vector<int>>> MSVModelController::GetScaffoldToEdge()
{
	std::map<std::pair<int,int>, std::map<int, std::vector<int>>> bases;
	std::pair<int, int> edge;
	std::map<int, std::vector<int>> hedge;
	std::vector<int> v;
	// edge (0,1) -> 387-357, 729-15
	edge = std::make_pair(0, 1);
	for (int i = 387; i >= 357; --i) {
		v.push_back(i);
	}
	hedge[0] = v;
	v.clear();
	for (int i = 729; i <= 744; ++i) {
		v.push_back(i);
	}
	for (int i = 1; i <= 15; ++i) {
		v.push_back(i);
	}
	hedge[1] = v;
	v.clear();
	bases.insert(std::make_pair(edge, hedge));
	// edge (1,2) -> 16-46, 232-202
	edge = std::make_pair(1, 2);
	for (int i = 232; i >= 202; --i) {
		v.push_back(i);
	}
	hedge[0] = v;
	v.clear();
	for (int i = 16; i <= 46; ++i) {
		v.push_back(i);
	}
	hedge[1] = v;
	v.clear();
	bases.insert(std::make_pair(edge, hedge));
	// edge (2,3) -> 47-62+683-697, 77-63+682-667
	edge = std::make_pair(2, 3);
	for (int i = 77; i >= 63; --i) {
		v.push_back(i);
	}
	for (int i = 682; i >= 667; --i) {
		v.push_back(i);
	}
	hedge[0] = v;
	v.clear();
	for (int i = 47; i <= 62; ++i) {
		v.push_back(i);
	}
	for (int i = 683; i <= 697; ++i) {
		v.push_back(i);
	}
	hedge[1] = v;
	v.clear();
	bases.insert(std::make_pair(edge, hedge));
	// edge (0,3) -> 728-698, 512-542 // edge (3,0) -> 698-728, 542-512
	edge = std::make_pair(3, 0);
	for (int i = 728; i >= 698; --i) {
		v.push_back(i);
	}
	hedge[0] = v;
	v.clear();
	for (int i = 512; i <= 542; ++i) {
		v.push_back(i);
	}
	hedge[1] = v;
	v.clear();
	bases.insert(std::make_pair(edge, hedge));
	// edge (4,5) -> 325-311+434-419, 295-310+435-449
	edge = std::make_pair(4, 5);
	for (int i = 325; i >= 311; --i) {
		v.push_back(i);
	}
	for (int i = 434; i >= 419; --i) {
		v.push_back(i);
	}
	hedge[0] = v;
	v.clear();
	for (int i = 295; i <= 310; ++i) {
		v.push_back(i);
	}
	for (int i = 435; i <= 449; ++i) {
		v.push_back(i);
	}
	hedge[1] = v;
	v.clear();
	bases.insert(std::make_pair(edge, hedge));
	// edge (5,6) -> 450-465+590-604, 480-466+589-574
	edge = std::make_pair(5, 6);
	for (int i = 480; i >= 466; --i) {
		v.push_back(i);
	}
	for (int i = 589; i >= 574; --i) {
		v.push_back(i);
	}
	hedge[0] = v;
	v.clear();
	for (int i = 450; i <= 465; ++i) {
		v.push_back(i);
	}
	for (int i = 590; i <= 604; ++i) {
		v.push_back(i);
	}
	hedge[1] = v;
	v.clear();
	bases.insert(std::make_pair(edge, hedge));
	// edge (6,7) -> 605-620+125-139, 635-621+124-109
	edge = std::make_pair(6, 7);
	for (int i = 635; i >= 621; --i) {
		v.push_back(i);
	}
	for (int i = 124; i >= 109; --i) {
		v.push_back(i);
	}
	hedge[0] = v;
	v.clear();
	for (int i = 605; i <= 620; ++i) {
		v.push_back(i);
	}
	for (int i = 125; i <= 139; ++i) {
		v.push_back(i);
	}
	hedge[1] = v;
	v.clear();
	bases.insert(std::make_pair(edge, hedge));
	// edge (4,7) -> 294-280+155-140, 264-279+156-170
	edge = std::make_pair(7, 4);
	for (int i = 294; i >= 280; --i) {
		v.push_back(i);
	}
	for (int i = 155; i >= 140; --i) {
		v.push_back(i);
	}
	hedge[0] = v;
	v.clear();
	for (int i = 264; i <= 279; ++i) {
		v.push_back(i);
	}
	for (int i = 156; i <= 170; ++i) {
		v.push_back(i);
	}
	hedge[1] = v;
	v.clear();
	bases.insert(std::make_pair(edge, hedge));
	// edge (1,4) ->356-326, 233-263
	edge = std::make_pair(1, 4);
	for (int i = 356; i >= 326; --i) {
		v.push_back(i);
	}
	hedge[0] = v;
	v.clear();
	for (int i = 233; i <= 263; ++i) {
		v.push_back(i);
	}
	hedge[1] = v;
	v.clear();
	bases.insert(std::make_pair(edge, hedge));
	// edge (2,7) -> 201-171, 78-108
	edge = std::make_pair(2, 7);
	for (int i = 201; i >= 171; --i) {
		v.push_back(i);
	}
	hedge[0] = v;
	v.clear();
	for (int i = 78; i <= 108; ++i) {
		v.push_back(i);
	}
	hedge[1] = v;
	v.clear();
	bases.insert(std::make_pair(edge, hedge));
	// edge (0,5) -> 388-418, 511-481
	edge = std::make_pair(0, 5);
	for (int i = 511; i >= 481; --i) {
		v.push_back(i);
	}
	hedge[0] = v;
	v.clear();
	for (int i = 388; i <= 418; ++i) {
		v.push_back(i);
	}
	hedge[1] = v;
	v.clear();
	bases.insert(std::make_pair(edge, hedge));
	// edge (3,6) -> 543-573, 666-636
	edge = std::make_pair(3, 6);
	for (int i = 666; i >= 636; --i) {
		v.push_back(i);
	}
	hedge[0] = v;
	v.clear();
	for (int i = 543; i <= 573; ++i) {
		v.push_back(i);
	}
	hedge[1] = v;
	v.clear();
	bases.insert(std::make_pair(edge, hedge));

	return bases;
}

void MSVModelController::testCube() {

  ANTPolyhedron poly = ANTPolyhedron();
  poly.LoadFromPLYFile(SB_ELEMENT_PATH + "/Data/cube.ply");
  ANTSingleStrand scaff = ANTSingleStrand();

  //model_->nanorobot_ = &ANTNanorobot::GenerateANTNanorobot();
  ANTNanorobot* nanorobot = new ANTNanorobot();
  model_->nanorobot_ = nanorobot;

  ANTPart* part = new ANTPart();
  part->SetPolyhedron(poly);

  initializeSingleStrands(part);
  parseBasePairs(SB_ELEMENT_PATH + "/Data/" + "base_pairs_sorted.csv");

  std::map<std::pair<int, int>, std::map<int, std::vector<int>>> scaff_to_edge = GetScaffoldToEdge();
  Edges edges = part->GetEdges();
  for (auto eit = edges.begin(); eit != edges.end(); ++eit) {
    part->bpLengths_.insert(std::make_pair(*eit, 31));
  }
  DASDaedalus::SetVerticesPositions(*part);

  unsigned int qid = 0;
  unsigned int qv_id = 0;

  std::map<int, ANTJoint*> added_vertices;
  Vertices vertex_list = part->GetVertices();
  Faces faces = part->GetFaces();

  for (auto it = faces.begin(); it != faces.end(); ++it) {
    ANTPolygon* f = *it;
    std::vector<int> vertices;
    ANTHalfEdge* begin = f->halfEdge_;
    ANTHalfEdge* he = begin;
    do {
      vertices.push_back(he->source_->id_);
      he = he->next_;
    } while (he != begin);

    std::vector<ANTBaseSegment*> quasiedges;
    ANTJoint* qv_prev = nullptr;
    ANTBaseSegment* pqe = nullptr;
    ANTBaseSegment* first_bs = nullptr;
    ANTJoint* first_joint = nullptr;
    bool fbs = true;
    for (auto vit = vertices.begin(); vit != vertices.end(); ++vit) {
      auto p_vit = vit;
      if (vit == vertices.begin()) {
        p_vit = std::prev(vertices.end());
      }
      else {
        p_vit = std::prev(vit);
      }
      std::pair<int, int> e_orig = std::make_pair(*p_vit, *vit);
      if (scaff_to_edge.find(e_orig) == scaff_to_edge.end()) {
        e_orig = std::make_pair(*vit, *p_vit);
      }

      SBPosition3 coords = vertex_list.at(*vit)->GetSBPosition();
      SBPosition3 p_coords = vertex_list.at(*p_vit)->GetSBPosition();
      SBVector3 edge_dir = (coords - p_coords).normalizedVersion();

      // Add quasi-vertices
      int bp_length = part->bpLengths_.at(vertex_list.at(*vit)->halfEdge_->edge_);
      double qv_step = BP_RISE;
      // first joint
      SBPosition3 c = p_coords;
      if (qv_prev == nullptr) {
        qv_prev = new ANTJoint(c);
        qv_prev->id_ = qv_id;
        ++qv_id;
        part->AddJoint(qv_prev);
        first_joint = qv_prev;
      }
      for (int i = 0; i < bp_length; ++i) {
        SBQuantity::angstrom l(i*BP_RISE);
        c = p_coords + l*edge_dir;
        ANTJoint* qv = new ANTJoint(c);
        qv->id_ = qv_id;
        ++qv_id;
        part->AddJoint(qv);

        int branch = 0;
        int index = i;
        if (*p_vit > *vit) { // this is a daedalus convention
          branch = 1;
          index = bp_length - i - 1;
        }
        ANTBaseSegment* qe = new ANTBaseSegment(qv_prev, qv);
        if (fbs) {
          first_bs = qe;
          fbs = false;
        }
        qe->number_ = i;
        if (pqe != nullptr) pqe->next_ = qe;
        qe->previous_ = pqe;
        //qe->face_ = f;
        part->AddBaseSegment(qe);
        // Hard coded for cube
        auto scaff_edge = scaff_to_edge.at(e_orig);
        int scaff_id1 = scaff_edge.at(branch).at(index);
        ANTNucleotide* nt = model_->getNucleotideByIndex(scaff_id1 - 1, 0);
        qe->SetNucleotide(nt);
        qe->id_ = qid;
        ++qid;
        qv_prev = qv;
        pqe = qe;
        quasiedges.push_back(qe);
      }
    }
    // fix last base segment
    pqe->next_ = first_bs;
    pqe->target_ = first_joint;
    first_bs->previous_ = pqe;
  }
  part->SetNtSegmentMap();
  nanorobot->RegisterPart(part);
}

void MSVModelController::test1BNA() {
  ANTPolyhedron poly = ANTPolyhedron();
  poly.LoadFromPLYFile(SB_ELEMENT_PATH + "/Data/cube.ply");
  ANTSingleStrand scaff = ANTSingleStrand();

  ANTNanorobot* nanorobot = new ANTNanorobot();
  model_->nanorobot_ = nanorobot;

  ANTPart* part = new ANTPart();
  part->SetPolyhedron(poly);

  initializeSingleStrands(part);
  parseBasePairs(SB_ELEMENT_PATH + "/Data/" + "1bna_base_pairs_sorted.csv");

  part->SetScaffold(0);

  std::map<std::pair<int, int>, std::map<int, std::vector<int>>> scaff_to_edge = GetScaffoldToEdge();
  Edges edges = nanorobot->GetEdges();
  for (auto eit = edges.begin(); eit != edges.end(); ++eit) {
    part->bpLengths_.insert(std::make_pair(*eit, 31));
  }
  DASDaedalus::SetVerticesPositions(*part);

  unsigned int qid = 0;
  unsigned int qv_id = 0;

  std::map<int, ANTJoint*> added_vertices;
  Vertices vertex_list = part->GetVertices();
  Faces faces = part->GetFaces();

  for (auto it = faces.begin(); it != faces.end(); ++it) {
    ANTPolygon* f = *it;
    std::vector<int> vertices;
    ANTHalfEdge* begin = f->halfEdge_;
    ANTHalfEdge* he = begin;
    do {
      vertices.push_back(he->source_->id_);
      he = he->next_;
    } while (he != begin);

    std::vector<ANTBaseSegment*> quasiedges;
    for (auto vit = vertices.begin(); vit != vertices.end(); ++vit) {
      auto p_vit = vit;
      if (vit == vertices.begin()) {
        p_vit = std::prev(vertices.end());
      }
      else {
        p_vit = std::prev(vit);
      }
      std::pair<int, int> e_orig = std::make_pair(*p_vit, *vit);
      if (scaff_to_edge.find(e_orig) == scaff_to_edge.end()) {
        e_orig = std::make_pair(*vit, *p_vit);
      }

      SBPosition3 coords = vertex_list.at(*vit)->GetSBPosition();
      SBPosition3 p_coords = vertex_list.at(*p_vit)->GetSBPosition();
      SBVector3 edge_dir = (coords - p_coords).normalizedVersion();

      // Add quasi-vertices
      int bp_length = part->bpLengths_.at(vertex_list.at(*vit)->halfEdge_->edge_);
      double qv_step = BP_RISE;
      ANTJoint* pqv = nullptr;
      for (int i = 0; i < bp_length + 1; ++i) {
        SBQuantity::angstrom l(i*BP_RISE);
        SBPosition3 c = p_coords + l*edge_dir;
        ANTJoint* qv = new ANTJoint(c);
        qv->id_ = qv_id;

        ++qv_id;
        part->AddJoint(qv);

        if (i > 0) {
          int branch;
          int index;
          if (*p_vit > *vit) { // this is a daedalus convention
            branch = 1;
            index = bp_length - i;
          }
          else {
            branch = 0;
            index = i - 1;
          }
          ANTBaseSegment* qe = new ANTBaseSegment(pqv, qv);
          // Hard coded for cube
          auto scaff_edge = scaff_to_edge.at(e_orig);
          int scaff_id1 = scaff_edge.at(branch).at(index);
          ANTNucleotide* nt = model_->getNucleotideByIndex(scaff_id1 - 1, 0);
          qe->SetNucleotide(nt);
          qe->id_ = qid;
          ++qid;
          quasiedges.push_back(qe);
        }
        pqv = qv;
      }
    }

    // Link all quasiedges in face
    for (auto qit = quasiedges.begin(); qit != quasiedges.end(); ++qit) {
      auto n_qit = std::next(qit);
      if (n_qit == quasiedges.end()) {
        n_qit = quasiedges.begin();
      }
      auto p_qit = qit;
      if (qit == quasiedges.begin()) {
        p_qit = std::prev(quasiedges.end());
      }
      else {
        p_qit = std::prev(qit);
      }
      ANTBaseSegment* qe = *qit;
      ANTBaseSegment* nqe = *n_qit;
      ANTBaseSegment* pqe = *p_qit;
      qe->next_ = nqe;
      qe->previous_ = pqe;
      //qe->face_ = f;

      part->AddBaseSegment(qe);
    }
  }
  part->SetNtSegmentMap();
  nanorobot->RegisterPart(part);
}

void MSVModelController::reindexModel() {
  SBNodeIndexer nodeIndexer;
  SAMSON::getActiveDocument()->getNodes(nodeIndexer);

  // duplicated code, we can use extract method from here on
  SBNodeIndexer temporaryAtomIndexer;
  SB_FOR(SBNode* node, nodeIndexer)
    node->getNodes(temporaryAtomIndexer, SBNode::IsType(SBNode::Atom));

  SB_FOR(SBNode* node, temporaryAtomIndexer) {
    atomIndexer_.addReferenceTarget(node);
    node->setVisibilityFlag(false);
  }

  SBNodeIndexer temporaryBondIndexer;
  SB_FOR(SBNode* node, nodeIndexer)
    node->getNodes(temporaryBondIndexer, SBNode::IsType(SBNode::Bond));

  SB_FOR(SBNode* node, temporaryBondIndexer)
    bondIndexer_.addReferenceTarget(node);

  SBNodeIndexer temporaryResidueIndexer;
  SB_FOR(SBNode* node, nodeIndexer)
    node->getNodes(temporaryResidueIndexer, SBNode::IsType(SBNode::Residue));

  SB_FOR(SBNode* node, temporaryResidueIndexer)
    residueIndexer_.addReferenceTarget(node);

  SBNodeIndexer temporaryChainIndexer;
  SB_FOR(SBNode* node, nodeIndexer)
    node->getNodes(temporaryChainIndexer, SBNode::IsType(SBNode::Chain));

  SB_FOR(SBNode* node, temporaryChainIndexer)
    chainIndexer_.addReferenceTarget(node);

  configure();
}