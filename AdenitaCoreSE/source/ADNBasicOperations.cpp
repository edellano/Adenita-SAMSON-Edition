#pragma once

#include "ADNBasicOperations.hpp"


ADNPointer<ADNSingleStrand> ADNBasicOperations::MergeSingleStrands(ADNPointer<ADNSingleStrand> first_strand, ADNPointer<ADNSingleStrand> second_strand)
{
  ADNPointer<ADNSingleStrand> ss = ADNPointer<ADNSingleStrand>(new ADNSingleStrand());

  auto fivePrimeF = first_strand->GetFivePrime();
  ADNPointer<ADNNucleotide> nt = fivePrimeF;
  while (nt != nullptr) {
    ADNPointer<ADNNucleotide> ntNext = nt->GetNext();
    ss->AddNucleotideThreePrime(nt);
    nt = ntNext;
  } 

  auto fivePrimeS = second_strand->GetFivePrime();
  nt = fivePrimeS;
  while (nt != nullptr) {
    ADNPointer<ADNNucleotide> ntNext = nt->GetNext();
    ss->AddNucleotideThreePrime(nt);
    nt = ntNext;
  }

  if (first_strand->IsScaffold() || second_strand->IsScaffold()) {
    ss->IsScaffold(true);
  }

  return ss;
}

ADNPointer<ADNSingleStrand> ADNBasicOperations::MergeSingleStrands(ADNPointer<ADNPart> part, ADNPointer<ADNSingleStrand> first_strand, ADNPointer<ADNSingleStrand> second_strand)
{
  ADNPointer<ADNSingleStrand> ss = ADNPointer<ADNSingleStrand>(new ADNSingleStrand());

  auto fivePrimeF = first_strand->GetFivePrime();
  ADNPointer<ADNNucleotide> nt = fivePrimeF;
  while (nt != nullptr) {
    ADNPointer<ADNNucleotide> ntNext = nt->GetNext();
    part->RegisterNucleotideThreePrime(ss, nt);
    nt = ntNext;
  }

  auto fivePrimeS = second_strand->GetFivePrime();
  nt = fivePrimeS;
  while (nt != nullptr) {
    ADNPointer<ADNNucleotide> ntNext = nt->GetNext();
    part->RegisterNucleotideThreePrime(ss, nt);
    nt = ntNext;
  }

  if (first_strand->IsScaffold() || second_strand->IsScaffold()) {
    ss->IsScaffold(true);
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

std::pair<ADNPointer<ADNSingleStrand>, ADNPointer<ADNSingleStrand>> ADNBasicOperations::BreakSingleStrand(ADNPointer<ADNNucleotide> nt)
{
  ADNPointer<ADNSingleStrand> ssFP = ADNPointer<ADNSingleStrand>(new ADNSingleStrand());
  ADNPointer<ADNSingleStrand> ssTP = ADNPointer<ADNSingleStrand>(new ADNSingleStrand());

  ADNPointer<ADNSingleStrand> ss = nt->GetStrand();

  auto fivePrime = ss->GetFivePrime();
  ADNPointer<ADNNucleotide> nucleo = fivePrime;

  while (nucleo != nt) {
    ADNPointer<ADNNucleotide> ntNext = nucleo->GetNext();
    ssFP->AddNucleotideThreePrime(nucleo);
    nucleo = ntNext;
  }

  while (nucleo != nullptr) {
    ADNPointer<ADNNucleotide> ntNext = nucleo->GetNext();
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

std::pair<ADNPointer<ADNSingleStrand>, ADNPointer<ADNSingleStrand>> ADNBasicOperations::DeleteNucleotide(ADNPointer<ADNNucleotide> nt)
{
  std::pair<ADNPointer<ADNSingleStrand>, ADNPointer<ADNSingleStrand>> res = std::make_pair(nullptr, nullptr);
  // first break
  auto ssPair = BreakSingleStrand(nt);
  res.first = ssPair.first;
  if (nt->GetEnd() == ThreePrime) {
    ssPair.second.deleteReferenceTarget();
  }
  else {
    // second break
    auto ssPair2 = BreakSingleStrand(nt->GetNext());
    res.second = ssPair2.second;

    // delete remaining single strand containing nt
    ssPair2.first.deleteReferenceTarget();
  }

  return res;
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

  while (nt != nullptr) {
    DNABlocks t = ADNModel::ResidueNameToType(seq[count]);
    MutateNucleotide(nt, t, changePair);
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


