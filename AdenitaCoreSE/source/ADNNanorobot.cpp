/** \file
 *  \author   Elisa de Llano <elisa.dellano.fl@ait.ac.at>
 */

#include "ADNNanorobot.hpp"


ADNNanorobot::ADNNanorobot(const ADNNanorobot & n) : Nameable(n), Positionable(n), Orientable(n)
{
  *this = n;
}

ADNNanorobot & ADNNanorobot::operator=(const ADNNanorobot& other)
{
  Nameable::operator =(other);
  Positionable::operator =(other);
  Orientable::operator =(other);

  return *this;
}

CollectionMap<ADNSingleStrand> ADNNanorobot::GetSingleStrands() const
{
  CollectionMap<ADNSingleStrand> singleStrands;

  SBNodeIndexer nodeIndexer;
  SAMSON::getActiveDocument()->getNodes(nodeIndexer, (SBNode::GetClass() == std::string("ADNSingleStrand")) && (SBNode::GetElementUUID() == SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));

  SB_FOR(SBNode* n, nodeIndexer) {
    singleStrands.addReferenceTarget(static_cast<ADNSingleStrand*>(n));
  }

  return singleStrands;
}

void ADNNanorobot::RegisterPart(ADNPointer<ADNPart> part)
{
  partsIndex_.addReferenceTarget(part());
}

void ADNNanorobot::DeregisterPart(ADNPointer<ADNPart> part)
{
  partsIndex_.removeReferenceTarget(part());
}

int ADNNanorobot::GetNumberOfDoubleStrands()
{
  auto parts = GetParts();
  int count = 0;

  SB_FOR (ADNPointer<ADNPart> part, parts) {
    count += part->GetNumberOfDoubleStrands();
  }

  return count;
}

int ADNNanorobot::GetNumberOfBaseSegments()
{
  auto parts = GetParts();
  int count = 0;

  SB_FOR(ADNPointer<ADNPart> part, parts) {
    count += part->GetNumberOfBaseSegments();
  }

  return count;
}

int ADNNanorobot::GetNumberOfSingleStrands()
{
  auto parts = GetParts();
  int count = 0;

  SB_FOR(ADNPointer<ADNPart> part, parts) {
    count += part->GetNumberOfSingleStrands();
  }

  return count;
}

int ADNNanorobot::GetNumberOfNucleotides()
{
  auto parts = GetParts();
  int count = 0;

  SB_FOR(ADNPointer<ADNPart> part, parts) {
    count += part->GetNumberOfNucleotides();
  }

  return count;
}

CollectionMap<ADNPart> ADNNanorobot::GetParts() const
{
  //CollectionMap<ADNPart> parts;

  //SBNodeIndexer nodeIndexer;
  //SAMSON::getActiveDocument()->getNodes(nodeIndexer, (SBNode::GetClass() == std::string("ADNPart")) && (SBNode::GetElementUUID() == SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")) );

  //SB_FOR(SBNode* n, nodeIndexer) {
  //  if (!n->isErased()) parts.addReferenceTarget(static_cast<ADNPart*>(n));
  //}

  //return parts;

  return partsIndex_;
}

//ANTSingleStrand* ANTNanorobot::MergeStrands(ANTSingleStrand * first_strand, ANTSingleStrand * second_strand, std::string sequence) {
//  if (first_strand == nullptr || second_strand == nullptr) return nullptr;
//
//  ANTPart* p1 = singleStrands_.at(first_strand);
//  ANTPart* p2 = singleStrands_.at(second_strand);
//  ANTSingleStrand* ss = nullptr;
//
//  if (p1 == p2) {
//    ss = p1->MergeStrands(first_strand, second_strand, sequence);
//    singleStrands_.insert(std::make_pair(ss, p1));
//    singleStrands_.erase(first_strand);
//    singleStrands_.erase(second_strand);
//  }
//  else {
//    // ANTPart* p = MergeParts(p1, p2);
//    // Set second strand as part of first ANTPart
//    p2->DeregisterSingleStrand(second_strand);
//    p1->RegisterSingleStrand(second_strand);
//    p1->SetNtSegmentMap();
//    ss = p1->MergeStrands(first_strand, second_strand, sequence);
//    singleStrands_.insert(std::make_pair(ss, p1));
//    singleStrands_.erase(first_strand);
//    singleStrands_.erase(second_strand);
//    //RegisterPart(p);
//  }
//  return ss;
//}
//
//std::pair<ANTDoubleStrand*, ANTPart*> ANTNanorobot::ConnectBaseSegments(ANTBaseSegment * bsFirst, ANTBaseSegment * bsSecond, int num_baseSegments, 
//  int numPolyT, std::string sequence, ANTNucleotide* nt1, ANTNucleotide* nt2, int connectionType) {
//
//  ANTDoubleStrand* ds1 = bsFirst->doubleStrand_;
//  ANTPart* part1 = doubleStrands_.at(ds1);
//  ANTDoubleStrand* ds2 = bsSecond->doubleStrand_;
//  ANTPart* part2 = doubleStrands_.at(ds2);
//
//  // merge parts and create new s.m.
//  ANTPart* part = part1;
//  if (part1 != part2) part = MergeParts(part1, part2);
//
//  SBPosition3 posFirst = bsFirst->GetSBPosition();
//  SBPosition3 posSecond = bsSecond->GetSBPosition();
//  SBVector3 dir = (posSecond - posFirst).normalizedVersion();
//
//  // find first position so base segments are centered
//  SBQuantity::length totalLength = num_baseSegments * SBQuantity::nanometer(ANTConstants::BP_RISE);
//  SBPosition3 middlePoint = (posFirst + posSecond)*0.5;
//  SBPosition3 pos = bsFirst->target_->GetSBPosition() + dir * ((middlePoint - posFirst).norm() - totalLength*0.5);
//
//  ANTBaseSegment* firstBs = nullptr;
//  //ANTJoint* source = new ANTJoint(bsFirst->target_->GetSBPosition() + SBQuantity::nanometer(ANTConstants::BP_RISE) * dir);
//  ANTJoint* source = new ANTJoint(pos);
//  part->AddJoint(source);
//  ANTBaseSegment* prevBs = nullptr;
//
//  ANTDoubleStrand* newDS = new ANTDoubleStrand();
//  ANTSingleStrand* newSS = new ANTSingleStrand();
//  ANTSingleStrand* newSSPair = new ANTSingleStrand();
//  ANTNucleotide* prevNt = nullptr;
//  ANTNucleotide* nextNtPair = nullptr;
//  unsigned int nt_id = 0;
//  unsigned int bs_num = 0;
//  int seqCounter = 0;
//  for (int i = 0; i < num_baseSegments; ++i) {
//    SBPosition3 pos = source->GetSBPosition() + SBQuantity::nanometer(ANTConstants::BP_RISE) * dir;
//    ANTJoint* target = new ANTJoint(pos);
//    ANTBaseSegment* bs = new ANTBaseSegment();
//    bs->source_ = source;
//    bs->target_ = target;
//    bs->direction_ = dir;
//    bs->doubleStrand_ = newDS;
//    bs->number_ = bs_num;
//    ++bs_num;
//    part->AddJoint(target);
//    part->AddBaseSegment(bs);
//
//    ANTNucleotide* nt = nullptr;
//    ANTNucleotide* ntPair = nullptr;
//
//    if (numPolyT > 0 && (i == 0 || i == num_baseSegments - 1)) {
//      // at beginning and end we add a polyT region
//      ANTLoopPair* cell = new ANTLoopPair();
//      ANTLoop* loopLeft = ANTPart::AppendPolyTLoopFiveToThree(newSS, bs, prevNt, numPolyT);
//      ANTLoop* loopRight = ANTPart::AppendPolyTLoopThreeToFive(newSSPair, bs, nextNtPair, numPolyT);
//      cell->SetLeftLoop(loopLeft);
//      cell->SetRightLoop(loopRight);
//      bs->SetCell(cell);
//      nt = loopLeft->endNt_;
//      ntPair = loopRight->startNt_;
//      nt_id = ntPair->id_;  // highest nt_id created is for loopRight
//    }
//    else {
//      nt = new ANTNucleotide();
//      nt->id_ = nt_id;
//      nt->type_ = DNABlocks::DN_;
//      if (sequence.size() > 0) nt->type_ = residue_names_.right.at(sequence[seqCounter]);
//      ++seqCounter;
//      nt->strand_ = newSS;
//      newSS->nucleotides_.insert(std::make_pair(nt->id_, nt));
//
//      ntPair = new ANTNucleotide();
//      ntPair->id_ = nt_id;
//      ntPair->type_ = ANTModel::GetComplementaryBase(nt->type_);
//      ntPair->strand_ = newSSPair;
//      newSSPair->nucleotides_.insert(std::make_pair(ntPair->id_, ntPair));
//
//      ANTBasePair* cell = new ANTBasePair();
//      cell->SetLeftNucleotide(nt);
//      cell->SetRightNucleotide(ntPair);
//      bs->SetCell(cell);
//
//      nt->pair_ = ntPair;
//      ntPair->pair_ = nt;
//
//      if (prevNt != nullptr) prevNt->next_ = nt;
//      nt->prev_ = prevNt;
//      if (nextNtPair != nullptr) nextNtPair->prev_ = ntPair;
//      ntPair->next_ = nextNtPair;
//    }
//
//    if (prevBs != nullptr) prevBs->next_ = bs;
//    bs->previous_ = prevBs;
//
//    if (i == 0) firstBs = bs;
//
//    source = target;
//    prevBs = bs;
//    nextNtPair = ntPair;
//    prevNt = nt;
//
//    ++nt_id;
//  }
//
//  newSS->SetNucleotidesEnd();
//  newSSPair->SetNucleotidesEnd();
//  part->RegisterSingleStrand(newSS);
//  part->RegisterSingleStrand(newSSPair);
//  part->RegisterDoubleStrand(newDS);
//  part->SetNtSegmentMap();
//
//  // Connect new nts to the corresponding strands
//  if (nt1 == nullptr || nt2 == nullptr) {
//    std::vector<ANTNucleotide*> sel;
//    auto selectedNuceotides = GetSelectedNucleotides();
//    for (auto &it : selectedNuceotides) {
//      for (auto &nit : it.second) {
//        sel.push_back(nit);
//      }
//    }
//
//    nt1 = sel.at(0);
//    nt2 = sel.at(1);
//  }
//
//  bool sameStrand = false;
//  if (nt1->strand_ == nt2->strand_) sameStrand = true;
//
//  auto firstChains = BreakStrand(nt1, nt1->next_);
//  auto secondChains = BreakStrand(nt2->prev_, nt2);
//  // update single strands and double strands in the nanorobot object too
//  UpdatePart(part);
//
//  if (connectionType == 0) {
//    // the connection is generated transversally
//    if (sameStrand) {
//      // we need to update firstChains as we have broke one of the resulting
//      firstChains.second = secondChains.first;
//      auto chain = MergeStrands(firstChains.first, newSS);
//      auto finalChain = MergeStrands(chain, secondChains.second);
//      ANTNucleotide* breakNt = newSSPair->nucleotides_.at(round(newSSPair->nucleotides_.size()*0.5));
//      auto finalChainPair = MergeStrands(secondChains.first, newSSPair);
//      finalChainPair->ShiftStart(breakNt);
//    }
//    else {
//      auto chain = MergeStrands(firstChains.first, newSS);
//      auto finalChain = MergeStrands(chain, secondChains.second);
//      auto chainPair = MergeStrands(secondChains.first, newSSPair);
//      auto finalChainPair = MergeStrands(chainPair, firstChains.second);
//    }
//  }
//  else if (connectionType == 1) {
//    if (nt1->pair_ != nullptr && nt2->pair_ != nullptr) {
//      if (sameStrand) {
//        // we need to update firstChains as we have broke one of the resulting
//        firstChains.second = secondChains.first;
//        ANTNucleotide* breakNt = newSSPair->nucleotides_.at(round(newSSPair->nucleotides_.size()*0.5));
//        auto finalChainPair = MergeStrands(secondChains.first, newSSPair);
//        finalChainPair->ShiftStart(breakNt);
//      }
//      else {
//        auto chainPair = MergeStrands(secondChains.first, newSSPair);
//        auto finalChainPair = MergeStrands(chainPair, firstChains.second);
//      }
//
//      std::pair<ANTSingleStrand*, ANTSingleStrand*> firstChainsPairs = std::make_pair(nullptr, nullptr);
//      std::pair<ANTSingleStrand*, ANTSingleStrand*> secondChainsPairs = std::make_pair(nullptr, nullptr);
//      if (nt1->pair_ != nullptr && nt1->pair_->prev_ != nullptr) firstChainsPairs = BreakStrand(nt1->pair_->prev_, nt1->pair_);
//      if (nt2->pair_ != nullptr && nt2->pair_->next_ != nullptr) secondChainsPairs = BreakStrand(nt2->pair_, nt2->pair_->next_);
//      UpdatePart(part);
//
//      auto chainTwo = MergeStrands(firstChainsPairs.first, newSS);
//      auto finalChainTwo = MergeStrands(chainTwo, secondChainsPairs.second);
//    }
//  }
//  else if (connectionType == 2) {
//    if (nt1->pair_ != nullptr && nt2->pair_ != nullptr) {
//      auto chainPair = MergeStrands(firstChains.first, newSS);
//      auto finalChainPair = MergeStrands(chainPair, secondChains.second);
//
//      std::pair<ANTSingleStrand*, ANTSingleStrand*> firstChainsPairs = std::make_pair(nullptr, nullptr);
//      std::pair<ANTSingleStrand*, ANTSingleStrand*> secondChainsPairs = std::make_pair(nullptr, nullptr);
//      if (nt1->pair_ != nullptr && nt1->pair_->prev_ != nullptr) firstChainsPairs = BreakStrand(nt1->pair_->prev_, nt1->pair_);
//      if (nt2->pair_ != nullptr && nt2->pair_->next_ != nullptr) secondChainsPairs = BreakStrand(nt2->pair_, nt2->pair_->next_);
//      UpdatePart(part);
//
//      auto chain = MergeStrands(secondChainsPairs.first, newSSPair);
//      auto finalChain = MergeStrands(chain, firstChainsPairs.second);
//    }
//  }
//  
//  part->SetNtSegmentMap();
//  UpdatePart(part);  
//
//  newDS->start_ = firstBs;
//  newDS->size_ = num_baseSegments;
//
//  return std::make_pair(newDS, part);
//}
//
//std::pair<ANTDoubleStrand*, ANTDoubleStrand*> ANTNanorobot::ConnectWithXO(ANTNucleotide* firstSelectedNucleotide, ANTNucleotide* secondSelectedNucleotide, int numBaseSegments) {
//  std::pair<ANTDoubleStrand*, ANTDoubleStrand*> resultDss = std::make_pair(nullptr, nullptr);
//
//  // we only need tw
//  if (firstSelectedNucleotide == nullptr || secondSelectedNucleotide == nullptr) return resultDss;
//  
//  ANTBaseSegment* firstBs = firstSelectedNucleotide->bs_;
//  ANTBaseSegment* secondBs = secondSelectedNucleotide->bs_;
//
//  auto firstConn = ConnectBaseSegments(firstBs, secondBs, numBaseSegments, 0, std::string(), 
//    firstSelectedNucleotide, secondSelectedNucleotide, 2);
//  ANTDoubleStrand* dsFirst = firstConn.first;
//
//  // advances 6bp (including ntPastFirstConn) to set up next double strand
//  ANTBaseSegment* firstBsSC = firstBs->next_;
//  ANTBaseSegment* secondBsSC = secondBs->previous_;
//  for (int i = 0; i < 4; ++i) {
//    ANTNucleotide* ntDelFS = firstBsSC->GetNucleotide();
//    ANTNucleotide* ntDelSS = secondBsSC->GetNucleotide();
//    ANTNucleotide* ntDelFSPair = ntDelFS->pair_;
//    ANTNucleotide* ntDelSSPair = ntDelSS->pair_;
//
//    DeleteNucleotide(ntDelFS);
//    DeleteNucleotide(ntDelFSPair);
//    DeleteNucleotide(ntDelSS);
//    DeleteNucleotide(ntDelSSPair);
//
//    firstBsSC = firstBsSC->next_;
//    // check directionality of the second ds
//    SBVector3 dir2 = (secondBs->doubleStrand_->start_->GetSBPosition() - secondBs->doubleStrand_->GetLastBaseSegment()->GetSBPosition()).normalizedVersion();
//    SBVector3 dir1 = (firstBs->doubleStrand_->start_->GetSBPosition() - firstBs->doubleStrand_->GetLastBaseSegment()->GetSBPosition()).normalizedVersion();
//    SBVector3 res = dir2*dir1;
//    if (res[0] + res[1] + res[2] > SBQuantity::dimensionless(0.0)) {
//      secondBsSC = secondBsSC->next_;
//    }
//    else {
//      secondBsSC = secondBsSC->previous_;
//    }
//  }
//  // store nts for later
//  ANTNucleotide* ntFirstSecondConn = firstBsSC->GetNucleotide();
//  ANTNucleotide* ntSecondSecondConn = secondBsSC->GetNucleotide();
//  auto secondConn = ConnectBaseSegments(firstBsSC, secondBsSC, numBaseSegments, 0, std::string(),
//    ntFirstSecondConn, ntSecondSecondConn, 1);
//  ANTDoubleStrand* dsSecond = secondConn.first;
//
//  // Delete last nucleotides
//  ANTNucleotide* ntDelFS = firstBsSC->GetNucleotide();
//  ANTNucleotide* ntDelSS = secondBsSC->GetNucleotide();
//  ANTNucleotide* ntDelFSPair = ntDelFS->pair_;
//  ANTNucleotide* ntDelSSPair = ntDelSS->pair_;
//
//  DeleteNucleotide(ntDelFS);
//  DeleteNucleotide(ntDelFSPair);
//  DeleteNucleotide(ntDelSS);
//  DeleteNucleotide(ntDelSSPair);
//
//  resultDss = std::make_pair(dsFirst, dsSecond);
//  //resultDss = std::make_pair(dsFirst, nullptr);
//
//  //int posXO = 0;
//  //// Create crossover
//  //if (numBaseSegments < 12) {
//  //  return resultDss;  // no space for crossover
//  //}
//  //else {
//  //  // place crossover slightly centered
//  //  double pos = 10.5;  // crossover has to be at a multiple of 10.5
//  //  int numWholeTurns = floor(numBaseSegments / 10.5);
//  //  posXO = floor(numWholeTurns * 10.5 * 0.5);
//  //}
//
//  //ANTBaseSegment* fBsXO = dsFirst->start_;
//  //ANTBaseSegment* sBsXO = dsSecond->GetLastBaseSegment();
//  //// advance the base segments to the XO position
//  //for (int i = 0; i < posXO - 1; ++i) {
//  //  fBsXO = fBsXO->next_;
//  //  sBsXO = sBsXO->previous_;
//  //}
//  //ANTNucleotide* fLeft = nullptr;
//  //ANTNucleotide* sLeft = nullptr;
//  //ANTCell* fCell = fBsXO->GetCell();
//  //if (fCell->GetType() == BasePair) {
//  //  ANTBasePair* bp = static_cast<ANTBasePair*>(fCell);
//  //  fLeft = bp->GetLeftNucleotide();
//  //}
//  //else if (fCell->GetType() == LoopPair) {
//  //  ANTLoopPair* lp = static_cast<ANTLoopPair*>(fCell);
//  //  ANTLoop* loop = lp->GetLeftLoop();
//  //  fLeft = loop->startNt_;
//  //}
//
//  //ANTCell* sCell = sBsXO->GetCell();
//  //if (sCell->GetType() == BasePair) {
//  //  ANTBasePair* bp = static_cast<ANTBasePair*>(sCell);
//  //  sLeft = bp->GetRightNucleotide();
//  //}
//  //else if (sCell->GetType() == LoopPair) {
//  //  ANTLoopPair* lp = static_cast<ANTLoopPair*>(sCell);
//  //  ANTLoop* loop = lp->GetRightLoop();
//  //  sLeft = loop->startNt_;
//  //}
//  
//  //auto fSSs = BreakStrand(fLeft, fLeft->next_);
//  //auto sSSs = BreakStrand(sLeft, sLeft->prev_);
//  /*ANTSingleStrand* firstSingleStrandDsFirst = fSSs.first;
//  ANTSingleStrand* secondSingleStrandDsFirst = fSSs.second;
//  ANTSingleStrand* firstSingleStrandDsSecond = sSSs.first;
//  ANTSingleStrand* secondSingleStrandDsSecond = sSSs.second;
//
//  MergeStrands(secondSingleStrandDsFirst, secondSingleStrandDsSecond);
//  MergeStrands(firstSingleStrandDsFirst, firstSingleStrandDsSecond);*/
//
//  //// Twist angle of the double helices so crossover is as short as possible
//  //double angleUnit = ANTConstants::BP_ROT;
//  //SBQuantity::length minDistance = (fLeft->GetSBBackboneCenter() - sLeft->GetSBBackboneCenter()).norm();
//  //double minAngle = 0.0;
//  //for (int i = 1; i < 12; ++i) {
//  //  dsFirst->initialTwistAngle_ += angleUnit;
//  //  dsSecond->initialTwistAngle_ -= angleUnit;
//  //  SBQuantity::length distance = (fLeft->GetSBBackboneCenter() - sLeft->GetSBBackboneCenter()).norm();
//  //  if (distance < minDistance) {
//  //    minAngle = i*angleUnit;
//  //  }
//  //}
//  //dsFirst->initialTwistAngle_ = minAngle;
//  //dsSecond->initialTwistAngle_ = -minAngle;
//
//  return resultDss;
//}
//
//std::pair<ANTSingleStrand*, ANTSingleStrand*> ANTNanorobot::BreakStrand(ANTNucleotide * first_nt, ANTNucleotide * second_nt) {
//  if (first_nt == nullptr || second_nt == nullptr) {
//    ANTSingleStrand* ss = nullptr;
//    if (first_nt != nullptr) {
//      ss = first_nt->strand_;
//    }
//    else {
//      ss = second_nt->strand_;
//    }
//    return std::make_pair(ss, ss);
//  }
//
//  ANTSingleStrand* ss = first_nt->strand_;
//  ANTPart* p = singleStrands_.at(ss);
//  auto strand_pair = p->BreakStrand(first_nt, second_nt);
//  singleStrands_.erase(ss);
//  if (strand_pair.first != nullptr) singleStrands_.insert(std::make_pair(strand_pair.first, p));
//  if (strand_pair.second != nullptr) singleStrands_.insert(std::make_pair(strand_pair.second, p));
//  return strand_pair;
//}
//
//std::pair<ANTSingleStrand*, ANTSingleStrand*> ANTNanorobot::DeleteNucleotide(ANTNucleotide * nt, bool createSBChain) {
//  ANTSingleStrand* ss = nt->strand_;
//  ANTPart* p = singleStrands_.at(ss);
//  auto strands = p->DeleteNucleotide(nt, createSBChain);
//  UpdatePart(p);
//  return strands;
//}
//
//void ANTNanorobot::DeleteNucleotides(std::map<ANTSingleStrand*, std::vector<ANTNucleotide*>> nucleotides) {
//  for (auto &p : nucleotides) {
//    ANTSingleStrand* ss = p.first;
//    std::vector<ANTNucleotide*> delNt = p.second;
//
//    if (delNt.size() == ss->nucleotides_.size()) {
//      // delete entire strand
//      SAMSON::beginHolding("deleting strand");
//      SBPointer<SBChain> chain = ss->chain_;
//      if (chain != nullptr) {
//        chain->getParent()->removeChild(chain());
//      }
//      DeleteStrand(ss);
//      SAMSON::endHolding();
//    }
//    else {
//      // group up in contiguous chunks
//      SBPointer<SBChain> oldChain = ss->chain_;
//      std::vector<ANTSingleStrand*> singleStrands;
//      for (auto& nt: delNt) {
//        // delete all nucleotides together
//        auto chain_ids = DeleteNucleotide(nt, false);
//        if (chain_ids.first != nullptr) singleStrands.push_back(chain_ids.first);
//        if (chain_ids.second != nullptr) singleStrands.push_back(chain_ids.second);
//      }
//      SAMSON::beginHolding("creating new strands");
//      for (auto& newSs: singleStrands) {
//        newSs->CreateSBChain(configuration_->use_atomic_details);
//      }
//      SAMSON::endHolding();
//    }
//  }
//}
//
//void ANTNanorobot::ShiftScaffold(ANTNucleotide * nt) {
//  ANTPart* part = singleStrands_.at(nt->strand_);
//  part->ShiftScaffold(nt);
//}
//
//ANTPart * ANTNanorobot::MergeParts(ANTPart * p1, ANTPart * p2) {
//  ANTPart* p = new ANTPart();
//  // remove parts from ANTNanorobot
//  parts_.erase(p1->GetId());
//  parts_.erase(p2->GetId());
//  std::vector<ANTSingleStrand*> ss_remove;
//  for (auto &sit : singleStrands_) {
//    if (sit.second == p1 || sit.second == p2) ss_remove.push_back(sit.first);
//  }
//  for (auto &sit : ss_remove) singleStrands_.erase(sit);
//  std::vector<ANTDoubleStrand*> ds_remove;
//  for (auto &dit : doubleStrands_) {
//    if (dit.second == p1 || dit.second == p2) ds_remove.push_back(dit.first);
//  }
//  for (auto &sit : ss_remove) singleStrands_.erase(sit);
//  for (auto &dit : ds_remove) doubleStrands_.erase(dit);
//
//  // merge ANTPolyhedrons
//  //ANTPolyhedron polyhedron = ANTPolyhedron();
//  //Vertices new_vertices;
//  //Edges new_edges;
//  //Faces new_faces;
//  //std::map<ANTVertex*, ANTVertex*> old_new_vertices;
//  //std::map<ANTHalfEdge*, ANTHalfEdge*> old_new_hes;
//  //std::map<ANTPolygon*, ANTPolygon*> old_new_faces;
//  //int v_id = 0;
//  //int he_id = 0;
//  //for (auto &vx : p1->GetVertices()) {
//  //  ANTVertex* v = new ANTVertex();
//  //  v->id_ = v_id;
//  //  v->position_ = vx.second->position_;
//  //  old_new_vertices.insert(std::make_pair(vx.second, v));
//  //  new_vertices.insert(std::make_pair(v_id, v));
//  //  ++v_id;
//  //}
//  //for (auto &vx : p2->GetVertices()) {
//  //  ANTVertex* v = new ANTVertex();
//  //  v->id_ = v_id;
//  //  v->position_ = vx.second->position_;
//  //  old_new_vertices.insert(std::make_pair(vx.second, v));
//  //  new_vertices.insert(std::make_pair(v_id, v));
//  //  ++v_id;
//  //}
//  //for (auto &ex : p1->GetEdges()) {
//  //  ANTEdge* new_edge = new ANTEdge();
//  //  new_edges.push_back(new_edge);
//  //  ANTHalfEdge* new_he = new ANTHalfEdge();
//  //  new_edge->halfEdge_ = new_he;
//  //  new_he->edge_ = new_edge;
//  //  new_he->id_ = he_id;
//  //  ++he_id;
//
//  //  ANTHalfEdge* he = ex->halfEdge_;
//  //  ANTVertex* old_source = he->source_;
//  //  ANTVertex* new_source = old_new_vertices.at(old_source);
//  //  he->source_ = new_source;
//  //  old_new_hes.insert(std::make_pair(ex->halfEdge_, new_he));
//
//  //  if (he->pair_ != nullptr) {
//  //    if (old_new_hes.find(he->pair_) != old_new_hes.end()) {
//  //      ANTHalfEdge* pair = old_new_hes.at(he->pair_);
//  //      new_he->pair_ = pair;
//  //      pair->pair_ = new_he;
//  //      pair->edge_ = new_edge;
//  //    }
//  //    else {
//  //      ANTVertex* old_target = he->pair_->source_;
//  //      ANTVertex* new_target = old_new_vertices.at(old_target);
//  //      ANTHalfEdge* new_pair = new ANTHalfEdge();
//  //      new_pair->pair_ = new_he;
//  //      new_he->pair_ = new_pair;
//  //      new_pair->source_ = new_target;
//  //      new_pair->id_ = he_id;
//  //      ++he_id;
//  //      old_new_hes.insert(std::make_pair(he->pair_, new_he));
//  //    }
//  //  }
//
//  //  if (he->next_ != nullptr) {
//  //    if (old_new_hes.find(he->next_) != old_new_hes.end()) {
//  //      ANTHalfEdge* next = old_new_hes.at(he->next_);
//  //      new_he->next_ = next;
//  //      next->prev_ = new_he;
//  //    }
//  //    else {
//  //      ANTVertex* old_target = he->next_->source_;
//  //      ANTVertex* new_target = old_new_vertices.at(old_target);
//  //      ANTHalfEdge* new_next = new ANTHalfEdge();
//  //      new_next->prev_ = new_he;
//  //      new_he->next_ = new_next;
//  //      new_next->source_ = new_target;
//  //      new_next->id_ = he_id;
//  //      ++he_id;
//  //      old_new_hes.insert(std::make_pair(he->next_, new_next));
//  //    }
//  //  }
//
//  //  if (he->prev_ != nullptr) {
//  //    if (old_new_hes.find(he->prev_) != old_new_hes.end()) {
//  //      ANTHalfEdge* prev = old_new_hes.at(he->prev_);
//  //      new_he->prev_ = prev;
//  //      prev->next_ = new_he;
//  //    }
//  //    else {
//  //      ANTVertex* old_target = he->prev_->source_;
//  //      ANTVertex* new_target = old_new_vertices.at(old_target);
//  //      ANTHalfEdge* new_prev = new ANTHalfEdge();
//  //      new_prev->next_ = new_he;
//  //      new_he->prev_ = new_prev;
//  //      new_prev->source_ = new_target;
//  //      new_prev->id_ = he_id;
//  //      ++he_id;
//  //      old_new_hes.insert(std::make_pair(he->prev_, new_he));
//  //    }
//  //  }
//  //}
//
//  //for (auto &ex : p2->GetEdges()) {
//  //  ANTEdge* new_edge = new ANTEdge();
//  //  new_edges.push_back(new_edge);
//  //  ANTHalfEdge* new_he = new ANTHalfEdge();
//  //  new_edge->halfEdge_ = new_he;
//  //  new_he->edge_ = new_edge;
//  //  new_he->id_ = he_id;
//  //  ++he_id;
//
//  //  ANTHalfEdge* he = ex->halfEdge_;
//  //  ANTVertex* old_source = he->source_;
//  //  ANTVertex* new_source = old_new_vertices.at(old_source);
//  //  new_he->source_ = new_source;
//  //  old_new_hes.insert(std::make_pair(he, new_he));
//
//  //  if (he->pair_ != nullptr) {
//  //    if (old_new_hes.find(he->pair_) != old_new_hes.end()) {
//  //      ANTHalfEdge* pair = old_new_hes.at(he->pair_);
//  //      new_he->pair_ = pair;
//  //      pair->pair_ = new_he;
//  //      pair->edge_ = new_edge;
//  //    }
//  //    else {
//  //      ANTVertex* old_target = he->pair_->source_;
//  //      ANTVertex* new_target = old_new_vertices.at(old_target);
//  //      ANTHalfEdge* new_pair = new ANTHalfEdge();
//  //      new_pair->pair_ = new_he;
//  //      new_he->pair_ = new_pair;
//  //      new_pair->source_ = new_target;
//  //      new_pair->id_ = he_id;
//  //      ++he_id;
//  //      old_new_hes.insert(std::make_pair(he->pair_, new_he));
//  //    }
//  //  }
//
//  //  if (he->next_ != nullptr) {
//  //    if (old_new_hes.find(he->next_) != old_new_hes.end()) {
//  //      ANTHalfEdge* next = old_new_hes.at(he->next_);
//  //      new_he->next_ = next;
//  //      next->prev_ = new_he;
//  //    }
//  //    else {
//  //      ANTVertex* old_target = he->next_->source_;
//  //      ANTVertex* new_target = old_new_vertices.at(old_target);
//  //      ANTHalfEdge* new_next = new ANTHalfEdge();
//  //      new_next->prev_ = new_he;
//  //      new_he->next_ = new_next;
//  //      new_next->source_ = new_target;
//  //      new_next->id_ = he_id;
//  //      ++he_id;
//  //      old_new_hes.insert(std::make_pair(he->next_, new_next));
//  //    }
//  //  }
//
//  //  if (he->prev_ != nullptr) {
//  //    if (old_new_hes.find(he->prev_) != old_new_hes.end()) {
//  //      ANTHalfEdge* prev = old_new_hes.at(he->prev_);
//  //      new_he->prev_ = prev;
//  //      prev->next_ = new_he;
//  //    }
//  //    else {
//  //      ANTVertex* old_target = he->prev_->source_;
//  //      ANTVertex* new_target = old_new_vertices.at(old_target);
//  //      ANTHalfEdge* new_prev = new ANTHalfEdge();
//  //      new_prev->next_ = new_he;
//  //      new_he->prev_ = new_prev;
//  //      new_prev->source_ = new_target;
//  //      new_prev->id_ = he_id;
//  //      ++he_id;
//  //      old_new_hes.insert(std::make_pair(he->prev_, new_he));
//  //    }
//  //  }
//  //}
//  //for (auto &face : p1->GetFaces()) {
//  //  ANTPolygon* new_face = new ANTPolygon();
//  //  ANTHalfEdge* he = old_new_hes.at(face->halfEdge_);
//  //  new_face->halfEdge_ = he;
//  //  he->left_ = new_face;
//  //  new_faces.push_back(new_face);
//  //  old_new_faces.insert(std::make_pair(face, new_face));
//  //}
//
//  //polyhedron.SetVertices(new_vertices, true);
//  //polyhedron.SetEdges(new_edges);
//  //polyhedron.SetFaces(new_faces);
//  //p->SetPolyhedron(polyhedron);
//
//  SBPosition3 pos = (p1->GetPosition() + p2->GetPosition()) * 0.5;
//  p->SetPosition(pos);
//
//  for (auto &jit: p1->GetSegmentJoints()) {
//    ANTJoint* j = jit;
//    j->id_ = -1;  // reset id so it gets asaigned a new one
//    p->AddJoint(j);
//  }
//  for (auto &jit : p2->GetSegmentJoints()) {
//    ANTJoint* j = jit;
//    j->id_ = -1;  // reset id so it gets asaigned a new one
//    p->AddJoint(j);
//  }
//  for (auto &bit: p1->GetBaseSegments()) {
//    ANTBaseSegment* bs = bit.second;
//    bs->id_ = -1;  // reset id so it gets asaigned a new one
//    p->AddBaseSegment(bs);
//  }
//  for (auto &bit : p2->GetBaseSegments()) {
//    ANTBaseSegment* bs = bit.second;
//    bs->id_ = -1;  // reset id so it gets asaigned a new one
//    p->AddBaseSegment(bs);
//  }
//
//  for (auto &sit : p1->GetSingleStrands()) {
//    ANTSingleStrand* ss = sit.second;
//    p->RegisterSingleStrand(ss);
//  }
//  for (auto &sit : p2->GetSingleStrands()) {
//    ANTSingleStrand* ss = sit.second;
//    p->RegisterSingleStrand(ss);
//  }
//  for (auto &dit : p1->GetDoubleStrands()) {
//    ANTDoubleStrand* ds = dit.second;
//    p->RegisterDoubleStrand(ds);
//  }
//  for (auto &dit : p2->GetDoubleStrands()) {
//    ANTDoubleStrand* ds = dit.second;
//    p->RegisterDoubleStrand(ds);
//  }
//
//  p->SetNtSegmentMap();
//  delete p1;
//  delete p2;
//  RegisterPart(p);
//  return p;
//}
//
//std::vector<ANTSingleStrand*> ANTNanorobot::GetScaffolds() {
//  std::vector<ANTSingleStrand*> scaffolds;
//
//  for (auto &p : parts_) {
//    std::vector<ANTSingleStrand *> s = p.second->GetScaffolds();
//    scaffolds.insert(scaffolds.end(), s.begin(), s.end());
//  }
//
//  return scaffolds;
//}
//
//std::vector<ANTNucleotide*> ANTNanorobot::GetNucleotides() {
//  std::vector<ANTNucleotide*> nts;
//  for (auto &s : singleStrands_) {
//    ANTSingleStrand* ss = s.first;
//    ANTNucleotideList nucleotides = ss->nucleotides_;
//    for (auto &nt : nucleotides) {
//      nts.push_back(nt.second);
//    }
//  }
//  return nts;
//}
//
//void ANTNanorobot::DeleteStrand(ANTSingleStrand * ss) {
//  // remove old strands from strands_ 
//  ANTPart* p = singleStrands_.at(ss);
//  singleStrands_.erase(ss);
//  p->DeleteSingleStrand(ss);
//}
//
//std::pair<ANTSingleStrand*, ANTSingleStrand*> ANTNanorobot::SwitchANTBaseSegmentToANTLoopPair(ANTBaseSegment* bs, 
//  std::string sequenceLoopLeft, std::string sequenceLoopRight) {
//
//  std::pair<ANTSingleStrand*, ANTSingleStrand*> ssPair = std::make_pair<ANTSingleStrand*, ANTSingleStrand*>(nullptr, nullptr);
//   
//  ANTCell* cell = bs->GetCell();
//  if (cell->GetType() != CellType::BasePair) return ssPair;
//
//  ANTBasePair* bp = static_cast<ANTBasePair*>(cell);
//  ANTNucleotide* ntLeft = bp->GetLeftNucleotide();
//  ANTNucleotide* ntRight = bp->GetRightNucleotide();
//
//  ANTLoopPair* lp = new ANTLoopPair();
//
//  if (ntLeft != nullptr) {
//    ANTLoop* left = ANTPart::AppendPolyTLoopFiveToThree(ntLeft->strand_, bs, ntLeft->prev_, sequenceLoopLeft.size());
//    left->endNt_->next_ = ntLeft->next_;
//    ntLeft->next_->prev_ = left->endNt_;
//    // change sequence
//    ANTNucleotide* nt = left->startNt_;
//    for (size_t i = 0; i < sequenceLoopLeft.size(); ++i) {
//      nt->type_ = residue_names_.right.at(sequenceLoopLeft[i]);
//      nt = nt->next_;
//      if (nt == left->endNt_->next_) break;
//    }
//    //if (ntLeft->prev_ != nullptr) ntLeft->prev_->next_ = left->startNt_;
//    lp->SetLeftLoop(left);
//    ssPair.first = left->strand_;
//
//    ntLeft->strand_->nucleotides_.erase(ntLeft->id_);
//    // before deleting we need to remove pointer content
//    ntLeft->pair_ = nullptr;
//    ntLeft->prev_ = nullptr;
//    ntLeft->next_ = nullptr;
//
//    delete ntLeft;
//  }
// 
//  if (ntRight != nullptr) {
//    ANTLoop* right = ANTPart::AppendPolyTLoopFiveToThree(ntRight->strand_, bs, ntRight->prev_, sequenceLoopRight.size());
//    right->endNt_->next_ = ntRight->next_;
//    ntRight->next_->prev_ = right->endNt_;
//    // change sequence
//    ANTNucleotide* nt = right->startNt_;
//    for (size_t i = 0; i < sequenceLoopLeft.size(); ++i) {
//      nt->type_ = residue_names_.right.at(sequenceLoopLeft[i]);
//      nt = nt->next_;
//      if (nt == right->endNt_->next_) break;
//    }
//
//    lp->SetRightLoop(right);
//    ssPair.second = right->strand_;
//
//    ntRight->strand_->nucleotides_.erase(ntRight->id_);
//    ntRight->pair_ = nullptr;
//    ntRight->prev_ = nullptr;
//    ntRight->next_ = nullptr;
//
//    delete ntRight;
//  }
//
//  bs->SetCell(lp);  
//
//  return ssPair;
//}
//
//std::vector<ANTSingleStrand*> ANTNanorobot::CreateSingleStrandPair() {
//  auto strands = GetSelectedSingleStrands();
//  bool newSs = true;
//  std::vector<ANTSingleStrand*> pairStrands;
//
//  for (auto &ss : strands) {
//    ANTSingleStrand* ssPair = nullptr;
//    ANTPart* p = GetPart(ss);
//    ANTNucleotide* nt = ss->fivePrime_;
//    do {
//      if (nt->pair_ == nullptr) {
//        if (newSs) {
//          ssPair = new ANTSingleStrand();
//          p->RegisterSingleStrand(ssPair);
//          singleStrands_.insert(std::make_pair(ssPair, p));
//          pairStrands.push_back(ssPair);
//          ssPair->chainName_ = "Chain pair " + std::to_string(ssPair->id_);
//          newSs = false;
//        }
//
//        // create new nucleotide
//        ANTNucleotide* ntPair = new ANTNucleotide();
//        ntPair->type_ = ANTModel::GetComplementaryBase(nt->type_);
//        ntPair->pair_ = nt;
//        nt->pair_ = ntPair;
//        ntPair->SetBaseSegment(nt->bs_);
//        ANTCell* cell = nt->bs_->GetCell();
//        if (cell->GetType() == CellType::BasePair) {
//          ANTBasePair* bp = static_cast<ANTBasePair*>(cell);
//          if (bp->GetLeftNucleotide() == nullptr) {
//            bp->SetLeftNucleotide(ntPair);
//          }
//          else if (bp->GetRightNucleotide() == nullptr) {
//            bp->SetRightNucleotide(ntPair);
//          }
//        }
//        ntPair->strand_ = ssPair;
//        if (ssPair->threePrime_ == nullptr) ssPair->threePrime_ = ntPair;
//        ntPair->next_ = ssPair->fivePrime_;
//        ssPair->fivePrime_ = ntPair;
//        int ntId = 0;
//        if (ssPair->nucleotides_.size() > 0) ntId = ssPair->nucleotides_.rbegin()->first + 1;
//        ntPair->id_ = ntId;
//        ssPair->nucleotides_.insert(std::make_pair(ntPair->id_, ntPair));
//      }
//      else {
//        if (ssPair != nullptr) {
//          ssPair->SetNucleotidesEnd();
//          //  // Add created chain
//          //  btta.SetNucleotidesPostions(*p, *ssPair);
//          //  SBPointer<SBChain> chain = ssPair->CreateSBChain(configuration_->use_atomic_details);
//          //  chain->create();
//          //  p->AddChainToModel(chain);
//        }
//        newSs = true;
//      }
//
//      nt = nt->next_;
//    } while (nt != ss->threePrime_->next_);
//  }
//
//  return pairStrands;
//}
//
//std::vector<std::pair<ANTNucleotide*, ANTNucleotide*>> ANTNanorobot::DetectCrossovers() {
//
//  std::vector<std::pair<ANTNucleotide*, ANTNucleotide*>> crossovers;
//  
//  auto singleStrands = GetSingleStrands();
//
//  for (auto & sid : singleStrands) {
//    ANTSingleStrand* strand = sid.second;
//    ANTNucleotide* nt = strand->fivePrime_;
//    do {
//      ANTNucleotide* nt_next = nt->next_;
//      ANTBaseSegment* bs = GetBaseSegment(nt);
//      ANTBaseSegment* bs_nt_next = GetBaseSegment(nt_next);
//      if (bs != nullptr && bs_nt_next != nullptr && !(bs_nt_next == bs->next_ || bs_nt_next == bs->previous_)) {
//        std::pair<ANTNucleotide*, ANTNucleotide*> co_pair = std::make_pair(nt, nt_next);
//        crossovers.push_back(co_pair);
//      }
//      nt = nt_next;
//    } while (nt != nullptr);
//  }
//
//  auto scaffs = GetScaffolds();
//
//  for (auto &scaff : scaffs) {
//    ANTNucleotide* nt = scaff->fivePrime_;
//    do {
//      ANTNucleotide* nt_next = nt->next_;
//      ANTBaseSegment* bs = GetBaseSegment(nt);
//      ANTBaseSegment* bs_nt_next = GetBaseSegment(nt_next);
//      if (bs != nullptr && bs_nt_next != nullptr && !(bs_nt_next == bs->next_ || bs_nt_next == bs->previous_)) {
//        std::pair<ANTNucleotide*, ANTNucleotide*> co_pair = std::make_pair(nt, nt_next);
//        crossovers.push_back(co_pair);
//      }
//      nt = nt_next;
//    } while (nt != nullptr);
//  }
//
//  return crossovers;
//}
//
//std::vector<std::pair<ANTNucleotide*, ANTNucleotide*>> ANTNanorobot::DetectPossibleCrossovers() {
//
//  std::vector<std::pair<ANTNucleotide*, ANTNucleotide*>> crossovers;
//
//  auto nucleotides = GetNucleotides();
//
//  for (auto& ntFirst : nucleotides) {
//    for (auto& ntSecond : nucleotides) {
//      if (ntFirst->strand_ == ntSecond->strand_ || ntFirst->pair_ == ntSecond) continue;
//
//      // check directionality
//      double z = ublas::inner_prod(ntFirst->e3_, ntSecond->e3_);
//      if (z >= 0.0) continue;
//
//      std::pair<ANTNucleotide*, ANTNucleotide*> co_pair = std::make_pair(ntFirst, ntSecond);
//      std::pair<ANTNucleotide*, ANTNucleotide*> co_pair_other = std::make_pair(ntSecond, ntFirst);
//
//      if (std::find(crossovers.begin(), crossovers.end(), co_pair_other) == crossovers.end()) {
//        // check if distance between nucleotides is less than threshold
//        SBQuantity::length dist = (ntFirst->GetSBPosition() - ntSecond->GetSBPosition()).norm();
//        if (dist < SBQuantity::angstrom(ANTConstants::CROSSOVER_THRESHOLD)) {
//          // check if direction of nucleotides is correct
//          double t = ublas::inner_prod(ntFirst->e1_, ntSecond->e1_);
//          if (t < 0.0 && abs(t) > cos(ANTVectorMath::DegToRad(ANTConstants::CROSSOVER_ANGLE_THRESHOLD))) {
//            crossovers.push_back(co_pair);
//          }
//        }
//      }
//    }
//  }
//
//  return crossovers;
//}
//
//void ANTNanorobot::SetScaffoldSequence(std::string seq) {
//  auto parts = GetSelectedParts();
//  for (auto &p : parts) {
//    p->SetScaffoldSequence(seq);
//  }
//  // if there are chains selected too (doesn't work since ANTPart includes de ss
//  /*auto chains = GetSelectedSingleStrands();
//  for (auto &c : chains) {
//    c->SetSequence(seq);
//  }*/
//}
//
//void ANTNanorobot::OutputToOxDNA(std::string folder, ANTAuxiliary::OxDNAOptions options) {
//  CreateOxDNAInputFile(folder, options);
//  CreateOxDNAConfAndTopoFile(folder, options);
//}
//
//void ANTNanorobot::CreateOxDNAInputFile(std::string folder, ANTAuxiliary::OxDNAOptions options) {
//
//  std::string fname = "input";
//  std::ofstream out = CreateOutputFile(fname, folder);
//
//  std::string backend = "CPU";
//  std::string backendPrecision = "double";
//  if (options.backend_ == ANTAuxiliary::CUDA) {
//    backend = "CUDA";
//    backendPrecision = "mixed";
//  }
//
//  std::string debug = "0";
//  if (options.debug_) debug = "1";
//
//  std::string interactionType = "DNA";
//  if (options.interactionType_ == ANTAuxiliary::DNA2) {
//    interactionType = "DNA2";
//  }
//  else if (options.interactionType_ == ANTAuxiliary::LJ) {
//    interactionType = "LJ";
//  }
//  else if (options.interactionType_ == ANTAuxiliary::Patchy) {
//    interactionType = "patchy";
//  }
//
//  std::string simType = "MD";
//  if (options.simType_ == ANTAuxiliary::MC) {
//    simType = "MC";
//  }
//  else if (options.simType_ == ANTAuxiliary::VMMC) {
//    simType = "VMMC";
//  }
//
//  std::string steps = std::to_string(options.steps_);
//  std::string saltConcentration = std::to_string(options.saltConcentration_);
//  std::string timeStep = std::to_string(options.mdTimeStep_);
//  std::string deltaTranslation = std::to_string(options.mcDeltaTranslation_);
//  std::string deltaRotation = std::to_string(options.mcDeltaRotation_);
//  std::string temperature = options.temp_;
//
//  out << "##############################" << std::endl;
//  out << "####  PROGRAM PARAMETERS  ####" << std::endl;
//  out << "##############################" << std::endl;
//  out << "interaction_type = " + interactionType << std::endl;
//  out << "sim_type = " + simType << std::endl;
//  out << "backend = " + backend << std::endl;
//  out << "backend_precision = " + backendPrecision << std::endl;
//
//  if (backend == "CUDA") {
//    out << "CUDA_list = verlet" << std::endl;
//    out << "CUDA_sort_every = 0" << std::endl;
//    out << "use_edge = 1" << std::endl;
//    out << "edge_n_forces = 1" << std::endl;
//  }
//
//  out << "debug = " + debug << std::endl;
//  out << std::endl;
//  out << "##############################" << std::endl;
//  out << "####    SIM PARAMETERS    ####" << std::endl;
//  out << "##############################" << std::endl;
//  out << "steps = " + steps << std::endl;
//  out << "restart_step_counter = 0" << std::endl;
//  out << "#seed = 54645" << std::endl;
//  out << "T = " + temperature << std::endl;
//  out << "#fix_diffusion = 1" << std::endl;
//  out << "verlet_skin = 0.05" << std::endl;
//  out << "#back_in_box = 0" << std::endl;
//
//  if (interactionType == "DNA2") {
//    out << "salt_concentration = " + saltConcentration << std::endl;
//  }
//
//  if (simType == "MD") {
//    out << "dt = " + timeStep << std::endl;
//    out << "thermostat = john" << std::endl;
//    out << "newtonian_steps = 103" << std::endl;
//    out << "#pt = 0.1" << std::endl;
//    out << "diff_coeff = 2.50" << std::endl;
//  }
//  else if (simType == "MC") {
//    out << "#check_energy_every=10" << std::endl;
//    out << "#check_energy_threshold=1e-4" << std::endl;
//    out << "ensemble = NVT" << std::endl;
//    out << "delta_translation = " + deltaTranslation << std::endl;
//    out << "delta_rotation = " + deltaRotation << std::endl;
//  }
//  
//  out << std::endl;
//  out << "##############################" << std::endl;
//  out << "####     INTERACTIONS     ####" << std::endl;
//  out << "##############################" << std::endl;
//  out << "#rcut = 2.0" << std::endl;
//  out << std::endl;
//  out << "##############################" << std::endl;
//  out << "####    INPUT / OUTPUT    ####" << std::endl;
//  out << "##############################" << std::endl;
//  out << "topology = topo.top" << std::endl;
//  out << "conf_file = config.conf" << std::endl;
//  out << "trajectory_file = trajectory.dat" << std::endl;
//  out << "refresh_vel = 1" << std::endl;
//  out << "#log_file = log.dat" << std::endl;
//  out << "#no_stdout_energy = 0" << std::endl;
//  out << "energy_file = energy.dat" << std::endl;
//  out << "print_conf_interval = 1000000" << std::endl;
//  out << "#print_conf_ppc = 51" << std::endl;
//  out << "print_energy_every = 1000" << std::endl;
//  out << "time_scale = linear" << std::endl;
//  out << "#equilibration_steps = 0" << std::endl;
//  out << "#timings_filename = timings_1_0.001" << std::endl;
//  out << "#print_timings = yes" << std::endl;
//
//  out.close();
//}
//
//void ANTNanorobot::CreateOxDNAConfAndTopoFile(std::string folder, ANTAuxiliary::OxDNAOptions options) {
//  std::string fnameConf = "config.conf";
//  std::ofstream outConf = CreateOutputFile(fnameConf, folder);
//  std::string fnameTopo = "topo.top";
//  std::ofstream outTopo = CreateOutputFile(fnameTopo, folder);
//
//  // config file header
//  std::string timeStep = "0";
//  std::string boxSizeX = std::to_string(options.boxSizeX_);
//  std::string boxSizeY = std::to_string(options.boxSizeY_);
//  std::string boxSizeZ = std::to_string(options.boxSizeZ_);
//  auto energies = std::tuple<std::string, std::string, std::string>("0.0", "0.0", "0.0");
//
//  outConf << "t = " + timeStep << std::endl;
//  outConf << "b = " + boxSizeX + " " + boxSizeY + " " + boxSizeZ << std::endl;
//  outConf << "E = " + std::get<0>(energies) + " " + std::get<1>(energies) + " " + std::get<2>(energies) << std::endl;
//
//  // topology file header
//  std::string numberNucleotides = std::to_string(GetNumberOfNucleotides());
//  std::string numberStrands = std::to_string(singleStrands_.size());
//
//  outTopo << numberNucleotides << " " << numberStrands << std::endl;
//
//  // config file: velocity and angular velocity are zero for all
//  std::string L = "0 0 0";
//  std::string v = "0 0 0";
//
//  // we assign new ids
//  unsigned int strandId = 1;
//  unsigned int ntId = 0;
//  for (auto &p: singleStrands_) {
//    ANTSingleStrand* ss = p.first;
//    ANTNucleotide* nt = ss->fivePrime_;
//    do {
//      // config file info
//      SBPosition3 pos = nt->GetSBPosition();
//      auto bbVector = nt->e1_;
//      auto normal = nt->e2_;
//
//      // box size is in nm, so position of nt has to be too
//      std::string positionVector = std::to_string(pos[0].getValue() / 1000.0) + " " + std::to_string(pos[1].getValue() / 1000.0) + " " + std::to_string(pos[2].getValue() / 1000.0);
//      std::string backboneBaseVector = std::to_string(bbVector[0]) + " " + std::to_string(bbVector[1]) + " " + std::to_string(bbVector[2]);
//      std::string normalVector = std::to_string(normal[0]) + " " + std::to_string(normal[1]) + " " + std::to_string(normal[2]);
//
//      outConf << positionVector + " " + backboneBaseVector + " " + normalVector + " " + v + " " + L << std::endl;
//      
//      // topology file info
//      std::string base(1, residue_names_.left.at(nt->type_));
//      std::string threePrime = "-1";
//      if (nt->prev_ != nullptr) threePrime = std::to_string(ntId - 1);
//      std::string fivePrime = "-1";
//      if (nt->next_ != nullptr) fivePrime = std::to_string(ntId + 1);
//
//      outTopo << std::to_string(strandId) + " " + base + " " + threePrime + " " + fivePrime << std::endl;
//
//      nt = nt->next_;
//      ntId++;
//    } while (nt != nullptr);
//
//    ++strandId;
//  }
//
//  outConf.close();
//  outTopo.close();
//}
//
//std::ofstream ANTNanorobot::CreateOutputFile(std::string fname, std::string folder) {
//  std::ofstream output(folder + "/" + fname);
//  return output;
//}
//
//int ANTNanorobot::GetNumberOfNucleotides() {
//  int num = 0;
//  for (auto & part : parts_) {
//    num += part.second->GetNumberOfNucleotides();
//  }
//
//  return num;
//}
//
//int ANTNanorobot::GetNumberOfAtoms() {
//  int num = 0;
//  for (auto & part : parts_) {
//    num += part.second->GetNumberOfAtoms();
//  }
//
//  return num;
//}
//
//std::vector<ANTVertex*> ANTNanorobot::GetVertices() {
//  std::vector<ANTVertex*> vertices;
//  for (auto &p : parts_) {
//    Vertices vs = p.second->GetVertices();
//    for (auto &v : vs) vertices.push_back(v.second);
//  }
//  return vertices;
//}
//
//Edges ANTNanorobot::GetEdges() {
//  Edges edges;
//  for (auto &p : parts_) {
//    Edges pe = p.second->GetEdges();
//    edges.insert(edges.end(), pe.begin(), pe.end());
//  }
//  return edges;
//}
//
//Faces ANTNanorobot::GetFaces() {
//  Faces faces;
//  for (auto &p : parts_) {
//    Faces pf = p.second->GetFaces();
//    faces.insert(faces.end(), pf.begin(), pf.end());
//  }
//  return faces;
//}
//
//int ANTNanorobot::GetNumberOfJoints(CellType celltype) {
//  return boost::numeric_cast<int>(GetSegmentJoints(celltype).size());
//}
//
//int ANTNanorobot::GetNumberOfBaseSegments(CellType celltype) {
//  return boost::numeric_cast<int>(GetBaseSegments(celltype).size());
//}
//
//ANTNanorobot & ANTNanorobot::GenerateANTNanorobot() {
//  ANTNanorobot nr = ANTNanorobot();
//
//  SBNodeIndexer smIndexer;
//  SAMSON::getActiveDocument()->getNodes(smIndexer, SBNode::IsType(SBNode::StructuralModel));
//
//  for (unsigned int j = 0; j < smIndexer.size(); ++j) {
//    // An ANTPart for every structural model
//    ANTPart* p = new ANTPart();
//    SBPointer<SBStructuralModel> smNode = static_cast<SBStructuralModel*>(smIndexer[j]);
//    p->SetName(smNode->getName());
//    SBNodeIndexer chainIndexer;
//    smNode->getNodes(chainIndexer, SBNode::IsType(SBNode::Chain));
//    std::map<SBNode*, ANTAtom*> sbAtomANTAtomMap;
//
//    for (unsigned int i=0; i < chainIndexer.size(); ++i) {
//      // create chains
//      ANTSingleStrand* ss = new ANTSingleStrand();
//      SBPointer<SBChain> chainNode = static_cast<SBChain*>(chainIndexer[i]);
//      ss->chainName_ = chainNode->getName();
//
//      SBNodeIndexer residueIndexer;
//      chainNode->getNodes(residueIndexer, SBNode::IsType(SBNode::Residue));
//
//      ANTNucleotide* prev = nullptr;
//      for (unsigned int k=0; k<residueIndexer.size(); ++k) {
//        ANTNucleotide* nt = new ANTNucleotide();
//        SBPointer<SBResidue> residueNode = static_cast<SBResidue*>(residueIndexer[k]);
//        nt->residue_ = residueNode;
//
//        SBPointer<SBBackbone> backboneNode = residueNode->getBackbone();
//        nt->backbone_ = backboneNode;
//
//        SBPointer<SBSideChain> sidechainNode = residueNode->getSideChain();
//        nt->sidechain_ = sidechainNode;
//
//        nt->strand_ = ss;
//        nt->type_ = residue_types_.right.at(residueNode->getResidueType());
//        nt->prev_ = prev;
//        if (prev != nullptr) prev->next_ = nt;
//
//        SBNodeIndexer atomIndexes;
//        residueNode->getNodes(atomIndexes, SBNode::IsType(SBNode::Atom));
//        for (unsigned int l = 0; l < atomIndexes.size(); ++l) {
//          ANTAtom* atom = nullptr;
//          SBPointer<SBAtom> atomNode = static_cast<SBAtom*>(atomIndexes[l]);
//          auto it = sbAtomANTAtomMap.find(atomIndexes[l]);
//          if (it == sbAtomANTAtomMap.end()) {
//            atom = new ANTAtom();
//            sbAtomANTAtomMap.insert(std::make_pair(atomIndexes[l], atom));
//          }
//          else {
//            atom = it->second;
//          }
//
//          atom->name_ = atom_type_.right.at(atomNode->getElementType());
//          atom->nt_ = nt;
//          atom->SetPosition(atomNode->getPosition());
//          // generate 2D and 1D positions
//          atom->sbAtom_ = atomNode;
//
//          SBNodeIndexer connectedAtoms;
//          atomNode->getConnectedComponent(connectedAtoms);
//          for (unsigned int m=0; m<connectedAtoms.size(); ++m) {
//            SBPointer<SBAtom> nAtom = static_cast<SBAtom*>(connectedAtoms[m]);
//            auto it = sbAtomANTAtomMap.find(connectedAtoms[m]);
//            if (it == sbAtomANTAtomMap.end()) {
//              // atom is not created, create and link
//              ANTAtom* at = new ANTAtom();
//              sbAtomANTAtomMap.insert(std::make_pair(connectedAtoms[m], at));
//              atom->connections_.push_back(at);
//            }
//            else {
//              atom->connections_.push_back(it->second);
//            }
//          }
//
//          nt->AddAtom(atom);
//        }
//        ss->RegisterNucleotide(nt);
//      }
//      // todo: detect nt pairs
//      // todo: generate base segments
//      ss->SetNucleotidesEnd();
//    }
//
//    p->SetNtSegmentMap();
//    // todo: set largest chain as p->SetScaffold();
//    nr.RegisterPart(p);
//  }
//  return nr;
//}
//
//std::vector<ANTBaseSegment*> ANTNanorobot::GetBaseSegments(CellType celltype) {
//  std::vector<ANTBaseSegment*> bss;
//  for (auto &p : parts_) {
//    BaseSegments pbs = p.second->GetBaseSegments(celltype);
//    for (auto &pp : pbs) bss.push_back(pp.second);
//  }
//  return bss;
//}
//
//Joints ANTNanorobot::GetSegmentJoints(CellType celltype) {
//  Joints joints;
//  for (auto &p : parts_) {
//    Joints pj = p.second->GetSegmentJoints(celltype);
//    joints.insert(joints.end(), pj.begin(), pj.end());
//  }
//  return joints;
//}
//
//std::map<ANTSingleStrand*, std::vector<ANTNucleotide*>> ANTNanorobot::GetSelectedNucleotides() {
//  std::map<ANTSingleStrand*, std::vector<ANTNucleotide*>> selectedNucleotides;
//  for (auto & it : singleStrands_) {
//    ANTSingleStrand* singleStrand = it.first;
//    ANTNucleotideList nucleotides = singleStrand->nucleotides_;
//    std::vector<ANTNucleotide*> selected;
//
//    for (auto & nit : nucleotides) {
//      ANTNucleotide* nucleotide = nit.second;
//      SBPointer<SBResidue> node = nucleotide->residue_;
//      unsigned int index = nucleotide->id_;
//
//      if (node != nullptr && node->isSelected()) {
//        selected.push_back(nucleotide);
//      }
//    }
//
//    if (selected.size() > 0) selectedNucleotides.insert(std::make_pair(singleStrand, selected));
//  }
//
//  return selectedNucleotides;
//}
//
//std::vector<ANTPart*> ANTNanorobot::GetSelectedParts() {
//  std::vector<ANTPart*> selectedParts;
//  for (auto & it : parts_) {
//    ANTPart* part = it.second;
//    SBPointer<SBStructuralModel> sm = part->GetSBModel();
//
//    if (sm != nullptr && sm->isSelected()) {
//        selectedParts.push_back(part);
//    }
//  }
//
//  return selectedParts;
//}
//
//std::vector<ANTSingleStrand*> ANTNanorobot::GetSelectedSingleStrands() {
//  std::vector<ANTSingleStrand*> selectedStrands;
//  for (auto & it : singleStrands_) {
//    ANTSingleStrand* ss = it.first;
//    SBPointer<SBChain> sm = ss->chain_;
//
//    if (sm != nullptr && sm->isSelected()) {
//      selectedStrands.push_back(ss);
//    }
//  }
//
//  return selectedStrands;
//}
//
//std::vector<ANTDoubleStrand*> ANTNanorobot::GetSelectedDoubleStrands() {
//  std::vector<ANTDoubleStrand*> selectedDoubleStrands;
//  auto selectedNucleotides = GetSelectedNucleotides();
//  for (auto & it : selectedNucleotides) {
//    auto nts = it.second;
//    for (auto &nt : nts) {
//      ANTBaseSegment* bs = GetBaseSegment(nt);
//      SBPointer<SBResidue> sm = nt->residue_;
//      ANTDoubleStrand* ds = bs->doubleStrand_;
//      if (sm != nullptr && sm->isSelected()) {
//        if (ds != nullptr && std::find(selectedDoubleStrands.begin(), selectedDoubleStrands.end(), ds) == selectedDoubleStrands.end()) 
//          selectedDoubleStrands.push_back(ds);
//      }
//    }
//  }
//
//  return selectedDoubleStrands;
//}
//
//ANTBaseSegment * ANTNanorobot::GetBaseSegment(ANTNucleotide * nt) {
//  if (nt != nullptr) {
//    ANTPart* part = singleStrands_.at(nt->strand_);
//    if (part != nullptr) {
//      return part->GetBaseSegment(nt);
//    }
//  }
//  return nullptr;
//}
//
//std::vector<ANTBaseSegment*> ANTNanorobot::GetSelectedBaseSegments() {
//  std::vector<ANTBaseSegment*> selectedBaseSegments;
//  auto selectedNucleotides = GetSelectedNucleotides();
//  for (auto & it : selectedNucleotides) {
//    auto nts = it.second;
//    for (auto &nt : nts) {
//      ANTBaseSegment* bs = GetBaseSegment(nt);
//      SBPointer<SBResidue> sm = nt->residue_;
//      if (sm != nullptr && sm->isSelected()) {
//        selectedBaseSegments.push_back(bs);
//      }
//    }
//  }
//
//  return selectedBaseSegments;
//}

CollectionMap<ADNSingleStrand> ADNNanorobot::GetSingleStrands(ADNPointer<ADNPart> p)
{
  return p->GetSingleStrands();
}

CollectionMap<ADNSingleStrand> ADNNanorobot::GetScaffolds(ADNPointer<ADNPart> p)
{
  return p->GetScaffolds();
}

CollectionMap<ADNDoubleStrand> ADNNanorobot::GetDoubleStrands(ADNPointer<ADNPart> p)
{
  return p->GetDoubleStrands();
}

CollectionMap<ADNNucleotide> ADNNanorobot::GetSingleStrandNucleotides(ADNPointer<ADNSingleStrand> ss)
{
  return ss->GetNucleotides();
}

ADNPointer<ADNNucleotide> ADNNanorobot::GetSingleStrandFivePrime(ADNPointer<ADNSingleStrand> ss)
{
  return ss->GetFivePrime();
}

bool ADNNanorobot::IsScaffold(ADNPointer<ADNSingleStrand> ss)
{
  return ss->IsScaffold();
}

End ADNNanorobot::GetNucleotideEnd(ADNPointer<ADNNucleotide> nt)
{
  return nt->GetEnd();
}

ADNPointer<ADNNucleotide> ADNNanorobot::GetNucleotideNext(ADNPointer<ADNNucleotide> nt)
{
  return nt->GetNext();
}

ADNPointer<ADNNucleotide> ADNNanorobot::GetNucleotidePair(ADNPointer<ADNNucleotide> nt)
{
  return nt->GetPair();
}

SBPosition3 ADNNanorobot::GetNucleotidePosition(ADNPointer<ADNNucleotide> nt)
{
  return nt->GetPosition();
}

SBPosition3 ADNNanorobot::GetNucleotideBackbonePosition(ADNPointer<ADNNucleotide> nt)
{
  return nt->GetBackbonePosition();
}

SBPosition3 ADNNanorobot::GetNucleotideSidechainPosition(ADNPointer<ADNNucleotide> nt)
{
  return nt->GetSidechainPosition();
}

void ADNNanorobot::HideCenterAtoms(ADNPointer<ADNNucleotide> nt)
{
  nt->HideCenterAtoms();
}
