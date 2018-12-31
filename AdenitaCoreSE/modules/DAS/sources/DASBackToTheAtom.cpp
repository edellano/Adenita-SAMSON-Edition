#include "DASBackToTheAtom.hpp"


DASBackToTheAtom::DASBackToTheAtom() {
  //LoadNucleotides();
  LoadNtPairs();
}

DASBackToTheAtom::~DASBackToTheAtom() {
  da_.deleteReferenceTarget();
  dc_.deleteReferenceTarget();
  dg_.deleteReferenceTarget();
  dt_.deleteReferenceTarget();
  da_dt_.first.deleteReferenceTarget();
  da_dt_.second.deleteReferenceTarget();
  dt_da_.first.deleteReferenceTarget();
  dt_da_.second.deleteReferenceTarget();
  dc_dg_.first.deleteReferenceTarget();
  dc_dg_.second.deleteReferenceTarget();
  dg_dc_.first.deleteReferenceTarget();
  dg_dc_.second.deleteReferenceTarget();
}

void DASBackToTheAtom::SetDoubleStrandPositions(ADNPointer<ADNDoubleStrand> ds) {
  ADNPointer<ADNBaseSegment> bs = ds->GetFirstBaseSegment();
  std::vector<ADNPointer<ADNBaseSegment>> loops;
  ADNLogger& logger = ADNLogger::GetLogger();

  for (size_t i = 0; i < ds->GetLength(); ++i) {
    if (bs == nullptr) {
      std::string msg = "SetDoubleStrandPositions() ERROR => nullptr BaseSegment on position " + std::to_string(i) 
        + " of doubleStrand " + std::to_string(ds->getNodeIndex()) + "(size " + std::to_string(ds->GetLength()) + ")";
      logger.Log(msg);
      break;
    }
    int num = bs->GetNumber();
    ADNPointer<ADNCell> cell = bs->GetCell();
    if (cell->GetType() == CellType::BasePair) {
      ADNPointer<ADNBasePair> bp = static_cast<ADNBasePair*>(cell());
      ADNPointer<ADNNucleotide> left = bp->GetLeftNucleotide();
      ADNPointer<ADNNucleotide> right = bp->GetRightNucleotide();
      bool paired = (left != nullptr && right != nullptr);
      SetNucleotidePosition(bs, paired, ds->GetInitialTwistAngle());
    }
    else if (cell->GetType() == CellType::LoopPair) {
      loops.push_back(bs);
    }
    bs = bs->GetNext();
    if (bs != nullptr && abs(bs->GetNumber() - num) != 1) {
      std::string msg = "Consecutive base segments have not consecutive numbers.";
      logger.Log(msg);
    }
  }

  // Calculate last the loops so we can use already inserted positions
  for (auto &bs : loops) {
    SetPositionLoopNucleotides(bs);
  }
}

void DASBackToTheAtom::SetNucleotidePosition(ADNPointer<ADNBaseSegment> bs, bool set_pair = false, double initialAngleDegrees) {
  ADNPointer<ADNNucleotide> nt_left = nullptr;
  ADNPointer<ADNNucleotide> nt_right = nullptr;
  ADNPointer<ADNCell> cell = bs->GetCell();
  if (cell->GetType() != CellType::BasePair) return;

  ADNPointer<ADNBasePair> bp = static_cast<ADNBasePair*>(cell());
  ADNPointer<ADNNucleotide> nt_l = bp->GetLeftNucleotide();
  ADNPointer<ADNNucleotide> nt_r = bp->GetRightNucleotide();

  if (nt_r == nullptr && nt_l == nullptr) return;

  NtPair pair = GetIdealBasePairNucleotides(nt_l, nt_r);
  nt_left = pair.first;
  nt_right = pair.second;

  ublas::matrix<double> positions = CreatePositionsMatrix(pair);

  // Place c.o.m. at bs position
  ublas::vector<double> sys_cm = ADNAuxiliary::SBPositionToUblas(bs->GetPosition());
  ublas::vector<double> t_vec = sys_cm - ADNVectorMath::CalculateCM(positions);
  ublas::matrix<double> input = ublas::matrix<double>(6, 3);

  ublas::row(input, 0) = ADNAuxiliary::SBPositionToUblas(nt_left->GetPosition());
  ublas::row(input, 1) = ADNAuxiliary::SBPositionToUblas(nt_left->GetBackbonePosition());
  ublas::row(input, 2) = ADNAuxiliary::SBPositionToUblas(nt_left->GetSidechainPosition());
  ublas::row(input, 3) = ADNAuxiliary::SBPositionToUblas(nt_right->GetPosition());
  ublas::row(input, 4) = ADNAuxiliary::SBPositionToUblas(nt_right->GetBackbonePosition());
  ublas::row(input, 5) = ADNAuxiliary::SBPositionToUblas(nt_right->GetSidechainPosition());

  // Calculate new residue positions in this system
  // Rotate system so z goes in proper direction
  ublas::matrix<double> subspace = CalculateBaseSegmentBasis(bs);

  // to calculate untwisted positions
  ublas::matrix<double> new_basisNoTwist = subspace;
  // apply rotation to basis (rotation has to be negative)
  double angle = -ADNVectorMath::DegToRad(initialAngleDegrees) - ADNVectorMath::DegToRad(bs->GetNumber() * ADNConstants::BP_ROT);
  ublas::matrix<double> rot_mat = ADNVectorMath::MakeRotationMatrix(bs->GetE3(), angle);
  ublas::matrix<double> new_basis = ADNVectorMath::ApplyTransformation(rot_mat, subspace);
  // Apply transformation
  new_basis = ublas::trans(new_basis);
  ublas::matrix<double> new_pos = ADNVectorMath::ApplyTransformation(new_basis, input);
  new_pos = ADNVectorMath::Translate(new_pos, t_vec);
  // Apply transformation for non twisted postions
  new_basisNoTwist = ublas::trans(new_basisNoTwist);
  ublas::matrix<double> new_posNoTwist = ADNVectorMath::ApplyTransformation(new_basisNoTwist, input);
  new_posNoTwist = ADNVectorMath::Translate(new_posNoTwist, t_vec);

  if (nt_l != nullptr) {
    nt_l->SetE1(ublas::column(new_basis, 0));
    nt_l->SetE2(ublas::column(new_basis, 1));
    nt_l->SetE3(ublas::column(new_basis, 2));

    if (nt_l->GetStrand()->IsScaffold()) {
      ADNLogger& logger = ADNLogger::GetLogger();
      std::string msg = "Left: " + std::to_string(nt_l->GetE3()[0]) + " " + std::to_string(nt_l->GetE3()[1]) + " " + std::to_string(nt_l->GetE3()[2]);
      logger.LogDebug(msg);
    }

    // Set new residue positions
    SBPosition3 p_left = UblasToSBPosition(ublas::row(new_pos, 0));
    nt_l->SetPosition(p_left);
    SBPosition3 p_bb_left = UblasToSBPosition(ublas::row(new_pos, 1));
    nt_l->SetBackbonePosition(p_bb_left);
    SBPosition3 p_sc_left = UblasToSBPosition(ublas::row(new_pos, 2));
    nt_l->SetSidechainPosition(p_sc_left);
  }
  
  if (nt_r != nullptr) {
    // right nt has a slightly different basis
    ublas::matrix<double> local_basis = ublas::identity_matrix<double>(3);
    local_basis.at_element(0, 0) = 1;
    local_basis.at_element(1, 1) = -1;
    local_basis.at_element(2, 2) = -1;
    auto basis_r = ublas::prod(new_basis, local_basis);
    //auto basis_r = ublas::prod(new_basis, ublas::identity_matrix<double>(3));
    nt_r->SetE1(ublas::column(basis_r, 0));
    nt_r->SetE2(ublas::column(basis_r, 1));
    nt_r->SetE3(ublas::column(basis_r, 2));

    if (nt_r->GetStrand()->IsScaffold()) {
      ADNLogger& logger = ADNLogger::GetLogger();
      std::string msg = "Right: " + std::to_string(nt_r->GetE3()[0]) + " " + std::to_string(nt_r->GetE3()[1]) + " " + std::to_string(nt_r->GetE3()[2]);
      logger.LogDebug(msg);
    }

    // Set positions
    SBPosition3 p_right = UblasToSBPosition(ublas::row(new_pos, 3));
    nt_r->SetPosition(p_right);
    SBPosition3 p_bb_right = UblasToSBPosition(ublas::row(new_pos, 4));
    nt_r->SetBackbonePosition(p_bb_right);
    SBPosition3 p_sc_right = UblasToSBPosition(ublas::row(new_pos, 5));
    nt_r->SetSidechainPosition(p_sc_right);
  }
}

void DASBackToTheAtom::UntwistNucleotidesPosition(ADNPointer<ADNBaseSegment> bs)
{
  auto nts = bs->GetNucleotides();
  SB_FOR(ADNPointer<ADNNucleotide> nt, nts) {
    UntwistNucleotidePosition(nt);
  }
}

ublas::matrix<double> DASBackToTheAtom::CreatePositionsMatrix(NtPair pair)
{
  ADNPointer<ADNNucleotide> nt_left = pair.first;
  ADNPointer<ADNNucleotide> nt_right = pair.second;
  size_t cols = 3;
  auto ntLeftAtoms = nt_left->GetAtoms();
  auto ntRightAtoms = nt_right->GetAtoms();
  size_t rows_left = ntLeftAtoms.size();
  size_t rows_right = ntRightAtoms.size();
  ublas::matrix<double> positions(rows_left + rows_right, cols);
  int i = 0;
  SB_FOR(ADNPointer<ADNAtom> n, ntLeftAtoms) {
    ublas::vector<double> ac_blas = ADNAuxiliary::SBPositionToUblas(n->GetPosition());
    ublas::row(positions, i) = ac_blas;
    ++i;
  }
  SB_FOR(ADNPointer<ADNAtom> n, ntRightAtoms) {
    ublas::vector<double> ac_blas = ADNAuxiliary::SBPositionToUblas(n->GetPosition());
    ublas::row(positions, i) = ac_blas;
    ++i;
  }

  return positions;
}

void DASBackToTheAtom::SetPositionLoopNucleotides(ADNPointer<ADNBaseSegment> bs) {

  if (bs->GetCell()->GetType() == CellType::LoopPair) {
    ADNPointer<ADNLoopPair> pair = static_cast<ADNLoopPair*>(bs->GetCell()());
    ADNPointer<ADNLoop> left = pair->GetLeftLoop();
    ADNPointer<ADNLoop> right = pair->GetRightLoop();

    SBPosition3 posPrevLeft;
    SBPosition3 posPrevRight;
    SBPosition3 posNextLeft;
    SBPosition3 posNextRight;
    SBVector3 prevE2Left;
    SBVector3 prevE2Right;
    SBVector3 nextE2Left;
    SBVector3 nextE2Right;

    if (left != nullptr) {
      ADNPointer<ADNNucleotide> ntPrevLeft = left->GetStart()->GetPrev();
      ADNPointer<ADNNucleotide> ntNextLeft = left->GetEnd()->GetNext();

      if (ntPrevLeft != nullptr) {
        posPrevLeft = ntPrevLeft->GetPosition();
        prevE2Left = ADNAuxiliary::UblasVectorToSBVector(ntPrevLeft->GetE1());
      }
      else if (bs->GetPrev() != nullptr) {
        posPrevLeft = bs->GetPrev()->GetPosition();
        prevE2Left = ADNAuxiliary::UblasVectorToSBVector(bs->GetPrev()->GetE1());
      }
      else {
        posPrevLeft = bs->GetPosition() - SBQuantity::nanometer(ADNConstants::BP_RISE) * ADNAuxiliary::UblasVectorToSBVector(bs->GetE3());
        prevE2Left = ADNAuxiliary::UblasVectorToSBVector(bs->GetE1());
      }

      if (ntNextLeft != nullptr) {
        posNextLeft = ntNextLeft->GetPosition();
        nextE2Left = ADNAuxiliary::UblasVectorToSBVector(ntNextLeft->GetE1());
      }
      else if (bs->GetNext() != nullptr) {
        posNextLeft = bs->GetNext()->GetPosition();
        nextE2Left = ADNAuxiliary::UblasVectorToSBVector(bs->GetNext()->GetE1());
      }
      else {
        posNextLeft = bs->GetPosition() + SBQuantity::nanometer(ADNConstants::BP_RISE) * ADNAuxiliary::UblasVectorToSBVector(bs->GetE3());
        nextE2Left = ADNAuxiliary::UblasVectorToSBVector(bs->GetE1());
      }

      PositionLoopNucleotides(left, posPrevLeft, posNextLeft);
      //PositionLoopNucleotidesQBezier(left, posPrevLeft, posNextLeft, prevE2Left, nextE2Left);
    }

    if (right != nullptr) {
      ADNPointer<ADNNucleotide> ntPrevRight = right->GetStart()->GetPrev();
      ADNPointer<ADNNucleotide> ntNextRight = right->GetEnd()->GetNext();

      if (ntPrevRight != nullptr) {
        posPrevRight = ntPrevRight->GetPosition();
        prevE2Right = ADNAuxiliary::UblasVectorToSBVector(ntPrevRight->GetE1());
      }
      else if (bs->GetPrev() != nullptr) {
        posPrevRight = bs->GetPrev()->GetPosition();
        prevE2Right = ADNAuxiliary::UblasVectorToSBVector(bs->GetPrev()->GetE1());
      }
      else {
        posPrevRight = bs->GetPosition() - SBQuantity::nanometer(ADNConstants::BP_RISE) * ADNAuxiliary::UblasVectorToSBVector(bs->GetE3());
        prevE2Right = ADNAuxiliary::UblasVectorToSBVector(bs->GetE1());
      }

      if (ntNextRight != nullptr) {
        posNextRight = ntNextRight->GetPosition();
        nextE2Right = ADNAuxiliary::UblasVectorToSBVector(ntNextRight->GetE2());
      }
      else if (bs->GetNext() != nullptr) {
        posNextRight = bs->GetNext()->GetPosition();
        nextE2Right = ADNAuxiliary::UblasVectorToSBVector(bs->GetNext()->GetE2());
      }
      else {
        posNextRight = bs->GetPosition() + SBQuantity::nanometer(ADNConstants::BP_RISE) * ADNAuxiliary::UblasVectorToSBVector(bs->GetE3());
        nextE2Right = ADNAuxiliary::UblasVectorToSBVector(bs->GetE2());
      }

      PositionLoopNucleotides(right, posPrevRight, posNextRight);
      //PositionLoopNucleotidesQBezier(right, posPrevRight, posNextRight, prevE2Right, nextE2Right);
    }
  }
}

void DASBackToTheAtom::CheckDistances(ADNPointer<ADNPart> part)
{
  auto singleStrands = part->GetSingleStrands();
  SBPosition3 prevPos;
  std::string prevName = "";
  ADNLogger& logger = ADNLogger::GetLogger();
  std::string msg = "Checking distances between nucleotides...";
  logger.Log(msg);
  SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
    int start = 0;
    auto nt = ss->GetFivePrime();
    std::string msg = " string " + ss->GetName();
    logger.Log(msg);
    while (nt != nullptr)
    {
      if (start != 0) {
        auto distance = (prevPos - nt->GetPosition()).norm();
        if (!ADNVectorMath::IsNearlyZero(distance.getValue() - ADNConstants::BP_RISE * 1000)) {
          //std::string msg = "\tNucleotides " + prevName + " and " + nt->GetName() + " too close or too further away: " + std::to_string(distance.getValue()) + "pm";
          //logger.Log(msg);
        }
      }
      else {
        start = 1;
      }
      prevPos = nt->GetPosition();
      prevName = nt->GetName();
      nt = nt->GetNext();
    }
  }

  auto baseSegments = part->GetBaseSegments();
  msg = "Checking distances between base segments...";
  logger.Log(msg);
  SB_FOR(ADNPointer<ADNBaseSegment> bs, baseSegments) {
    auto bsNext = bs->GetNext();
    if (bsNext != nullptr) {
      SBPosition3 nextPos = bsNext->GetPosition();
      std::string nextName = bs->getName();
      auto distance = (nextPos - bs->GetPosition()).norm();
      if (!ADNVectorMath::IsNearlyZero(distance.getValue() - ADNConstants::BP_RISE * 1000)) {
        //msg = "\tBase Segments " + nextName + " and " + bs->getName() + " too close or too further away: " + std::to_string(distance.getValue()) + "pm";
        //logger.Log(msg);
      }
      if (bs->GetCellType() == BasePair && bsNext->GetCellType() == BasePair) {
        ADNPointer<ADNBasePair> bp = static_cast<ADNBasePair*>(bs->GetCell()());
        ADNPointer<ADNBasePair> bpNext = static_cast<ADNBasePair*>(bsNext->GetCell()());
        ADNPointer<ADNNucleotide> left = bp->GetLeftNucleotide();
        ADNPointer<ADNNucleotide> right = bp->GetRightNucleotide();
        ADNPointer<ADNNucleotide> leftNext = bpNext->GetLeftNucleotide();
        ADNPointer<ADNNucleotide> rightNext = bpNext->GetRightNucleotide();
        if (left != nullptr && right != nullptr && leftNext != nullptr && rightNext != nullptr) {
          auto basePairCenterPosition = (left->GetPosition() + right->GetPosition())*0.5;
          auto basePairCenterPositionNext = (leftNext->GetPosition() + rightNext->GetPosition())*0.5;
          auto bpDistance = (basePairCenterPosition - basePairCenterPositionNext).norm();
          if (!ADNVectorMath::IsNearlyZero(bpDistance.getValue() - ADNConstants::BP_RISE * 1000)) {
            msg = "\tBase Pairs " + nextName + " and " + bs->getName() + " too close or too further away: " + std::to_string(distance.getValue()) + "pm";
            logger.Log(msg);
          }
        }
      }
    }
  }
}

void DASBackToTheAtom::UntwistNucleotidePosition(ADNPointer<ADNNucleotide> nt)
{
  ADNPointer<ADNBaseSegment> bs = nt->GetBaseSegment();

  ADNPointer<ADNCell> cell = bs->GetCell();
  if (cell->GetType() != CellType::BasePair) return;

  ADNPointer<ADNBasePair> bp = static_cast<ADNBasePair*>(cell());
  ADNPointer<ADNNucleotide> nt_l = bp->GetLeftNucleotide();
  ADNPointer<ADNNucleotide> nt_r = bp->GetRightNucleotide();

  if (nt_r == nullptr && nt_l == nullptr) return;

  NtPair pair = GetIdealBasePairNucleotides(nt_l, nt_r);
  ADNPointer<ADNNucleotide> nt_left = pair.first;
  ADNPointer<ADNNucleotide> nt_right = pair.second;
  ublas::matrix<double> positions = CreatePositionsMatrix(pair);

  ublas::matrix<double> basisNoTwist = CalculateBaseSegmentBasis(bs);

  // Place c.o.m. at bs position
  ublas::vector<double> sys_cm = ADNAuxiliary::SBPositionToUblas(bs->GetPosition());
  ublas::vector<double> t_vec = sys_cm - ADNVectorMath::CalculateCM(positions);
  ublas::matrix<double> input = ublas::matrix<double>(6, 3);

  ublas::row(input, 0) = ADNAuxiliary::SBPositionToUblas(nt_left->GetPosition());
  ublas::row(input, 1) = ADNAuxiliary::SBPositionToUblas(nt_left->GetBackbonePosition());
  ublas::row(input, 2) = ADNAuxiliary::SBPositionToUblas(nt_left->GetSidechainPosition());
  ublas::row(input, 3) = ADNAuxiliary::SBPositionToUblas(nt_right->GetPosition());
  ublas::row(input, 4) = ADNAuxiliary::SBPositionToUblas(nt_right->GetBackbonePosition());
  ublas::row(input, 5) = ADNAuxiliary::SBPositionToUblas(nt_right->GetSidechainPosition());
  // Apply transformation for non twisted postions
  basisNoTwist = ublas::trans(basisNoTwist);
  ublas::matrix<double> posNoTwist = ADNVectorMath::ApplyTransformation(basisNoTwist, input);
  posNoTwist = ADNVectorMath::Translate(posNoTwist, t_vec);

  if (nt_l != nullptr) {
    nt_l->SetE1(ublas::column(basisNoTwist, 0));
    nt_l->SetE2(ublas::column(basisNoTwist, 1));
    nt_l->SetE3(ublas::column(basisNoTwist, 2));

    // Set new residue positions
    SBPosition3 p_left = UblasToSBPosition(ublas::row(posNoTwist, 0));
    nt_l->SetPosition(p_left);
    SBPosition3 p_bb_left = UblasToSBPosition(ublas::row(posNoTwist, 1));
    nt_l->SetBackbonePosition(p_bb_left);
    SBPosition3 p_sc_left = UblasToSBPosition(ublas::row(posNoTwist, 2));
    nt_l->SetSidechainPosition(p_sc_left);
  }

  if (nt_r != nullptr) {
    // right nt has a slightly different basis
    ublas::matrix<double> local_basis = ublas::identity_matrix<double>(3);
    local_basis.at_element(0, 0) = -1;
    local_basis.at_element(1, 1) = -1;
    local_basis.at_element(2, 2) = -1;
    auto basis_r = ublas::prod(basisNoTwist, local_basis);
    //auto basis_r = ublas::prod(new_basis, ublas::identity_matrix<double>(3));
    nt_r->SetE1(ublas::column(basis_r, 0));
    nt_r->SetE2(ublas::column(basis_r, 1));
    nt_r->SetE3(ublas::column(basis_r, 2));

    // Set positions
    SBPosition3 p_right = UblasToSBPosition(ublas::row(posNoTwist, 3));
    nt_r->SetPosition(p_right);
    SBPosition3 p_bb_right = UblasToSBPosition(ublas::row(posNoTwist, 4));
    nt_r->SetBackbonePosition(p_bb_right);
    SBPosition3 p_sc_right = UblasToSBPosition(ublas::row(posNoTwist, 5));
    nt_r->SetSidechainPosition(p_sc_right);
  }
}

void DASBackToTheAtom::PositionLoopNucleotides(ADNPointer<ADNLoop> loop, SBPosition3 bsPositionPrev, SBPosition3 bsPositionNext)
{
  auto nucleotides = loop->GetNucleotides();
  if (nucleotides.size() == 0) return;

  ADNPointer<ADNNucleotide> startNt = loop->GetStart();
  ADNPointer<ADNNucleotide> endNt = loop->GetEnd();

  auto order = ADNBasicOperations::OrderNucleotides(startNt, endNt);
  startNt = order.first;
  endNt = order.second;

  if (startNt != nullptr && endNt != nullptr) {
    SBPosition3 start_pos = bsPositionPrev;
    SBPosition3 end_pos = bsPositionNext;
    SBPosition3 shifted = end_pos - start_pos;
    ADNPointer<ADNNucleotide> nt = startNt;

    ublas::vector<double> e3 = ADNAuxiliary::SBVectorToUblasVector(shifted.normalizedVersion());
    auto subspace = ADNVectorMath::FindOrthogonalSubspace(e3);
    ublas::vector<double> e1 = ublas::row(subspace, 0);
    ublas::vector<double> e2 = ublas::row(subspace, 1);

    int i = 0;
    auto frac = shifted.norm() / (nucleotides.size() + 1);
    while (nt != endNt->GetNext()) {
      //float frac = float(i) / (nucleotides.size() + 1);
      SBPosition3 shift = (i+1) * frac * shifted.normalizedVersion();
      shift += start_pos;

      nt->SetPosition(shift);
      nt->SetSidechainPosition(shift);
      nt->SetBackbonePosition(shift);

      nt->SetE1(e1);
      nt->SetE2(e2);
      nt->SetE3(e3);

      nt = nt->GetNext();
      i++;
    }
  }
  else {
    //if loop is at the beginning
  }
}

void DASBackToTheAtom::PositionLoopNucleotidesQBezier(ADNPointer<ADNLoop> loop, SBPosition3 bsPositionPrev, SBPosition3 bsPositionNext, SBVector3 bsPrevE3, SBVector3 bsNextE3)
{
  auto numNts = loop->getNumberOfNucleotides();
  // height of the curve depends on the number of nucleotides
  // just try and error
  SBPosition3 P0 = bsPositionPrev;
  SBPosition3 P2 = bsPositionNext;
  SBVector3 nDir = -(bsPrevE3 + bsNextE3).normalizedVersion();
  SBQuantity::length step = SBQuantity::nanometer(ADNConstants::BP_RISE)*0.9;
  SBQuantity::length estLength = numNts*step;
  SBPosition3 P1 = (P0 + P2)*0.5 + estLength*nDir;

  SBQuantity::length length = ADNVectorMath::LengthQuadraticBezier(P0, P1, P2);

  P1 += 3*step*nDir;
  //while (length < estLength) {
  //  P1 += step*nDir;
  //  length = ADNVectorMath::LengthQuadraticBezier(P0, P1, P2);
  //}

  // calculate step of the bezier curve
  double deltaT = 1.0 / (numNts - 1);

  ADNPointer<ADNNucleotide> startNt = loop->GetStart();
  ADNPointer<ADNNucleotide> endNt = loop->GetEnd();

  auto order = ADNBasicOperations::OrderNucleotides(startNt, endNt);
  startNt = order.first;
  endNt = order.second;

  if (startNt != nullptr && endNt != nullptr) {
    auto t = 0.5*deltaT;
    ADNPointer<ADNNucleotide> nt = startNt;

    ublas::vector<double> e3 = ADNAuxiliary::SBVectorToUblasVector(ADNVectorMath::DerivativeQuadraticBezier(P0, P1, P2, t));
    auto subspace = ADNVectorMath::FindOrthogonalSubspace(e3);
    ublas::vector<double> e1 = ublas::row(subspace, 0);
    ublas::vector<double> e2 = ublas::row(subspace, 1);

    while (nt != endNt->GetNext()) {
      //float frac = float(i) / (nucleotides.size() + 1);
      SBPosition3 shift = ADNVectorMath::QuadraticBezierPoint(P0, P1, P2, t);

      nt->SetPosition(shift);
      nt->SetSidechainPosition(shift);
      nt->SetBackbonePosition(shift);

      nt->SetE1(e1);
      nt->SetE2(e2);
      nt->SetE3(e3);

      nt = nt->GetNext();
      t +=  deltaT;
    }
  }
}

void DASBackToTheAtom::CreateBonds(ADNPointer<ADNPart> origami)
{
  auto nts = origami->GetNucleotides();

  SB_FOR(ADNPointer<ADNNucleotide> nt, nts) {
    auto atoms = nt->GetAtoms();
    auto bb = nt->GetBackbone();
    auto sc = nt->GetSidechain();
    auto connections = ADNModel::GetNucleotideBonds(nt->GetType());

    SB_FOR(ADNPointer<ADNAtom> at, atoms) {
      ADNPointer<ADNAtom> atC = nullptr;
      std::string atName = at->GetName();
      if (connections.find(atName) != connections.end()) {
        auto conns = connections.at(at->GetName());
        for (std::string name : conns) {
          auto lst = nt->GetAtomsByName(name);
          if (lst.size() == 1) {
            atC = *lst.begin();
            SBPointer<SBBond> bond = new SBBond(at(), atC());
            if (at->IsInBackbone()) {
              bb->addChild(bond());
            }
            else {
              sc->addChild(bond());
            }
          }
        }
      }
    }

    // create connection with previous nucleotide
    if (nt->GetEnd() != FivePrime && nt->GetEnd() != FiveAndThreePrime) {
      auto prevNt = nt->GetPrev(true);
      ADNPointer<ADNAtom> atP = *nt->GetAtomsByName("P").begin();
      ADNPointer<ADNAtom> atO3p = *prevNt->GetAtomsByName("O3'").begin();
      SBPointer<SBBond> bond = new SBBond(atP(), atO3p());
      bb->addChild(bond());
    }
  }
}

void DASBackToTheAtom::FindAtomsPositions(ADNPointer<ADNNucleotide> nt)
{
  auto bs = nt->GetBaseSegment();
  // Save positions in matrix
  size_t cols = 3;
  auto ntLeftAtoms = nt->GetAtoms();
  size_t rows = ntLeftAtoms.size();
  ublas::matrix<double> positions(rows, cols);
  int i = 0;
  SB_FOR(ADNPointer<ADNAtom> n, ntLeftAtoms) {
    ublas::vector<double> ac_blas = ADNAuxiliary::SBPositionToUblas(n->GetPosition());
    ublas::row(positions, i) = ac_blas;
    ++i;
  }

  // to calculate the translation we need to take into account the base pair
  // even if pair is not defined, thus we use the ideal base pairs
  NtPair pair = GetIdealBasePairNucleotides(nt->GetType(), ADNModel::GetComplementaryBase(nt->GetType()));
  ublas::matrix<double> bpPositions = CreatePositionsMatrix(pair);

  // Calculate translation vector
  // because atoms are fetched from bp, local coordinates refer to base pair c.o.m.
  ublas::vector<double> sys_cm = ADNAuxiliary::SBPositionToUblas(bs->GetPosition());
  ublas::vector<double> t_vec = sys_cm - ADNVectorMath::CalculateCM(bpPositions);
  ublas::matrix<double> input = positions;
  // Apply global basis
  auto transf = nt->GetGlobalBasisTransformation();
  ublas::matrix<double> new_pos = ADNVectorMath::ApplyTransformation(transf, input);
  new_pos = ADNVectorMath::Translate(new_pos, t_vec);
  // Set new atom positions
  auto ntLAtoms = nt->GetAtoms();
  int count = 0;
  SB_FOR(ADNPointer<ADNAtom> n, ntLAtoms) {
    ADNPointer<ADNAtom> atom = n;
    SBPosition3 pos = UblasToSBPosition(ublas::row(new_pos, count));
    atom->SetPosition(pos);
    ++count;
  }
}

void DASBackToTheAtom::PopulateWithMockAtoms(ADNPointer<ADNPart> origami, bool positionsFromNucleotide)
{
  auto nts = origami->GetNucleotides();
  SB_FOR(ADNPointer<ADNNucleotide> nt, nts) {
    auto bb = nt->GetBackbone();
    auto sc = nt->GetSidechain();

    auto cBB = bb->GetCenterAtom();
    auto cSC = sc->GetCenterAtom();
    if (positionsFromNucleotide) {
      cBB->SetPosition(nt->GetPosition());
      cSC->SetPosition(nt->GetPosition());
    }
    
    origami->RegisterAtom(nt, NucleotideGroup::Backbone, cBB);
    origami->RegisterAtom(nt, NucleotideGroup::SideChain, cSC);
    // hiding atoms here cause when they are created is too slow
    nt->HideCenterAtoms();
  }
}

void DASBackToTheAtom::PopulateNucleotideWithAllAtoms(ADNPointer<ADNPart> origami, ADNPointer<ADNNucleotide> nt)
{
  ADNPointer<ADNNucleotide> nt_left;
  ADNPointer<ADNNucleotide> nt_right;
  DNABlocks nt_type = nt->GetType();

  // for DN_ we use DA_
  nt_left = da_dt_.first;
  if (nt_type == DNABlocks::DA) {
    nt_left = da_dt_.first;
  }
  else if (nt_type == DNABlocks::DC) {
    nt_left = dc_dg_.first;
  }
  else if (nt_type == DNABlocks::DG) {
    nt_left = dg_dc_.first;
  }
  else if (nt_type == DNABlocks::DT) {
    nt_left = dt_da_.first;
  }

  auto atoms = nt_left->GetAtoms();
  SB_FOR(ADNPointer<ADNAtom> atom, atoms) {
    NucleotideGroup g = NucleotideGroup::SideChain;
    if (atom->IsInBackbone()) g = NucleotideGroup::Backbone;

    ADNPointer<ADNAtom> newAtom = CopyAtom(atom);
    origami->RegisterAtom(nt, g, newAtom);
  }
}

void DASBackToTheAtom::GenerateAllAtomModel(ADNPointer<ADNPart> origami)
{
  auto nts = origami->GetNucleotides();
  SB_FOR(ADNPointer<ADNNucleotide> nt, nts) {
    auto atoms = nt->GetAtoms();
    // delete previous atoms if they have been created
    SB_FOR(ADNPointer<ADNAtom> a, atoms) {
      // todo: check that the node is only deleted from datagraph but reference is not destroyed
      origami->DeregisterAtom(a);
    }
    // populate nucleotides with the correct atoms
    PopulateNucleotideWithAllAtoms(origami, nt);
    // find atomic positions
    FindAtomsPositions(nt);
  }

  CreateBonds(origami);
}

std::tuple<SBPosition3, SBPosition3, SBPosition3> DASBackToTheAtom::CalculateCenters(ADNPointer<ADNNucleotide> nt) {
  SBPosition3 cm_bb = SBPosition3();
  SBPosition3 cm_sc = SBPosition3();
  SBPosition3 cm = SBPosition3();
  ublas::matrix<double> positions_bb = ublas::matrix<double>(0, 3);
  ublas::matrix<double> positions_sc = ublas::matrix<double>(0, 3);
  ublas::matrix<double> positions = ublas::matrix<double>(0, 3);
  auto ntAtoms = nt->GetAtoms();
  SB_FOR(ADNPointer<ADNAtom> n, ntAtoms) {
    ublas::vector<double> ac_blas = ADNAuxiliary::SBPositionToUblas(n->GetPosition());
    ADNVectorMath::AddRowToMatrix(positions, ac_blas);
    if (n->IsInBackbone()) {
      ADNVectorMath::AddRowToMatrix(positions_bb, ac_blas);
    }
    else {
      ADNVectorMath::AddRowToMatrix(positions_sc, ac_blas);
    }
  }

  ublas::vector<double> cm_bb_vec = ADNVectorMath::CalculateCM(positions_bb);
  ublas::vector<double> cm_sc_vec = ADNVectorMath::CalculateCM(positions_sc);
  ublas::vector<double> cm_vec = ADNVectorMath::CalculateCM(positions);
  cm_bb = UblasToSBPosition(cm_bb_vec);
  cm_sc = UblasToSBPosition(cm_sc_vec);
  cm = UblasToSBPosition(cm_vec);
  return std::make_tuple(cm, cm_bb, cm_sc);
}

std::tuple<SBPosition3, SBPosition3, SBPosition3> DASBackToTheAtom::CalculateCentersOfMass(ADNPointer<ADNNucleotide> nt)
{
  SBPosition3 cm_bb = SBPosition3();
  SBPosition3 cm_sc = SBPosition3();
  SBPosition3 cm = SBPosition3();
  ublas::matrix<double> positions_bb = ublas::matrix<double>(0, 3);
  ublas::matrix<double> positions_sc = ublas::matrix<double>(0, 3);
  ublas::matrix<double> positions = ublas::matrix<double>(0, 3);
  auto ntAtoms = nt->GetAtoms();
  double totalMass(0.0);
  double totalBBMass(0.0);
  double totalSCMass(0.0);
  SB_FOR(ADNPointer<ADNAtom> n, ntAtoms) {
    double mass = n->getAtomicWeight().getValue();
    ublas::vector<double> ac_blas = mass * ADNAuxiliary::SBPositionToUblas(n->GetPosition());
    totalMass += mass;
    ADNVectorMath::AddRowToMatrix(positions, ac_blas);
    if (n->IsInBackbone()) {
      ADNVectorMath::AddRowToMatrix(positions_bb, ac_blas);
      totalBBMass += mass;
    }
    else {
      ADNVectorMath::AddRowToMatrix(positions_sc, ac_blas);
      totalSCMass += mass;
    }
  }

  ublas::vector<double> cm_bb_vec = ADNVectorMath::CalculateCM(positions_bb, totalBBMass);
  ublas::vector<double> cm_sc_vec = ADNVectorMath::CalculateCM(positions_sc, totalSCMass);
  ublas::vector<double> cm_vec = ADNVectorMath::CalculateCM(positions, totalMass);
  cm_bb = UblasToSBPosition(cm_bb_vec);
  cm_sc = UblasToSBPosition(cm_sc_vec);
  cm = UblasToSBPosition(cm_vec);
  return std::make_tuple(cm, cm_bb, cm_sc);
}

SBPosition3 DASBackToTheAtom::UblasToSBPosition(ublas::vector<double> vec) {
  // we assume vec is in picometers!
  std::vector<double> pos = ADNVectorMath::CreateStdVector(vec);
  SBPosition3 res = SBPosition3();
  res[0] = SBQuantity::angstrom(pos[0] * 0.01);
  res[1] = SBQuantity::angstrom(pos[1] * 0.01);
  res[2] = SBQuantity::angstrom(pos[2] * 0.01);

  return res;
}

ADNPointer<ADNAtom> DASBackToTheAtom::CopyAtom(ADNPointer<ADNAtom> atom)
{
  ADNPointer<ADNAtom> newAt = new ADNAtom();
  newAt->SetPosition(atom->GetPosition());
  newAt->setElementType(atom->getElementType());
  newAt->setName(atom->getName());
  return newAt;
}

void DASBackToTheAtom::SetReferenceFrame(NtPair pair) {
  ADNPointer<ADNNucleotide> nt_left = pair.first;
  ADNPointer<ADNNucleotide> nt_right = pair.second;
  // Fetch needed points
  std::vector<double> c1_prime_left_std;
  std::vector<double> c1_prime_right_std;
  std::vector<double> c5_prime_left_std;
  std::vector<double> c5_prime_right_std;
  std::vector<double> c3_prime_left_std;
  std::vector<double> c3_prime_right_std;

  auto atoms = nt_left->GetAtoms();
  std::vector<std::vector<double>> positions;
  std::vector<int> sidechain_indices;
  int idx = 0;
  SB_FOR(ADNPointer<ADNAtom> a, atoms) {
    positions.push_back(ADNAuxiliary::SBPositionToVector(a->GetPosition()));
    if (a->GetName() == "C1'") {
      c1_prime_left_std = ADNAuxiliary::SBPositionToVector(a->GetPosition());
    }
    if (a->GetName() == "C5'") {
      c5_prime_left_std = ADNAuxiliary::SBPositionToVector(a->GetPosition());
    }
    if (a->GetName() == "C3'") {
      c3_prime_left_std = ADNAuxiliary::SBPositionToVector(a->GetPosition());
    }
    if (!a->IsInBackbone()) {
      sidechain_indices.push_back(idx);
    }
    ++idx;
  }
  atoms = nt_right->GetAtoms();
  SB_FOR(ADNPointer<ADNAtom> a, atoms) {
    positions.push_back(ADNAuxiliary::SBPositionToVector(a->GetPosition()));
    if (a->GetName() == "C1'") {
      c1_prime_right_std = ADNAuxiliary::SBPositionToVector(a->GetPosition());
    }
    if (a->GetName() == "C5'") {
      c5_prime_right_std = ADNAuxiliary::SBPositionToVector(a->GetPosition());
    }
    if (a->GetName() == "C3'") {
      c3_prime_right_std = ADNAuxiliary::SBPositionToVector(a->GetPosition());
    }
    if (!a->IsInBackbone()) {
      sidechain_indices.push_back(idx);
    }
    ++idx;
  }

  ublas::vector<double> c5_prime_left = ADNVectorMath::CreateBoostVector(c5_prime_left_std);
  ublas::vector<double> c3_prime_left = ADNVectorMath::CreateBoostVector(c3_prime_left_std);
  ublas::vector<double> c1_prime_left = ADNVectorMath::CreateBoostVector(c1_prime_left_std);
  ublas::vector<double> c1_prime_right = ADNVectorMath::CreateBoostVector(c1_prime_right_std);

  // Center system
  ublas::matrix<double> positions_matrix = ADNVectorMath::CreateBoostMatrix(positions);
  ublas::matrix<double> new_positions = ADNVectorMath::CenterSystem(positions_matrix);

  // Fit sidechain to plane to calculate z (we get directions for left nt)
  ublas::matrix<double> sidechain_positions(0, 3);
  for (auto &it : sidechain_indices) {
    ADNVectorMath::AddRowToMatrix(sidechain_positions, ublas::row(new_positions, it));
  }
  ublas::vector<double> z = ADNVectorMath::CalculatePlane(sidechain_positions);
  // z has to go 5' -> 3'
  ublas::vector<double> d_5p3p = c3_prime_left - c5_prime_left;
  double chk = ublas::inner_prod(z, d_5p3p);
  if (chk < 0) {
    z *= -1.0;
  }
  // y is on the calculated plane c1' -- c1' direction
  ublas::vector<double> y_prime = c1_prime_left - c1_prime_right;
  // we need to make sure it is contained in the plane
  ublas::vector<double> y = y_prime - (ublas::inner_prod(y_prime, z))*z;
  y /= ublas::norm_2(y);
  // third component
  ublas::vector<double> x = ADNVectorMath::CrossProduct(y, z);
  x /= ublas::norm_2(x);
  // we want to transform all positions so local base is the standard basis
  ublas::matrix<double> transf(3, 3);
  ublas::column(transf, 0) = x;
  ublas::column(transf, 1) = y;
  ublas::column(transf, 2) = z;
  ublas::matrix<double> inv_transf = ADNVectorMath::InvertMatrix(transf);

  ublas::matrix<double> coords = ADNVectorMath::ApplyTransformation(inv_transf, new_positions);

  atoms = nt_left->GetAtoms();
  int r_id = SetAtomsPositions(atoms, coords, 0);
  atoms = nt_right->GetAtoms();
  r_id = SetAtomsPositions(atoms, coords, r_id);
}

int DASBackToTheAtom::SetAtomsPositions(CollectionMap<ADNAtom> atoms, ublas::matrix<double> new_positions, int r_id) {
  SB_FOR(ADNPointer<ADNAtom> a, atoms) {
    ublas::vector<double> new_pos = ublas::row(new_positions, r_id);
    std::vector<double> np = ADNVectorMath::CreateStdVector(new_pos);
    SBPosition3 pos = SBPosition3();
    pos[0] = SBQuantity::angstrom(np[0] * 0.01); // we need to convert from picometers
    pos[1] = SBQuantity::angstrom(np[1] * 0.01);
    pos[2] = SBQuantity::angstrom(np[2] * 0.01);
    a->SetPosition(pos);
    ++r_id;
  }

  return r_id;
}

void DASBackToTheAtom::SetNucleotidesPostions(ADNPointer<ADNPart> part) {
  auto doubleStrands = part->GetDoubleStrands();
  SB_FOR(ADNPointer<ADNDoubleStrand> ds, doubleStrands) {
    SetDoubleStrandPositions(ds);
  }
}

//void DASBackToTheAtom::RotateNucleotide(ADNPointer<ADNNucleotide> nt, double angle, bool set_pair = false) {
//
//  ublas::matrix<double> subspace(3, 3);
//  ublas::row(subspace, 0) = nt->GetE1();
//  ublas::row(subspace, 1) = nt->GetE2();
//  ublas::row(subspace, 2) = nt->GetE3();
//
//  ublas::matrix<double> rot_mat = ADNVectorMath::MakeRotationMatrix(nt->GetE3(), angle);
//  ublas::matrix<double> new_basis = ADNVectorMath::ApplyTransformation(rot_mat, subspace);
//
//  nt->SetE1(ublas::row(new_basis, 0));
//  nt->SetE2(ublas::row(new_basis, 1));
//  nt->SetE3(ublas::row(new_basis, 2));
//
//  // rotation matrix is defined with respect to origin
//  // we need first to translate coordinates
//  ublas::vector<double> sys_cm = ADNAuxiliary::SBPositionToUblas(nt->GetPosition());
//  ublas::vector<double> sys_bb = ADNAuxiliary::SBPositionToUblas(nt->GetBackbonePosition());
//  ublas::vector<double> sys_sc = ADNAuxiliary::SBPositionToUblas(nt->GetSidechainPosition());
//
//  ublas::matrix<double> positions(0, 3);
//  ADNVectorMath::AddRowToMatrix(positions, sys_cm);
//  ADNVectorMath::AddRowToMatrix(positions, sys_bb);
//  ADNVectorMath::AddRowToMatrix(positions, sys_sc);
//
//  positions = ADNVectorMath::Translate(positions, -sys_cm);
//  auto transf = nt->GetGlobalBasisTransformation();
//  ublas::matrix<double> new_pos = ADNVectorMath::ApplyTransformation(transf, positions);
//  new_pos = ADNVectorMath::Translate(new_pos, sys_cm);
//
//  if (set_pair && nt->GetPair() != nullptr) {
//    RotateNucleotide(nt->GetPair(), angle, false);
//  }
//}

//void DASBackToTheAtom::DisplayDNABlock(std::string block) {
//  SBPointer<SBStructuralModel> structuralModel = new SBMStructuralModel();
//  structuralModel->setName("test");
//  SBPointer<SBChain> chain = new SBChain();
//  chain->setName("test chain");
//  if (block == "A") {
//    SBPointer<SBResidue> res = da_->CreateSBResidue();
//    chain->addChild(res());
//  }
//  else if (block == "C") {
//    SBPointer<SBResidue> res = dc_->CreateSBResidue();
//    chain->addChild(res());
//  }
//  else if (block == "G") {
//    SBPointer<SBResidue> res = dg_->CreateSBResidue();
//    chain->addChild(res());
//  }
//  else if (block == "T") {
//    SBPointer<SBResidue> res = dt_->CreateSBResidue();
//    chain->addChild(res());
//  }
//  else if (block == "AT") {
//    SBPointer<SBResidue> res1 = da_dt_.first->CreateSBResidue();
//    SBPointer<SBResidue> res2 = da_dt_.second->CreateSBResidue();
//    chain->addChild(res1());
//    SBPointer<SBChain> chain2 = new SBChain();
//    chain2->setName("test chain 2");
//    chain2->addChild(res2());
//    structuralModel->getStructuralRoot()->addChild(chain2());
//  }
//  else if (block == "CG") {
//    SBPointer<SBResidue> res1 = dc_dg_.first->CreateSBResidue();
//    SBPointer<SBResidue> res2 = dc_dg_.second->CreateSBResidue();
//    chain->addChild(res1());
//    SBPointer<SBChain> chain2 = new SBChain();
//    chain2->setName("test chain 2");
//    chain2->addChild(res2());
//    structuralModel->getStructuralRoot()->addChild(chain2());
//  }
//  else if (block == "GC") {
//    SBPointer<SBResidue> res1 = dg_dc_.first->CreateSBResidue();
//    SBPointer<SBResidue> res2 = dg_dc_.second->CreateSBResidue();
//    chain->addChild(res1());
//    SBPointer<SBChain> chain2 = new SBChain();
//    chain2->setName("test chain 2");
//    chain2->addChild(res2());
//    structuralModel->getStructuralRoot()->addChild(chain2());
//  }
//  else if (block == "TA") {
//    SBPointer<SBResidue> res1 = dt_da_.first->CreateSBResidue();
//    SBPointer<SBResidue> res2 = dt_da_.second->CreateSBResidue();
//    chain->addChild(res1());
//    SBPointer<SBChain> chain2 = new SBChain();
//    chain2->setName("test chain 2");
//    chain2->addChild(res2());
//    structuralModel->getStructuralRoot()->addChild(chain2());
//  }
//  structuralModel->getStructuralRoot()->addChild(chain());
//  SAMSON::beginHolding("Add model");
//  structuralModel->create();
//  SAMSON::getActiveLayer()->addChild(structuralModel());
//  SAMSON::endHolding();
//}

void DASBackToTheAtom::LoadNucleotides() {
  std::map<DNABlocks, char> residueNames = { { DNABlocks::DA, 'A' },{ DNABlocks::DT, 'T' },{ DNABlocks::DC, 'C' },{ DNABlocks::DG, 'G' } };;
  for (auto it = residueNames.begin(); it != residueNames.end(); ++it) {
    if (it->second == 'N') continue;
    std::string name(1, it->second);
    std::string nt_source = SB_ELEMENT_PATH + "/Data/" + name + ".pdb";
    ADNPointer<ADNNucleotide> nt = ParsePDB(nt_source);
    nt->SetType(it->first);
    auto atoms = nt->GetAtoms();
    std::vector<std::vector<double>> positions;
    std::vector<std::vector<double>> base_plane;
    SB_FOR(ADNPointer<ADNAtom> a, atoms) {
      auto coords = ADNAuxiliary::SBPositionToVector(a->GetPosition());
      positions.push_back(coords);
    }
    // Set same coordinate system for all nucleotides
    ublas::matrix<double> positions_matrix = ADNVectorMath::CreateBoostMatrix(positions);
    ublas::matrix<double> new_positions = ADNVectorMath::CenterSystem(positions_matrix);
    int r_id = SetAtomsPositions(atoms, new_positions, 0);
    // nt->SetReferenceFrame();
    auto nt_cms = CalculateCentersOfMass(nt);
    nt->SetPosition(std::get<0>(nt_cms));
    nt->SetBackbonePosition(std::get<1>(nt_cms));
    nt->SetSidechainPosition(std::get<2>(nt_cms));

    if (it->first == DNABlocks::DA) {
      da_ = nt;
    }
    else if (it->first == DNABlocks::DC) {
      dc_ = nt;
    }
    else if (it->first == DNABlocks::DG) {
      dg_ = nt;
    }
    else if (it->first == DNABlocks::DT) {
      dt_ = nt;
    }
  }
}

void DASBackToTheAtom::LoadNtPairs() {
  for (auto it = nt_pairs_names_.begin(); it != nt_pairs_names_.end(); ++it) {
    std::string name = it->right;
    if (name == "NN") continue;
    std::string nt_source = SB_ELEMENT_PATH + "/Data/" + name + ".pdb";
    NtPair nt_pair = ParseBasePairPDB(nt_source);
    ADNPointer<ADNNucleotide> nt_left = nt_pair.first;
    ADNPointer<ADNNucleotide> nt_right = nt_pair.second;
    nt_left->SetType(it->left.first);
    nt_right->SetType(it->left.second);

    SetReferenceFrame(nt_pair);

    // Set positions
    auto nt_right_cms = CalculateCentersOfMass(nt_right);
    auto nt_left_cms = CalculateCentersOfMass(nt_left);
    // center pair
    auto total_cms = (std::get<0>(nt_right_cms) + std::get<0>(nt_left_cms))*0.5;

    nt_right->SetPosition(std::get<0>(nt_right_cms) - total_cms);
    nt_left->SetPosition(std::get<0>(nt_left_cms) - total_cms);
    nt_right->SetBackbonePosition(std::get<1>(nt_right_cms) - total_cms);
    nt_left->SetBackbonePosition(std::get<1>(nt_left_cms) - total_cms);
    nt_right->SetSidechainPosition(std::get<2>(nt_right_cms) - total_cms);
    nt_left->SetSidechainPosition(std::get<2>(nt_left_cms) - total_cms);


    if (it->left == std::make_pair(DNABlocks::DA, DNABlocks::DT)) {
      da_dt_ = nt_pair;
    }
    else if (it->left == std::make_pair(DNABlocks::DC, DNABlocks::DG)) {
      dc_dg_ = nt_pair;
    }
    else if (it->left == std::make_pair(DNABlocks::DG, DNABlocks::DC)) {
      dg_dc_ = nt_pair;
    }
    else if (it->left == std::make_pair(DNABlocks::DT, DNABlocks::DA)) {
      dt_da_ = nt_pair;
    }
  }
}

std::pair<ADNPointer<ADNNucleotide>, ADNPointer<ADNNucleotide>> DASBackToTheAtom::ParseBasePairPDB(std::string source) {
  std::ifstream file(source.c_str(), std::ios::in);
  ADNLogger& logger = ADNLogger::GetLogger();
  if (!file) {
    std::string msg = "Could not open file " + source;
    logger.Log(msg);
  }

  ADNPointer<ADNNucleotide> nt_left = new ADNNucleotide();
  ADNPointer<ADNNucleotide> nt_right = new ADNNucleotide();

  char line[1024];
  int atom_id_left = 0;
  int atom_id_right = 0;
  std::map<int, ADNPointer<ADNAtom>> atoms_by_pdb_id;
  int r_num_f = -1;
  std::string prev_residue_chain = "";
  while (file.good()) {
    file.getline(line, 1023);
    std::string s = line;
    std::string record_name = s.substr(0, 6);
    if (record_name == "ATOM  ") {
      auto residue_chain = s.substr(21, 1);
      if (prev_residue_chain != "" && residue_chain != prev_residue_chain) {
        r_num_f *= -1;
      }
      std::string pdb_id = s.substr(6, 5);
      int p_id = std::stoi(pdb_id);

      ADNPointer<ADNAtom> atom = new ADNAtom();
      std::string name = s.substr(12, 4);
      boost::trim(name);
      atom->SetName(name);
      atom->setElementType(ADNModel::GetElementType(name));
      std::string x = s.substr(30, 8);
      std::string y = s.substr(38, 8);
      std::string z = s.substr(46, 8);
      SBPosition3 pos = SBPosition3();
      pos[0] = SBQuantity::angstrom(std::stod(x));
      pos[1] = SBQuantity::angstrom(std::stod(y));
      pos[2] = SBQuantity::angstrom(std::stod(z));
      atom->SetPosition(pos);
      if (r_num_f == -1) {
        NucleotideGroup g = SBNode::SideChain;
        if (ADNModel::IsAtomInBackboneByName(atom->GetName())) g = SBNode::Backbone;
        nt_left->AddAtom(g, atom);
      }
      else {
        NucleotideGroup g = SBNode::SideChain;
        if (ADNModel::IsAtomInBackboneByName(atom->GetName())) g = SBNode::Backbone;
        nt_right->AddAtom(g, atom);
      }
      atoms_by_pdb_id.insert(std::make_pair(p_id, atom));
      prev_residue_chain = residue_chain;
    }
    if (record_name == "CONECT") {
      // Check length of connect field
      size_t l = s.size();
      std::string a_id = s.substr(7, 5);
      boost::trim(a_id);
      int aid = std::stoi(a_id);
      std::string c_id1 = s.substr(12, 5);
      boost::trim(c_id1);
      int cid = std::stoi(c_id1);
      int cid2 = -1;
      if (l >= 17) {
        std::string c_id2 = s.substr(17, 5);
        boost::trim(c_id2);
        if (c_id2.size() > 0) {
          cid2 = std::stoi(c_id2);
        }
      }
      int cid3 = -1;
      if (l >= 22) {
        std::string c_id3 = s.substr(22, 5);
        boost::trim(c_id3);
        if (c_id3.size() > 0) {
          cid3 = std::stoi(c_id3);
        }
      }
      int cid4 = -1;
      if (l >= 27) {
        std::string c_id4 = s.substr(27, 5);
        boost::trim(c_id4);
        if (c_id4.size() > 0) {
          cid4 = std::stoi(c_id4);
        }
      }
      std::vector<int> neighbours{ cid, cid2, cid3, cid4 };
      ADNPointer<ADNAtom> atom = atoms_by_pdb_id.at(aid);
      /*for (auto it = neighbours.begin(); it != neighbours.end(); ++it) {
        if (*it > 0) {
          ADNPointer<ADNAtom> at = atoms_by_pdb_id.at(*it);
          atom->connections_.push_back(at);
        }
      }*/
    }
  }

  return std::make_pair(nt_left, nt_right);
}

NtPair DASBackToTheAtom::GetIdealBasePairNucleotides(ADNPointer<ADNNucleotide> nt_l, ADNPointer<ADNNucleotide> nt_r)
{
  // scaffold nucleotide is always on the left
  std::pair<DNABlocks, DNABlocks> pair_type;
  if (nt_l == nullptr) {
    pair_type = std::make_pair(ADNModel::GetComplementaryBase(nt_r->GetType()), nt_r->GetType());
  }
  else if (nt_r == nullptr) {
    pair_type = std::make_pair(nt_l->GetType(), ADNModel::GetComplementaryBase(nt_l->GetType()));
  }
  else {
    pair_type = std::make_pair(nt_l->GetType(), nt_r->GetType());
  }

  return GetIdealBasePairNucleotides(pair_type.first, pair_type.second);
}

NtPair DASBackToTheAtom::GetIdealBasePairNucleotides(DNABlocks nt_l, DNABlocks nt_r)
{
  // scaffold nucleotide is always on the left
  std::pair<DNABlocks, DNABlocks> pair_type = std::make_pair(nt_l, nt_r);

  NtPair pair = da_dt_;

  if (pair_type == std::make_pair(DNABlocks::DA, DNABlocks::DT)) {
    pair = da_dt_;
  }
  else if (pair_type == std::make_pair(DNABlocks::DC, DNABlocks::DG)) {
    pair = dc_dg_;
  }
  else if (pair_type == std::make_pair(DNABlocks::DG, DNABlocks::DC)) {
    pair = dg_dc_;
  }
  else if (pair_type == std::make_pair(DNABlocks::DT, DNABlocks::DA)) {
    pair = dt_da_;
  }

  return pair;
}

ublas::matrix<double> DASBackToTheAtom::CalculateBaseSegmentBasis(ADNPointer<ADNBaseSegment> bs)
{
  ublas::vector<double> new_z(3);
  auto direction = bs->GetE3();
  new_z[0] = direction[0];
  new_z[1] = direction[1];
  new_z[2] = direction[2];
  new_z /= ublas::norm_2(new_z);
  bs->SetE3(new_z);
  // if normal_ or u_ are defined we already have the subspace
  ublas::matrix<double> subspace(0, 3);
  if (ublas::norm_2(bs->GetE2()) > 0) {
    ublas::vector<double> normal = bs->GetE2();
    ublas::vector<double> u(3);
    if (ublas::norm_2(bs->GetE1()) > 0) {
      u = bs->GetE1();
    }
    else {
      u = ADNVectorMath::CrossProduct(new_z, normal);
      bs->SetE1(u);
    }
    ADNVectorMath::AddRowToMatrix(subspace, u);
    ADNVectorMath::AddRowToMatrix(subspace, normal);
  }
  else {
    subspace = ADNVectorMath::FindOrthogonalSubspace(new_z);
    bs->SetE1(ublas::row(subspace, 0));
    bs->SetE2(ublas::row(subspace, 1));
  }
  ADNVectorMath::AddRowToMatrix(subspace, new_z);

  return subspace;
}

ADNPointer<ADNNucleotide> DASBackToTheAtom::ParsePDB(std::string source) {
  std::ifstream file(source.c_str(), std::ios::in);
  if (!file) {
    std::cout << "Could not open file " << source << std::endl;
  }

  ADNPointer<ADNNucleotide> nt = new ADNNucleotide();

  char line[1024];
  int atom_id = 1;

  while (file.good()) {
    file.getline(line, 1023);
    std::string s = line;
    std::string record_name = s.substr(0, 6);
    if (record_name == "ATOM  ") {
      ADNPointer<ADNAtom> atom = new ADNAtom();
      std::string name = s.substr(12, 4);
      boost::trim(name);
      atom->SetName(name);
      std::string x = s.substr(30, 8);
      std::string y = s.substr(38, 8);
      std::string z = s.substr(46, 8);
      SBPosition3 pos = SBPosition3();
      pos[0] = SBQuantity::angstrom(std::stod(x));
      pos[1] = SBQuantity::angstrom(std::stod(y));
      pos[2] = SBQuantity::angstrom(std::stod(z));
      atom->SetPosition(pos);
      NucleotideGroup g = SBNode::SideChain;
      if (ADNModel::IsAtomInBackboneByName(atom->GetName())) g = SBNode::Backbone;
      nt->AddAtom(g, atom);
    }
    if (record_name == "CONECT") {
      // Check length of connect field
      size_t l = s.size();
      std::string a_id = s.substr(7, 5);
      boost::trim(a_id);
      int aid = std::stoi(a_id);
      std::string c_id1 = s.substr(12, 5);
      boost::trim(c_id1);
      int cid = std::stoi(c_id1);
      int cid2 = -1;
      if (l >= 17) {
        std::string c_id2 = s.substr(17, 5);
        boost::trim(c_id2);
        if (c_id2.size() > 0) {
          cid2 = std::stoi(c_id2);
        }
      }
      int cid3 = -1;
      if (l >= 22) {
        std::string c_id3 = s.substr(22, 5);
        boost::trim(c_id3);
        if (c_id3.size() > 0) {
          cid3 = std::stoi(c_id3);
        }
      }
      int cid4 = -1;
      if (l >= 27) {
        std::string c_id4 = s.substr(27, 5);
        boost::trim(c_id4);
        if (c_id4.size() > 0) {
          cid4 = std::stoi(c_id4);
        }
      }
      std::vector<int> neighbours{ cid, cid2, cid3, cid4 };
      /*ADNPointer<ADNAtom> atom = nt->GetAtom(aid);
      for (auto it = neighbours.begin(); it != neighbours.end(); ++it) {
        if (*it > 0) {
          ADNPointer<ADNAtom> at = nt->GetAtom(*it);
          atom->connections_.push_back(at);
        }
      }*/
    }
  }

  return nt;
}