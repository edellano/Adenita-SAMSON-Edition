#pragma once

#include "ADNBasicOperations.hpp"


ADNPointer<ADNSingleStrand> ADNBasicOperations::MergeSingleStrands(ADNPointer<ADNPart> part1, ADNPointer<ADNPart> part2, 
  ADNPointer<ADNSingleStrand> first_strand, ADNPointer<ADNSingleStrand> second_strand)
{
  ADNPointer<ADNSingleStrand> ss = ADNPointer<ADNSingleStrand>(new ADNSingleStrand());
  ss->SetName("Merged Strand");
  ss->create();
  part1->RegisterSingleStrand(ss);

  auto fivePrimeF = first_strand->GetFivePrime();
  ADNPointer<ADNNucleotide> nt = fivePrimeF;
  while (nt != nullptr) {
    ADNPointer<ADNNucleotide> ntNext = nt->GetNext();
    // keep record of nt position in base segment
    auto info = GetBaseSegmentInfo(nt);

    part1->DeregisterNucleotide(nt, true, false);
    part1->RegisterNucleotideThreePrime(ss, nt);

    // nt was removed from base segment add it again
    SetBackNucleotideIntoBaseSegment(nt, info);
    nt = ntNext;
  }

  auto fivePrimeS = second_strand->GetFivePrime();
  nt = fivePrimeS;
  while (nt != nullptr) {
    ADNPointer<ADNNucleotide> ntNext = nt->GetNext();
    // keep record of nt position in base segment
    auto info = GetBaseSegmentInfo(nt);

    part2->DeregisterNucleotide(nt, true, false);
    part1->RegisterNucleotideThreePrime(ss, nt);

    // nt was removed from base segment add it again
    SetBackNucleotideIntoBaseSegment(nt, info);

    nt = ntNext;
  }

  if (first_strand->IsScaffold() || second_strand->IsScaffold()) {
    ss->IsScaffold(true);
  }

  auto firstSize = first_strand->getNumberOfNucleotides();
  auto secondSize = second_strand->getNumberOfNucleotides();
  if (firstSize > 0 || secondSize > 0) {
    std::string msg = "Possible error when merging strands inside part";
    ADNLogger::LogDebug(msg);
  }
  else {
    // deregister single strands
    part1->DeregisterSingleStrand(first_strand);
    part2->DeregisterSingleStrand(second_strand);
  }

  return ss;
}

ADNPointer<ADNDoubleStrand> ADNBasicOperations::MergeDoubleStrand(ADNPointer<ADNPart> part, ADNPointer<ADNDoubleStrand> first_strand, ADNPointer<ADNDoubleStrand> second_strand)
{
  ADNPointer<ADNDoubleStrand> ds = ADNPointer<ADNDoubleStrand>(new ADNDoubleStrand());
  ds->setName("Merged Double Strand");
  ds->create();
  part->RegisterDoubleStrand(ds);

  auto firstF = first_strand->GetFirstBaseSegment();
  ADNPointer<ADNBaseSegment> bs = firstF;
  while (bs != nullptr) {
    ADNPointer<ADNBaseSegment> nextBs = bs->GetNext();
    part->DeregisterBaseSegment(bs, true, true);
    part->RegisterBaseSegmentEnd(ds, bs);
    bs = nextBs;
  }

  auto firstS = second_strand->GetFirstBaseSegment();
  bs = firstS;
  while (bs != nullptr) {
    ADNPointer<ADNBaseSegment> nextBs = bs->GetNext();
    part->DeregisterBaseSegment(bs, true, true);
    part->RegisterBaseSegmentEnd(ds, bs);
    bs = nextBs;
  }

  auto firstSize = first_strand->GetLength();
  auto secondSize = second_strand->GetLength();
  if (firstSize > 0 || secondSize > 0) {
    std::string msg = "Possible error when merging strands inside part";
    ADNLogger::LogDebug(msg);
  }

  return ds;
}

CollectionMap<ADNNucleotide> ADNBasicOperations::AddNucleotidesThreePrime(ADNPointer<ADNPart> part, ADNPointer<ADNSingleStrand> ss, int number, SBVector3 dir)
{
  CollectionMap<ADNNucleotide> nts;

  for (int i = 0; i < number; ++i) {

    ADNPointer<ADNNucleotide> nt = ss->GetThreePrime();
    auto bs = nt->GetBaseSegment();
    auto ds = bs->GetDoubleStrand();
    bool isLeft = bs->IsLeft(nt);
    auto n = GetNextBaseSegment(nt);
    End e = n.first;
    ADNPointer<ADNBaseSegment> nextBs = n.second;
    SBPosition3 pos = nt->GetPosition() + SBQuantity::nanometer(ADNConstants::BP_RISE) * dir;

    if (nextBs == nullptr) {
      nextBs = new ADNBaseSegment(BasePair);
      nextBs->create();
      if (e == ThreePrime) {
        ds->AddBaseSegmentEnd(nextBs);
      }
      else {
        ds->AddBaseSegmentBeginning(nextBs);
      }
      nextBs->SetPosition(pos);
      nextBs->SetE3(ADNAuxiliary::SBVectorToUblasVector(dir));
    }
    ADNPointer<ADNBasePair> bp = static_cast<ADNBasePair*>(nextBs->GetCell()());

    ADNPointer<ADNNucleotide> newNt = new ADNNucleotide();
    newNt->Init();

    newNt->SetPosition(pos);
    newNt->SetBaseSegment(nextBs);
    if (isLeft) {
      bp->SetLeftNucleotide(newNt);
    }
    else {
      bp->SetRightNucleotide(newNt);
    }
    newNt->create();
    part->RegisterNucleotideThreePrime(ss, newNt);

    nts.addReferenceTarget(newNt());
  }

  return nts;
}

ADNPointer<ADNPart> ADNBasicOperations::MergeParts(ADNPointer<ADNPart> part1, ADNPointer<ADNPart> part2)
{
  ADNPointer<ADNPart> part = part1;

  auto doubleStrands = part2->GetDoubleStrands();
  SB_FOR(ADNPointer<ADNDoubleStrand> ds, doubleStrands) {
    part2->DeregisterDoubleStrand(ds);
    part->RegisterDoubleStrand(ds);
    auto bs = ds->GetFirstBaseSegment();
    while (bs != nullptr) {
      auto bsNext = bs->GetNext();
      part2->DeregisterBaseSegment(bs, false);
      part->RegisterBaseSegmentEnd(ds, bs, false);
      bs = bsNext;
    }
    auto numBs = ds->GetBaseSegments().size();
    int test = 1;
  }

  auto singleStrands = part2->GetSingleStrands();
  SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
    part2->DeregisterSingleStrand(ss);
    part->RegisterSingleStrand(ss);
    auto nt = ss->GetFivePrime();
    while (nt != nullptr) {
      auto ntNext = nt->GetNext();
      auto info = GetBaseSegmentInfo(nt);

      part2->DeregisterNucleotide(nt, false);
      // nt has de-registered from bp or loop pair
      part->RegisterNucleotideThreePrime(ss, nt, false);

      SetBackNucleotideIntoBaseSegment(nt, info);

      auto atoms = nt->GetAtoms();
      SB_FOR(ADNPointer<ADNAtom> atom, atoms) {
        part2->DeregisterAtom(atom, false);
        NucleotideGroup g = NucleotideGroup::SideChain;
        if (atom->IsInBackbone()) g = NucleotideGroup::Backbone;
        part->RegisterAtom(nt, g, atom);
      }

      nt = ntNext;
    }
  }

  return part;
}

std::pair<ADNPointer<ADNSingleStrand>, ADNPointer<ADNSingleStrand>> ADNBasicOperations::BreakSingleStrand(ADNPointer<ADNPart> part, ADNPointer<ADNNucleotide> nt)
{
  ADNPointer<ADNSingleStrand> ssFP = new ADNSingleStrand();
  ssFP->SetName("Broken Strand 1");
  ssFP->create();
  part->RegisterSingleStrand(ssFP);
  ADNPointer<ADNSingleStrand> ssTP = new ADNSingleStrand();
  ssTP->SetName("Broken Strand 2");
  ssTP->create();
  part->RegisterSingleStrand(ssTP);

  ADNPointer<ADNSingleStrand> ss = nt->GetStrand();

  auto fivePrime = ss->GetFivePrime();
  ADNPointer<ADNNucleotide> nucleo = fivePrime;

  while (nucleo != nt) {
    ADNPointer<ADNNucleotide> ntNext = nucleo->GetNext();
    auto info = GetBaseSegmentInfo(nucleo);
    part->DeregisterNucleotide(nucleo, true, false);
    part->RegisterNucleotideThreePrime(ssFP, nucleo);
    SetBackNucleotideIntoBaseSegment(nucleo, info);
    nucleo = ntNext;
  }

  while (nucleo != nullptr) {
    ADNPointer<ADNNucleotide> ntNext = nucleo->GetNext();
    auto info = GetBaseSegmentInfo(nucleo);
    part->DeregisterNucleotide(nucleo, true, false);
    part->RegisterNucleotideThreePrime(ssTP, nucleo);
    SetBackNucleotideIntoBaseSegment(nucleo, info);
    nucleo = ntNext;
  }

  if (ss->IsScaffold()) {
    // largest strand stays as scaffold
    auto sizeF = ssFP->GetNucleotides().size();
    auto sizeT = ssTP->GetNucleotides().size();
    if (sizeF > sizeT) {
      ssFP->IsScaffold(true);
    }
    else {
      ssTP->IsScaffold(true);
    }
  }

  auto sz = ss->getNumberOfNucleotides();
  if (sz > 0) {
    std::string msg = "Possible error when breaking strands inside part";
    ADNLogger::LogDebug(msg);
  }
  else {
    // Deregister old strand
    part->DeregisterSingleStrand(ss);
  }

  std::pair<ADNPointer<ADNSingleStrand>, ADNPointer<ADNSingleStrand>> ssPair = std::make_pair(ssFP, ssTP);
  return ssPair;
}

std::pair<ADNPointer<ADNDoubleStrand>, ADNPointer<ADNDoubleStrand>> ADNBasicOperations::BreakDoubleStrand(ADNPointer<ADNPart> part, ADNPointer<ADNBaseSegment> bs)
{
  ADNPointer<ADNDoubleStrand> dsFP = new ADNDoubleStrand();
  dsFP->setName("Broken Double Strand 1");
  dsFP->create();
  part->RegisterDoubleStrand(dsFP);
  ADNPointer<ADNDoubleStrand> dsTP = new ADNDoubleStrand();
  dsTP->setName("Broken Double Strand 2");
  dsTP->create();
  part->RegisterDoubleStrand(dsTP);

  ADNPointer<ADNDoubleStrand> ds = bs->GetDoubleStrand();

  auto firstBs = ds->GetFirstBaseSegment();
  ADNPointer<ADNBaseSegment> baseS = firstBs;
  int num = bs->GetNumber();  // store number for calculating angle twist later

  while (baseS != bs) {
    ADNPointer<ADNBaseSegment> bsNext = baseS->GetNext();
    part->DeregisterBaseSegment(baseS, true, false);
    part->RegisterBaseSegmentEnd(dsFP, baseS);
    baseS = bsNext;
  }

  while (baseS != nullptr) {
    ADNPointer<ADNBaseSegment> bsNext = baseS->GetNext();
    part->DeregisterBaseSegment(baseS, true, false);
    part->RegisterBaseSegmentEnd(dsTP, baseS);
    baseS = bsNext;
  }

  auto initAngle = ds->GetInitialTwistAngle();
  dsFP->SetInitialTwistAngle(initAngle);
  dsTP->SetInitialTwistAngle(initAngle + ADNConstants::BP_ROT * num);

  auto sz = ds->GetBaseSegments().size();
  if (sz > 0) {
    std::string msg = "Possible error when breaking strands inside part";
    ADNLogger::LogDebug(msg);
  }
  else {
    // Deregister old strand
    part->DeregisterDoubleStrand(ds);
  }

  std::pair<ADNPointer<ADNDoubleStrand>, ADNPointer<ADNDoubleStrand>> dsPair = std::make_pair(dsFP, dsTP);
  return dsPair;
}

std::pair<ADNPointer<ADNSingleStrand>, ADNPointer<ADNSingleStrand>> ADNBasicOperations::DeleteNucleotide(ADNPointer<ADNPart> part, ADNPointer<ADNNucleotide> nt)
{
  SEConfig& config = SEConfig::GetInstance();

  auto numNts = part->GetNumberOfNucleotides();
  auto numSS = part->GetNumberOfSingleStrands();
  
  auto ss = nt->GetStrand();
  End e = nt->GetEnd();

  std::pair<ADNPointer<ADNSingleStrand>, ADNPointer<ADNSingleStrand>> res = std::make_pair(nullptr, nullptr);

  if (e == FiveAndThreePrime || e == FivePrime) {
    // we don't need to break, just delete
    ADNPointer<ADNNucleotide> ntNext = nt->GetNext();
    if (ntNext != nullptr) {
      if (ntNext->GetEnd() == ThreePrime) {
        ntNext->SetEnd(FiveAndThreePrime);
      }
      else {
        ntNext->SetEnd(e);
      }
      ss->SetFivePrime(ntNext);
      res.first = ss;
    }
    else {
      // e should be FiveAndThreePrime
      part->DeregisterSingleStrand(ss);
    }
    part->DeregisterNucleotide(nt);
    nt.deleteReferenceTarget();
  }
  else {
    // first break
    auto ssPair = BreakSingleStrand(part, nt);
    res.first = ssPair.first;
    part->RegisterSingleStrand(res.first);  // register new strand

    if (e == ThreePrime) {
      part->DeregisterSingleStrand(ssPair.second);
    }
    else {
      // second break
      auto ssPair2 = BreakSingleStrand(part, nt->GetNext());
      res.second = ssPair2.second;
      part->RegisterSingleStrand(res.second);
      part->DeregisterSingleStrand(ssPair2.first);  // deregister strand containing only the nt we want to delete
    }

    auto bs = nt->GetBaseSegment();
    if (bs->GetCellType() == LoopPair) {
      ADNPointer<ADNLoopPair> loopPair = static_cast<ADNLoopPair*>(bs->GetCell()());
      loopPair->RemoveNucleotide(nt);
    }
    part->DeregisterNucleotide(nt);
    nt.deleteReferenceTarget();
  }

  auto numNtsNew = part->GetNumberOfNucleotides();
  auto numSSNew = part->GetNumberOfSingleStrands();

  if (config.mode == DEBUG_NO_LOG || config.mode == DEBUG_LOG) {
    std::string msg = "  --> DELETING NUCLEOTIDE";
    ADNLogger::LogDebug(msg);
    msg = "         Nucleotides before deletion: " + std::to_string(numNts) + "\n";
    msg += "         Nucleotides after deletion: " + std::to_string(numNtsNew);
    ADNLogger::LogDebug(msg);
    msg = "         Single Strands before deletion: " + std::to_string(numSS) + "\n";
    msg += "         Single Strands after deletion: " + std::to_string(numSSNew);
    ADNLogger::LogDebug(msg);
  }

  return res;
}

void ADNBasicOperations::DeleteNucleotideWithoutBreak(ADNPointer<ADNPart> part, ADNPointer<ADNNucleotide> nt)
{
  if (nt->GetEnd() != FiveAndThreePrime) {
    ADNPointer<ADNNucleotide> next = nt->GetNext();
    ADNPointer<ADNNucleotide> prev = nt->GetPrev();
    if (nt->GetEnd() == FivePrime) {
      next->SetEnd(FivePrime);
    }
    if (nt->GetEnd() == ThreePrime) {
      prev->SetEnd(ThreePrime);
    }
  }
  
  ADNPointer<ADNSingleStrand> ss = nt->GetStrand();
  ss->removeChild(nt());
}

std::pair<ADNPointer<ADNDoubleStrand>, ADNPointer<ADNDoubleStrand>> ADNBasicOperations::DeleteBaseSegment(ADNPointer<ADNPart> part, ADNPointer<ADNBaseSegment> bs)
{
  SEConfig& config = SEConfig::GetInstance();

  auto numBss = part->GetNumberOfBaseSegments();
  auto numDS = part->GetNumberOfDoubleStrands();

  std::pair<ADNPointer<ADNDoubleStrand>, ADNPointer<ADNDoubleStrand>> res = std::make_pair(nullptr, nullptr);

  ADNPointer<ADNDoubleStrand> ds = bs->GetDoubleStrand();
  ADNPointer<ADNBaseSegment> oldNext = bs->GetNext();
  bool fst = bs->GetPrev() == nullptr;
  bool fstAndLst = bs->GetNext() == nullptr && fst;

  if (fstAndLst || fst) {
    // we don't need to break, just delete
    if (!fstAndLst) {
      if (oldNext->GetNext() == nullptr) {
        ds->SetEnd(oldNext);
      }
      ds->SetStart(oldNext);
      res.first = ds;
    }
    else {
      // last base segment of the double strand
      part->DeregisterDoubleStrand(ds);
    }
    part->DeregisterBaseSegment(bs);
    bs.deleteReferenceTarget();
  }
  else {
    // first break
    auto dsPair = BreakDoubleStrand(part, bs);
    res.first = dsPair.first;
    part->RegisterDoubleStrand(res.first);  // register new strand

    if (oldNext == nullptr) {
      part->DeregisterDoubleStrand(dsPair.second);
    }
    else {
      // second break
      auto dsPair2 = BreakDoubleStrand(part, oldNext);
      res.second = dsPair2.second;
      part->RegisterDoubleStrand(res.second);
      part->DeregisterDoubleStrand(dsPair2.first);  // deregister strand containing only the nt we want to delete
    }

    part->DeregisterBaseSegment(bs);
    bs.deleteReferenceTarget();
  }

  auto numBssNew = part->GetNumberOfBaseSegments();
  auto numDSNew = part->GetNumberOfDoubleStrands();

  if (config.mode == DEBUG_NO_LOG || config.mode == DEBUG_LOG) {
    std::string msg = "  --> DELETING NUCLEOTIDE";
    ADNLogger::LogDebug(msg);
    msg = "         Nucleotides before deletion: " + std::to_string(numBss) + "\n";
    msg += "         Nucleotides after deletion: " + std::to_string(numBssNew);
    ADNLogger::LogDebug(msg);
    msg = "         Single Strands before deletion: " + std::to_string(numDS) + "\n";
    msg += "         Single Strands after deletion: " + std::to_string(numDSNew);
    ADNLogger::LogDebug(msg);
  }

  return res;
}

void ADNBasicOperations::DeleteSingleStrand(ADNPointer<ADNSingleStrand> ss)
{
  ss.deleteReferenceTarget();
}

void ADNBasicOperations::DeleteDoubleStrand(ADNPointer<ADNDoubleStrand> ds)
{
  ds.deleteReferenceTarget();
}

void ADNBasicOperations::MutateNucleotide(ADNPointer<ADNNucleotide> nt, DNABlocks newType, bool changePair)
{
  nt->SetType(newType);
  if (changePair) {
    auto ntPair = nt->GetPair();
    if (ntPair != nullptr) {
      ntPair->SetType(ADNModel::GetComplementaryBase(newType));
    }
  }
}

void ADNBasicOperations::SetSingleStrandSequence(ADNPointer<ADNSingleStrand> ss, std::string seq, bool changePair, bool overwrite)
{
  ADNPointer<ADNNucleotide> fivePrime = ss->GetFivePrime();
  ADNPointer<ADNNucleotide> nt = fivePrime;
  int count = 0;

  while (count < seq.size() && nt != nullptr) {
    DNABlocks t = ADNModel::ResidueNameToType(seq[count]);
    if (overwrite || (!overwrite && nt->GetType() == DNABlocks::DI)) MutateNucleotide(nt, t, changePair);
    ++count;
    nt = nt->GetNext();
  }
}

void ADNBasicOperations::MutateBasePairIntoLoopPair(ADNPointer<ADNBaseSegment> bs)
{
  auto cell = bs->GetCell();
  if (cell->GetType() != CellType::BasePair) return;

  ADNPointer<ADNBasePair> bp(static_cast<ADNBasePair*>(cell()));
  ADNPointer<ADNNucleotide> leftNt = bp->GetLeftNucleotide();
  ADNPointer<ADNNucleotide> rightNt = bp->GetRightNucleotide();
  // unpair
  leftNt->SetPair(nullptr);
  rightNt->SetPair(nullptr);

  ADNPointer<ADNLoopPair> lp = ADNPointer<ADNLoopPair>(new ADNLoopPair());
  ADNPointer<ADNLoop> leftLoop = ADNPointer<ADNLoop>(new ADNLoop());
  ADNPointer<ADNLoop> rightLoop = ADNPointer<ADNLoop>(new ADNLoop());
  lp->SetLeftLoop(leftLoop);
  lp->SetRightLoop(rightLoop);
  leftLoop->AddNucleotide(leftNt);
  leftLoop->SetStart(leftNt);
  leftLoop->SetEnd(leftNt);
  rightLoop->AddNucleotide(rightNt);
  rightLoop->SetStart(rightNt);
  rightLoop->SetEnd(rightNt);

  bs->SetCell(lp());
}

void ADNBasicOperations::SetStart(ADNPointer<ADNNucleotide> nt, bool resetSequence)
{
  auto ss = nt->GetStrand();
  ss->ShiftStart(nt, resetSequence);
}

void ADNBasicOperations::MoveStrand(ADNPointer<ADNPart> oldPart, ADNPointer<ADNPart> part, ADNPointer<ADNDoubleStrand> ds)
{
  oldPart->DeregisterDoubleStrand(ds);
  part->RegisterDoubleStrand(ds);

  auto bs = ds->GetFirstBaseSegment();
  std::vector<ADNPointer<ADNSingleStrand>> strands;
  while (bs != nullptr) {
    oldPart->DeregisterBaseSegment(bs);
    part->RegisterBaseSegmentEnd(ds, bs);

    auto nts = bs->GetNucleotides();
    SB_FOR(ADNPointer<ADNNucleotide> nt, nts) {
      ADNPointer<ADNSingleStrand> ss = nt->GetStrand();
      if (std::find(strands.begin(), strands.end(), ss) == strands.end()) {
        MoveStrand(oldPart, part, ss);
        strands.push_back(ss);
      }
    }
    bs = bs->GetNext();
  }
}

void ADNBasicOperations::MoveStrand(ADNPointer<ADNPart> oldPart, ADNPointer<ADNPart> part, ADNPointer<ADNSingleStrand> ss)
{
  oldPart->DeregisterSingleStrand(ss);
  part->RegisterSingleStrand(ss);

  auto nt = ss->GetFivePrime();
  while (nt != nullptr) {
    oldPart->DeregisterNucleotide(nt);
    part->RegisterNucleotideThreePrime(ss, nt);

    auto atoms = nt->GetAtoms();
    SB_FOR(ADNPointer<ADNAtom> at, atoms) {
      NucleotideGroup g = NucleotideGroup::Backbone;
      if (!at->IsInBackbone()) g = NucleotideGroup::SideChain;
      oldPart->DeregisterAtom(at);
      part->RegisterAtom(nt, g, at);
    }
    nt = nt->GetNext();
  }
}

void ADNBasicOperations::TwistDoubleHelix(ADNPointer<ADNDoubleStrand> ds, double deg)
{
  ds->SetInitialTwistAngle(deg);
}

void ADNBasicOperations::CenterPart(ADNPointer<ADNPart> part)
{
  SEConfig& config = SEConfig::GetInstance();
  SBPosition3 trans = -CalculateCenterOfMass(part);
  auto baseSegments = part->GetBaseSegments();
  SB_FOR(ADNPointer<ADNBaseSegment> bs, baseSegments) {
    bs->SetPosition(bs->GetPosition() + trans);
    auto nucleotides = bs->GetNucleotides();
    SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {
      nt->SetPosition(nt->GetPosition() + trans);
      nt->SetBackbonePosition(nt->GetBackbonePosition() + trans);
      nt->SetSidechainPosition(nt->GetSidechainPosition() + trans);
      if (config.use_atomic_details) {
        auto atoms = nt->GetAtoms();
        SB_FOR(ADNPointer<ADNAtom> a, atoms) {
          a->SetPosition(a->GetPosition() + trans);
        }
      }
    }
  }
}

SBPosition3 ADNBasicOperations::CalculateCenterOfMass(ADNPointer<ADNPart> part)
{
  auto atoms = part->GetAtoms();
  SBPosition3 cm(SBQuantity::picometer(0.0));
  SB_FOR(ADNPointer<ADNAtom> a, atoms) {
    cm += a->GetPosition();
  }
  auto sz = atoms.size();
  cm *= (1.0 / sz);
  return cm;
}

std::pair<ADNPointer<ADNNucleotide>, ADNPointer<ADNNucleotide>> ADNBasicOperations::OrderNucleotides(ADNPointer<ADNNucleotide> nt1, ADNPointer<ADNNucleotide> nt2)
{
  std::pair<ADNPointer<ADNNucleotide>, ADNPointer<ADNNucleotide>> res = std::make_pair(nullptr, nullptr);

  if (nt1->GetStrand() != nt2->GetStrand()) return res;

  ADNPointer<ADNSingleStrand> ss = nt1->GetStrand();
  ADNPointer<ADNNucleotide> nt = ss->GetFivePrime();

  std::vector<ADNPointer<ADNNucleotide>> list;
  while (nt != nullptr) {
    if (nt == nt1 || nt == nt2) {
      list.push_back(nt);
    }

    nt = nt->GetNext();
  }

  if (list.size() == 1 && nt1 == nt2) {
    // loop has only one nucleotide
    res.first = list[0];
    res.second = list[0];
  }
  else if (list.size() == 2) {
    res.first = list[0];
    res.second = list[1];
  }

  return res;
}

std::pair<End, ADNPointer<ADNBaseSegment>> ADNBasicOperations::GetNextBaseSegment(ADNPointer<ADNNucleotide> nt)
{
  ADNPointer<ADNBaseSegment> nextBs = nullptr;
  End end = NotEnd;

  auto bs = nt->GetBaseSegment();
  auto ds = bs->GetDoubleStrand();
  auto e3 = nt->GetE3();
  auto bsE3 = bs->GetE3();

  if (ublas::inner_prod(e3, bsE3) > 0) {
    nextBs = bs->GetNext(true);
    end = ThreePrime;
  }
  else {
    nextBs = bs->GetPrev(true);
    end = FivePrime;
  }

  return std::make_pair(end, nextBs);
}

std::tuple<ADNPointer<ADNBaseSegment>, bool, bool, bool> ADNBasicOperations::GetBaseSegmentInfo(ADNPointer<ADNNucleotide> nt)
{
  auto bs = nt->GetBaseSegment();
  auto cell = bs->GetCell();
  bool left = bs->IsLeft(nt);
  bool start = false;
  bool end = false;
  if (bs->GetCellType() == LoopPair) {
    ADNPointer<ADNLoopPair> lp = static_cast<ADNLoopPair*>(cell());
    if (left) {
      auto leftLoop = lp->GetLeftLoop();
      start = leftLoop->GetStart() == nt;
      end = leftLoop->GetEnd() == nt;
    }
    else {
      auto rightLoop = lp->GetRightLoop();
      start = rightLoop->GetStart() == nt;
      end = rightLoop->GetEnd() == nt;
    }
  }

  return std::make_tuple(bs, left, start, end);
}

void ADNBasicOperations::SetBackNucleotideIntoBaseSegment(ADNPointer<ADNNucleotide> nt, std::tuple<ADNPointer<ADNBaseSegment>, bool, bool, bool> info)
{
  auto bs = std::get<0>(info);
  bool left = std::get<1>(info);
  bool start = std::get<2>(info);
  bool end = std::get<3>(info);

  auto cell = bs->GetCell();
  if (bs->GetCellType() == BasePair) {
    ADNPointer<ADNBasePair> bp = static_cast<ADNBasePair*>(cell());
    if (left) bp->SetLeftNucleotide(nt);
    else bp->SetRightNucleotide(nt);
  }
  else if (bs->GetCellType() == LoopPair) {
    ADNPointer<ADNLoopPair> lp = static_cast<ADNLoopPair*>(cell());
    if (left) {
      auto leftLoop = lp->GetLeftLoop();
      leftLoop->AddNucleotide(nt);
      if (start) leftLoop->SetStart(nt);
      if (end) leftLoop->SetEnd(nt);
    }
    else {
      auto rightLoop = lp->GetRightLoop();
      rightLoop->AddNucleotide(nt);
      if (start) rightLoop->SetStart(nt);
      if (end) rightLoop->SetEnd(nt);
    }
  }
}
