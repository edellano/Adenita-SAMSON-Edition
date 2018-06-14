#pragma once

#include "ADNBasicOperations.hpp"


//ANTSingleStrand* ADNPart::MergeStrands(ANTSingleStrand * first_strand, ANTSingleStrand * second_strand, std::string sequence) {
//
//  ANTSingleStrand* ss = new ANTSingleStrand();
//  //ss->chainName_ = first_strand->chainName_ + "-" + second_strand->chainName_;
//  if (first_strand->isScaffold_ == true || second_strand->isScaffold_ == true) ss->isScaffold_ = true;
//  ss->direction_ = first_strand->direction_;
//  ss->color_ = first_strand->color_;
//
//  // don't keep old ids
//  int nt_id = 0;
//  ANTNucleotide* l_f = first_strand->threePrime_;
//  l_f->end_ = NotEnd;
//  for (auto & nit : first_strand->nucleotides_) {
//    ANTNucleotide* nt = nit.second;
//    nt->id_ = nt_id;
//    ++nt_id;
//    ss->nucleotides_.insert(std::make_pair(nt->id_, nt));
//  }
//
//  ANTNucleotide* l_s = second_strand->fivePrime_;
//  l_f->next_ = l_s;
//  l_s->prev_ = l_f;
//
//  // we need to set the 1D position of the strands
//  //SBPosition3 startNT1D = first_strand->fivePrime_->GetSBPosition1D();
//  //SBPosition3 endNT1D = l_f->GetSBPosition1D();
//  //SBPosition3 deltNT1D = (endNT1D - startNT1D) / first_strand->nucleotides_.size();
//  //SBPosition3 posNT1D = endNT1D + deltNT1D;
//
//  // merge strands using a sequence
//  if (sequence.size() > 0) {
//    // the connection creates a new double helix
//    ANTDoubleStrand* ds = new ANTDoubleStrand();
//    // to create base segments
//    ANTJoint* source = GetBaseSegment(l_f)->target_;
//    ANTBaseSegment* fBs = nullptr;
//    ANTBaseSegment* prev_bs = nullptr;
//
//    SBPosition3 startNT = l_f->GetSBPosition();
//    //SBPosition3 startNT2D = l_f->GetSBPosition2D();
//    SBPosition3 endNT = l_s->GetSBPosition();
//    //SBPosition3 endNT2D = l_s->GetSBPosition2D();
//    SBVector3 dir = (endNT - startNT).normalizedVersion();
//    //SBPosition3 deltNT = (endNT - startNT) / sequence.size();
//    SBPosition3 deltNT = SBQuantity::nanometer(ANTConstants::BP_RISE) * dir;
//    //SBPosition3 deltNT2D = (endNT2D - startNT2D) / sequence.size();
//
//    // create nucleotides
//    ANTNucleotide* prev_nt = first_strand->threePrime_;
//    SBPosition3 posNT = startNT + deltNT;
//    //SBPosition3 posNT2D = startNT2D + deltNT2D;
//    size_t len = sequence.size();
//    for (int count = 0; count < len; ++count) {
//      ANTJoint* target = new ANTJoint();
//      if (count == len - 1) {
//        target = l_s->bs_->source_;
//      }
//      else {
//        target->SetCoordinates(posNT);
//        AddJoint(target);
//      }
//      ANTBaseSegment* bs = new ANTBaseSegment(source, target);
//      target->edge_ = bs;
//      if (prev_bs != nullptr) prev_bs->next_ = bs;
//      bs->previous_ = prev_bs;
//      bs->number_ = count;
//      bs->doubleStrand_ = ds;
//      bs->direction_ = dir;
//      AddBaseSegment(bs);
//      if (count == 0) fBs = bs;
//
//      ANTNucleotide* nt = new ANTNucleotide();
//      nt->id_ = nt_id;
//      nt->type_ = residue_names_.right.at(sequence[count]);
//      nt->prev_ = prev_nt;
//      nt->SetPosition(bs->GetSBPosition());
//      nt->SetBackboneCenter(bs->GetSBPosition());
//      nt->SetSidechainCenter(bs->GetSBPosition());
//      //nt->SetPosition1D(posNT1D);
//      //nt->SetPosition2D(posNT2D);
//      posNT += deltNT;
//      //posNT2D += deltNT2D;
//      //posNT1D += deltNT1D;
//      prev_nt->next_ = nt;
//      ss->nucleotides_.insert(std::make_pair(nt->id_, nt));
//      bs->SetNucleotide(nt);
//      nt->SetBaseSegment(bs);
//      ++nt_id;
//      prev_nt = nt;
//      source = target;
//      prev_bs = bs;
//    }
//    prev_nt->next_ = l_s;
//    l_s->prev_ = prev_nt;
//
//    ds->start_ = fBs;
//    ds->size_ = boost::numeric_cast<unsigned int>(len);
//
//    RegisterDoubleStrand(ds);
//  }
//
//  for (auto & nit : second_strand->nucleotides_) {
//    l_s = nit.second;
//    l_s->id_ = nt_id;
//    ++nt_id;
//    ss->nucleotides_.insert(std::make_pair(l_s->id_, l_s));
//    //l_s->SetPosition1D(posNT1D);
//    //posNT1D += deltNT1D;
//  }
//  ss->SetNucleotidesEnd();
//
//  // remove old strands from strands_ (do not delete, since nts would be deleted too)
//  DeregisterSingleStrand(first_strand);
//  DeregisterSingleStrand(second_strand);
//  RegisterSingleStrand(ss);
//  ss->SetDefaultName();
//
//  return ss;
//}
//
//std::pair<ANTSingleStrand*, ANTSingleStrand*> ADNPart::BreakStrand(ANTNucleotide * first_nt, ANTNucleotide * second_nt, bool createSBChain) {
//
//  if (first_nt->strand_->id_ != second_nt->strand_->id_) return std::make_pair(nullptr, nullptr);
//
//  ANTSingleStrand* orig_strand = first_nt->strand_;
//
//  ANTSingleStrand* f_ss = new ANTSingleStrand();
//  f_ss->direction_ = orig_strand->direction_;
//  f_ss->color_ = orig_strand->color_;
//  ANTSingleStrand* s_ss = new ANTSingleStrand();
//  s_ss->direction_ = orig_strand->direction_;
//  s_ss->color_ = orig_strand->color_;
//
//  // don't keep old ids
//  int nt_id = 0;
//  ANTNucleotide* nt = orig_strand->fivePrime_;
//  while (true) {
//    nt->id_ = nt_id;
//    f_ss->nucleotides_.insert(std::make_pair(nt_id, nt));
//    if (nt == first_nt) break;
//    nt = nt->next_;
//    ++nt_id;
//    // they keep 1D positions
//  }
//  first_nt->next_ = nullptr;
//  f_ss->SetNucleotidesEnd();
//
//  // we need to set the 1D position of the second strands
//  SBPosition3 startNT1D = orig_strand->fivePrime_->GetSBPosition1D();
//  SBPosition3 endNT1D = orig_strand->threePrime_->GetSBPosition1D();
//  SBPosition3 deltNT1D = (endNT1D - startNT1D) / orig_strand->nucleotides_.size();
//  SBPosition3 posNT1D = startNT1D;
//
//  nt_id = 0;
//  nt = second_nt;
//  second_nt->prev_ = nullptr;
//  do {
//    nt->id_ = nt_id;
//    s_ss->nucleotides_.insert(std::make_pair(nt_id, nt));
//    // we need to assign them new 1D positions
//    nt->SetPosition1D(posNT1D);
//    posNT1D += deltNT1D;
//    nt = nt->next_;
//    ++nt_id;
//  } while (nt != nullptr);
//  s_ss->SetNucleotidesEnd();
//
//  if (orig_strand->isScaffold_) {
//    if (s_ss->nucleotides_.size() > f_ss->nucleotides_.size()) {
//      s_ss->isScaffold_ = true;
//    }
//    else {
//      f_ss->isScaffold_ = true;
//    }
//  }
//
//  // remove old strands from strands_ (do not delete, since nts would be deleted too)
//  DeregisterSingleStrand(orig_strand);
//  // insert new one
//  RegisterSingleStrand(f_ss);
//  f_ss->SetDefaultName();
//  RegisterSingleStrand(s_ss);
//  s_ss->SetDefaultName();
//
//  // create new SBChain
//  if (createSBChain) {
//    f_ss->CreateSBChain(configuration_->use_atomic_details);
//    s_ss->CreateSBChain(configuration_->use_atomic_details);
//  }
//
//  return std::make_pair(f_ss, s_ss);
//}
//
//std::pair<ANTSingleStrand*, ANTSingleStrand*> ADNPart::DeleteNucleotide(ANTNucleotide * nt, bool createSBChain) {
//  auto res = std::pair<ANTSingleStrand*, ANTSingleStrand*>(nullptr, nullptr);
//
//  ANTSingleStrand* orig_strand = nt->strand_;
//  if (nt->end_ == FivePrime || nt->end_ == ThreePrime) {
//    if (nt->next_ != nullptr) nt->next_->prev_ = nullptr;
//    if (nt->prev_ != nullptr) nt->prev_->next_ = nullptr;
//    orig_strand->nucleotides_.erase(nt->id_);
//    orig_strand->SetNucleotidesEnd();
//    if (createSBChain) orig_strand->CreateSBChain(configuration_->use_atomic_details);
//    res.first = orig_strand;
//    // in case it was last nucleotide of the strand
//    if (orig_strand->nucleotides_.size() == 0) {
//      DeregisterSingleStrand(orig_strand);
//      delete orig_strand;
//    }
//  }
//  else {
//    // strand is splitted in two
//    ANTNucleotide* f_nt = nt->prev_;
//    ANTNucleotide* s_nt = nt->next_;
//    res = BreakStrand(f_nt, s_nt, createSBChain);
//  }
//  ANTBaseSegment* bs = nt->GetBaseSegment();
//  if (bs != nullptr) {
//    bs->RemoveNucleotide(nt);
//  }
//  else {
//    std::string msg = "Orphaned nucleotide removed";
//    ANTAuxiliary::log(msg);
//  }
//
//  if (ntSegmentMap_.find(nt) != ntSegmentMap_.end()) ntSegmentMap_.erase(nt);
//  if (nt->pair_ != nullptr) nt->pair_->pair_ = nullptr;
//  delete nt;
//
//  return res;
//}
//
//void ADNPart::ShiftScaffold(ANTNucleotide * nt) {
//  if (nt->strand_->isScaffold_) nt->strand_->ShiftStart(nt);
//}
//
//void ADNPart::SetScaffoldSequence(std::string seq) {
//  auto scaffs = GetScaffolds();
//  for (auto &s : scaffs) {
//    s->SetSequence(seq);
//  }
//}
//
//ANTLoop * ADNPart::AppendPolyTLoopThreeToFive(ANTSingleStrand * chain, ANTBaseSegment * bs, ANTNucleotide * next_nt, unsigned int numPolyT) {
//  ANTLoop* loop = new ANTLoop();
//  loop->strand_ = chain;
//  int nt_id = 0;
//  if (chain->nucleotides_.size() > 0) nt_id = chain->nucleotides_.rbegin()->first + 1;
//
//  for (unsigned int k = 0; k < numPolyT; ++k) {
//    ANTNucleotide* nt = new ANTNucleotide();
//    nt->id_ = nt_id;
//    nt->strand_ = chain;
//    nt->type_ = DNABlocks::DT_;
//    nt->SetBaseSegment(bs);
//    nt->SetPosition(bs->GetSBPosition());
//    nt->SetBackboneCenter(bs->GetSBPosition());
//    nt->SetSidechainCenter(bs->GetSBPosition());
//    chain->nucleotides_.insert(std::make_pair(nt_id, nt));
//    nt->next_ = next_nt;
//    if (next_nt != nullptr) next_nt->prev_ = nt;
//    next_nt = nt;
//    ++nt_id;
//    loop->AddNucleotide(nt);
//    if (k == 0) loop->endNt_ = nt;
//    if (k == numPolyT - 1) loop->startNt_ = nt;
//  }
//  return loop;
//}
//
//ANTLoop * ADNPart::AppendPolyTLoopFiveToThree(ANTSingleStrand * chain, ANTBaseSegment * bs, ANTNucleotide * prev_nt, unsigned int numPolyT) {
//  ANTLoop* loop = new ANTLoop();
//  loop->strand_ = chain;
//  int nt_id = 0;
//  if (chain->nucleotides_.size() > 0) nt_id = chain->nucleotides_.rbegin()->first + 1;
//
//  for (unsigned int k = 0; k < numPolyT; ++k) {
//    ANTNucleotide* nt = new ANTNucleotide();
//    nt->id_ = nt_id;
//    nt->strand_ = chain;
//    nt->type_ = DNABlocks::DT_;
//    nt->SetBaseSegment(bs);
//    nt->SetPosition(bs->GetSBPosition());
//    nt->SetBackboneCenter(bs->GetSBPosition());
//    nt->SetSidechainCenter(bs->GetSBPosition());
//    chain->nucleotides_.insert(std::make_pair(nt_id, nt));
//    nt->prev_ = prev_nt;
//    if (prev_nt != nullptr) prev_nt->next_ = nt;
//    prev_nt = nt;
//    ++nt_id;
//    loop->AddNucleotide(nt);
//    if (k == 0) loop->startNt_ = nt;
//    if (k == numPolyT - 1) loop->endNt_ = nt;
//  }
//  return loop;
//}

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


