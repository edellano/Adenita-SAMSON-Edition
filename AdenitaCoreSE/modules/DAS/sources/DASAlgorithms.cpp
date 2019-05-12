#include "DASAlgorithms.hpp"
#include <random>

bool DASAlgorithms::CheckCrossoverBetweenNucleotides(ADNPointer<ADNNucleotide> ntFirst, ADNPointer<ADNNucleotide> ntSecond, double angle_threshold, double dist_threshold) {
  bool crssv = false;

  // check directionality of strand
  double z = ublas::inner_prod(ntFirst->GetE3(), ntSecond->GetE3());
  if (z < 0.0 && ntFirst->GetStrand() != ntSecond->GetStrand()) {
    // check distance
    SBPosition3 dif = ntSecond->GetSidechainPosition() - ntFirst->GetSidechainPosition();
    SBQuantity::length dist = dif.norm();
    if (dist < SBQuantity::angstrom(dist_threshold)) {
      // check if direction of nucleotides is correct
      double t = ublas::inner_prod(ntFirst->GetE2(), ntSecond->GetE2());
      // check that they are "in front" of each other
      ublas::vector<double> df = ADNAuxiliary::SBPositionToUblas(dif);
      double n = ublas::inner_prod(ntFirst->GetE1(), df);
      if (n > 0 && t < 0.0 && abs(t) > cos(ADNVectorMath::DegToRad(angle_threshold))) {
        crssv = true;
      }
    }
  }
        
  return crssv;
}

//std::vector<std::pair<ADNPointer<ADNNucleotide>, ADNPointer<ADNNucleotide>>> DASAlgorithms::DetectPossibleCrossovers(ADNPointer<ADNPart> nanorobot, double angle_threshold, double dist_threshold) {
//  std::vector<std::pair<ADNPointer<ADNNucleotide>, ADNPointer<ADNNucleotide>>> crossovers;
//  std::vector<ADNPointer<ADNDoubleStrand>> doneDS;
//  auto doubleStrands = nanorobot->GetDoubleStrands();
//
//  std::vector<std::string> errors;
//  // crossover are formed between distinct double strands only
//  SB_FOR(ADNPointer<ADNDoubleStrand> dsFirst, doubleStrands) {
//    ADNPointer<ADNBaseSegment> bsFirst = dsFirst->GetFirstBaseSegment();
//    // go through the base segments and check if nucleotides can form crossovers
//    unsigned int sz = dsFirst->GetSize();
//    for (unsigned int i = 0; i < sz; ++i) {
//      if (bsFirst == nullptr) {
//        std::string msg = "DetectPossibleNucleotides error. Base segment at position " + std::to_string(i) + " is nullptr.\n";
//        msg += "  --> doubleStrand id " + std::to_string(dsFirst->id_) + "(size " + std::to_string(sz) + ")";
//        errors.push_back(msg);
//        break;
//      }
//
//      ADNPointer<ADNNucleotide> ntFirst = bsFirst->GetNucleotide();
//      ADNPointer<ADNNucleotide> ntFirst_pair = ntFirst->pair_;
//      for (auto& pSecond : doubleStrands) {
//        ADNPointer<ADNDoubleStrand> dsSecond = pSecond.first;
//        ADNPointer<ADNBaseSegment> bsSecond = dsSecond->start_;
//        if (dsSecond == dsFirst || std::find(doneDS.begin(), doneDS.end(), dsSecond) != doneDS.end()) continue;
//
//        unsigned int szSecond = dsSecond->size_;
//        for (unsigned int j = 0; j < szSecond; ++j) {
//          if (bsSecond == nullptr) {
//            std::string msg = "DetectPossibleNucleotides error. Base segment at position " + std::to_string(j) + " is nullptr.\n";
//            msg += "  --> doubleStrand id " + std::to_string(dsSecond->id_) + "(size " + std::to_string(szSecond) + ")";
//            errors.push_back(msg);
//            break;
//          }
//
//          ADNPointer<ADNNucleotide> ntSecond = bsSecond->GetNucleotide();
//          ADNPointer<ADNNucleotide> ntSecond_pair = ntSecond->pair_;
//
//          // Check ntFirst vs ntSecond
//          std::pair<ADNPointer<ADNNucleotide>, ADNPointer<ADNNucleotide>> co_pair = std::make_pair(ntFirst, ntSecond);
//          std::pair<ADNPointer<ADNNucleotide>, ADNPointer<ADNNucleotide>> co_pair_other = std::make_pair(ntSecond, ntFirst);
//          if (std::find(crossovers.begin(), crossovers.end(), co_pair_other) == crossovers.end()) {
//            if (CheckCrossoverBetweenNucleotides(ntFirst, ntSecond, angle_threshold, dist_threshold)) crossovers.push_back(co_pair);
//          }
//          // Check ntFirst vs ntSecond_pair
//          if (ntSecond_pair != nullptr) {
//            co_pair = std::make_pair(ntFirst, ntSecond_pair);
//            co_pair_other = std::make_pair(ntSecond_pair, ntFirst);
//            if (std::find(crossovers.begin(), crossovers.end(), co_pair_other) == crossovers.end()) {
//              if (CheckCrossoverBetweenNucleotides(ntFirst, ntSecond_pair, angle_threshold, dist_threshold)) crossovers.push_back(co_pair);
//            }
//          }
//          // Check ntFirst_pair vs ntSecond
//          if (ntFirst_pair != nullptr) {
//            co_pair = std::make_pair(ntFirst_pair, ntSecond);
//            co_pair_other = std::make_pair(ntSecond, ntFirst_pair);
//            if (std::find(crossovers.begin(), crossovers.end(), co_pair_other) == crossovers.end()) {
//              if (CheckCrossoverBetweenNucleotides(ntFirst_pair, ntSecond, angle_threshold, dist_threshold)) crossovers.push_back(co_pair);
//            }
//          }
//          // Check ntFirst vs ntSecond_pair
//          if (ntFirst_pair != nullptr && ntSecond_pair != nullptr) {
//            co_pair = std::make_pair(ntFirst_pair, ntSecond_pair);
//            co_pair_other = std::make_pair(ntSecond_pair, ntFirst_pair);
//            if (std::find(crossovers.begin(), crossovers.end(), co_pair_other) == crossovers.end()) {
//              if (CheckCrossoverBetweenNucleotides(ntFirst_pair, ntSecond_pair, angle_threshold, dist_threshold)) crossovers.push_back(co_pair);
//            }
//          }
//          bsSecond = bsSecond->next_;
//        }
//      }
//      bsFirst = bsFirst->next_;
//      doneDS.push_back(dsFirst);
//    }
//  }
//
//  if (errors.size() < 20) {
//    for (auto &er : errors) {
//      ADNAuxiliary::log(er);
//    }
//  }
//  else {
//    std::string msg = "DetectPossibleNucleotides() ERROR. Too many interupted doubleStrands \
//                      (nullptr was reached before end of double strand) to display.";
//    ADNAuxiliary::log(msg);
//  }
//
//  return crossovers;
//}

//std::vector<DASCrossover*> DASAlgorithms::DetectPossibleCrossovers(ADNPointer<ADNDoubleStrand> dsF, ADNPointer<ADNDoubleStrand> dsS, double angle_threshold, double dist_threshold) {
//  
//  // need to fully redo this function
//
//  std::vector<DASCrossover*> crossovers;
//
//  std::vector<std::string> errors;
//
//  if (dsF == dsS || dsF == nullptr || dsS == nullptr) return crossovers;
//
//  ADNPointer<ADNDoubleStrand> dsFirst = dsF;
//  ADNPointer<ADNBaseSegment> bsFirst = dsFirst->start_;
//  // go through the base segments and check if nucleotides can form crossovers
//  unsigned int sz = dsFirst->size_;
//  for (unsigned int i = 0; i < sz; ++i) {
//    if (bsFirst == nullptr) {
//      std::string msg = "DetectPossibleNucleotides error. Base segment at position " + std::to_string(i) + " is nullptr.\n";
//      msg += "  --> doubleStrand id " + std::to_string(dsFirst->id_) + "(size " + std::to_string(sz) + ")";
//      errors.push_back(msg);
//      break;
//    }
//
//    SBPosition3 bsFirstPos = bsFirst->GetSBPosition();
//    ADNPointer<ADNNucleotide> ntFirstLeft = nullptr;
//    ADNPointer<ADNNucleotide> ntFirstRight = nullptr;
//    // check nucleotides of the base pair (so far only working for base pairs
//    ANTCell* cellFirst = bsFirst->GetCell();
//    if (cellFirst->GetType() != BasePair) continue;
//    ANTBasePair* bpFirst = static_cast<ANTBasePair*>(cellFirst);
//    ntFirstLeft = bpFirst->GetLeftNucleotide();
//    ntFirstRight = bpFirst->GetRightNucleotide();
//
//    ADNPointer<ADNDoubleStrand> dsSecond = dsS;
//    ADNPointer<ADNBaseSegment> bsSecond = dsSecond->start_;
//    unsigned int szSecond = dsSecond->size_;
//    for (unsigned int j = 0; j < szSecond; ++j) {
//      if (bsSecond == nullptr) {
//        std::string msg = "DetectPossibleNucleotides error. Base segment at position " + std::to_string(j) + " is nullptr.\n";
//        msg += "  --> doubleStrand id " + std::to_string(dsSecond->id_) + "(size " + std::to_string(szSecond) + ")";
//        errors.push_back(msg);
//        break;
//      }
//
//      SBPosition3 bsSecondPos = bsSecond->GetSBPosition();
//
//      // check that base segments are within distance threshold
//      SBQuantity::length dist = (bsFirstPos - bsSecondPos).norm();
//
//      if (dist < SBQuantity::angstrom(2*dist_threshold)) {
//        ADNPointer<ADNNucleotide> ntSecondLeft = nullptr;
//        ADNPointer<ADNNucleotide> ntSecondRight = nullptr;
//        // check nucleotides of the base pair (so far only working for base pairs
//        ANTCell* cellSecond = bsSecond->GetCell();
//        if (cellSecond->GetType() != BasePair) continue;
//        ANTBasePair* bpSecond = static_cast<ANTBasePair*>(cellSecond);
//        ntSecondLeft = bpSecond->GetLeftNucleotide();
//        ntSecondRight = bpSecond->GetRightNucleotide();
//
//        // depending on strands relative directionality we can have all four cases?
//        if (CheckCrossoverBetweenNucleotides(ntFirstLeft, ntSecondLeft, angle_threshold, dist_threshold)) {
//          DASCrossover* cs = new DASCrossover();
//          cs->firstNt_ = ntFirstLeft;
//          cs->secondNt_ = ntSecondLeft;
//
//          crossovers.push_back(cs);
//
//          //DASCrossover* csNext = new DASCrossover();
//          //csNext->firstNt = ntFirstLeft->next_;
//          //csNext->secondNt = ntSecondLeft->prev_;
//
//          //crossovers.push_back(csNext);
//        }
//        else if (CheckCrossoverBetweenNucleotides(ntFirstRight, ntSecondRight, angle_threshold, dist_threshold)) {
//          DASCrossover* cs = new DASCrossover();
//          cs->firstNt_ = ntFirstRight;
//          cs->secondNt_ = ntSecondRight;
//
//          crossovers.push_back(cs);
//
//          //DASCrossover* csNext = new DASCrossover();
//          //csNext->firstNt = ntFirstRight->next_;
//          //csNext->secondNt = ntSecondRight->prev_;
//
//          //crossovers.push_back(csNext);
//        }
//        else if (CheckCrossoverBetweenNucleotides(ntFirstRight, ntSecondLeft, angle_threshold, dist_threshold)) {
//          DASCrossover* cs = new DASCrossover();
//          cs->firstNt_ = ntFirstRight;
//          cs->secondNt_ = ntSecondLeft;
//
//          crossovers.push_back(cs);
//
//          //DASCrossover* csNext = new DASCrossover();
//          //csNext->firstNt = ntFirstRight->next_;
//          //csNext->secondNt = ntSecondLeft->prev_;
//
//          //crossovers.push_back(csNext);
//        }
//        else if (CheckCrossoverBetweenNucleotides(ntFirstLeft, ntSecondRight, angle_threshold, dist_threshold)) {
//          DASCrossover* cs = new DASCrossover();
//          cs->firstNt_ = ntFirstLeft;
//          cs->secondNt_ = ntSecondRight;
//
//          crossovers.push_back(cs);
//
//          //DASCrossover* csNext = new DASCrossover();
//          //csNext->firstNt = ntFirstLeft->next_;
//          //csNext->secondNt = ntSecondRight->prev_;
//
//          //crossovers.push_back(csNext);
//        }
//      }
//      bsSecond = bsSecond->next_;
//    }
//    bsFirst = bsFirst->next_;
//  }
//  
//  if (errors.size() < 20) {
//    for (auto &er : errors) {
//      ADNAuxiliary::log(er);
//    }
//  }
//  else {
//    std::string msg = "DetectPossibleNucleotides() ERROR. Too many interupted doubleStrands \
//                      (nullptr was reached before end of double strand) to display.";
//    ADNAuxiliary::log(msg);
//  }
//
//  return crossovers;
//}

//std::vector<std::pair<ADNPointer<ADNNucleotide>, ADNPointer<ADNNucleotide>>> DASAlgorithms::DetectCrossovers(ADNPointer<ADNNanorobot> nanorobot) {
//  std::vector<std::pair<ADNPointer<ADNNucleotide>, ADNPointer<ADNNucleotide>>> crossovers;
//
//  auto singleStrands = nanorobot.GetSingleStrands();
//
//  for (auto & sid : singleStrands) {
//    ADNPointer<ADNSingleStrand> strand = sid.second;
//    ADNPointer<ADNNucleotide> nt = strand->fivePrime_;
//    do {
//      ADNPointer<ADNNucleotide> nt_next = nt->next_;
//      ADNPointer<ADNBaseSegment> bs = nanorobot.GetBaseSegment(nt);
//      ADNPointer<ADNBaseSegment> bs_nt_next = nanorobot.GetBaseSegment(nt_next);
//      if (bs != nullptr && bs_nt_next != nullptr && !(bs_nt_next == bs->next_ || bs_nt_next == bs->previous_)) {
//        std::pair<ADNPointer<ADNNucleotide>, ADNPointer<ADNNucleotide>> co_pair = std::make_pair(nt, nt_next);
//        crossovers.push_back(co_pair);
//      }
//      nt = nt_next;
//    } while (nt != nullptr);
//  }
//
//  auto scaffs = nanorobot.GetScaffolds();
//
//  for (auto &scaff : scaffs) {
//    ADNPointer<ADNNucleotide> nt = scaff->fivePrime_;
//    do {
//      ADNPointer<ADNNucleotide> nt_next = nt->next_;
//      ADNPointer<ADNBaseSegment> bs = nanorobot.GetBaseSegment(nt);
//      ADNPointer<ADNBaseSegment> bs_nt_next = nanorobot.GetBaseSegment(nt_next);
//      if (bs != nullptr && bs_nt_next != nullptr && !(bs_nt_next == bs->next_ || bs_nt_next == bs->previous_)) {
//        std::pair<ADNPointer<ADNNucleotide>, ADNPointer<ADNNucleotide>> co_pair = std::make_pair(nt, nt_next);
//        crossovers.push_back(co_pair);
//      }
//      nt = nt_next;
//    } while (nt != nullptr);
//  }
//
//  return crossovers;
//}

//void DASAlgorithms::TraceScaffoldOrigami(ANTPart * part) {
//  // Get double strands with the same axis direction
//
//
//}

//double DASAlgorithms::CalculateTwistAngle(ANTDoubleStrand * ds) {
//  double minAngle = NULL;
//  ADNPointer<ADNBaseSegment> start = ds->start_;
//  ADNPointer<ADNBaseSegment> end = ds->GetLastBaseSegment();
//  // save angle
//  double initAngle = ds->initialTwistAngle_;
//  double angleUnit = ANTConstants::BP_ROT;
//
//  // Get pairs on the start end
//  std::vector<std::pair<ADNPointer<ADNNucleotide>, ADNPointer<ADNNucleotide>>> startPairs = GetLinkingNucleotides(start);
//  std::vector<std::pair <ADNPointer<ADNNucleotide>, ADNPointer<ADNNucleotide>>> endPairs = GetLinkingNucleotides(end);
//
//  startPairs.insert(startPairs.end(), endPairs.begin(), endPairs.end());
//  auto minDistance = CalculateAverageDistance(startPairs);
//
//  DASBackToTheAtom btta;
//  for (int i=0; i < 12; ++i) {
//    ds->initialTwistAngle_ += angleUnit;
//    btta.SetDoubleStrandPositions(*ds);
//    auto distance = CalculateAverageDistance(startPairs);
//    if (distance < minDistance) {
//      minAngle = angleUnit*i;
//      minDistance = distance;
//    }
//  }
//
//  ds->initialTwistAngle_ = initAngle;
//  btta.SetDoubleStrandPositions(*ds);  // leave ds with the original positions
//
//  return minAngle;
//}

//std::vector<std::pair<ADNPointer<ADNNucleotide>, ADNPointer<ADNNucleotide>>> DASAlgorithms::GetLinkingNucleotides(ANTBaseSegment * bs) {
//  std::vector<std::pair<ADNPointer<ADNNucleotide>, ADNPointer<ADNNucleotide>>> res;
//
//  ADNPointer<ADNNucleotide> ntLeft = nullptr;
//  ADNPointer<ADNNucleotide> ntRight = nullptr;
//  ANTCell* c = bs->GetCell();
//  if (c->GetType() == BasePair) {
//    ANTBasePair* bp = static_cast<ANTBasePair*>(c);
//    ntLeft = bp->GetLeftNucleotide();
//    ntRight = bp->GetRightNucleotide();
//  }
//  else if (c->GetType() == LoopPair) {
//    ANTLoopPair* lp = static_cast<ANTLoopPair*>(c);
//    ANTLoop* left = lp->GetLeftLoop();
//    ANTLoop* right = lp->GetRightLoop();
//    ntLeft = left->startNt_;
//    ntRight = right->endNt_;
//  }
//
//  res.push_back(std::make_pair(ntLeft, ntLeft->prev_));
//  res.push_back(std::make_pair(ntRight, ntLeft->next_));
//
//  return res;
//}

//SBQuantity::length DASAlgorithms::CalculateAverageDistance(std::vector<std::pair<ADNPointer<ADNNucleotide>, ADNPointer<ADNNucleotide>>> ntPairsList) {
//  SBQuantity::length avg = SBQuantity::length(NULL);
//
//  int count = 0;
//  for (auto &pair : ntPairsList) {
//    if (pair.first != nullptr && pair.second != nullptr) {
//      SBPosition3 firstPos = pair.first->GetSBBackboneCenter();
//      SBPosition3 secondPos = pair.second->GetSBBackboneCenter();
//
//      avg += (firstPos - secondPos).norm();
//      count += 1;
//    }
//  }
//  if (count > 0) avg /= count;
//  return avg;
//}

//void DASAlgorithms::MinimizeDSconnections(ANTDoubleStrand * ds) {
//  double minAngle = 0.0;
//
//  ADNPointer<ADNBaseSegment> start = ds->start_;
//  ADNPointer<ADNBaseSegment> end = ds->GetLastBaseSegment();
//
//  ADNPointer<ADNNucleotide> ntLeft = nullptr;
//  ADNPointer<ADNNucleotide> ntRight = nullptr;
//  ADNPointer<ADNNucleotide> ntLeftLast = nullptr;
//  ADNPointer<ADNNucleotide> ntRightLast = nullptr;
//
//  ANTCell* cell = start->GetCell();
//  if (cell->GetType() == BasePair) {
//    ANTBasePair* bp = static_cast<ANTBasePair*>(cell);
//    ntLeft = bp->GetLeftNucleotide();
//    ntRight = bp->GetRightNucleotide();
//  }
//  else if (cell->GetType() == LoopPair) {
//    ANTLoopPair* lp = static_cast<ANTLoopPair*>(cell);
//    ANTLoop* left = lp->GetLeftLoop();
//    ANTLoop* right = lp->GetRightLoop();
//    ntLeft = left->startNt_;
//    ntRight = right->endNt_;
//  }
//
//  ANTCell* cellLast = end->GetCell();
//  if (cellLast->GetType() == BasePair) {
//    ANTBasePair* bp = static_cast<ANTBasePair*>(cellLast);
//    ntLeftLast = bp->GetLeftNucleotide();
//    ntRightLast = bp->GetRightNucleotide();
//  }
//  else if (cellLast->GetType() == LoopPair) {
//    ANTLoopPair* lp = static_cast<ANTLoopPair*>(cellLast);
//    ANTLoop* left = lp->GetLeftLoop();
//    ANTLoop* right = lp->GetRightLoop();
//    ntLeftLast = left->startNt_;
//    ntRightLast = right->endNt_;
//  }
//
//  SBQuantity::length distLeft = (ntLeft->GetSBBackboneCenter() - ntLeft->prev_->GetSBPosition()).norm();
//  SBQuantity::length distRight = (ntRight->GetSBBackboneCenter() - ntRight->next_->GetSBPosition()).norm();
//  SBQuantity::length distLeftLast = (ntLeftLast->GetSBBackboneCenter() - ntLeftLast->prev_->GetSBPosition()).norm();
//  SBQuantity::length distRightLast = (ntRightLast->GetSBBackboneCenter() - ntRightLast->next_->GetSBPosition()).norm();
//  SBQuantity::length minDistance = (distRight + distLeft + distLeftLast + distRightLast)*0.5;
//
//  DASBackToTheAtom btta;
//
//  double angleUnit = ANTConstants::BP_ROT;
//  for (int i = 1; i < 12; ++i) {
//    ds->initialTwistAngle_ += angleUnit;
//    btta.SetDoubleStrandPositions(*ds);
//    distLeft = (ntLeft->GetSBBackboneCenter() - ntLeft->prev_->GetSBPosition()).norm();
//    distRight = (ntRight->GetSBBackboneCenter() - ntRight->next_->GetSBPosition()).norm();
//    distLeftLast = (ntLeftLast->GetSBBackboneCenter() - ntLeftLast->prev_->GetSBPosition()).norm();
//    distRightLast = (ntRightLast->GetSBBackboneCenter() - ntRightLast->next_->GetSBPosition()).norm();
//    SBQuantity::length distance = (distRight + distLeft + distLeftLast + distRightLast)*0.5;
//    if (distance < minDistance) {
//      minAngle = i*angleUnit;
//    }
//  }
//  ds->initialTwistAngle_ = minAngle;
//}

void DASCrossover::CreateCrossover(ADNPointer<ADNPart> part) {

  ADNPointer<ADNSingleStrand> firstSingleStrandO = firstNt_->GetStrand();
  ADNPointer<ADNSingleStrand> secondSingleStrandO = secondNt_->GetStrand();

  if (firstSingleStrandO == secondSingleStrandO) return;

  //auto first_chain_idsO = ADNBasicOperations::BreakSingleStrand(firstNt_);
  //auto second_chain_idsO = ADNBasicOperations::BreakSingleStrand(secondNt_->GetPrev());
  //// todo: take into account that at crossover point there might already 
  //// be two different strands. Workaround
  //bool pair = true;
  //if (second_chain_idsO.first == second_chain_idsO.second) pair = false;

  //ADNPointer<ADNSingleStrand> mergedChainO = ADNBasicOperations::MergeSingleStrands(first_chain_idsO.first, second_chain_idsO.second);
  //if (pair) ADNPointer<ADNSingleStrand> mergedChainPairO = ADNBasicOperations::MergeSingleStrands(second_chain_idsO.first, first_chain_idsO.second);
}

bool DASCrossover::IsScaffoldCrossover() {
  return (firstNt_->GetStrand()->IsScaffold()|| secondNt_->GetStrand()->IsScaffold());
}

std::string DASAlgorithms::GenerateSequence(double gcCont, int maxContGs, int sz)
{
  std::string seq = "";

  bool g = true;

  auto count = sz;
  int numGs = 0;

  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_real_distribution<double> dist(0.0, std::nextafter(1.0, std::numeric_limits<double>::max()));
  std::uniform_int_distribution<int> coin(0, 1);

  while (count > 0) {
    double v = dist(mt);
    int c = coin(mt);

    std::string base = "N";
    if (v <= gcCont) {
      // GC
      if (c == 0 && g) {
        base = "G";
        ++numGs;
      }
      else base = "C";
    }
    else {
      // AT
      if (c == 0) base = "A";
      else base = "T";
    }

    seq += base;
    if (numGs == maxContGs) {
      g = false;
      numGs = 0;
    }
    else {
      g = true;
    }

    --count;
  }
  
  return seq;
}
