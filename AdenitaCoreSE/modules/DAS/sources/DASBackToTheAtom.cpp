#include "DASBackToTheAtom.hpp"


DASBackToTheAtom::DASBackToTheAtom() {
  LoadNucleotides();
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

  // set this for NN pairs
  nt_left = da_dt_.first;
  nt_right = da_dt_.second;
  if (pair_type == std::make_pair(DNABlocks::DA, DNABlocks::DT)) {
    nt_left = da_dt_.first;
    nt_right = da_dt_.second;
  }
  else if (pair_type == std::make_pair(DNABlocks::DC, DNABlocks::DG)) {
    nt_left = dc_dg_.first;
    nt_right = dc_dg_.second;
  }
  else if (pair_type == std::make_pair(DNABlocks::DG, DNABlocks::DC)) {
    nt_left = dg_dc_.first;
    nt_right = dg_dc_.second;
  }
  else if (pair_type == std::make_pair(DNABlocks::DT, DNABlocks::DA)) {
    nt_left = dt_da_.first;
    nt_right = dt_da_.second;
  }
  // Calculate total center of mass
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
  ublas::vector<double> new_z(3);
  auto direction = bs->GetE3();
  new_z[0] = direction[0];
  new_z[1] = direction[1];
  new_z[2] = direction[2];
  new_z /= ublas::norm_2(new_z);
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
    }
    ADNVectorMath::AddRowToMatrix(subspace, u);
    ADNVectorMath::AddRowToMatrix(subspace, normal);
  }
  else {
    subspace = ADNVectorMath::FindOrthogonalSubspace(new_z);
  }
  ADNVectorMath::AddRowToMatrix(subspace, new_z);
  // to calculate untwisted positions
  ublas::matrix<double> new_basisNoTwist = subspace;
  // apply rotation to basis (rotation has to be negative)
  double angle = -ADNVectorMath::DegToRad(initialAngleDegrees) - ADNVectorMath::DegToRad(bs->GetNumber() * ADNConstants::BP_ROT);
  ublas::matrix<double> rot_mat = ADNVectorMath::MakeRotationMatrix(new_z, angle);
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

    // Set new residue positions
    SBPosition3 p_left = UblasToSBPosition(ublas::row(new_pos, 0));
    nt_l->SetPosition(p_left);
    SBPosition3 p_bb_left = UblasToSBPosition(ublas::row(new_pos, 1));
    nt_l->SetBackbonePosition(p_bb_left);
    SBPosition3 p_sc_left = UblasToSBPosition(ublas::row(new_pos, 2));
    nt_l->SetSidechainPosition(p_sc_left);
    // Set untwisted positions
    //p_left = UblasToSBPosition(ublas::row(new_posNoTwist, 0));
    //nt_l->SetPositionNoTwist(p_left);
    //p_bb_left = UblasToSBPosition(ublas::row(new_posNoTwist, 1));
    //nt_l->SetBackboneCenterNoTwist(p_bb_left);
    //p_sc_left = UblasToSBPosition(ublas::row(new_posNoTwist, 2));
    //nt_l->SetSidechainCenterNoTwist(p_sc_left);
  }
  
  if (nt_r != nullptr) {
    // right nt has a slightly different basis
    ublas::matrix<double> local_basis = ublas::identity_matrix<double>(3);
    local_basis.at_element(0, 0) = -1;
    local_basis.at_element(1, 1) = -1;
    local_basis.at_element(2, 2) = -1;
    auto basis_r = ublas::prod(new_basis, local_basis);
    //auto basis_r = ublas::prod(new_basis, ublas::identity_matrix<double>(3));
    nt_r->SetE1(ublas::column(basis_r, 0));
    nt_r->SetE2(ublas::column(basis_r, 1));
    nt_r->SetE3(ublas::column(basis_r, 2));

    // Set positions
    SBPosition3 p_right = UblasToSBPosition(ublas::row(new_pos, 3));
    nt_r->SetPosition(p_right);
    SBPosition3 p_bb_right = UblasToSBPosition(ublas::row(new_pos, 4));
    nt_r->SetBackbonePosition(p_bb_right);
    SBPosition3 p_sc_right = UblasToSBPosition(ublas::row(new_pos, 5));
    nt_r->SetSidechainPosition(p_sc_right);
    // Set untwisted positions
    //p_right = UblasToSBPosition(ublas::row(new_posNoTwist, 3));
    //nt_r->SetPositionNoTwist(p_right);
    //p_bb_right = UblasToSBPosition(ublas::row(new_posNoTwist, 4));
    //nt_r->SetBackboneCenterNoTwist(p_bb_right);
    //p_sc_right = UblasToSBPosition(ublas::row(new_posNoTwist, 5));
    //nt_r->SetSidechainCenterNoTwist(p_sc_right); 
  }
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

    if (left != nullptr) {
      ADNPointer<ADNNucleotide> ntPrevLeft = left->GetStart()->GetPrev();
      ADNPointer<ADNNucleotide> ntNextLeft = left->GetEnd()->GetNext();

      if (ntPrevLeft != nullptr) {
        posPrevLeft = ntPrevLeft->GetPosition();
      }
      else if (bs->GetPrev() != nullptr) {
        posPrevLeft = bs->GetPrev()->GetPosition();
      }
      else {
        posPrevLeft = bs->GetPosition() - SBQuantity::nanometer(ADNConstants::BP_RISE) * ADNAuxiliary::UblasVectorToSBVector(bs->GetE3());
      }

      if (ntNextLeft != nullptr) {
        posNextLeft = ntNextLeft->GetPosition();
      }
      else if (bs->GetNext() != nullptr) {
        posNextLeft = bs->GetNext()->GetPosition();
      }
      else {
        posNextLeft = bs->GetPosition() + SBQuantity::nanometer(ADNConstants::BP_RISE) * ADNAuxiliary::UblasVectorToSBVector(bs->GetE3());
      }

      PositionLoopNucleotides(left, posPrevLeft, posNextLeft);
    }

    if (right != nullptr) {
      ADNPointer<ADNNucleotide> ntPrevRight = right->GetStart()->GetPrev();
      ADNPointer<ADNNucleotide> ntNextRight = right->GetEnd()->GetNext();

      if (ntPrevRight != nullptr) {
        posPrevRight = ntPrevRight->GetPosition();
      }
      else if (bs->GetPrev() != nullptr) {
        posPrevRight = bs->GetPrev()->GetPosition();
      }
      else {
        posPrevRight = bs->GetPosition() - SBQuantity::nanometer(ADNConstants::BP_RISE) * ADNAuxiliary::UblasVectorToSBVector(bs->GetE3());
      }

      if (ntNextRight != nullptr) {
        posNextRight = ntNextRight->GetPosition();
      }
      else if (bs->GetNext() != nullptr) {
        posNextRight = bs->GetNext()->GetPosition();
      }
      else {
        posNextRight = bs->GetPosition() + SBQuantity::nanometer(ADNConstants::BP_RISE) * ADNAuxiliary::UblasVectorToSBVector(bs->GetE3());
      }

      PositionLoopNucleotides(right, posPrevRight, posNextRight);
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
          std::string msg = "\tNucleotides " + prevName + " and " + nt->GetName() + " too close or too further away: " + std::to_string(distance.getValue()) + "pm";
          logger.Log(msg);
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
    while (nt != endNt->GetNext()) {
      float frac = float(i) / (nucleotides.size() + 1);
      SBPosition3 shift = shifted * frac;
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
  // Calculate translation vector
  // because atoms are fetched from bp, local coordinates refer to base pair c.o.m.
  ublas::vector<double> sys_cm = ADNAuxiliary::SBPositionToUblas(bs->GetPosition());
  ublas::vector<double> t_vec = sys_cm - ADNVectorMath::CalculateCM(positions);
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

void DASBackToTheAtom::PopulateWithMockAtoms(ADNPointer<ADNPart> origami)
{
  auto nts = origami->GetNucleotides();
  SB_FOR(ADNPointer<ADNNucleotide> nt, nts) {
    auto bb = nt->GetBackbone();
    auto sc = nt->GetSidechain();

    auto cBB = bb->GetCenterAtom();
    origami->RegisterAtom(nt, NucleotideGroup::Backbone, cBB);
    auto cSC = sc->GetCenterAtom();
    origami->RegisterAtom(nt, NucleotideGroup::SideChain, cSC);
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
  // delete previous atoms if they have been created
  auto atoms = origami->GetAtoms();
  SB_FOR(ADNPointer<ADNAtom> a, atoms) {
    // todo: check that the node is only deleted from datagraph but reference is not destroyed
    origami->DeregisterAtom(a);
  }

  auto nts = origami->GetNucleotides();
  SB_FOR(ADNPointer<ADNNucleotide> nt, nts) {
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
  PopulateWithMockAtoms(part);
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
    auto nt_cms = CalculateCenters(nt);
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
    auto nt_right_cms = CalculateCenters(nt_right);
    auto nt_left_cms = CalculateCenters(nt_left);
    nt_right->SetPosition(std::get<0>(nt_right_cms));
    nt_left->SetPosition(std::get<0>(nt_left_cms));
    nt_right->SetBackbonePosition(std::get<1>(nt_right_cms));
    nt_left->SetBackbonePosition(std::get<1>(nt_left_cms));
    nt_right->SetSidechainPosition(std::get<2>(nt_right_cms));
    nt_left->SetSidechainPosition(std::get<2>(nt_left_cms));


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

//void BackToTheAtom::SetNucleotidesPostions(ANTNanorobot& origami) {
//  BaseSegments bases = origami.GetBaseSegments();
//
//  for (auto bit = bases.begin(); bit != bases.end(); ++bit) {
//    BaseSegment* bs = bit->second;
//    SBPosition3 pos = bs->GetPositionSB();
//    SBQuantity::angstrom rad = SBQuantity::angstrom(dh_diameter_ * 0.25); // A
//
//    SBPosition3 pos_scaffold = pos; //- rad*bs->u_;
//    SBPosition3 pos_staple = pos; // +rad*bs->u_;
//
//    ADNPointer<ADNNucleotide> nt1 = bs->nt_;
//    nt1->SetPosition(pos_scaffold);
//    nt1->SetBackboneCenter(pos_scaffold);
//    nt1->SetSidechainCenter(pos_scaffold);
//    if (nt1->pair_ != nullptr) {
//      nt1->pair_->SetPosition(pos_staple);
//      nt1->pair_->SetBackboneCenter(pos_staple);
//      nt1->pair_->SetSidechainCenter(pos_staple);
//    }
//  }
//}
//
//std::vector<ADNPointer<ADNNucleotide>> BackToTheAtom::GenerateAllAtomModel(ANTEdge* edge, std::string sequence) {
//
//  std::vector<ADNPointer<ADNNucleotide>> nts;
//
//  std::map<DNABlocks, ADNPointer<ADNNucleotide>> nucleotides = LoadNucleotides();
//
//  std::vector<double> start{ 0.0, 0.0, 0.0 };
//
//  SequencePositions* positions = CalculatePositions(edge, 0); // 0 set only for the moment
//  std::vector<DNABlocks> seq = FormatSequence(sequence);
//
//  ublas::vector<double> p = ADNVectorMath::CreateBoostVector(edge->halfEdge_->source_->GetVectorCoordinates());
//  ublas::vector<double> q = ADNVectorMath::CreateBoostVector(edge->halfEdge_->pair_->source_->GetVectorCoordinates());
//  ublas::vector<double> v_dir = ADNVectorMath::DirectionVector(p, q);
//
//  for (auto pit = positions->begin(); pit != positions->end(); ++pit) {
//    BasePositions b_pos = pit->second;
//    for (auto bit = b_pos.begin(); bit != b_pos.end(); ++bit) {
//      int p = bit->first;
//      std::vector<double> pos = bit->second;
//      ADNPointer<ADNNucleotide> res = nucleotides.at(seq[p]);
//      res->id_ = p;
//      //res->SetPosition(pos);
//      RotateE3(res, v_dir);
//      double degree = p * bp_rot_;
//      RotateCoordinateSystem(res, v_dir, degree);
//      nts.push_back(res);
//    }
//  }
//  return nts;
//}
//
//std::vector<ADNPointer<ADNNucleotide>> BackToTheAtom::GenerateAllAtomModel(SBPosition3 s, SBPosition3 e, std::string sequence) {
//  std::vector<ADNPointer<ADNNucleotide>> nts;
//
//  std::map<DNABlocks, ADNPointer<ADNNucleotide>> nucleotides = LoadNucleotides();
//
//  std::vector<double> start{ 0.0, 0.0, 0.0 };
//  // Divide by 10000 to match units (??)
//  std::vector<double> s_p{ s[0].getValue(), s[1].getValue(), s[2].getValue() };
//  std::vector<double> e_p{ e[0].getValue(), e[1].getValue(), e[2].getValue() };
//
//  SequencePositions* positions = CalculatePositions(s_p, e_p, sequence.size());
//  std::vector<DNABlocks> seq = FormatSequence(sequence);
//
//  ublas::vector<double> p = ADNVectorMath::CreateBoostVector(s_p) / 100.0;
//  ublas::vector<double> q = ADNVectorMath::CreateBoostVector(e_p) / 100.0;
//  ublas::vector<double> v_dir = ADNVectorMath::DirectionVector(p, q);
//
//  for (auto pit = positions->begin(); pit != positions->end(); ++pit) {
//    BasePositions b_pos = pit->second;
//    for (auto bit = b_pos.begin(); bit != b_pos.end(); ++bit) {
//      int pid = bit->first;
//      std::vector<double> pos = bit->second;
//      ADNPointer<ADNNucleotide> res = nucleotides.at(seq[pid]);
//      res->id_ = pid;
//      //res->SetPosition(pos);
//      RotateE3(res, v_dir);
//      double degree = pid * bp_rot_;
//      RotateCoordinateSystem(res, v_dir, degree);
//      nts.push_back(res);
//    }
//  }
//  return nts;
//}
//
//void BackToTheAtom::RotateE3(ADNPointer<ADNNucleotide> residue, ublas::vector<double> v_dir) {
//  ublas::vector<double> e3 = ADNVectorMath::CreateBoostVector(residue->GetE3());
//  ublas::vector<double> v = ADNVectorMath::CrossProduct(e3, v_dir);
//  ublas::matrix<double> m_skew = ADNVectorMath::SkewMatrix(v);
//  double c = ublas::inner_prod(e3, v_dir);
//  double s = ublas::norm_2(v);
//
//  ublas::matrix<double> ident = ublas::identity_matrix<double>(3);
//  ublas::matrix<double> rot_matrix;
//  if (s == 0.0) {
//    rot_matrix = ident + m_skew + ublas::prod(m_skew, m_skew) * (1 - c) / (s * s);
//  }
//  else {
//    rot_matrix = ident * c;
//  }
//  // Set new coordinate system
//  residue->RotateResidue(rot_matrix);
//}
//
//void BackToTheAtom::RotateCoordinateSystem(ADNPointer<ADNNucleotide> residue, ublas::vector<double> axis, double degree) {
//  double rot_bp = ADNVectorMath::DegToRad(degree);
//  ublas::matrix<double> rot_mat = ADNVectorMath::MakeRotationMatrix(axis, rot_bp);
//  residue->RotateResidue(rot_mat);
//}
//
//std::vector<DNABlocks> BackToTheAtom::FormatSequence(std::string sequence) {
//  std::vector<DNABlocks> seq;
//  for (auto sit = sequence.begin(); sit != sequence.end(); ++sit) {
//    DNABlocks nt = residue_names_.right.at(*sit);
//    seq.push_back(nt);
//  }
//  return seq;
//}
//
////void BackToTheAtom::GenerateAllAtomModel(Origami &origami) {
////
////  SBMStructuralModel* model = new SBMStructuralModel();
////  SBNodeIndexer* atomIndexer = new SBDDataGraphNodeIndexer();
////
////  std::map<DNABlocks_, PMNucleotide*> nucleotides = LoadNucleotides();
////  std::vector<PMChain*> chains;
////
////  // Initialize parameters
////  int chain_id = 1;
////  int res_id = 1;
////  int last_atom_id = 1;
////  std::vector<float> start{ 0.0, 0.0, 0.0 };
////
////  // Initialize holders
////  SequencePositions* positions;
////  positions = CalculatePositions(origami);
////
////  auto routing = origami.GetScaffoldRouting();
////
////  //std::map<int, std::vector<float>> start_node_coordinates;
////  //std::map<EdgePair, std::vector<float>> e3_axis;
////  //std::vector<PMNucleotide> residues;
////  //std::map<int, std::vector<PMNucleotide>> chains;
////
////  //// Constants
////  //float move = 20.0;
////  //float move_stp = 10.0;
////
////  //// Calculate positions
////  //std::map<int, std::vector<int>> faces = origami.GetFaces();
////  //std::map<int, std::vector<int>>::iterator f_it;
////  //std::map<int, std::vector<float>> vertex_coordinates = origami.GetVerticesCoordinates();
////  //for (f_it = faces.begin(); f_it != faces.end(); ++f_it) {
////  //  EdgeList edges = GetEdgesInFace(f_it->second);
////  //  std::vector<EdgePair>::iterator e_it;
////  //  std::map<EdgePair, std::vector<float>> v_dirs = GetEdgeDirs(edges, vertex_coordinates);
////  //  std::vector<float> normal = GetNormalToFace(f_it->second, vertex_coordinates);
////  //  for (e_it = edges.begin(); e_it != edges.end(); ++e_it) {
////  //    std::vector<float> v_dir = v_dirs.at(*e_it);
////  //    int length = lengths.at(*e_it);
////  //    EdgePair p_e = ChangeDir(*e_it);
////  //    std::vector<float> tras_scaff = MultiplyScalarVector(move, normal);
////  //    std::vector<float> tras_stp = MultiplyScalarVector(move_stp, normal);
////  //    int start_node = e_it->first;
////
////  //    EdgePair key = *e_it;
////  //    if (!InVector(*e_it, edge_list)) {
////  //      key = p_e;
////  //    }
////
////  //    int t = 0;
////  //    if (key.first > key.second) {
////  //      t = 1;
////  //    }
////
////  //    if (start_node_coordinates.find(start_node) != start_node_coordinates.end()) {
////  //      std::vector<float> start = start_node_coordinates.at(start_node);
////  //    }
////  //    else {
////  //      faces.insert(*f_it);
////  //      continue;
////  //    }
////
////  //    std::vector<float> scaff_pos = SumVectors(start, tras_scaff);
////  //    std::vector<float> stap_pos = SumVectors(start, tras_stp);
////  //    InsertPositionValue(key, 0, t, scaff_pos, scaffold_positions);
////  //    InsertPositionValue(key, 0, t, stap_pos, staple_positions);
////  //    for (int i = 1; i < length; ++i) {
////  //      std::vector<float> scaff_base = FindPositionValue(key, i-1, t, scaffold_positions);
////  //      std::vector<float> stap_base = FindPositionValue(key, i-1, t, staple_positions);
////  //      std::vector<float> tras_dist = MultiplyScalarVector(bp_rise, v_dir);
////  //      scaff_pos = SumVectors(scaff_base, tras_dist);
////  //      stap_pos = SumVectors(stap_base, tras_dist);
////  //      InsertPositionValue(key, i, t, scaff_pos, scaffold_positions);
////  //      InsertPositionValue(key, i, t, stap_pos, staple_positions);
////  //    }
////  //  }
////  //}
////}
//
//SequencePositions* BackToTheAtom::CalculatePositions(const ANTNanorobot &origami) {
//  SequencePositions* positions;
//  //auto lengths = origami.GetEdgeLengths();
//  //auto faces = origami.GetFaces(); // faces needs to return face objects.
//  //ublas::vector<double> start(3);
//  //
//  //for (auto fit = faces.begin(); fit != faces.end(); ++fit) {
//  //  Face face = fit->second;
//  //  auto edges = face.GetEdges();
//  //  auto v_dirs = GetVDirs(edges, origami);
//  //  auto normal = face.GetNormal();
//
//  //  for (auto eit = edges.begin(); eit != edges.end(); ++eit) {
//  //    Edge* edge = eit->second;
//  //    ublas::vector<double> v_dir = edge->GetDirection();
//  //    Edge* pair_edge = edge->GetPair();
//  //    Edge* prev_edge = edge->GetPrevEdge();
//  //    // initialize position
//  //    ublas::vector<double> mv = dh_dist*normal + sz_trun*v_dir;
//  //    mv *= sz_trun*prev_edge->GetDirection();
//  //    // rescale first edge to find initial position and sum it mv
//  //    for i as base in edge:  
//  //      positions[edge][i] = positions[edge][i-1] + v_dir*bp_rise;
//  //  }
//  //}
//  return positions;
//}
//
//SequencePositions* BackToTheAtom::CalculatePositions(std::vector<double> s_p, std::vector<double> e_p, int length) {
//  SequencePositions* positions = new SequencePositions();
//
//  ublas::vector<double> p = ADNVectorMath::CreateBoostVector(s_p) / 100.0;
//  ublas::vector<double> q = ADNVectorMath::CreateBoostVector(e_p) / 100.0;
//  ublas::vector<double> v_dir = ADNVectorMath::DirectionVector(p, q);
//
//  if (length < 0) {
//    std::exit(EXIT_FAILURE);
//  }
//
//  BasePositions b_pos;
//  for (int i = 0; i < length; ++i) {
//    ublas::vector<double> pos = p + (i+1)*v_dir*bp_rise_;
//    b_pos.insert(std::make_pair(i, ADNVectorMath::CreateStdVector(pos)));
//  }
//  // Dummy edge for POC
//  ANTEdge* edge;
//  positions->insert(std::make_pair(edge, b_pos));
//
//  return positions;
//}
//
//SequencePositions* BackToTheAtom::CalculatePositions(ANTEdge* edge, int length) {
//  SequencePositions* positions;
//
//  ublas::vector<double> p = ADNVectorMath::CreateBoostVector(edge->halfEdge_->source_->GetVectorCoordinates());
//  ublas::vector<double> q = ADNVectorMath::CreateBoostVector(edge->halfEdge_->pair_->source_->GetVectorCoordinates());
//  ublas::vector<double> v_dir = ADNVectorMath::DirectionVector(p, q);
//
//  if (length < 0) {
//    std::exit(EXIT_FAILURE);
//  }
//
//  BasePositions b_pos;
//  for (int i = 0; i < length; ++i) {
//    ublas::vector<double> pos = p + v_dir*bp_rise_;
//    b_pos.insert(std::make_pair(i, ADNVectorMath::CreateStdVector(pos)));
//  }
//  positions->insert(std::make_pair(edge, b_pos));
//
//  return positions;
//}
//
//ADNPointer<ADNNucleotide> BackToTheAtom::ParsePDB(std::string source) {
//  std::ifstream file(source.c_str(), std::ios::in);
//  if (!file) {
//    std::cout << "Could not open file " << source << std::endl;
//  }
//
//  ADNPointer<ADNNucleotide> nt = new ANTNucleotide();
//
//  char line[1024];
//  int atom_id = 1;
//
//  while (file.good()) {
//    file.getline(line, 1023);
//    std::string s = line;
//    std::string record_name = s.substr(0, 6);
//    if (record_name == "ATOM  ") {
//      ADNPointer<ADNAtom> atom = new ANTAtom();
//      atom->id_ = atom_id;
//      ++atom_id;
//      std::string name = s.substr(12, 4);
//      boost::trim(name);
//      atom->name_ = name;
//      atom->nt_ = nt;
//      std::string x = s.substr(30, 8);
//      std::string y = s.substr(38, 8);
//      std::string z = s.substr(46, 8);
//      //atom->SetPosition(std::stod(x), std::stod(y), std::stod(z));
//      nt->AddAtom(atom);
//    }
//    if (record_name == "CONECT") {
//      // Check length of connect field
//      int l = s.size();
//      std::string a_id = s.substr(7, 5);
//      boost::trim(a_id);
//      int aid = std::stoi(a_id);
//      std::string c_id1 = s.substr(12, 5);
//      boost::trim(c_id1);
//      int cid = std::stoi(c_id1);
//      int cid2 = -1;
//      if (l >= 17) {
//        std::string c_id2 = s.substr(17, 5);
//        boost::trim(c_id2);
//        if (c_id2.size() > 0) {
//          cid2 = std::stoi(c_id2);
//        }
//      }
//      int cid3 = -1;
//      if (l >= 22) {
//        std::string c_id3 = s.substr(22, 5);
//        boost::trim(c_id3);
//        if (c_id3.size() > 0) {
//          cid3 = std::stoi(c_id3);
//        }
//      }
//      int cid4 = -1;
//      if (l >= 27) {
//        std::string c_id4 = s.substr(27, 5);
//        boost::trim(c_id4);
//        if (c_id4.size() > 0) {
//          cid4 = std::stoi(c_id4);
//        }
//      }
//      std::vector<int> neighbours{ cid, cid2, cid3, cid4 };
//      ADNPointer<ADNAtom> atom = nt->GetAtom(aid);
//      for (auto it = neighbours.begin(); it != neighbours.end(); ++it) {
//        if (*it > 0) {
//          ADNPointer<ADNAtom> at = nt->GetAtom(*it);
//          atom->connections_.push_back(at);
//        }
//      }
//    }
//  }
//
//  return nt;
//}
//
//std::map<DNABlocks, ADNPointer<ADNNucleotide>> BackToTheAtom::LoadNucleotides() {
//  std::map<DNABlocks, ADNPointer<ADNNucleotide>> nucleotides;
//  std::map<DNABlocks, std::string> nts{ { DA_, "DA" }, { DT_, "DT" }, { DC_, "DC" }, { DG_, "DG" } };
//
//  for (auto it = nts.begin(); it != nts.end(); ++it) {
//    std::string nt_source = SB_ELEMENT_PATH + "/Data/" + it->second + ".pdb";
//    ADNPointer<ADNNucleotide> nt = ParsePDB(nt_source);
//    nt->type_ = it->first;
//    auto atoms = nt->atomList_;
//    std::vector<std::vector<double>> positions;
//    for (auto ait = atoms.begin(); ait != atoms.end(); ++ait) {
//      ADNPointer<ADNAtom> a = ait->second;
//      positions.push_back(a->GetVectorPosition());
//    }
//    ublas::matrix<double> positions_matrix = ADNVectorMath::CreateBoostMatrix(positions);
//    ublas::vector<double> r = ADNVectorMath::CalculateCM(positions_matrix);
//    for (auto ait = atoms.begin(); ait != atoms.end(); ++ait) {
//      std::vector<double> coords = ait->second->GetVectorPosition();
//      ublas::vector<double> c = ADNVectorMath::CreateBoostVector(coords);
//      ublas::vector<double> new_pos = c + r;
//      std::vector<double> np = ADNVectorMath::CreateStdVector(new_pos);
//      //ait->second->SetPosition(np);
//    }
//    //nt->SetLocalCoordinateSystem();
//    nucleotides.insert(std::make_pair(it->first, nt));
//  }
//
//  return nucleotides;
//}
////
////EdgeList BackToTheAtom::GetEdgesInFace(std::vector<int> vertices) {
////  EdgeList edge_list;
////  return edge_list;
////}
////
////std::map<EdgePair, std::vector<float>> BackToTheAtom::GetEdgeDirs(EdgeList edges, std::map<int, std::vector<float>> vertex_coordinates) {
////  std::map<EdgePair, std::vector<float>> dirs;
////  return dirs;
////}
////
////std::vector<float> BackToTheAtom::GetNormalToFace(std::vector<int> vertices, std::map<int, std::vector<float>> vertex_coordinates) {
////  std::vector<float> normal;
////  return normal;
////}
////
////EdgePair BackToTheAtom::ChangeDir(EdgePair e) {
////  EdgePair edge;
////  edge.first = e.second;
////  edge.second = e.first;
////  return edge;
////}
////
////template <typename T>
////bool BackToTheAtom::InVector(T member, std::vector<T> member_list) {
////  bool res;
////  return res;
////}
////
////void BackToTheAtom::InsertPositionValue(EdgePair key, int pos, int t, std::vector<float> position, SequencePositions& positions) {
////
////}
////
////std::vector<float> BackToTheAtom::FindPositionValue(EdgePair key, int pos, int t, SequencePositions& positions) {
////  std::vector<float> vec;
////  return vec;
////}