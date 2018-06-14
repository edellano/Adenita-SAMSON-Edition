/** \file
*   \author   Elisa de Llano <elisa.dellano.fl@ait.ac.at>
*/


#include "DASBackToTheAtom.hpp"

DASBackToTheAtom::DASBackToTheAtom() {
  LoadNucleotides();
  LoadNtPairs();
}

//BackToTheAtom::~BackToTheAtom() {
//  delete da_;
//  delete dc_;
//  delete dg_;
//  delete dt_;
//}


void DASBackToTheAtom::SetDoubleStrandPositions(ANTDoubleStrand & ds) {
  ANTBaseSegment* bs = ds.start_;
  std::vector<ANTBaseSegment*> loops;

  for (unsigned int i = 0; i < ds.size_; ++i) {
    if (bs == nullptr) {
      std::string msg = "SetDoubleStrandPositions() ERROR => nullptr BaseSegment on position " + std::to_string(i) 
        + " of doubleStrand " + std::to_string(ds.id_) + "(size " + std::to_string(ds.size_) + ")";
      ANTAuxiliary::log(msg);
      break;
    }
    int num = bs->number_;
    ANTCell* cell = bs->GetCell();
    if (cell->GetType() == CellType::BasePair) {
      bool paired = (bs->GetNucleotide()->pair_ != nullptr);
      SetNucleotidePositionPaired(bs, paired, ds.initialTwistAngle_);
    }
    else if (cell->GetType() == CellType::LoopPair) {
      loops.push_back(bs);
    }
    bs = bs->next_;
    if (bs != nullptr && abs(bs->number_ - num) != 1) {
      std::string msg = "Consecutive base segments have not consecutive numbers.";
      ANTAuxiliary::log(msg);
    }
  }

  // Calculate last the loops so we can use already inserted positions
  for (auto &bs : loops) {
    SetPositionLoopNucleotides(bs);
  }
}

void DASBackToTheAtom::SetNucleotidePosition(ANTPart& origami, ANTNucleotide* nt, bool set_pair = false) {
  ANTBaseSegment* bs = nt->GetBaseSegment();
  std::vector<ANTBaseSegment*> loops;
  if (bs != nullptr) {
    ANTCell* cell = bs->GetCell();
    if (cell->GetType() == CellType::BasePair) {
      SetNucleotidePositionPaired(bs, set_pair, 0.0); 
    }
    else if (cell->GetType() == CellType::LoopPair) {
      SetPositionLoopNucleotides(bs);
    }
  }
  else {
    // todo
  }
}

void DASBackToTheAtom::SetNucleotidePositionUnpaired(ANTBaseSegment* bs) {
  ANTNucleotide* inf_nt = da_dt_.first; // we take DA as default
  ANTNucleotide* pair = da_dt_.second;
  ANTNucleotide* nt = bs->GetNucleotide();

  if (nt->type_ == DNABlocks::DA_) {
    inf_nt = da_dt_.first;
    pair = da_dt_.second;
  }
  else if (nt->type_ == DNABlocks::DC_) {
    inf_nt = dc_dg_.first;
    pair = dc_dg_.second;
  }
  else if (nt->type_ == DNABlocks::DG_) {
    inf_nt = dg_dc_.first;
    pair = dg_dc_.second;
  }
  else if (nt->type_ == DNABlocks::DT_) {
    inf_nt = dt_da_.first;
    pair = dt_da_.second;
  }
  // Calculate total center of mass
  size_t cols = 3;
  size_t rows_left = inf_nt->atomList_.size() + pair->atomList_.size();
  ublas::matrix<double> positions(rows_left, cols);
  int i = 0;
  for (auto ait = inf_nt->atomList_.begin(); ait != inf_nt->atomList_.end(); ++ait) {
    std::vector<double> a_coords = ait->second->GetVectorPosition();
    ublas::vector<double> ac_blas = ANTVectorMath::CreateBoostVector(a_coords);
    ublas::row(positions, i) = ac_blas;
    ++i;
  }
  for (auto ait = pair->atomList_.begin(); ait != pair->atomList_.end(); ++ait) {
    std::vector<double> a_coords = ait->second->GetVectorPosition();
    ublas::vector<double> ac_blas = ANTVectorMath::CreateBoostVector(a_coords);
    ublas::row(positions, i) = ac_blas;
    ++i;
  }
  // Place c.o.m. at bs position
  std::vector<double> sys_cm_pos = bs->GetVectorPosition();
  ublas::vector<double> sys_cm = ANTVectorMath::CreateBoostVector(sys_cm_pos);
  ublas::vector<double> t_vec = sys_cm - ANTVectorMath::CalculateCM(positions);
  ublas::matrix<double> input = ublas::matrix<double>(3, 3);

  ublas::row(input, 0) = ANTVectorMath::CreateBoostVector(inf_nt->GetVectorPosition());
  ublas::row(input, 1) = ANTVectorMath::CreateBoostVector(inf_nt->GetVectorBackboneCenter());
  ublas::row(input, 2) = ANTVectorMath::CreateBoostVector(inf_nt->GetVectorSidechainCenter());

  // Calculate new residue positions in this system
  // Rotate system so z goes in proper direction
  ublas::vector<double> new_z(3);
  new_z[0] = bs->direction_[0].getValue();
  new_z[1] = bs->direction_[1].getValue();
  new_z[2] = bs->direction_[2].getValue();
  new_z /= ublas::norm_2(new_z);
  ublas::matrix<double> subspace = ANTVectorMath::FindOrthogonalSubspace(new_z);
  ANTVectorMath::AddRowToMatrix(subspace, new_z);
  // apply rotation to basis (rotation has to be negative)
  double angle = -ANTVectorMath::DegToRad(bs->number_ * ANTConstants::BP_ROT);
  ublas::matrix<double> rot_mat = ANTVectorMath::MakeRotationMatrix(new_z, angle);
  ublas::matrix<double> new_basis = ANTVectorMath::ApplyTransformation(rot_mat, subspace);
  /*!
  * Save new basis
  */
  nt->e1_ = ublas::row(new_basis, 0);
  nt->e2_ = ublas::row(new_basis, 1);
  nt->e3_ = ublas::row(new_basis, 2);

  // Apply transformation
  new_basis = ublas::trans(new_basis);
  ublas::matrix<double> new_pos = ANTVectorMath::ApplyTransformation(new_basis, input);
  new_pos = ANTVectorMath::Translate(new_pos, t_vec);

  // Set new residue positions
  SBPosition3 p_left = UblasToSBPosition(ublas::row(new_pos, 0));
  nt->SetPosition(p_left);
  SBPosition3 p_bb_left = UblasToSBPosition(ublas::row(new_pos, 1));
  nt->SetBackboneCenter(p_bb_left);
  SBPosition3 p_sc_left = UblasToSBPosition(ublas::row(new_pos, 2));
  nt->SetSidechainCenter(p_sc_left);
}

void DASBackToTheAtom::SetNucleotidePositionPaired(ANTBaseSegment* bs, bool set_pair = false, double initialAngleDegrees) {
  ANTNucleotide* nt_left = nullptr;
  ANTNucleotide* nt_right = nullptr;
  ANTCell* cell = bs->GetCell();
  if (cell->GetType() != CellType::BasePair) return;

  ANTBasePair* bp = static_cast<ANTBasePair*>(cell);
  ANTNucleotide* nt_l = bp->GetLeftNucleotide();
  ANTNucleotide* nt_r = bp->GetRightNucleotide();

  if (nt_r == nullptr && nt_l == nullptr) return;

  // scaffold nucleotide is always on the left
  std::pair<DNABlocks, DNABlocks> pair_type;
  if (nt_l == nullptr) {
    pair_type = std::make_pair(ANTModel::GetComplementaryBase(nt_r->type_), nt_r->type_);
  }
  else if (nt_r == nullptr) {
    pair_type = std::make_pair(nt_l->type_, ANTModel::GetComplementaryBase(nt_l->type_));
  }
  else {
    pair_type = std::make_pair(nt_l->type_, nt_r->type_);
  }

  // set this for NN pairs
  nt_left = da_dt_.first;
  nt_right = da_dt_.second;
  if (pair_type == std::make_pair(DA_, DT_)) {
    nt_left = da_dt_.first;
    nt_right = da_dt_.second;
  }
  else if (pair_type == std::make_pair(DC_, DG_)) {
    nt_left = dc_dg_.first;
    nt_right = dc_dg_.second;
  }
  else if (pair_type == std::make_pair(DG_, DC_)) {
    nt_left = dg_dc_.first;
    nt_right = dg_dc_.second;
  }
  else if (pair_type == std::make_pair(DT_, DA_)) {
    nt_left = dt_da_.first;
    nt_right = dt_da_.second;
  }
  // Calculate total center of mass
  size_t cols = 3;
  size_t rows_left = nt_left->atomList_.size();
  size_t rows_right = nt_right->atomList_.size();
  ublas::matrix<double> positions(rows_left + rows_right, cols);
  int i = 0;
  for (auto ait = nt_left->atomList_.begin(); ait != nt_left->atomList_.end(); ++ait) {
    std::vector<double> a_coords = ait->second->GetVectorPosition();
    ublas::vector<double> ac_blas = ANTVectorMath::CreateBoostVector(a_coords);
    ublas::row(positions, i) = ac_blas;
    ++i;
  }
  for (auto ait = nt_right->atomList_.begin(); ait != nt_right->atomList_.end(); ++ait) {
    std::vector<double> a_coords = ait->second->GetVectorPosition();
    ublas::vector<double> ac_blas = ANTVectorMath::CreateBoostVector(a_coords);
    ublas::row(positions, i) = ac_blas;
    ++i;
  }
  // Place c.o.m. at bs position
  std::vector<double> sys_cm_pos = bs->GetVectorPosition();
  ublas::vector<double> sys_cm = ANTVectorMath::CreateBoostVector(sys_cm_pos);
  ublas::vector<double> t_vec = sys_cm - ANTVectorMath::CalculateCM(positions);
  ublas::matrix<double> input = ublas::matrix<double>(6, 3);

  ublas::row(input, 0) = ANTVectorMath::CreateBoostVector(nt_left->GetVectorPosition());
  ublas::row(input, 1) = ANTVectorMath::CreateBoostVector(nt_left->GetVectorBackboneCenter());
  ublas::row(input, 2) = ANTVectorMath::CreateBoostVector(nt_left->GetVectorSidechainCenter());
  ublas::row(input, 3) = ANTVectorMath::CreateBoostVector(nt_right->GetVectorPosition());
  ublas::row(input, 4) = ANTVectorMath::CreateBoostVector(nt_right->GetVectorBackboneCenter());
  ublas::row(input, 5) = ANTVectorMath::CreateBoostVector(nt_right->GetVectorSidechainCenter());

  // Calculate new residue positions in this system
  // Rotate system so z goes in proper direction
  ublas::vector<double> new_z(3);
  new_z[0] = bs->direction_[0].getValue();
  new_z[1] = bs->direction_[1].getValue();
  new_z[2] = bs->direction_[2].getValue();
  new_z /= ublas::norm_2(new_z);
  // if normal_ or u_ are defined we already have the subspace
  ublas::matrix<double> subspace(0, 3);
  if (bs->normal_.norm() > 0) {
    ublas::vector<double> normal(3);
    normal[0] = bs->normal_[0].getValue();
    normal[1] = bs->normal_[1].getValue();
    normal[2] = bs->normal_[2].getValue();
    ublas::vector<double> u(3);
    if (bs->u_.norm() > 0) {
      u[0] = bs->u_[0].getValue();
      u[1] = bs->u_[1].getValue();
      u[2] = bs->u_[2].getValue();
    }
    else {
      u = ANTVectorMath::CrossProduct(new_z, normal);
    }
    ANTVectorMath::AddRowToMatrix(subspace, u);
    ANTVectorMath::AddRowToMatrix(subspace, normal);
  }
  else {
    subspace = ANTVectorMath::FindOrthogonalSubspace(new_z);
  }
  ANTVectorMath::AddRowToMatrix(subspace, new_z);
  // to calculate untwisted positions
  ublas::matrix<double> new_basisNoTwist = subspace;
  // apply rotation to basis (rotation has to be negative)
  double angle = -ANTVectorMath::DegToRad(initialAngleDegrees) - ANTVectorMath::DegToRad(bs->number_ * ANTConstants::BP_ROT);
  ublas::matrix<double> rot_mat = ANTVectorMath::MakeRotationMatrix(new_z, angle);
  ublas::matrix<double> new_basis = ANTVectorMath::ApplyTransformation(rot_mat, subspace);
  // Apply transformation
  new_basis = ublas::trans(new_basis);
  ublas::matrix<double> new_pos = ANTVectorMath::ApplyTransformation(new_basis, input);
  new_pos = ANTVectorMath::Translate(new_pos, t_vec);
  // Apply transformation for non twisted postions
  new_basisNoTwist = ublas::trans(new_basisNoTwist);
  ublas::matrix<double> new_posNoTwist = ANTVectorMath::ApplyTransformation(new_basisNoTwist, input);
  new_posNoTwist = ANTVectorMath::Translate(new_posNoTwist, t_vec);

  if (nt_l != nullptr) {
    nt_l->e1_ = ublas::column(new_basis, 0);
    nt_l->e2_ = ublas::column(new_basis, 1);
    nt_l->e3_ = ublas::column(new_basis, 2);

    // Set new residue positions
    SBPosition3 p_left = UblasToSBPosition(ublas::row(new_pos, 0));
    nt_l->SetPosition(p_left);
    SBPosition3 p_bb_left = UblasToSBPosition(ublas::row(new_pos, 1));
    nt_l->SetBackboneCenter(p_bb_left);
    SBPosition3 p_sc_left = UblasToSBPosition(ublas::row(new_pos, 2));
    nt_l->SetSidechainCenter(p_sc_left);
    // Set untwisted positions
    p_left = UblasToSBPosition(ublas::row(new_posNoTwist, 0));
    nt_l->SetPositionNoTwist(p_left);
    p_bb_left = UblasToSBPosition(ublas::row(new_posNoTwist, 1));
    nt_l->SetBackboneCenterNoTwist(p_bb_left);
    p_sc_left = UblasToSBPosition(ublas::row(new_posNoTwist, 2));
    nt_l->SetSidechainCenterNoTwist(p_sc_left);
  }
  
  if (nt_r != nullptr) {
    // right nt has a slightly different basis
    ublas::matrix<double> local_basis = ublas::identity_matrix<double>(3);
    local_basis.at_element(0, 0) = -1;
    local_basis.at_element(1, 1) = -1;
    local_basis.at_element(2, 2) = -1;
    auto basis_r = ublas::prod(new_basis, local_basis);
    //auto basis_r = ublas::prod(new_basis, ublas::identity_matrix<double>(3));
    nt_r->e1_ = ublas::column(basis_r, 0);
    nt_r->e2_ = ublas::column(basis_r, 1);
    nt_r->e3_ = ublas::column(basis_r, 2);

    // Set positions
    SBPosition3 p_right = UblasToSBPosition(ublas::row(new_pos, 3));
    nt_r->SetPosition(p_right);
    SBPosition3 p_bb_right = UblasToSBPosition(ublas::row(new_pos, 4));
    nt_r->SetBackboneCenter(p_bb_right);
    SBPosition3 p_sc_right = UblasToSBPosition(ublas::row(new_pos, 5));
    nt_r->SetSidechainCenter(p_sc_right);
    // Set untwisted positions
    p_right = UblasToSBPosition(ublas::row(new_posNoTwist, 3));
    nt_r->SetPositionNoTwist(p_right);
    p_bb_right = UblasToSBPosition(ublas::row(new_posNoTwist, 4));
    nt_r->SetBackboneCenterNoTwist(p_bb_right);
    p_sc_right = UblasToSBPosition(ublas::row(new_posNoTwist, 5));
    nt_r->SetSidechainCenterNoTwist(p_sc_right); 
  }
}

void DASBackToTheAtom::SetPositionLoopNucleotides(ANTBaseSegment* bs) {

  if (bs->GetCell()->GetType() == CellType::LoopPair) {
    ANTLoopPair * pair = static_cast<ANTLoopPair*>(bs->GetCell());
    ANTLoop * left = pair->GetLeftLoop();
    ANTLoop * right = pair->GetRightLoop();

    SBPosition3 posPrevLeft;
    SBPosition3 posPrevRight;
    SBPosition3 posNextLeft;
    SBPosition3 posNextRight;

    if (left != nullptr) {
      ANTNucleotide* ntPrevLeft = left->startNt_->prev_;
      ANTNucleotide* ntNextLeft = left->endNt_->next_;

      if (ntPrevLeft != nullptr) {
        posPrevLeft = ntPrevLeft->GetSBPosition();
      }
      else if (bs->previous_ != nullptr) {
        posPrevLeft = bs->previous_->GetSBPosition();
      }
      else {
        posPrevLeft = bs->GetSBPosition() - SBQuantity::nanometer(ANTConstants::BP_RISE) * bs->direction_;
      }

      if (ntNextLeft != nullptr) {
        posNextLeft = ntNextLeft->GetSBPosition();
      }
      else if (bs->next_ != nullptr) {
        posNextLeft = bs->next_->GetSBPosition();
      }
      else {
        posNextLeft = bs->GetSBPosition() + SBQuantity::nanometer(ANTConstants::BP_RISE) * bs->direction_;
      }

      left->PositionLoopNucleotides(posPrevLeft, posNextLeft);
    }

    if (right != nullptr) {
      ANTNucleotide* ntPrevRight = right->startNt_->prev_;
      ANTNucleotide* ntNextRight = right->endNt_->next_;

      if (ntPrevRight != nullptr) {
        posPrevRight = ntPrevRight->GetSBPosition();
      }
      else if (bs->previous_ != nullptr) {
        posPrevRight = bs->previous_->GetSBPosition();
      }
      else {
        posPrevRight = bs->GetSBPosition() - SBQuantity::nanometer(ANTConstants::BP_RISE) * bs->direction_;
      }

      if (ntNextRight != nullptr) {
        posNextRight = ntNextRight->GetSBPosition();
      }
      else if (bs->next_ != nullptr) {
        posNextRight = bs->next_->GetSBPosition();
      }
      else {
        posNextRight = bs->GetSBPosition() + SBQuantity::nanometer(ANTConstants::BP_RISE) * bs->direction_;
      }

      right->PositionLoopNucleotides(posPrevRight, posNextRight);
    }
  }
}

void DASBackToTheAtom::FindAtomsPositions(ANTBaseSegment* bs, ANTNucleotide * nt_l) {
  ANTNucleotide* nt_left;
  ANTNucleotide* nt_right;
  DNABlocks nt_type = nt_l->type_;

  // for DN_ we use DA_
  nt_left = da_dt_.first;
  nt_right = da_dt_.second;
  if (nt_type == DNABlocks::DA_) {
    nt_left = da_dt_.first;
    nt_right = da_dt_.second;
  }
  else if (nt_type == DNABlocks::DC_) {
    nt_left = dc_dg_.first;
    nt_right = dc_dg_.second;
  }
  else if (nt_type == DNABlocks::DG_) {
    nt_left = dg_dc_.first;
    nt_right = dg_dc_.second;
  }
  else if (nt_type == DNABlocks::DT_) {
    nt_left = dt_da_.first;
    nt_right = dt_da_.second;
  }

  // Save positions in matrix
  size_t cols = 3;
  size_t rows = nt_left->atomList_.size() + nt_right->atomList_.size();
  ublas::matrix<double> positions(rows, cols);
  int i = 0;
  for (auto ait = nt_left->atomList_.begin(); ait != nt_left->atomList_.end(); ++ait) {
    std::vector<double> a_coords = ait->second->GetVectorPosition();
    ublas::vector<double> ac_blas = ANTVectorMath::CreateBoostVector(a_coords);
    ublas::row(positions, i) = ac_blas;
    ++i;
  }
  for (auto ait = nt_right->atomList_.begin(); ait != nt_right->atomList_.end(); ++ait) {
    std::vector<double> a_coords = ait->second->GetVectorPosition();
    ublas::vector<double> ac_blas = ANTVectorMath::CreateBoostVector(a_coords);
    ublas::row(positions, i) = ac_blas;
    ++i;
  }
  
  // Calculate translation vector
  std::vector<double> sys_cm_pos = bs->GetVectorPosition(); // because atoms are fetched from bp, local coordinates refer to base pair c.o.m.
  ublas::vector<double> sys_cm = ANTVectorMath::CreateBoostVector(sys_cm_pos);
  ublas::vector<double> t_vec = sys_cm - ANTVectorMath::CalculateCM(positions);
  ublas::matrix<double> input = positions;
  // Apply global basis
  auto transf = nt_l->GetGlobalBasisTransformation();
  ublas::matrix<double> new_pos = ANTVectorMath::ApplyTransformation(transf, input);
  new_pos = ANTVectorMath::Translate(new_pos, t_vec);
  // Set new atom positions
  nt_left->CopyAtoms(*nt_l);
  for (auto ait = nt_l->atomList_.begin(); ait != nt_l->atomList_.end(); ++ait) {
    ANTAtom* atom = ait->second;
    SBPosition3 pos = UblasToSBPosition(ublas::row(new_pos, ait->first));
    atom->SetPosition(pos);
  }
}

void DASBackToTheAtom::FindAtomsPositions2D(ANTBaseSegment* bs, ANTNucleotide * nt_l) {
  ANTNucleotide* nt_left;
  ANTNucleotide* nt_right;
  DNABlocks nt_type = nt_l->type_;

  // for DN_ we use DA_
  nt_left = da_dt_.first;
  nt_right = da_dt_.second;
  if (nt_type == DNABlocks::DA_) {
    nt_left = da_dt_.first;
    nt_right = da_dt_.second;
  }
  else if (nt_type == DNABlocks::DC_) {
    nt_left = dc_dg_.first;
    nt_right = dc_dg_.second;
  }
  else if (nt_type == DNABlocks::DG_) {
    nt_left = dg_dc_.first;
    nt_right = dg_dc_.second;
  }
  else if (nt_type == DNABlocks::DT_) {
    nt_left = dt_da_.first;
    nt_right = dt_da_.second;
  }

  // Save positions in matrix
  size_t cols = 3;
  size_t rows = nt_left->atomList_.size() + nt_right->atomList_.size();
  ublas::matrix<double> positions(rows, cols);
  int i = 0;
  for (auto ait = nt_left->atomList_.begin(); ait != nt_left->atomList_.end(); ++ait) {
    std::vector<double> a_coords = ait->second->GetVectorPosition();
    ublas::vector<double> ac_blas = ANTVectorMath::CreateBoostVector(a_coords);
    ublas::row(positions, i) = ac_blas;
    ++i;
  }
  for (auto ait = nt_right->atomList_.begin(); ait != nt_right->atomList_.end(); ++ait) {
    std::vector<double> a_coords = ait->second->GetVectorPosition();
    ublas::vector<double> ac_blas = ANTVectorMath::CreateBoostVector(a_coords);
    ublas::row(positions, i) = ac_blas;
    ++i;
  }

  // Calculate translation vector
  std::vector<double> sys_cm_pos = bs->GetVectorPosition2D(); // because atoms are fetched from bp, local coordinates refer to base pair c.o.m.
  ublas::vector<double> sys_cm = ANTVectorMath::CreateBoostVector(sys_cm_pos);
  ublas::vector<double> t_vec = sys_cm - ANTVectorMath::CalculateCM(positions);
  ublas::matrix<double> input = positions;
  // Apply global basis
  auto transf = nt_l->GetGlobalBasisTransformation();
  ublas::matrix<double> new_pos = ANTVectorMath::ApplyTransformation(transf, input);
  new_pos = ANTVectorMath::Translate(new_pos, t_vec);
  // Set new atom positions
  nt_left->CopyAtoms(*nt_l);
  for (auto ait = nt_l->atomList_.begin(); ait != nt_l->atomList_.end(); ++ait) {
    ANTAtom* atom = ait->second;
    SBPosition3 pos = UblasToSBPosition(ublas::row(new_pos, ait->first));
    atom->SetPosition2D(pos);
  }
}

void DASBackToTheAtom::FindAtomsPositions1D(ANTBaseSegment * bs, ANTNucleotide * nt) {
  ANTNucleotide* nt_left;
  ANTNucleotide* nt_right;
  DNABlocks nt_type = nt->type_;

  // for DN_ we use DA_
  nt_left = da_dt_.first;
  nt_right = da_dt_.second;
  if (nt_type == DNABlocks::DA_) {
    nt_left = da_dt_.first;
    nt_right = da_dt_.second;
  }
  else if (nt_type == DNABlocks::DC_) {
    nt_left = dc_dg_.first;
    nt_right = dc_dg_.second;
  }
  else if (nt_type == DNABlocks::DG_) {
    nt_left = dg_dc_.first;
    nt_right = dg_dc_.second;
  }
  else if (nt_type == DNABlocks::DT_) {
    nt_left = dt_da_.first;
    nt_right = dt_da_.second;
  }

  // Save positions in matrix
  size_t cols = 3;
  size_t rows = nt_left->atomList_.size(); // +nt_right->atomList_.size();
  ublas::matrix<double> positions(rows, cols);
  int i = 0;
  for (auto ait = nt_left->atomList_.begin(); ait != nt_left->atomList_.end(); ++ait) {
    std::vector<double> a_coords = ait->second->GetVectorPosition();
    ublas::vector<double> ac_blas = ANTVectorMath::CreateBoostVector(a_coords);
    ublas::row(positions, i) = ac_blas;
    ++i;
  }
  //for (auto ait = nt_right->atomList_.begin(); ait != nt_right->atomList_.end(); ++ait) {
  //  std::vector<double> a_coords = ait->second->GetVectorPosition();
  //  ublas::vector<double> ac_blas = ANTVectorMath::CreateBoostVector(a_coords);
  //  ublas::row(positions, i) = ac_blas;
  //  ++i;
  //}

  // Calculate translation vector
  std::vector<double> sys_cm_pos = nt->GetVectorPosition1D();  // for 1-D
  double temp_x = sys_cm_pos[0];
  sys_cm_pos[0] = sys_cm_pos[1];
  sys_cm_pos[1] = temp_x;

  ublas::vector<double> sys_cm = ANTVectorMath::CreateBoostVector(sys_cm_pos);
  ublas::vector<double> t_vec = sys_cm - ANTVectorMath::CalculateCM(positions);
  ublas::matrix<double> input = positions;
  // Apply global basis
  auto transf = nt->GetGlobalBasisTransformation();

  ublas::matrix<double> new_pos = ANTVectorMath::ApplyTransformation(transf, input);
  new_pos = ANTVectorMath::Translate(new_pos, t_vec);
  // Set new atom positions
  nt_left->CopyAtoms(*nt);
  for (auto ait = nt->atomList_.begin(); ait != nt->atomList_.end(); ++ait) {
    ANTAtom* atom = ait->second;
    SBPosition3 pos = UblasToSBPosition(ublas::row(new_pos, ait->first));
    atom->SetPosition1D(pos);
  }
}

std::tuple<SBPosition3, SBPosition3, SBPosition3> DASBackToTheAtom::CalculateCenters(ANTNucleotide * nt) {
  SBPosition3 cm_bb = SBPosition3();
  SBPosition3 cm_sc = SBPosition3();
  SBPosition3 cm = SBPosition3();
  ublas::matrix<double> positions_bb = ublas::matrix<double>(0, 3);
  ublas::matrix<double> positions_sc = ublas::matrix<double>(0, 3);
  ublas::matrix<double> positions = ublas::matrix<double>(0, 3);
  for (auto ait = nt->atomList_.begin(); ait != nt->atomList_.end(); ++ait) {
    std::vector<double> a_coords = ait->second->GetVectorPosition();
    ublas::vector<double> ac_blas = ANTVectorMath::CreateBoostVector(a_coords);
    ANTVectorMath::AddRowToMatrix(positions, ac_blas);
    if (ait->second->IsInBackbone()) {
      ANTVectorMath::AddRowToMatrix(positions_bb, ac_blas);
    }
    else {
      ANTVectorMath::AddRowToMatrix(positions_sc, ac_blas);
    }
  }

  ublas::vector<double> cm_bb_vec = ANTVectorMath::CalculateCM(positions_bb);
  ublas::vector<double> cm_sc_vec = ANTVectorMath::CalculateCM(positions_sc);
  ublas::vector<double> cm_vec = ANTVectorMath::CalculateCM(positions);
  cm_bb = UblasToSBPosition(cm_bb_vec);
  cm_sc = UblasToSBPosition(cm_sc_vec);
  cm = UblasToSBPosition(cm_vec);
  return std::make_tuple(cm, cm_bb, cm_sc);
}

SBPosition3 DASBackToTheAtom::UblasToSBPosition(ublas::vector<double> vec) {
  // we assume vec is in picometers!
  std::vector<double> pos = ANTVectorMath::CreateStdVector(vec);
  SBPosition3 res = SBPosition3();
  res[0] = SBQuantity::angstrom(pos[0] * 0.01);
  res[1] = SBQuantity::angstrom(pos[1] * 0.01);
  res[2] = SBQuantity::angstrom(pos[2] * 0.01);

  return res;
}

void DASBackToTheAtom::SetReferenceFrame(NtPair pair) {
  ANTNucleotide* nt_left = pair.first;
  ANTNucleotide* nt_right = pair.second;
  // Fetch needed points
  std::vector<double> c1_prime_left_std;
  std::vector<double> c1_prime_right_std;
  std::vector<double> c5_prime_left_std;
  std::vector<double> c5_prime_right_std;
  std::vector<double> c3_prime_left_std;
  std::vector<double> c3_prime_right_std;

  auto atoms = nt_left->atomList_;
  std::vector<std::vector<double>> positions;
  std::vector<int> sidechain_indices;
  int idx = 0;
  for (auto ait = atoms.begin(); ait != atoms.end(); ++ait) {
    ANTAtom* a = ait->second;
    positions.push_back(a->GetVectorPosition());
    if (a->name_ == "C1'") {
      c1_prime_left_std = a->GetVectorPosition();
    }
    if (a->name_ == "C5'") {
      c5_prime_left_std = a->GetVectorPosition();
    }
    if (a->name_ == "C3'") {
      c3_prime_left_std = a->GetVectorPosition();
    }
    if (!a->IsInBackbone()) {
      sidechain_indices.push_back(idx);
    }
    ++idx;
  }
  atoms = nt_right->atomList_;
  for (auto ait = atoms.begin(); ait != atoms.end(); ++ait) {
    ANTAtom* a = ait->second;
    positions.push_back(a->GetVectorPosition());
    if (a->name_ == "C1'") {
      c1_prime_right_std = a->GetVectorPosition();
    }
    if (a->name_ == "C5'") {
      c5_prime_right_std = a->GetVectorPosition();
    }
    if (a->name_ == "C3'") {
      c3_prime_right_std = a->GetVectorPosition();
    }
    if (!a->IsInBackbone()) {
      sidechain_indices.push_back(idx);
    }
    ++idx;
  }

  ublas::vector<double> c5_prime_left = ANTVectorMath::CreateBoostVector(c5_prime_left_std);
  ublas::vector<double> c3_prime_left = ANTVectorMath::CreateBoostVector(c3_prime_left_std);
  ublas::vector<double> c1_prime_left = ANTVectorMath::CreateBoostVector(c1_prime_left_std);
  ublas::vector<double> c1_prime_right = ANTVectorMath::CreateBoostVector(c1_prime_right_std);

  // Center system
  ublas::matrix<double> positions_matrix = ANTVectorMath::CreateBoostMatrix(positions);
  ublas::matrix<double> new_positions = ANTVectorMath::CenterSystem(positions_matrix);

  // Fit sidechain to plane to calculate z (we get directions for left nt)
  ublas::matrix<double> sidechain_positions(0, 3);
  for (auto &it : sidechain_indices) {
    ANTVectorMath::AddRowToMatrix(sidechain_positions, ublas::row(new_positions, it));
  }
  ublas::vector<double> z = ANTVectorMath::CalculatePlane(sidechain_positions);
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
  ublas::vector<double> x = ANTVectorMath::CrossProduct(y, z);
  x /= ublas::norm_2(x);
  // we want to transform all positions so local base is the standard basis
  ublas::matrix<double> transf(3, 3);
  ublas::column(transf, 0) = x;
  ublas::column(transf, 1) = y;
  ublas::column(transf, 2) = z;
  ublas::matrix<double> inv_transf = ANTVectorMath::InvertMatrix(transf);

  ublas::matrix<double> coords = ANTVectorMath::ApplyTransformation(inv_transf, new_positions);

  atoms = nt_left->atomList_;
  int r_id = SetAtomsPositions(atoms, coords, 0);
  atoms = nt_right->atomList_;
  r_id = SetAtomsPositions(atoms, coords, r_id);
}

int DASBackToTheAtom::SetAtomsPositions(std::map<int, ANTAtom *> &atoms, ublas::matrix<double> new_positions, int r_id) {
  for (auto ait = atoms.begin(); ait != atoms.end(); ++ait) {
    ublas::vector<double> new_pos = ublas::row(new_positions, r_id);
    std::vector<double> np = ANTVectorMath::CreateStdVector(new_pos);
    SBPosition3 pos = SBPosition3();
    pos[0] = SBQuantity::angstrom(np[0] * 0.01); // we need to convert from picometers
    pos[1] = SBQuantity::angstrom(np[1] * 0.01);
    pos[2] = SBQuantity::angstrom(np[2] * 0.01);
    ait->second->SetPosition(pos);
    ++r_id;
  }

  return r_id;
}

void DASBackToTheAtom::SetNucleotidesPostions(ANTPart& part) {
  auto doubleStrands = part.GetDoubleStrands();
  for (auto &s : doubleStrands) {
    ANTDoubleStrand* ds = s.second;
    SetDoubleStrandPositions(*ds);
  }
}

void DASBackToTheAtom::SetNucleotidesPostionsLegacy(ANTPart & part) {
  // Set position of all paired nucleotides
  auto strands = part.GetSingleStrands();
  std::vector<ANTNucleotide*> done_nts;
  for (auto &s : strands) {
    ANTSingleStrand* ss = s.second;
    for (auto nit = ss->nucleotides_.begin(); nit != ss->nucleotides_.end(); ++nit) {
      if (std::find(done_nts.begin(), done_nts.end(), nit->second) == done_nts.end()) {
        SetNucleotidePosition(part, nit->second, true);
        done_nts.push_back(nit->second);
      }
    }
  }
}

void DASBackToTheAtom::SetNucleotidesPostions(ANTPart& origami, ANTSingleStrand & single_strand) {
  for (auto nit = single_strand.nucleotides_.begin(); nit != single_strand.nucleotides_.end(); ++nit) {
    SetNucleotidePosition(origami, nit->second, true);
  }
}

void DASBackToTheAtom::SetAllAtomsPostions(ANTPart & origami) {
  auto strands = origami.GetSingleStrands();
  NtSegments bases = origami.GetNtSegmentMap();
  for (auto sit = strands.begin(); sit != strands.end(); ++sit) {
    ANTSingleStrand* chain = sit->second;
    for (auto & nit : chain->nucleotides_) {
      ANTNucleotide* bs_nt = nit.second;
      /*if (!chain->isScaffold_) {
        bs_nt = bs_nt->pair_;
      }*/
      if (bs_nt->BaseIsSet()) FindAtomsPositions(bases.at(bs_nt), nit.second);
    }
  }
}

void DASBackToTheAtom::SetAllAtomsPostions2D(ANTPart & origami) {
  auto strands = origami.GetSingleStrands();
  NtSegments bases = origami.GetNtSegmentMap();
  for (auto sit = strands.begin(); sit != strands.end(); ++sit) {
    ANTSingleStrand* chain = sit->second;
    for (auto & nit : chain->nucleotides_) {
      ANTNucleotide* bs_nt = nit.second;
      //if (!chain->isScaffold_) {
      //  bs_nt = bs_nt->pair_;
      //}
      if (bs_nt->BaseIsSet()) FindAtomsPositions2D(bases.at(bs_nt), nit.second);
    }
  }
}

void DASBackToTheAtom::SetAllAtomsPostions1D(ANTPart & origami) {
  auto strands = origami.GetSingleStrands();
  NtSegments bases = origami.GetNtSegmentMap();
  for (auto sit = strands.begin(); sit != strands.end(); ++sit) {
    ANTSingleStrand* chain = sit->second;
    for (auto & nit : chain->nucleotides_) {
      ANTNucleotide* bs_nt = nit.second;
      //if (!chain->isScaffold_) {
      //  bs_nt = bs_nt->pair_;
      //}
      if (bs_nt->BaseIsSet()) FindAtomsPositions1D(bases.at(bs_nt), nit.second);
    }
  }
}

void DASBackToTheAtom::RotateNucleotide(ANTNucleotide * nt, double angle, bool set_pair = false) {

  ublas::matrix<double> subspace(3, 3);
  ublas::row(subspace, 0) = nt->e1_;
  ublas::row(subspace, 1) = nt->e2_;
  ublas::row(subspace, 2) = nt->e3_;

  ublas::matrix<double> rot_mat = ANTVectorMath::MakeRotationMatrix(nt->e3_, angle);
  ublas::matrix<double> new_basis = ANTVectorMath::ApplyTransformation(rot_mat, subspace);

  nt->e1_ = ublas::row(new_basis, 0);
  nt->e2_ = ublas::row(new_basis, 1);
  nt->e3_ = ublas::row(new_basis, 2);

  // rotation matrix is defined with respect to origin
  // we need first to translate coordinates
  std::vector<double> sys_cm_pos = nt->GetVectorPosition();
  std::vector<double> sys_bb_pos = nt->GetVectorBackboneCenter();
  std::vector<double> sys_sc_pos = nt->GetVectorSidechainCenter();
  ublas::vector<double> sys_cm = ANTVectorMath::CreateBoostVector(sys_cm_pos);
  ublas::vector<double> sys_bb = ANTVectorMath::CreateBoostVector(sys_bb_pos);
  ublas::vector<double> sys_sc = ANTVectorMath::CreateBoostVector(sys_sc_pos);

  ublas::matrix<double> positions(0, 3);
  ANTVectorMath::AddRowToMatrix(positions, sys_cm);
  ANTVectorMath::AddRowToMatrix(positions, sys_bb);
  ANTVectorMath::AddRowToMatrix(positions, sys_sc);

  positions = ANTVectorMath::Translate(positions, -sys_cm);
  auto transf = nt->GetGlobalBasisTransformation();
  ublas::matrix<double> new_pos = ANTVectorMath::ApplyTransformation(transf, positions);
  new_pos = ANTVectorMath::Translate(new_pos, sys_cm);

  if (set_pair && nt->pair_ != nullptr) {
    RotateNucleotide(nt->pair_, angle, false);
  }
}

void DASBackToTheAtom::DisplayDNABlock(std::string block) {
  SBPointer<SBStructuralModel> structuralModel = new SBMStructuralModel();
  structuralModel->setName("test");
  SBPointer<SBChain> chain = new SBChain();
  chain->setName("test chain");
  if (block == "A") {
    SBPointer<SBResidue> res = da_->CreateSBResidue();
    chain->addChild(res());
  }
  else if (block == "C") {
    SBPointer<SBResidue> res = dc_->CreateSBResidue();
    chain->addChild(res());
  }
  else if (block == "G") {
    SBPointer<SBResidue> res = dg_->CreateSBResidue();
    chain->addChild(res());
  }
  else if (block == "T") {
    SBPointer<SBResidue> res = dt_->CreateSBResidue();
    chain->addChild(res());
  }
  else if (block == "AT") {
    SBPointer<SBResidue> res1 = da_dt_.first->CreateSBResidue();
    SBPointer<SBResidue> res2 = da_dt_.second->CreateSBResidue();
    chain->addChild(res1());
    SBPointer<SBChain> chain2 = new SBChain();
    chain2->setName("test chain 2");
    chain2->addChild(res2());
    structuralModel->getStructuralRoot()->addChild(chain2());
  }
  else if (block == "CG") {
    SBPointer<SBResidue> res1 = dc_dg_.first->CreateSBResidue();
    SBPointer<SBResidue> res2 = dc_dg_.second->CreateSBResidue();
    chain->addChild(res1());
    SBPointer<SBChain> chain2 = new SBChain();
    chain2->setName("test chain 2");
    chain2->addChild(res2());
    structuralModel->getStructuralRoot()->addChild(chain2());
  }
  else if (block == "GC") {
    SBPointer<SBResidue> res1 = dg_dc_.first->CreateSBResidue();
    SBPointer<SBResidue> res2 = dg_dc_.second->CreateSBResidue();
    chain->addChild(res1());
    SBPointer<SBChain> chain2 = new SBChain();
    chain2->setName("test chain 2");
    chain2->addChild(res2());
    structuralModel->getStructuralRoot()->addChild(chain2());
  }
  else if (block == "TA") {
    SBPointer<SBResidue> res1 = dt_da_.first->CreateSBResidue();
    SBPointer<SBResidue> res2 = dt_da_.second->CreateSBResidue();
    chain->addChild(res1());
    SBPointer<SBChain> chain2 = new SBChain();
    chain2->setName("test chain 2");
    chain2->addChild(res2());
    structuralModel->getStructuralRoot()->addChild(chain2());
  }
  structuralModel->getStructuralRoot()->addChild(chain());
  SAMSON::beginHolding("Add model");
  structuralModel->create();
  SAMSON::getActiveLayer()->addChild(structuralModel());
  SAMSON::endHolding();
}

void DASBackToTheAtom::LoadNucleotides() {  
  for (auto it = residue_names_.begin(); it != residue_names_.end(); ++it) {
    if (it->right == 'N') continue;
    std::string name(1, it->right);
    std::string nt_source = SB_ELEMENT_PATH + "/Data/" + name + ".pdb";
    ANTNucleotide* nt = ParsePDB(nt_source);
    nt->type_ = it->left;
    auto atoms = nt->atomList_;
    std::vector<std::vector<double>> positions;
    std::vector<std::vector<double>> base_plane;
    for (auto ait = atoms.begin(); ait != atoms.end(); ++ait) {
      ANTAtom* a = ait->second;
      auto coords = a->GetVectorPosition();
      positions.push_back(coords);
    }
    // Set same coordinate system for all nucleotides
    ublas::matrix<double> positions_matrix = ANTVectorMath::CreateBoostMatrix(positions);
    ublas::matrix<double> new_positions = ANTVectorMath::CenterSystem(positions_matrix);
    int r_id = SetAtomsPositions(atoms, new_positions, 0);
    // nt->SetReferenceFrame();
    auto nt_cms = CalculateCenters(nt);
    nt->SetPosition(std::get<0>(nt_cms));
    nt->SetBackboneCenter(std::get<1>(nt_cms));
    nt->SetSidechainCenter(std::get<2>(nt_cms));

    if (it->left == DNABlocks::DA_) {
      da_ = nt;
    }
    else if (it->left == DNABlocks::DC_) {
      dc_ = nt;
    }
    else if (it->left == DNABlocks::DG_) {
      dg_ = nt;
    }
    else if (it->left == DNABlocks::DT_) {
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
    ANTNucleotide* nt_left = nt_pair.first;
    ANTNucleotide* nt_right = nt_pair.second;
    nt_left->type_ = it->left.first;
    nt_right->type_ = it->left.second;

    SetReferenceFrame(nt_pair);

    // Set positions
    auto nt_right_cms = CalculateCenters(nt_right);
    auto nt_left_cms = CalculateCenters(nt_left);
    nt_right->SetPosition(std::get<0>(nt_right_cms));
    nt_left->SetPosition(std::get<0>(nt_left_cms));
    nt_right->SetBackboneCenter(std::get<1>(nt_right_cms));
    nt_left->SetBackboneCenter(std::get<1>(nt_left_cms));
    nt_right->SetSidechainCenter(std::get<2>(nt_right_cms));
    nt_left->SetSidechainCenter(std::get<2>(nt_left_cms));


    if (it->left == std::make_pair(DA_, DT_)) {
      da_dt_ = nt_pair;
    }
    else if (it->left == std::make_pair(DC_, DG_)) {
      dc_dg_ = nt_pair;
    }
    else if (it->left == std::make_pair(DG_, DC_)) {
      dg_dc_ = nt_pair;
    }
    else if (it->left == std::make_pair(DT_, DA_)) {
      dt_da_ = nt_pair;
    }
  }
}

std::pair<ANTNucleotide*, ANTNucleotide*> DASBackToTheAtom::ParseBasePairPDB(std::string source) {
  std::ifstream file(source.c_str(), std::ios::in);
  if (!file) {
    std::cout << "Could not open file " << source << std::endl;
  }

  ANTNucleotide* nt_left = new ANTNucleotide();
  ANTNucleotide* nt_right = new ANTNucleotide();

  char line[1024];
  int atom_id_left = 0;
  int atom_id_right = 0;
  std::map<int, ANTAtom*> atoms_by_pdb_id;
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

      ANTAtom* atom = new ANTAtom();
      std::string name = s.substr(12, 4);
      boost::trim(name);
      atom->name_ = name;
      std::string x = s.substr(30, 8);
      std::string y = s.substr(38, 8);
      std::string z = s.substr(46, 8);
      SBPosition3 pos = SBPosition3();
      pos[0] = SBQuantity::angstrom(std::stod(x));
      pos[1] = SBQuantity::angstrom(std::stod(y));
      pos[2] = SBQuantity::angstrom(std::stod(z));
      atom->SetPosition(pos);
      if (r_num_f == -1) {
        atom->id_ = atom_id_left;
        ++atom_id_left;
        atom->nt_ = nt_left;
        nt_left->AddAtom(atom);
      }
      else {
        atom->id_ = atom_id_right;
        ++atom_id_right;
        atom->nt_ = nt_right;
        nt_right->AddAtom(atom);
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
      ANTAtom* atom = atoms_by_pdb_id.at(aid);
      for (auto it = neighbours.begin(); it != neighbours.end(); ++it) {
        if (*it > 0) {
          ANTAtom* at = atoms_by_pdb_id.at(*it);
          atom->connections_.push_back(at);
        }
      }
    }
  }

  return std::make_pair(nt_left, nt_right);
}

ANTNucleotide* DASBackToTheAtom::ParsePDB(std::string source) {
  std::ifstream file(source.c_str(), std::ios::in);
  if (!file) {
    std::cout << "Could not open file " << source << std::endl;
  }

  ANTNucleotide* nt = new ANTNucleotide();

  char line[1024];
  int atom_id = 1;

  while (file.good()) {
    file.getline(line, 1023);
    std::string s = line;
    std::string record_name = s.substr(0, 6);
    if (record_name == "ATOM  ") {
      ANTAtom* atom = new ANTAtom();
      atom->id_ = atom_id;
      ++atom_id;
      std::string name = s.substr(12, 4);
      boost::trim(name);
      atom->name_ = name;
      atom->nt_ = nt;
      std::string x = s.substr(30, 8);
      std::string y = s.substr(38, 8);
      std::string z = s.substr(46, 8);
      SBPosition3 pos = SBPosition3();
      pos[0] = SBQuantity::angstrom(std::stod(x));
      pos[1] = SBQuantity::angstrom(std::stod(y));
      pos[2] = SBQuantity::angstrom(std::stod(z));
      atom->SetPosition(pos);
      nt->AddAtom(atom);
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
      ANTAtom* atom = nt->GetAtom(aid);
      for (auto it = neighbours.begin(); it != neighbours.end(); ++it) {
        if (*it > 0) {
          ANTAtom* at = nt->GetAtom(*it);
          atom->connections_.push_back(at);
        }
      }
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
//    ANTNucleotide* nt1 = bs->nt_;
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
//std::vector<ANTNucleotide*> BackToTheAtom::GenerateAllAtomModel(ANTEdge* edge, std::string sequence) {
//
//  std::vector<ANTNucleotide*> nts;
//
//  std::map<DNABlocks, ANTNucleotide*> nucleotides = LoadNucleotides();
//
//  std::vector<double> start{ 0.0, 0.0, 0.0 };
//
//  SequencePositions* positions = CalculatePositions(edge, 0); // 0 set only for the moment
//  std::vector<DNABlocks> seq = FormatSequence(sequence);
//
//  ublas::vector<double> p = ANTVectorMath::CreateBoostVector(edge->halfEdge_->source_->GetVectorCoordinates());
//  ublas::vector<double> q = ANTVectorMath::CreateBoostVector(edge->halfEdge_->pair_->source_->GetVectorCoordinates());
//  ublas::vector<double> v_dir = ANTVectorMath::DirectionVector(p, q);
//
//  for (auto pit = positions->begin(); pit != positions->end(); ++pit) {
//    BasePositions b_pos = pit->second;
//    for (auto bit = b_pos.begin(); bit != b_pos.end(); ++bit) {
//      int p = bit->first;
//      std::vector<double> pos = bit->second;
//      ANTNucleotide* res = nucleotides.at(seq[p]);
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
//std::vector<ANTNucleotide*> BackToTheAtom::GenerateAllAtomModel(SBPosition3 s, SBPosition3 e, std::string sequence) {
//  std::vector<ANTNucleotide*> nts;
//
//  std::map<DNABlocks, ANTNucleotide*> nucleotides = LoadNucleotides();
//
//  std::vector<double> start{ 0.0, 0.0, 0.0 };
//  // Divide by 10000 to match units (??)
//  std::vector<double> s_p{ s[0].getValue(), s[1].getValue(), s[2].getValue() };
//  std::vector<double> e_p{ e[0].getValue(), e[1].getValue(), e[2].getValue() };
//
//  SequencePositions* positions = CalculatePositions(s_p, e_p, sequence.size());
//  std::vector<DNABlocks> seq = FormatSequence(sequence);
//
//  ublas::vector<double> p = ANTVectorMath::CreateBoostVector(s_p) / 100.0;
//  ublas::vector<double> q = ANTVectorMath::CreateBoostVector(e_p) / 100.0;
//  ublas::vector<double> v_dir = ANTVectorMath::DirectionVector(p, q);
//
//  for (auto pit = positions->begin(); pit != positions->end(); ++pit) {
//    BasePositions b_pos = pit->second;
//    for (auto bit = b_pos.begin(); bit != b_pos.end(); ++bit) {
//      int pid = bit->first;
//      std::vector<double> pos = bit->second;
//      ANTNucleotide* res = nucleotides.at(seq[pid]);
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
//void BackToTheAtom::RotateE3(ANTNucleotide* residue, ublas::vector<double> v_dir) {
//  ublas::vector<double> e3 = ANTVectorMath::CreateBoostVector(residue->GetE3());
//  ublas::vector<double> v = ANTVectorMath::CrossProduct(e3, v_dir);
//  ublas::matrix<double> m_skew = ANTVectorMath::SkewMatrix(v);
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
//void BackToTheAtom::RotateCoordinateSystem(ANTNucleotide* residue, ublas::vector<double> axis, double degree) {
//  double rot_bp = ANTVectorMath::DegToRad(degree);
//  ublas::matrix<double> rot_mat = ANTVectorMath::MakeRotationMatrix(axis, rot_bp);
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
//  ublas::vector<double> p = ANTVectorMath::CreateBoostVector(s_p) / 100.0;
//  ublas::vector<double> q = ANTVectorMath::CreateBoostVector(e_p) / 100.0;
//  ublas::vector<double> v_dir = ANTVectorMath::DirectionVector(p, q);
//
//  if (length < 0) {
//    std::exit(EXIT_FAILURE);
//  }
//
//  BasePositions b_pos;
//  for (int i = 0; i < length; ++i) {
//    ublas::vector<double> pos = p + (i+1)*v_dir*bp_rise_;
//    b_pos.insert(std::make_pair(i, ANTVectorMath::CreateStdVector(pos)));
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
//  ublas::vector<double> p = ANTVectorMath::CreateBoostVector(edge->halfEdge_->source_->GetVectorCoordinates());
//  ublas::vector<double> q = ANTVectorMath::CreateBoostVector(edge->halfEdge_->pair_->source_->GetVectorCoordinates());
//  ublas::vector<double> v_dir = ANTVectorMath::DirectionVector(p, q);
//
//  if (length < 0) {
//    std::exit(EXIT_FAILURE);
//  }
//
//  BasePositions b_pos;
//  for (int i = 0; i < length; ++i) {
//    ublas::vector<double> pos = p + v_dir*bp_rise_;
//    b_pos.insert(std::make_pair(i, ANTVectorMath::CreateStdVector(pos)));
//  }
//  positions->insert(std::make_pair(edge, b_pos));
//
//  return positions;
//}
//
//ANTNucleotide* BackToTheAtom::ParsePDB(std::string source) {
//  std::ifstream file(source.c_str(), std::ios::in);
//  if (!file) {
//    std::cout << "Could not open file " << source << std::endl;
//  }
//
//  ANTNucleotide* nt = new ANTNucleotide();
//
//  char line[1024];
//  int atom_id = 1;
//
//  while (file.good()) {
//    file.getline(line, 1023);
//    std::string s = line;
//    std::string record_name = s.substr(0, 6);
//    if (record_name == "ATOM  ") {
//      ANTAtom* atom = new ANTAtom();
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
//      ANTAtom* atom = nt->GetAtom(aid);
//      for (auto it = neighbours.begin(); it != neighbours.end(); ++it) {
//        if (*it > 0) {
//          ANTAtom* at = nt->GetAtom(*it);
//          atom->connections_.push_back(at);
//        }
//      }
//    }
//  }
//
//  return nt;
//}
//
//std::map<DNABlocks, ANTNucleotide*> BackToTheAtom::LoadNucleotides() {
//  std::map<DNABlocks, ANTNucleotide*> nucleotides;
//  std::map<DNABlocks, std::string> nts{ { DA_, "DA" }, { DT_, "DT" }, { DC_, "DC" }, { DG_, "DG" } };
//
//  for (auto it = nts.begin(); it != nts.end(); ++it) {
//    std::string nt_source = SB_ELEMENT_PATH + "/Data/" + it->second + ".pdb";
//    ANTNucleotide* nt = ParsePDB(nt_source);
//    nt->type_ = it->first;
//    auto atoms = nt->atomList_;
//    std::vector<std::vector<double>> positions;
//    for (auto ait = atoms.begin(); ait != atoms.end(); ++ait) {
//      ANTAtom* a = ait->second;
//      positions.push_back(a->GetVectorPosition());
//    }
//    ublas::matrix<double> positions_matrix = ANTVectorMath::CreateBoostMatrix(positions);
//    ublas::vector<double> r = ANTVectorMath::CalculateCM(positions_matrix);
//    for (auto ait = atoms.begin(); ait != atoms.end(); ++ait) {
//      std::vector<double> coords = ait->second->GetVectorPosition();
//      ublas::vector<double> c = ANTVectorMath::CreateBoostVector(coords);
//      ublas::vector<double> new_pos = c + r;
//      std::vector<double> np = ANTVectorMath::CreateStdVector(new_pos);
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