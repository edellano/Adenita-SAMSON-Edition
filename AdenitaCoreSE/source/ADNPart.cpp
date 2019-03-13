#include "ADNPart.hpp"


ADNPart::ADNPart(const ADNPart & n) : SBStructuralModel(n) 
{
  *this = n;
}

ADNPart & ADNPart::operator=(const ADNPart& other) 
{
  SBStructuralModel::operator =(other);  

  if (this != &other) {
    
  }

  return *this;
}

void ADNPart::serialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber) const
{
  SBStructuralModel::serialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);

  //serializer->writeIntElement("number_ds", GetNumberOfDoubleStrands());
  //serializer->writeIntElement("number_ss", GetNumberOfSingleStrands());

  //// bottom scales
  //serializer->writeStartElement("single_strands");  // start single strand
  //SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrandsIndex_) {
  //  serializer->writeUnsignedIntElement("ss_idx", nodeIndexer.getIndex(ss()));
  //  serializer->writeStringElement("name", ss->GetName());
  //  serializer->writeBoolElement("isScaffold", ss->IsScaffold());
  //  serializer->writeBoolElement("isCircular", ss->IsCircular());
  //  serializer->writeIntElement("num_nts", ss->getNumberOfNucleotides());

  //  ADNPointer<ADNNucleotide> nt = ss->GetFivePrime();

  //  serializer->writeStartElement("nucleotides");  // start nucleotide list
  //  while (nt != nullptr) {
  //    serializer->writeUnsignedIntElement("nt_idx", nodeIndexer.getIndex(nt()));
  //    serializer->writeStartElement("nt_data");  // start nucleotide data

  //    serializer->writeIntElement("type", nt->GetType());
  //    serializer->writeStringElement("pos", ADNAuxiliary::SBPositionToString(nt->GetPosition()));
  //    serializer->writeStringElement("bbPos", ADNAuxiliary::SBPositionToString(nt->GetBackbonePosition()));
  //    serializer->writeStringElement("scPos", ADNAuxiliary::SBPositionToString(nt->GetSidechainPosition()));
  //    serializer->writeStringElement("e1", ADNAuxiliary::UblasVectorToString(nt->GetE1()));
  //    serializer->writeStringElement("e2", ADNAuxiliary::UblasVectorToString(nt->GetE2()));
  //    serializer->writeStringElement("e3", ADNAuxiliary::UblasVectorToString(nt->GetE3()));

  //    serializer->writeEndElement();  // end nucleotide data
  //    nt = nt->GetNext();
  //  }
  //  serializer->writeEndElement();  // end nucleotide list
  //}
  //serializer->writeEndElement();  // end single strands

  //// top scales
  //serializer->writeStartElement("double_strands");  // start double strands
  //SB_FOR(ADNPointer<ADNDoubleStrand> ds, doubleStrandsIndex_) {
  //  serializer->writeUnsignedIntElement("ds_idx", nodeIndexer.getIndex(ds()));
  //  serializer->writeStringElement("name", ds->getName());
  //  serializer->writeBoolElement("isCircular", ds->IsCircular());
  //  serializer->writeIntElement("num_bases", ds->GetLength());
  //  serializer->writeDoubleElement("initialTwistAngle", ds->GetInitialTwistAngle());

  //  ADNPointer<ADNBaseSegment> bs = ds->GetFirstBaseSegment();
  //  serializer->writeStartElement("bases");  // start base segments

  //  while (bs != nullptr) {
  //    serializer->writeUnsignedIntElement("bs_idx", nodeIndexer.getIndex(bs()));
  //    serializer->writeStartElement("bs_data");  // start base data

  //    serializer->writeStringElement("pos", ADNAuxiliary::SBPositionToString(bs->GetPosition()));
  //    serializer->writeStringElement("e1", ADNAuxiliary::UblasVectorToString(bs->GetE1()));
  //    serializer->writeStringElement("e2", ADNAuxiliary::UblasVectorToString(bs->GetE2()));
  //    serializer->writeStringElement("e3", ADNAuxiliary::UblasVectorToString(bs->GetE3()));
  //    serializer->writeIntElement("number", bs->GetNumber());

  //    serializer->writeStartElement("cell");  // start cell

  //    auto type = bs->GetCellType();
  //    ADNPointer<ADNCell> cell = bs->GetCell();
  //    serializer->writeIntElement("type", type);
  //    
  //    if (type == CellType::BasePair) {
  //      ADNPointer<ADNBasePair> bp = static_cast<ADNBasePair*>(cell());

  //      int idLeft = -1;
  //      if (bp->GetLeftNucleotide() != nullptr) idLeft = nodeIndexer.getIndex(bp->GetLeftNucleotide()());
  //      serializer->writeIntElement("left", idLeft);

  //      int idRight = -1;
  //      if (bp->GetRightNucleotide() != nullptr) idRight = nodeIndexer.getIndex(bp->GetRightNucleotide()());
  //      serializer->writeIntElement("right", idRight);
  //    }
  //    else if (type == CellType::LoopPair) {
  //      ADNPointer<ADNLoopPair> lp = static_cast<ADNLoopPair*>(cell());

  //      ADNPointer<ADNLoop> left = lp->GetLeftLoop();
  //      serializer->writeStartElement("leftLoop");  // start left loop
  //      if (left != nullptr) {
  //        int startNtId = -1;
  //        if (left->GetStart() != nullptr) startNtId = nodeIndexer.getIndex(left->GetStart()());
  //        serializer->writeIntElement("startNt", startNtId);

  //        int endNtId = -1;
  //        if (left->GetEnd() != nullptr) endNtId = nodeIndexer.getIndex(left->GetEnd()());
  //        serializer->writeIntElement("endNt", endNtId);

  //        auto nts = left->GetNucleotides();
  //        std::vector<int> ntList;
  //        SB_FOR(SBStructuralNode* n, nts) {
  //          ADNPointer<ADNNucleotide> nt = static_cast<ADNNucleotide*>(n);
  //          ntList.push_back(nodeIndexer.getIndex(nt()));
  //        }
  //        std::string str = ADNAuxiliary::VectorToString(ntList);
  //        serializer->writeStringElement("nt_list", str);
  //      }
  //      serializer->writeEndElement();  // end left loop

  //      ADNPointer<ADNLoop> right = lp->GetRightLoop();
  //      serializer->writeStartElement("rightLoop");
  //      if (right != nullptr) {
  //        int startNtId = -1;
  //        if (right->GetStart() != nullptr) startNtId = nodeIndexer.getIndex(right->GetStart()());
  //        serializer->writeIntElement("startNt", startNtId);

  //        int endNtId = -1;
  //        if (right->GetEnd() != nullptr) endNtId = nodeIndexer.getIndex(right->GetEnd()());
  //        serializer->writeIntElement("endNt", endNtId);

  //        auto nts = right->GetNucleotides();
  //        std::vector<int> ntList;
  //        SB_FOR(SBStructuralNode* n, nts) {
  //          ADNPointer<ADNNucleotide> nt = static_cast<ADNNucleotide*>(n);
  //          ntList.push_back(nodeIndexer.getIndex(nt()));
  //        }
  //        std::string strRight = ADNAuxiliary::VectorToString(ntList);
  //        serializer->writeStringElement("nucleotides", strRight);
  //      }
  //      serializer->writeEndElement();  // end right loop
  //    }

  //    serializer->writeEndElement();  // end cell

  //    serializer->writeEndElement();  // end base data
  //    bs = bs->GetNext();
  //  }
  //  serializer->writeEndElement();  // end base segments
  //}
  //serializer->writeEndElement();  // end double strands
}

void ADNPart::unserialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber)
{
  SBStructuralModel::unserialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);

  //int numDss = serializer->readIntElement();
  //int numSss = serializer->readIntElement();

  //serializer->readStartElement();  // begin single strands

  //std::map<int, ADNPointer<ADNNucleotide>> oldIdMap;

  //// build bottom-up
  //for (int i = 0; i < numSss; ++i) {
  //  unsigned int id = serializer->readUnsignedIntElement();
  //  std::string name = serializer->readStringElement();
  //  bool isScaf = serializer->readBoolElement();
  //  bool isCirc = serializer->readBoolElement();
  //  int numNt = serializer->readIntElement();

  //  ADNPointer<ADNSingleStrand> ss = new ADNSingleStrand();
  //  RegisterSingleStrand(ss);
  //  ss->SetName(name);
  //  ss->IsScaffold(isScaf);
  //  ss->IsCircular(isCirc);

  //  serializer->readStartElement();  // begin nucleotides

  //  for (int j = 0; j < numNt; ++j) {
  //    unsigned int nt_id = serializer->readUnsignedIntElement();
  //    ADNPointer<ADNNucleotide> nt = new ADNNucleotide();
  //    oldIdMap.insert(std::make_pair(nt_id, nt));

  //    serializer->readStartElement();  // nucleotide data

  //    DNABlocks t = DNABlocks(serializer->readIntElement());
  //    SBPosition3 pos = ADNAuxiliary::StringToSBPosition(serializer->readStringElement());
  //    SBPosition3 bbPos = ADNAuxiliary::StringToSBPosition(serializer->readStringElement());
  //    SBPosition3 scPos = ADNAuxiliary::StringToSBPosition(serializer->readStringElement());
  //    auto e1 = ADNAuxiliary::StringToUblasVector(serializer->readStringElement());
  //    auto e2 = ADNAuxiliary::StringToUblasVector(serializer->readStringElement());
  //    auto e3 = ADNAuxiliary::StringToUblasVector(serializer->readStringElement());

  //    nt->SetType(t);
  //    nt->SetPosition(pos);
  //    nt->SetBackbonePosition(bbPos);
  //    nt->SetSidechainPosition(scPos);
  //    nt->SetE1(e1);
  //    nt->SetE2(e2);
  //    nt->SetE3(e3);

  //    RegisterNucleotideThreePrime(ss, nt);

  //    serializer->readEndElement();  // end nucleotide data
  //  }

  //  serializer->readEndElement();  // end nucleotides
  //}

  //serializer->readEndElement();  // end single strands

  //// build top-down
  //serializer->readStartElement();  // begin double strands
  //for (int i = 0; i < numDss; ++i) {
  //  unsigned int id = serializer->readUnsignedIntElement();
  //  std::string name = serializer->readStringElement();
  //  bool isCirc = serializer->readBoolElement();
  //  int numBases = serializer->readIntElement();
  //  double initAngle = serializer->readDoubleElement();

  //  ADNPointer<ADNDoubleStrand> ds = new ADNDoubleStrand();
  //  RegisterDoubleStrand(ds);
  //  ds->setName(name);
  //  ds->IsCircular(isCirc);
  //  ds->SetInitialTwistAngle(initAngle);

  //  serializer->readStartElement();  // begin bases
  //  for (int j = 0; j < numBases; ++j) {
  //    unsigned int bs_id = serializer->readUnsignedIntElement();
  //    ADNPointer<ADNBaseSegment> bs = new ADNBaseSegment();

  //    serializer->readStartElement();  // begin base segment data

  //    SBPosition3 pos = ADNAuxiliary::StringToSBPosition(serializer->readStringElement());
  //    auto e1 = ADNAuxiliary::StringToUblasVector(serializer->readStringElement());
  //    auto e2 = ADNAuxiliary::StringToUblasVector(serializer->readStringElement());
  //    auto e3 = ADNAuxiliary::StringToUblasVector(serializer->readStringElement());
  //    int number = serializer->readIntElement();

  //    bs->SetPosition(pos);
  //    bs->SetE1(e1);
  //    bs->SetE2(e2);
  //    bs->SetE3(e3);
  //    bs->SetNumber(number);

  //    serializer->readStartElement();  // begin cell
  //    CellType t = CellType(serializer->readIntElement());

  //    if (t == CellType::BasePair) {
  //      ADNPointer<ADNBasePair> bp_cell = new ADNBasePair();
  //      int idLeft = serializer->readIntElement();
  //      int idRight = serializer->readIntElement();
  //      ADNPointer<ADNNucleotide> ntLeft = nullptr;
  //      ADNPointer<ADNNucleotide> ntRight = nullptr;
  //      if (idLeft != -1) {
  //        ntLeft = oldIdMap[idLeft];
  //        ntLeft->SetBaseSegment(bs);
  //      }

  //      if (idRight != -1) {
  //        ntRight = oldIdMap[idRight];
  //        ntRight->SetBaseSegment(bs);
  //      }

  //      bp_cell->AddPair(ntLeft, ntRight);
  //      bs->SetCell(bp_cell());
  //    }
  //    else if (t == CellType::LoopPair) {
  //      ADNPointer<ADNLoopPair> lp_cell = new ADNLoopPair();

  //      ADNPointer<ADNLoop> leftLoop = ADNPointer<ADNLoop>(new ADNLoop());
  //      serializer->readStartElement();  // begin left loop
  //      int startNtId = serializer->readIntElement();
  //      int endNtId = serializer->readIntElement();
  //      std::string ntList = serializer->readStringElement();

  //      ADNPointer<ADNNucleotide> startNt = static_cast<ADNNucleotide*>(nodeIndexer.getNode(startNtId));
  //      ADNPointer<ADNNucleotide> lastNt = static_cast<ADNNucleotide*>(nodeIndexer.getNode(endNtId));
  //      leftLoop->SetStart(startNt);
  //      leftLoop->SetEnd(lastNt);

  //      std::vector<int> ntVec = ADNAuxiliary::StringToVector(ntList);
  //      for (auto &i : ntVec) {
  //        ADNPointer<ADNNucleotide> nt = static_cast<ADNNucleotide*>(nodeIndexer.getNode(i));
  //        leftLoop->AddNucleotide(nt);
  //        nt->SetBaseSegment(bs);
  //      }

  //      serializer->readEndElement();  // end left loop

  //      ADNPointer<ADNLoop> rightLoop = ADNPointer<ADNLoop>(new ADNLoop());
  //      serializer->readStartElement();  // begin right loop
  //      startNtId = serializer->readIntElement();
  //      endNtId = serializer->readIntElement();
  //      ntList = serializer->readStringElement();

  //      startNt = static_cast<ADNNucleotide*>(nodeIndexer.getNode(startNtId));
  //      lastNt = static_cast<ADNNucleotide*>(nodeIndexer.getNode(endNtId));
  //      rightLoop->SetStart(startNt);
  //      rightLoop->SetEnd(lastNt);

  //      ntVec = ADNAuxiliary::StringToVector(ntList);
  //      for (auto &i : ntVec) {
  //        ADNPointer<ADNNucleotide> nt = static_cast<ADNNucleotide*>(nodeIndexer.getNode(i));
  //        rightLoop->AddNucleotide(nt);
  //        nt->SetBaseSegment(bs);
  //      }

  //      serializer->readEndElement();  // end right loop

  //      lp_cell->SetLeftLoop(leftLoop);
  //      lp_cell->SetRightLoop(rightLoop);
  //      bs->SetCell(lp_cell());
  //    }
  //    else {
  //      ADNPointer<ADNSkipPair> sk_cell = new ADNSkipPair();
  //      bs->SetCell(sk_cell());
  //    }

  //    serializer->readEndElement();  // end cell

  //    RegisterBaseSegmentEnd(ds, bs);

  //    serializer->readEndElement();  // end base segment data
  //  }
  //  serializer->readEndElement();  // end bases
  //}
  //serializer->readEndElement();  // end double strands

  //loadedViaSAMSON(true);
}

std::string const & ADNPart::GetName() const
{
  return getName();
}

void ADNPart::SetName(const std::string & name)
{
  setName(name);
}

CollectionMap<ADNBaseSegment> ADNPart::GetBaseSegments(CellType celltype) const 
{
  CollectionMap<ADNBaseSegment> bsList;
  if (celltype == ALL) {
    bsList = baseSegmentsIndex_;
  }
  else {
    SB_FOR(ADNPointer<ADNBaseSegment> bs, baseSegmentsIndex_) {
      if (bs->GetCellType() == celltype) {
        bsList.addReferenceTarget(bs());
      }
    }
  }

  return bsList;
}

CollectionMap<ADNSingleStrand> ADNPart::GetScaffolds() const 
{
  CollectionMap<ADNSingleStrand> chainList;

  SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrandsIndex_) {
    if (ss->IsScaffold()) {
      chainList.addReferenceTarget(ss());
    }
  }

  return chainList;
}

CollectionMap<ADNNucleotide> ADNPart::GetNucleotides(CellType celltype) const
{
  return nucleotidesIndex_;
}

int ADNPart::GetNumberOfNucleotides() const
{
  return boost::numeric_cast<int>(GetNucleotides().size());
}

int ADNPart::getNumberOfNucleotides() const
{
  return GetNumberOfNucleotides();
}

int ADNPart::GetNumberOfAtoms() const
{
  return boost::numeric_cast<int>(GetAtoms().size());
}

int ADNPart::getNumberOfAtoms() const
{
  return GetNumberOfAtoms();
}

int ADNPart::GetNumberOfBaseSegments() const
{
  return boost::numeric_cast<int>(GetBaseSegments().size());
}

int ADNPart::getNumberOfBaseSegments() const
{
  return GetNumberOfBaseSegments();
}

CollectionMap<ADNSingleStrand> ADNPart::GetSingleStrands() const
{
  return singleStrandsIndex_;
}

CollectionMap<ADNDoubleStrand> ADNPart::GetDoubleStrands() const
{
  return doubleStrandsIndex_;
}

CollectionMap<ADNAtom> ADNPart::GetAtoms() const
{
  return atomsIndex_;
}

int ADNPart::GetNumberOfDoubleStrands() const
{
  return boost::numeric_cast<int>(GetDoubleStrands().size());
}

int ADNPart::getNumberOfDoubleStrands() const
{
  return GetNumberOfDoubleStrands();
}

int ADNPart::GetNumberOfSingleStrands() const
{
  return boost::numeric_cast<int>(GetSingleStrands().size());
}

int ADNPart::getNumberOfSingleStrands() const
{
  return GetNumberOfSingleStrands();
}

void ADNPart::DeregisterSingleStrand(ADNPointer<ADNSingleStrand> ss, bool removeFromParent, bool removeFromIndex) 
{
  if (removeFromParent) {
    auto root = getStructuralRoot();
    root->removeChild(ss());
  }

  if (removeFromIndex) singleStrandsIndex_.removeReferenceTarget(ss());
}

void ADNPart::DeregisterNucleotide(ADNPointer<ADNNucleotide> nt, bool removeFromSs, bool removeFromBs, bool removeFromIndex)
{
  if (removeFromSs) {
    ADNPointer<ADNSingleStrand> ss = nt->GetStrand();
    ss->removeChild(nt());
  }
  if (removeFromBs) {
    auto bs = nt->GetBaseSegment();
    bs->RemoveNucleotide(nt);
  }
  
  if (removeFromIndex) nucleotidesIndex_.removeReferenceTarget(nt());
}

void ADNPart::DeregisterDoubleStrand(ADNPointer<ADNDoubleStrand> ds, bool removeFromParent, bool removeFromIndex)
{
  if (removeFromParent) {
    auto root = getStructuralRoot();
    root->removeChild(ds());
  }

  if (removeFromIndex) doubleStrandsIndex_.removeReferenceTarget(ds());
}

void ADNPart::DeregisterBaseSegment(ADNPointer<ADNBaseSegment> bs, bool removeFromDs, bool removeFromIndex)
{
  if (removeFromDs) bs->getParent()->removeChild(bs());
  if (removeFromIndex) baseSegmentsIndex_.removeReferenceTarget(bs());
}

void ADNPart::DeregisterAtom(ADNPointer<ADNAtom> atom, bool removeFromAtom)
{
  if (removeFromAtom) atom->getParent()->removeChild(atom());
  atomsIndex_.removeReferenceTarget(atom());
}

bool ADNPart::loadedViaSAMSON()
{
  return loadedViaSAMSON_;
}

void ADNPart::loadedViaSAMSON(bool l)
{
  loadedViaSAMSON_ = l;
}

void ADNPart::RegisterSingleStrand(ADNPointer<ADNSingleStrand> ss) 
{
  if (ss->getName().empty()) {
    ss->setName("Single Strand " + std::to_string(singleStrandId_));
    ++singleStrandId_;
  }

  auto root = getStructuralRoot();
  root->addChild(ss());

  singleStrandsIndex_.addReferenceTarget(ss());
}

void ADNPart::RegisterNucleotideThreePrime(ADNPointer<ADNSingleStrand> ss, ADNPointer<ADNNucleotide> nt, bool addToSs)
{
  if (nt->getName().empty()) {
    nt->setName(ADNModel::GetResidueName(nt->GetType()) + " " + std::to_string(nucleotideId_));
    ++nucleotideId_;
  }
  if (addToSs) ss->AddNucleotideThreePrime(nt);

  nucleotidesIndex_.addReferenceTarget(nt());
}

void ADNPart::RegisterNucleotideFivePrime(ADNPointer<ADNSingleStrand> ss, ADNPointer<ADNNucleotide> nt, bool addToSs)
{
  if (nt->getName().empty()) {
    nt->setName(ADNModel::GetResidueName(nt->GetType()) + " " + std::to_string(nucleotideId_));
    ++nucleotideId_;
  }

  if (addToSs) ss->AddNucleotideFivePrime(nt);

  nucleotidesIndex_.addReferenceTarget(nt());
}

void ADNPart::RegisterNucleotide(ADNPointer<ADNSingleStrand> ss, ADNPointer<ADNNucleotide> nt, 
  ADNPointer<ADNNucleotide> ntNext, bool addToSs)
{
  if (nt->getName().empty()) {
    nt->setName(ADNModel::GetResidueName(nt->GetType()) + " " + std::to_string(nucleotideId_));
    ++nucleotideId_;
  }

  if (addToSs) ss->AddNucleotide(nt, ntNext);

  nucleotidesIndex_.addReferenceTarget(nt());
}

void ADNPart::RegisterAtom(ADNPointer<ADNNucleotide> nt, NucleotideGroup g, ADNPointer<ADNAtom> at, bool create)
{
  if (create) {
    at->create();
  }

  nt->AddAtom(g, at);

  atomsIndex_.addReferenceTarget(at());
}

void ADNPart::RegisterAtom(ADNPointer<ADNBaseSegment> bs, ADNPointer<ADNAtom> at, bool create)
{
  if (create) {
    at->create();
  }

  bs->addChild(at());

  atomsIndex_.addReferenceTarget(at());
}

void ADNPart::RegisterBaseSegmentEnd(ADNPointer<ADNDoubleStrand> ds, ADNPointer<ADNBaseSegment> bs, bool addToDs)
{
  if (addToDs) ds->AddBaseSegmentEnd(bs);

  baseSegmentsIndex_.addReferenceTarget(bs());
}

unsigned int ADNPart::GetBaseSegmentIndex(ADNPointer<ADNBaseSegment> bs)
{
  return baseSegmentsIndex_.getIndex(bs());
}

void ADNPart::RegisterDoubleStrand(ADNPointer<ADNDoubleStrand> ds) 
{
  if (ds->getName().empty()) {
    ds->setName("Double Strand " + std::to_string(doubleStrandId_));
    ++doubleStrandId_;
  }
  auto root = getStructuralRoot();
  root->addChild(ds());

  doubleStrandsIndex_.addReferenceTarget(ds());
}