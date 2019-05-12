#include "DASComplexOperations.hpp"
#include "DASBackToTheAtom.hpp"

DASOperations::Connections DASOperations::PrepareStrandsForConnection(ADNPointer<ADNPart> part1, ADNPointer<ADNPart> part2, 
  ADNPointer<ADNNucleotide> nt1, ADNPointer<ADNNucleotide> nt2)
{
  Connections conn;

  if (nt1->GetEnd() == ThreePrime && nt2->GetEnd() == ThreePrime) return conn;

  // nt1 will be left as 3' while nt2 will be left as 5'
  if (nt1->GetEnd() == FivePrime) {
    if (nt2->GetEnd() == FivePrime) return conn;
    ADNPointer<ADNNucleotide> tmp = nt1;
    nt1 = nt2;
    nt2 = tmp;
    ADNPointer<ADNPart> tmpPart = part1;
    part1 = part2;
    part2 = tmpPart;
  }

  if (nt1->IsEnd() && nt2->IsEnd()) {
    // we merge in the order they are now
    MergePair pair;
    pair.first = nt1->GetStrand();
    pair.second = nt2->GetStrand();
    pair.firstPart = part1;
    pair.secondPart = part2;
    conn.stringPair = pair;
  }
  else {
    MergePair pair;
    MergePair compPair;

    ADNPointer<ADNNucleotide> firstNext = nullptr;
    ADNPointer<ADNNucleotide> secondPrev = nullptr;

    // break first nucleotide in 3'
    if (nt1->GetEnd() != ThreePrime) {
      firstNext = nt1->GetNext(true);
      auto p = ADNBasicOperations::BreakSingleStrand(part1, firstNext);
    }

    // break second nucleotide in 5'
    if (nt2->GetEnd() != FivePrime) {
      secondPrev = nt2->GetPrev();
      auto p = ADNBasicOperations::BreakSingleStrand(part2, nt2);
    }

    pair.first = nt1->GetStrand();
    if (firstNext != nullptr) compPair.second = firstNext->GetStrand();
    pair.second = nt2->GetStrand();
    if (secondPrev != nullptr) compPair.first = secondPrev->GetStrand();

    pair.firstPart = part1;
    pair.secondPart = part2;
    compPair.firstPart = part2;
    compPair.secondPart = part1;
    conn.stringPair = pair;
    conn.compStringPair = compPair;
  }

  return conn;
}

void DASOperations::CreateCrossover(ADNPointer<ADNPart> part1, ADNPointer<ADNPart> part2, 
  ADNPointer<ADNNucleotide> nt1, ADNPointer<ADNNucleotide> nt2, bool two, std::string seq)
{
  auto conn = PrepareStrandsForConnection(part1, part2, nt1, nt2);
  auto pair = conn.stringPair;
  auto compPair = conn.compStringPair;

  // create joint strands if necessary
  ADNPointer<ADNSingleStrand> joinStrand1 = nullptr;
  ADNPointer<ADNSingleStrand> joinStrand2 = nullptr;

  if (pair.first != nullptr && pair.second != nullptr) {
    if (!seq.empty()) {
      int seqLength = boost::numeric_cast<int>(seq.size());

      ADNPointer<ADNBaseSegment> bs1 = pair.first->GetThreePrime()->GetBaseSegment();
      ADNPointer<ADNBaseSegment> bs2 = pair.second->GetFivePrime()->GetBaseSegment();
      SBVector3 direction = (bs2->GetPosition() - bs1->GetPosition()).normalizedVersion();
      SBQuantity::length availLength = (bs2->GetPosition() - bs1->GetPosition()).norm();
      SBQuantity::length expectedLength = SBQuantity::nanometer(ADNConstants::BP_RISE) * (seqLength + 1);  // we need to accomodate space for distance between the ends
      SBQuantity::length offset = (availLength - expectedLength)*0.5;
      SBPosition3 startPos = bs1->GetPosition() + (SBQuantity::nanometer(ADNConstants::BP_RISE) + offset)*direction;
      if (two) {
        auto res = DASCreator::AddDoubleStrandToADNPart(pair.firstPart, seqLength, startPos, direction);
        joinStrand1 = res.ss1;
        joinStrand2 = res.ss2;

        joinStrand1->SetSequence(seq);
        DASBackToTheAtom* btta = new DASBackToTheAtom();
        btta->SetPositionsForNewNucleotides(pair.firstPart, joinStrand1->GetNucleotides());

        // since we are modifying created parts, we need to call samson creator
        // after generating atoms
        res.ds->create();
        pair.firstPart->DeregisterDoubleStrand(res.ds);
        pair.firstPart->RegisterDoubleStrand(res.ds);

        joinStrand1->create();
        pair.firstPart->DeregisterSingleStrand(joinStrand1);
        pair.firstPart->RegisterSingleStrand(joinStrand1);
        joinStrand2->create();
        pair.firstPart->DeregisterSingleStrand(joinStrand2);
        pair.firstPart->RegisterSingleStrand(joinStrand2);
      }
      else {
        auto res = DASCreator::AddSingleStrandToADNPart(pair.firstPart, seqLength, startPos, direction);
        joinStrand1 = res.ss1;
        joinStrand1->SetSequence(seq);

        DASBackToTheAtom* btta = new DASBackToTheAtom();
        btta->SetPositionsForNewNucleotides(pair.firstPart, joinStrand1->GetNucleotides());

        res.ds->create();
        pair.firstPart->DeregisterDoubleStrand(res.ds);  // we need to register after creation
        pair.firstPart->RegisterDoubleStrand(res.ds);
        joinStrand1->create();
        pair.firstPart->DeregisterSingleStrand(joinStrand1);
        pair.firstPart->RegisterSingleStrand(joinStrand1);
      }
    }
    // connect
    if (pair.first != pair.second) {
      if (joinStrand1 == nullptr) {
        ADNBasicOperations::MergeSingleStrands(pair.firstPart, pair.secondPart, pair.first, pair.second);
      }
      else {
        auto ss = ADNBasicOperations::MergeSingleStrands(pair.firstPart, pair.firstPart, pair.first, joinStrand1);
        ADNBasicOperations::MergeSingleStrands(pair.firstPart, pair.secondPart, ss, pair.second);
      }
    }
    else {
      if (joinStrand1 != nullptr) {
        auto ss = ADNBasicOperations::MergeSingleStrands(pair.firstPart, pair.firstPart, pair.first, joinStrand1);
        ss->IsCircular(true);
      }
      else {
        pair.first->IsCircular(true);
      }
    }
  }

  if (two) {
    if (compPair.first != nullptr && compPair.second != nullptr) {
      // connect
      if (compPair.first != compPair.second) {
        if (joinStrand2 == nullptr) {
          ADNBasicOperations::MergeSingleStrands(compPair.firstPart, compPair.secondPart, compPair.first, compPair.second);
        }
        else {
          auto ss = ADNBasicOperations::MergeSingleStrands(compPair.firstPart, pair.firstPart, compPair.first, joinStrand2);
          ADNBasicOperations::MergeSingleStrands(compPair.firstPart, compPair.secondPart, ss, compPair.second);
        }
      }
      else {
        if (joinStrand2 != nullptr) {
          auto ss = ADNBasicOperations::MergeSingleStrands(compPair.firstPart, pair.firstPart, compPair.first, joinStrand1);
          ss->IsCircular(true);
        }
        else {
          compPair.first->IsCircular(true);
        }
      }
    }
  }
}

void DASOperations::AddComplementaryStrands(ADNNanorobot* nanorobot, CollectionMap<ADNNucleotide> selectedNucleotides)
{
  DASBackToTheAtom btta = DASBackToTheAtom();

  ADNPointer<ADNPart> prevPart = nullptr;
  ADNPointer<ADNSingleStrand> ss = nullptr;
  int i = 1;
  CollectionMap<ADNNucleotide> nucleotides;
  bool createSs = true;

  SB_FOR(ADNPointer<ADNNucleotide> nt, selectedNucleotides) {
    auto bs = nt->GetBaseSegment();
    ADNPointer<ADNPart> part = nanorobot->GetPart(bs->GetDoubleStrand());
    auto next = nt->GetNext(true);

    if (nt->GetPair() == nullptr) {
      if (prevPart != part) {
        if (ss != nullptr) {
          btta.SetPositionsForNewNucleotides(prevPart, nucleotides);
          ss->create();
          prevPart->DeregisterSingleStrand(ss);
          prevPart->RegisterSingleStrand(ss);
          nucleotides.clear();
        }
        createSs = true;
      }

      if (createSs) {
        // create new strand if we change part
        ss = new ADNSingleStrand();
        ss->SetName("Paired Strand " + std::to_string(i));
        ++i;
        part->RegisterSingleStrand(ss);
        createSs = false;
      }

      ADNPointer<ADNNucleotide> pair = new ADNNucleotide();
      pair->Init();
      pair->SetType(ADNModel::GetComplementaryBase(nt->GetType()));
      nucleotides.addReferenceTarget(pair());

      auto cellType = bs->GetCellType();
      if (cellType == BasePair) {
        ADNPointer<ADNBasePair> bp = static_cast<ADNBasePair*>(bs->GetCell()());
        if (bp->IsLeft(nt)) {
          bp->SetRightNucleotide(pair);
        }
        else {
          bp->SetLeftNucleotide(pair);
        }
        bp->PairNucleotides();
      }
      pair->SetBaseSegment(bs);

      part->RegisterNucleotideFivePrime(ss, pair);

      if (!selectedNucleotides.hasIndex(next())) {
        if (ss != nullptr) {
          btta.SetPositionsForNewNucleotides(part, nucleotides);
          ss->create();
          part->DeregisterSingleStrand(ss);
          part->RegisterSingleStrand(ss);
          nucleotides.clear();
        }
        createSs = true;
      }
    }
    prevPart = part;
  }
}
