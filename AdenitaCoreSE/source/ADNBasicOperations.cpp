#pragma once

#include "ADNBasicOperations.hpp"


ADNPointer<ADNSingleStrand> ADNBasicOperations::MergeSingleStrands(ADNPointer<ADNPart> part, ADNPointer<ADNSingleStrand> first_strand, ADNPointer<ADNSingleStrand> second_strand)
{
  ADNPointer<ADNSingleStrand> ss = ADNPointer<ADNSingleStrand>(new ADNSingleStrand());
  ss->SetName("Merged Strand");
  ss->create();
  part->RegisterSingleStrand(ss);

  auto fivePrimeF = first_strand->GetFivePrime();
  ADNPointer<ADNNucleotide> nt = fivePrimeF;
  while (nt != nullptr) {
    ADNPointer<ADNNucleotide> ntNext = nt->GetNext();
    first_strand->removeChild(nt());
    part->RegisterNucleotideThreePrime(ss, nt);
    nt = ntNext;
  }

  auto fivePrimeS = second_strand->GetFivePrime();
  nt = fivePrimeS;
  while (nt != nullptr) {
    ADNPointer<ADNNucleotide> ntNext = nt->GetNext();
    second_strand->removeChild(nt());
    part->RegisterNucleotideThreePrime(ss, nt);
    nt = ntNext;
  }

  if (first_strand->IsScaffold() || second_strand->IsScaffold()) {
    ss->IsScaffold(true);
  }

  auto firstSize = first_strand->getNumberOfNucleotides();
  auto secondSize = second_strand->getNumberOfNucleotides();
  if (firstSize > 0 || secondSize > 0) {
    std::string msg = "Possible error when merging strands inside part";
    ADNLogger& logger = ADNLogger::GetLogger();
    logger.LogDebug(msg);
  }
  return ss;
}

ADNPointer<ADNDoubleStrand> ADNBasicOperations::MergeDoubleStrand(ADNPointer<ADNDoubleStrand> first_strand, ADNPointer<ADNDoubleStrand> second_strand)
{
  ADNPointer<ADNDoubleStrand> ds = ADNPointer<ADNDoubleStrand>(new ADNDoubleStrand());

  auto firstF = first_strand->GetFirstBaseSegment();
  ADNPointer<ADNBaseSegment> bs = firstF;
  while (bs != nullptr) {
    ADNPointer<ADNBaseSegment> nextBs = bs->GetNext();
    ds->AddBaseSegmentEnd(bs);
    bs = nextBs;
  }

  auto firstS = second_strand->GetFirstBaseSegment();
  bs = firstS;
  while (bs != nullptr) {
    ADNPointer<ADNBaseSegment> nextBs = bs->GetNext();
    ds->AddBaseSegmentEnd(bs);
    bs = nextBs;
  }

  return ds;
}

ADNPointer<ADNDoubleStrand> ADNBasicOperations::MergeDoubleStrand(ADNPointer<ADNPart> part, ADNPointer<ADNDoubleStrand> first_strand, ADNPointer<ADNDoubleStrand> second_strand)
{
  ADNPointer<ADNDoubleStrand> ds = ADNPointer<ADNDoubleStrand>(new ADNDoubleStrand());

  auto firstF = first_strand->GetFirstBaseSegment();
  ADNPointer<ADNBaseSegment> bs = firstF;
  while (bs != nullptr) {
    ADNPointer<ADNBaseSegment> nextBs = bs->GetNext();
    part->RegisterBaseSegmentEnd(ds, bs);
    bs = nextBs;
  }

  auto firstS = second_strand->GetFirstBaseSegment();
  bs = firstS;
  while (bs != nullptr) {
    ADNPointer<ADNBaseSegment> nextBs = bs->GetNext();
    part->RegisterBaseSegmentEnd(ds, bs);
    bs = nextBs;
  }

  return ds;
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
      part2->DeregisterBaseSegment(bs);
      part->RegisterBaseSegmentEnd(ds, bs);
      bs = bs->GetNext();
    }
  }

  auto singleStrands = part2->GetSingleStrands();
  SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
    part2->DeregisterSingleStrand(ss);
    part->RegisterSingleStrand(ss);
    auto nt = ss->GetFivePrime();
    while (nt != nullptr) {
      part2->DeregisterNucleotide(nt);
      part->RegisterNucleotideThreePrime(ss, nt);

      auto atoms = nt->GetAtoms();
      SB_FOR(ADNPointer<ADNAtom> atom, atoms) {
        part2->DeregisterAtom(atom);
        NucleotideGroup g = NucleotideGroup::SideChain;
        if (atom->IsInBackbone()) g = NucleotideGroup::Backbone;
        part->RegisterAtom(nt, g, atom);
      }

      nt = nt->GetNext();
    }
  }

  return part;
}

std::pair<ADNPointer<ADNSingleStrand>, ADNPointer<ADNSingleStrand>> ADNBasicOperations::BreakSingleStrand(ADNPointer<ADNPart> part, ADNPointer<ADNNucleotide> nt)
{
  ADNPointer<ADNSingleStrand> ssFP = ADNPointer<ADNSingleStrand>(new ADNSingleStrand());
  ssFP->SetName("Broken Strand 1");
  ssFP->create();
  part->RegisterSingleStrand(ssFP);
  ADNPointer<ADNSingleStrand> ssTP = ADNPointer<ADNSingleStrand>(new ADNSingleStrand());
  ssTP->SetName("Broken Strand 2");
  ssTP->create();
  part->RegisterSingleStrand(ssTP);

  ADNPointer<ADNSingleStrand> ss = nt->GetStrand();

  auto fivePrime = ss->GetFivePrime();
  ADNPointer<ADNNucleotide> nucleo = fivePrime;

  while (nucleo != nt) {
    ADNPointer<ADNNucleotide> ntNext = nucleo->GetNext();
    ss->removeChild(nucleo());
    ssFP->AddNucleotideThreePrime(nucleo);
    nucleo = ntNext;
  }

  while (nucleo != nullptr) {
    ADNPointer<ADNNucleotide> ntNext = nucleo->GetNext();
    ss->removeChild(nucleo());
    ssTP->AddNucleotideThreePrime(nucleo);
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
    ADNLogger& logger = ADNLogger::GetLogger();
    logger.LogDebug(msg);
  }

  std::pair<ADNPointer<ADNSingleStrand>, ADNPointer<ADNSingleStrand>> ssPair = std::make_pair(ssFP, ssTP);
  return ssPair;
}

std::pair<ADNPointer<ADNDoubleStrand>, ADNPointer<ADNDoubleStrand>> ADNBasicOperations::BreakDoubleStrand(ADNPointer<ADNBaseSegment> bs)
{
  ADNPointer<ADNDoubleStrand> dsFP = ADNPointer<ADNDoubleStrand>(new ADNDoubleStrand());
  ADNPointer<ADNDoubleStrand> dsTP = ADNPointer<ADNDoubleStrand>(new ADNDoubleStrand());

  ADNPointer<ADNDoubleStrand> ds = bs->GetDoubleStrand();

  auto firstBs = ds->GetFirstBaseSegment();
  ADNPointer<ADNBaseSegment> baseS = firstBs;
  int num = bs->GetNumber();  // store number for calculating angle twist later

  while (baseS != bs) {
    ADNPointer<ADNBaseSegment> bsNext = baseS->GetNext();
    dsFP->AddBaseSegmentEnd(baseS);
    baseS = bsNext;
  }

  while (baseS != nullptr) {
    ADNPointer<ADNBaseSegment> bsNext = baseS->GetNext();
    dsTP->AddBaseSegmentEnd(baseS);
    baseS = bsNext;
  }

  auto initAngle = ds->GetInitialTwistAngle();
  dsFP->SetInitialTwistAngle(initAngle);
  dsTP->SetInitialTwistAngle(initAngle + ADNConstants::BP_ROT * num);

  std::pair<ADNPointer<ADNDoubleStrand>, ADNPointer<ADNDoubleStrand>> dsPair = std::make_pair(dsFP, dsTP);
  return dsPair;
}

std::pair<ADNPointer<ADNSingleStrand>, ADNPointer<ADNSingleStrand>> ADNBasicOperations::DeleteNucleotide(ADNPointer<ADNPart> part, ADNPointer<ADNNucleotide> nt)
{
  SEConfig& config = SEConfig::GetInstance();
  ADNLogger& logger = ADNLogger::GetLogger();

  auto numNts = part->GetNumberOfNucleotides();
  auto numSS = part->GetNumberOfSingleStrands();
  
  auto ss = nt->GetStrand();
  End e = nt->GetEnd();

  std::pair<ADNPointer<ADNSingleStrand>, ADNPointer<ADNSingleStrand>> res = std::make_pair(nullptr, nullptr);

  if (e == FiveAndThreePrime || e == FivePrime) {
    // we don't need to break, just delete
    ADNPointer<ADNNucleotide> ntNext = nt->GetNext();
    if (ntNext != nullptr) {
      ntNext->SetEnd(e);
      ss->SetFivePrime(ntNext);
    }
    part->DeregisterNucleotide(nt);
    res.first = ss;
  }
  else {
    // first break
    auto ssPair = BreakSingleStrand(part, nt);
    res.first = ssPair.first;
    part->RegisterSingleStrand(res.first);  // register new strand
    part->DeregisterSingleStrand(ss);  // deregister old one

    if (e == ThreePrime) {
      part->DeregisterSingleStrand(ssPair.second);
    }
    else {
      // second break
      auto ssPair2 = BreakSingleStrand(part, nt->GetNext());
      res.second = ssPair2.second;
      part->RegisterSingleStrand(res.second);
      part->DeregisterSingleStrand(ssPair.second);  // deregister second strand from first break
      part->DeregisterSingleStrand(ssPair2.first);  // deregister strand containing only the nt we want to delete
    }

    auto bs = nt->GetBaseSegment();
    if (bs->GetCellType() == LoopPair) {
      ADNPointer<ADNLoopPair> loopPair = static_cast<ADNLoopPair*>(bs->GetCell()());
      loopPair->RemoveNucleotide(nt);
    }
    nt.deleteReferenceTarget();
  }

  auto numNtsNew = part->GetNumberOfNucleotides();
  auto numSSNew = part->GetNumberOfSingleStrands();

  if (config.mode == DEBUG_NO_LOG || config.mode == DEBUG_LOG) {
    logger.LogDateTime();
    std::string msg = "  --> DELETING NUCLEOTIDE";
    logger.LogDebug(msg);
    msg = "         Nucleotides before deletion: " + std::to_string(numNts) + "\n";
    msg += "         Nucleotides after deletion: " + std::to_string(numNtsNew);
    logger.LogDebug(msg);
    msg = "         Single Strands before deletion: " + std::to_string(numSS) + "\n";
    msg += "         Single Strands after deletion: " + std::to_string(numSSNew);
    logger.LogDebug(msg);
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

std::pair<ADNPointer<ADNDoubleStrand>, ADNPointer<ADNDoubleStrand>> ADNBasicOperations::DeleteBaseSegment(ADNPointer<ADNBaseSegment> bs)
{
  std::pair<ADNPointer<ADNDoubleStrand>, ADNPointer<ADNDoubleStrand>> res = std::make_pair(nullptr, nullptr);
  // first break
  auto ssPair = BreakDoubleStrand(bs);
  res.first = ssPair.first;
  if (bs->GetNext() == nullptr) {
    ssPair.second.deleteReferenceTarget();
  }
  else {
    // second break
    auto ssPair2 = BreakDoubleStrand(bs->GetNext());
    res.second = ssPair2.second;

    // delete remaining single strand containing nt
    ssPair2.first.deleteReferenceTarget();
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

void ADNBasicOperations::SetSingleStrandSequence(ADNPointer<ADNSingleStrand> ss, std::string seq, bool changePair)
{
  ADNPointer<ADNNucleotide> fivePrime = ss->GetFivePrime();
  ADNPointer<ADNNucleotide> nt = fivePrime;
  int count = 0;

  while (count < seq.size() && nt != nullptr) {
    DNABlocks t = ADNModel::ResidueNameToType(seq[count]);
    MutateNucleotide(nt, t, changePair);
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


