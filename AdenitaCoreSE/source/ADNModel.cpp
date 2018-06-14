#include "ADNModel.hpp"

ADNAtom::ADNAtom(const ADNAtom & other) : SBAtom(other), Identifiable(other)
{
  *this = other;
}

ADNAtom & ADNAtom::operator=(const ADNAtom & other)
{
  SBAtom::operator =(other);
  Identifiable::operator =(other);

  return *this;
}

std::string const & ADNAtom::GetName() const
{
  return getName();
}

void ADNAtom::SetName(const std::string & name)
{
  setName(name);
}

Position3D const & ADNAtom::GetPosition() const
{
  return getPosition();
}

void ADNAtom::SetPosition(Position3D const & newPosition)
{
  setPosition(newPosition);
}

bool ADNAtom::IsInBackbone() 
{
  return isFromNucleicAcidBackbone();
}

ADNNucleotide::ADNNucleotide(const ADNNucleotide & other) : Identifiable(other), PositionableSB(other), SBResidue(other), Orientable(other)
{
  *this = other;
}

ADNNucleotide & ADNNucleotide::operator=(const ADNNucleotide & other) {
  PositionableSB::operator =(other);
  Identifiable::operator =(other);
  Orientable::operator =(other);
  SBResidue::operator =(other);

  return *this;
}

void ADNNucleotide::SetType(DNABlocks t)
{
  setResidueType(t);
}

DNABlocks ADNNucleotide::GetType()
{
  return getResidueType();
}

void ADNNucleotide::SetPair(ADNPointer<ADNNucleotide> nt)
{
  pair_ = ADNWeakPointer<ADNNucleotide>(nt);
}

ADNPointer<ADNNucleotide> ADNNucleotide::GetPair()
{
  return pair_;
}

ADNPointer<ADNNucleotide> ADNNucleotide::GetPrev()
{
  auto p = static_cast<ADNNucleotide*>(getPreviousNucleicAcid());
  return ADNPointer<ADNNucleotide>(p);
}

ADNPointer<ADNNucleotide> ADNNucleotide::GetNext()
{
  auto p = static_cast<ADNNucleotide*>(getNextNucleicAcid());
  return ADNPointer<ADNNucleotide>(p);
}

ADNPointer<ADNSingleStrand> ADNNucleotide::GetStrand()
{
  auto p = static_cast<ADNSingleStrand*>(getParent());
  return ADNPointer<ADNSingleStrand>(p);
}

void ADNNucleotide::SetBaseSegment(ADNPointer<ADNBaseSegment> bs) {
  bs_ = ADNWeakPointer<ADNBaseSegment>(bs);
}

std::string const & ADNNucleotide::GetName() const
{
  return getName();
}

void ADNNucleotide::SetName(const std::string & name)
{
  setName(name);
}

void ADNNucleotide::AddAtom(NucleotideGroup g, ADNPointer<ADNAtom> a)
{
  if (g == SBNode::Type::Backbone) {
    ADNPointer<ADNBackbone> bb = GetBackbone();
    bb->AddAtom(a);
  }
  else if (g == SBNode::Type::SideChain) {
    ADNPointer<ADNSidechain> sc = GetSidechain();
    sc->AddAtom(a);
  }
}

void ADNNucleotide::DeleteAtom(NucleotideGroup g, ADNPointer<ADNAtom> a)
{
  if (g == SBNode::Type::Backbone) {
    ADNPointer<ADNBackbone> bb = GetBackbone();
    bb->DeleteAtom(a);
  }
  else if (g == SBNode::Type::SideChain) {
    ADNPointer<ADNSidechain> sc = GetSidechain();
    sc->DeleteAtom(a);
  }
}

CollectionMap<ADNAtom> ADNNucleotide::GetAtoms()
{
  auto bb = GetBackbone();
  auto sc = GetSidechain();
  auto bbAtoms = bb->GetAtoms();
  auto scAtoms = sc->GetAtoms();

  auto atoms = bbAtoms;
  SB_FOR(ADNPointer<ADNAtom> a, scAtoms) atoms.addReferenceTarget(a());

  return atoms;
}

ublas::matrix<double> ADNNucleotide::GetGlobalBasisTransformation() {
  ublas::matrix<double> transf(3, 3, 0.0);
  ublas::column(transf, 0) = GetE1();
  ublas::column(transf, 1) = GetE2();
  ublas::column(transf, 2) = GetE3();
  return transf;
}

ADNPointer<ADNBaseSegment> ADNNucleotide::GetBaseSegment() {
  return bs_;
}

End ADNNucleotide::GetEnd()
{
  return end_;
}

void ADNNucleotide::SetEnd(End e)
{
  end_ = e;
}

ADNPointer<ADNBackbone> ADNNucleotide::GetBackbone()
{
  auto bb = static_cast<ADNBackbone*>(getBackbone());
  return ADNPointer<ADNBackbone>(bb);
}

ADNPointer<ADNSidechain> ADNNucleotide::GetSidechain()
{
  auto sc = static_cast<ADNSidechain*>(getSideChain());
  return ADNPointer<ADNSidechain>(sc);
}

void ADNNucleotide::SetSidechainPosition(Position3D pos)
{
  auto sc = GetSidechain();
  sc->SetPosition(pos);
}

Position3D ADNNucleotide::GetSidechainPosition()
{
  auto sc = GetSidechain();
  return sc->GetPosition();
}

void ADNNucleotide::SetBackbonePosition(Position3D pos)
{
  auto bb = GetBackbone();
  bb->SetPosition(pos);
}

Position3D ADNNucleotide::GetBackbonePosition()
{
  auto bb = GetBackbone();
  return bb->GetPosition();
}

bool ADNNucleotide::GlobalBaseIsSet() {
  bool set = false;

  ublas::matrix<double> mat(3, 3);
  ublas::column(mat, 0) = GetE1();
  ublas::column(mat, 1) = GetE2();
  ublas::column(mat, 2) = GetE3();

  if (!ADNVectorMath::IsNearlyZero(ADNVectorMath::Determinant(mat))) set = true;

  return set;
}

//void ADNNucleotide::CopyAtoms(ADNPointer<ADNNucleotide> target) {
//
//  ADNPointer<ADNBackbone> bb(new ADNBackbone());
//  ADNPointer<ADNSidechain> sc(new ADNSidechain());
//
//  std::map<ADNPointer<ADNAtom>, ADNPointer<ADNAtom>> atomMap;
//
//  ADNPointer<ADNBackbone> bbO = GetBackbone();
//  for (auto ait : bbO->GetAtoms()) {
//    auto oat = ait.second;
//    ADNPointer<ADNAtom> atom(new ADNAtom(*oat));
//    AddAtom(bb, atom);
//    atomMap.insert(std::make_pair(oat, atom));
//  }
//  ADNPointer<ADNSidechain> scO = GetSidechain();
//  for (auto ait : scO->GetAtoms()) {
//    auto oat = ait.second;
//    ADNPointer<ADNAtom> atom(new ADNAtom(*oat));
//    AddAtom(sc, atom);
//    atomMap.insert(std::make_pair(oat, atom));
//  }
//
//  auto bonds = GetBonds();
//  for (auto b : bonds) {
//    ADNPointer<ADNAtom> b1 = b.first;
//    ADNPointer<ADNAtom> b2 = b.second;
//    auto newB1 = atomMap.at(b1);
//    auto newB2 = atomMap.at(b2);
//    target->AddBond(newB1, newB2);
//  }
//
//  target->SetBackbone(bb);
//  target->SetSidechain(sc);
//}

//ADNSingleStrand::ADNSingleStrand(int numNts) : Nameable(), Identifiable(), Collection<ADNNucleotide>() 
//{
//  for (int i = 0; i < numNts; ++i) {
//    ADNPointer<ADNNucleotide> nt = ADNPointer<ADNNucleotide>(new ADNNucleotide());;
//    nt->SetId(GetLastKey() + 1);
//    AddNucleotideThreePrime(nt);
//  }
//}

//ADNSingleStrand::ADNSingleStrand(std::vector<ADNPointer<ADNNucleotide>> nts) : Nameable(), Identifiable(), Collection<ADNNucleotide>()
//{
//  for (auto nt: nts) {
//    nt->SetId(GetLastKey() + 1);
//    AddNucleotideThreePrime(nt);
//  }
//}

ADNSingleStrand::ADNSingleStrand(const ADNSingleStrand & other)
{
  *this = other;
}

ADNSingleStrand & ADNSingleStrand::operator=(const ADNSingleStrand & other) {

  SBChain::operator =(other);
  Identifiable::operator =(other);

  isScaffold_ = other.isScaffold_;
  fivePrime_ = other.fivePrime_;
  threePrime_ = other.threePrime_;

  return *this;
}

std::string const & ADNSingleStrand::GetName() const
{
  return getName();
}

void ADNSingleStrand::SetName(const std::string & name)
{
  setName(name);
}

ADNPointer<ADNNucleotide> ADNSingleStrand::GetFivePrime()
{
  return fivePrime_;
}

ADNPointer<ADNNucleotide> ADNSingleStrand::GetThreePrime()
{
  return threePrime_;
}

void ADNSingleStrand::SetFivePrime(int ntId)
{
  fivePrime_ = GetNucleotide(ntId);
}

void ADNSingleStrand::SetThreePrime(int ntId)
{
  threePrime_ = GetNucleotide(ntId);
}

void ADNSingleStrand::IsScaffold(bool b)
{
  isScaffold_ = b;
}

bool ADNSingleStrand::IsScaffold()
{
  return isScaffold_;
}

CollectionMap<ADNNucleotide> ADNSingleStrand::GetNucleotides() const
{
  CollectionMap<ADNNucleotide> ntList;

  SBPointerList<SBStructuralNode> children = *getChildren();
  SB_FOR(SBStructuralNode* n, children) {
    ADNPointer<ADNNucleotide> a = static_cast<ADNNucleotide*>(n);
    ntList.addReferenceTarget(a());
  }

  return ntList;
}

ADNPointer<ADNNucleotide> ADNSingleStrand::GetNucleotide(unsigned int id) const
{
  auto ntList = GetNucleotides();
  return ntList[id];
}

void ADNSingleStrand::AddNucleotideThreePrime(ADNPointer<ADNNucleotide> nt)
{
  if (threePrime_ != nullptr) {
    threePrime_->SetEnd(NotEnd);
  }
  else {
    // nt is also fivePrime_
    fivePrime_ = nt;
  }
  nt->SetEnd(ThreePrime);
  addChild(nt());
  threePrime_ = nt;
}

void ADNSingleStrand::AddNucleotideFivePrime(ADNPointer<ADNNucleotide> nt)
{
  if (fivePrime_ != nullptr) {
    fivePrime_->SetEnd(NotEnd);
  }
  else {
    // nt is also fivePrime_
    threePrime_ = nt;
  }
  nt->SetEnd(FivePrime);
  addChild(nt(), fivePrime_());
  fivePrime_ = nt;
}

void ADNSingleStrand::ShiftStart(ADNPointer<ADNNucleotide> nt, bool shiftSeq) {
  if (nt == fivePrime_) return;

  auto origThreePrime = threePrime_;
  auto loopNt = origThreePrime;
  auto stopNt = nt->GetPrev();

  while (loopNt != stopNt) {
    removeChild(loopNt());
    AddNucleotideFivePrime(loopNt);
    loopNt = loopNt->GetPrev();
  }
  
  if (shiftSeq) {
    std::string seq = GetSequence();
    SetSequence(seq);
  }
}

std::string ADNSingleStrand::GetSequence() {
  std::string seq = "";
  ADNPointer<ADNNucleotide> nt = fivePrime_;
  while (nt != nullptr) {
    seq += ADNModel::GetResidueName(nt->GetType());
    nt = nt->GetNext();
  }
  return seq;
}

double ADNSingleStrand::GetGCContent() {
  double gcCont = 0.0;
  auto nucleotides = GetNucleotides();

  SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {
    if (nt->GetType() == DNABlocks::DC || nt->GetType() == DNABlocks::DG) {
      gcCont += 1.0;
    }
  }
  gcCont /= nucleotides.size();
  return gcCont;
}

void ADNSingleStrand::SetSequence(std::string seq) {
  if (isScaffold_ != true) {
    std::string msg = "Forcing sequence on staple " + GetName();
    ADNLogger& logger = ADNLogger::GetLogger();
    logger.Log(msg);
  }
  ADNPointer<ADNNucleotide> nt = fivePrime_;
  int count = 0;
  while (nt != nullptr) {
    DNABlocks type = ADNModel::ResidueNameToType(seq[count]);
    nt->SetType(type);
    //std::string name = seq[count] + std::to_string(nt->GetId());
    //nt->SetName(name);
    if (isScaffold_ && nt->GetPair() != nullptr) {
      DNABlocks compType = ADNModel::GetComplementaryBase(type);
      nt->GetPair()->SetType(compType);
      //std::string namePair = std::string(1, residue_names_.left.at(compType)) + std::to_string(nt->GetPair()->GetId());
      //nt->pair_->SetName(namePair);
    }
    nt = nt->GetNext();
    ++count;
  }
}

void ADNSingleStrand::SetDefaultName() {
  std::string name = "Strand";
  if (isScaffold_) name = "Scaffold";
  setName(name + " " + std::to_string(GetId()));
}

DNABlocks ADNModel::GetComplementaryBase(DNABlocks base) {
  DNABlocks comp_base = SBResidue::ResidueType::DI;
  if (base == SBResidue::ResidueType::DA) {
    comp_base = SBResidue::ResidueType::DT;
  }
  else if (base == SBResidue::ResidueType::DT) {
    comp_base = SBResidue::ResidueType::DA;
  }
  else if (base == SBResidue::ResidueType::DG) {
    comp_base = SBResidue::ResidueType::DC;
  }
  else if (base == SBResidue::ResidueType::DC) {
    comp_base = SBResidue::ResidueType::DG;
  }
  return comp_base;
}

char ADNModel::GetResidueName(DNABlocks t)
{
  std::string name = SBResidue::getResidueTypeString(t);
  return name[0];
}

DNABlocks ADNModel::ResidueNameToType(char n)
{
  DNABlocks t = SBResidue::ResidueType::DI;
  if (n == 'A') {
    t = SBResidue::ResidueType::DA;
  }
  else if (n == 'T') {
    t = SBResidue::ResidueType::DT;
  }
  else if (n == 'C') {
    t = SBResidue::ResidueType::DC;
  }
  else if (n == 'G') {
    t = SBResidue::ResidueType::DG;
  }
  return DNABlocks();
}

ADNBaseSegment::ADNBaseSegment(const ADNBaseSegment & other) : Identifiable(other), PositionableSB(other), Orientable(other), SBStructuralNode(other)
{
  *this = other;
}

ADNBaseSegment & ADNBaseSegment::operator=(const ADNBaseSegment & other) {
  PositionableSB::operator =(other);
  Orientable::operator =(other);
  Identifiable::operator =(other);
  SBStructuralNode::operator =(other);

  if (this != &other) {
    number_ = other.number_;
    cell_ = other.GetCell();
  }
  return *this;
}

void ADNBaseSegment::SetNumber(int n)
{
  number_ = n;
}

int ADNBaseSegment::GetNumber() const
{
  return number_;
}

void ADNBaseSegment::setNumber(int n)
{
  SetNumber(n);
}

int ADNBaseSegment::getNumber() const
{
  return GetNumber();
}

ADNPointer<ADNBaseSegment> ADNBaseSegment::GetPrev()
{
  auto p = static_cast<ADNBaseSegment*>(getPreviousStructuralNode());
  return ADNPointer<ADNBaseSegment>(p);
}

ADNPointer<ADNBaseSegment> ADNBaseSegment::GetNext()
{
  auto p = static_cast<ADNBaseSegment*>(getNextStructuralNode());
  return ADNPointer<ADNBaseSegment>(p);
}

ADNPointer<ADNDoubleStrand> ADNBaseSegment::GetDoubleStrand()
{
  auto p = static_cast<ADNDoubleStrand*>(getParent());
  return ADNPointer<ADNDoubleStrand>(p);
}

//SBPosition3 ANTBaseSegment::GetSBPosition() {
//  SBPosition3 target_pos = target_->GetSBPosition();
//  SBPosition3 source_pos = source_->GetSBPosition();
//
//  SBPosition3 position = (target_pos + source_pos)*0.5;
//
//  return position;
//}
//
//SBPosition3 ANTBaseSegment::GetSBPosition2D() {
//  SBPosition3 target_pos = target_->position2D_;
//  SBPosition3 source_pos = source_->position2D_;
//
//  SBPosition3 position = (target_pos + source_pos)*0.5;
//
//  return position;
//}
//
//SBPosition3 ANTBaseSegment::GetSBPosition1D() {
//  SBPosition3 target_pos = target_->position1D_;
//  SBPosition3 source_pos = source_->position1D_;
//
//  SBPosition3 position = (target_pos + source_pos)*0.5;
//
//  return position;
//}

//std::vector<double> ANTBaseSegment::GetVectorPosition() {
//  //SBPosition3 pos = GetSBPosition();
//  std::vector<double> coord = {
//    pos[0].getValue(),
//    pos[1].getValue(),
//    pos[2].getValue(),
//  };
//
//  return coord;
//}

//std::vector<double> ANTBaseSegment::GetVectorPosition2D() {
//  //SBPosition3 pos = GetSBPosition2D();
//  std::vector<double> coord = {
//    pos[0].getValue(),
//    pos[1].getValue(),
//    pos[2].getValue(),
//  };
//
//  return coord;
//}

//std::vector<double> ANTBaseSegment::GetVectorPosition1D() {
//  SBPosition3 pos = GetSBPosition1D();
//  std::vector<double> coord = {
//    pos[0].getValue(),
//    pos[1].getValue(),
//    pos[2].getValue(),
//  };
//
//  return coord;
//}

void ADNBaseSegment::SetCell(ADNCell* c) {
  cell_ = ADNPointer<ADNCell>(c);
}

ADNPointer<ADNCell> ADNBaseSegment::GetCell() const {
  return cell_;
}

CellType ADNBaseSegment::GetCellType() const
{
  ADNPointer<ADNCell> cell = GetCell();
  return cell->GetType();
}

void ADNBaseSegment::RemoveNucleotide(ADNPointer<ADNNucleotide> nt) {
  ADNPointer<ADNCell> cell = GetCell();
  cell->RemoveNucleotide(nt);
}

//ADNDoubleStrand::ADNDoubleStrand(int numBases) : Identifiable(), Collection<ADNBaseSegment>()
//{
//  for (int i = 0; i < numBases; ++i) {
//    ADNPointer<ADNBaseSegment> bs = ADNPointer<ADNBaseSegment>(new ADNBaseSegment());;
//    bs->SetId(GetLastKey() + 1);
//    AddBaseSegmentEnd(bs);
//  }
//}
//
//ADNDoubleStrand::ADNDoubleStrand(std::vector<ADNPointer<ADNBaseSegment>> bss)
//{
//  for (auto bs : bss) {
//    bs->SetId(GetLastKey() + 1);
//    AddBaseSegmentEnd(bs);
//  }
//}

ADNDoubleStrand::ADNDoubleStrand(const ADNDoubleStrand & other) : Identifiable(other), SBStructuralGroup(other)
{
  *this = other;
}

ADNDoubleStrand & ADNDoubleStrand::operator=(const ADNDoubleStrand & other) {
  
  SBStructuralGroup::operator =(other);
  Identifiable::operator =(other);

  if (this != &other) {
    start_ = other.start_;
    end_ = other.end_;
  }
  return *this;
}

void ADNDoubleStrand::SetInitialTwistAngle(double angle)
{
  initialTwistAngle_ = angle;
}

double ADNDoubleStrand::GetInitialTwistAngle() const
{
  return initialTwistAngle_;
}

CollectionMap<ADNBaseSegment> ADNDoubleStrand::GetBaseSegments() const
{
  CollectionMap<ADNBaseSegment> bsList;

  auto children = *getChildren();
  SB_FOR(SBStructuralNode* n, children) {
    ADNBaseSegment* a = static_cast<ADNBaseSegment*>(n);
    bsList.addReferenceTarget(a);
  }

  return bsList;
}

ADNPointer<ADNBaseSegment> ADNDoubleStrand::GetNthBaseSegment(int n)
{
  ADNPointer<ADNBaseSegment> bs = start_;
  for (int i = 0; i < n; ++i) {
    bs = bs->GetNext();
  }
  return bs;
}

ADNPointer<ADNBaseSegment> ADNDoubleStrand::GetFirstBaseSegment()
{
  return start_;
}

ADNPointer<ADNBaseSegment> ADNDoubleStrand::GetLastBaseSegment()
{
  return end_;
}

void ADNDoubleStrand::AddBaseSegmentBeginning(ADNPointer<ADNBaseSegment> bs)
{
  int number = 0;
  if (start_ != nullptr) {
    number = start_->GetNumber() - 1;
  }
  else {
    // nt is also fivePrime_
    end_ = bs;
  }
  bs->SetNumber(number);
  addChild(bs(), start_());
  start_ = bs;
}

void ADNDoubleStrand::AddBaseSegmentEnd(ADNPointer<ADNBaseSegment> bs)
{
  int number = 0;
  if (end_ == nullptr) {
    // bs is also first
    start_ = bs;
  }
  else {
    number = end_->GetNumber() + 1;
  }
  addChild(bs());
  end_ = bs;
  bs->SetNumber(number);
}

ADNPointer<ADNNucleotide> ADNBasePair::GetLeftNucleotide() {
  return left_;
}

void ADNBasePair::SetLeftNucleotide(ADNPointer<ADNNucleotide> nt) {
  if (right_ != nullptr && right_->GetPair() != nt) {
    std::string msg = "Forming an ANTBasePair with unpaired nucleotides.";
    ADNLogger& logger = ADNLogger::GetLogger();
    logger.Log(msg);
  }
  left_ = nt;
}

ADNPointer<ADNNucleotide> ADNBasePair::GetRightNucleotide() {
  return right_;
}

void ADNBasePair::SetRightNucleotide(ADNPointer<ADNNucleotide> nt) {
  if (left_ != nullptr && left_->GetPair() != nt) {
    std::string msg = "Forming an ANTBasePair with unpaired nucleotides.";
    ADNLogger& logger = ADNLogger::GetLogger();
    logger.Log(msg);
  }
  right_ = nt;
}

void ADNBasePair::RemoveNucleotide(ADNPointer<ADNNucleotide> nt) {
  if (left_ == nt) {
    left_ = nullptr;
  }
  else if (right_ == nt) {
    right_ = nullptr;
  }
}

void ADNSkipPair::RemoveNucleotide(ADNPointer<ADNNucleotide> nt) {
}

void ADNLoopPair::RemoveNucleotide(ADNPointer<ADNNucleotide> nt) {
  if (left_ != nullptr) {
    left_->RemoveNucleotide(nt);
  }
  if (right_ != nullptr) {
    right_->RemoveNucleotide(nt);
  }
}

void ADNLoop::SetStart(ADNPointer<ADNNucleotide> nt)
{
  startNt_ = nt;
}

ADNPointer<ADNNucleotide> ADNLoop::GetStart()
{
  return startNt_;
}

void ADNLoop::SetEnd(ADNPointer<ADNNucleotide> nt)
{
  endNt_ = nt;
}

ADNPointer<ADNNucleotide> ADNLoop::GetEnd()
{
  return endNt_;
}

CollectionMap<ADNNucleotide> ADNLoop::GetNucleotides() const
{
  CollectionMap<ADNNucleotide> ntList;

  auto children = *getChildren();
  SB_FOR(SBStructuralNode* n, children) {
    ADNPointer<ADNNucleotide> a = static_cast<ADNNucleotide*>(n);
    ntList.addReferenceTarget(a());
  }

  return ntList;
}

void ADNLoop::AddNucleotide(ADNPointer<ADNNucleotide> nt) {
  addChild(nt());
}

void ADNLoop::RemoveNucleotide(ADNPointer<ADNNucleotide> nt) {
  removeChild(nt());
}

//void ADNLoop::PositionLoopNucleotides(ublas::vector<double> bsPositionPrev, ublas::vector<double> bsPositionNext) {
//
//  auto sz = GetCollection().size();
//  if (sz == 0) return;
//
//  if (startNt_ != nullptr && endNt_ != nullptr) {
//    ublas::vector<double> start_pos = bsPositionPrev;
//    ublas::vector<double> end_pos = bsPositionNext;
//    ublas::vector<double> shifted = end_pos - start_pos;
//    ADNPointer<ADNNucleotide> nt = startNt_;
//
//    // this doesn't work because not all nt positions have been already determined
//    /*ublas::vector<double> e1 = (startNt_->e1_ + endNt_->e1_)*0.5;
//    ublas::vector<double> e2 = (startNt_->e2_ + endNt_->e2_)*0.5;*/
//    ublas::vector<double> e3 = shifted / ADNVectorMath::CalculateVectorNorm(shifted);
//    auto subspace = ADNVectorMath::FindOrthogonalSubspace(e3);
//    ublas::vector<double> e1 = ublas::row(subspace, 0);
//    ublas::vector<double> e2 = ublas::row(subspace, 1);
//
//    int i = 0;
//    while (nt != endNt_->GetNext()) {
//      float frac = float(i) / (sz + 1);
//      ublas::vector<double> shift = shifted * frac;
//      shift += start_pos;
//
//      nt->SetPosition(shift);
//      nt->SetSidechainPosition(shift);
//      nt->SetBackbonePosition(shift);
//
//      nt->SetE1(e1);
//      nt->SetE2(e2);
//      nt->SetE3(e3);
//
//      nt = nt->GetNext();
//      i++;
//    }
//  }
//  else {
//    //if loop is at the beginning
//  }
//}

bool ADNLoop::IsEmpty() {
  bool empty = true;
  if (GetNucleotides().size() > 0) empty = false;
  return empty;
}

ADNPointer<ADNLoop> ADNLoopPair::GetLeftLoop() {
  return left_;
}

void ADNLoopPair::SetLeftLoop(ADNPointer<ADNLoop> lp) {
  left_ = lp;
}

ADNPointer<ADNLoop> ADNLoopPair::GetRightLoop() {
  return right_;
}

void ADNLoopPair::SetRightLoop(ADNPointer<ADNLoop> lp) {
  right_ = lp;
}

ADNBackbone::ADNBackbone(const ADNBackbone & other)
{
  *this = other;
}

ADNBackbone & ADNBackbone::operator=(const ADNBackbone & other)
{
  PositionableSB::operator =(other);
  SBBackbone::operator =(other);

  return *this;
}

bool ADNBackbone::AddAtom(ADNPointer<ADNAtom> atom)
{
  return addChild(atom());
}

bool ADNBackbone::DeleteAtom(ADNPointer<ADNAtom> atom)
{
  return removeChild(atom());
}

CollectionMap<ADNAtom> ADNBackbone::GetAtoms() const
{
  CollectionMap<ADNAtom> atomList;

  auto children = *getChildren();
  SB_FOR(SBStructuralNode* n, children) {
    ADNAtom* a = static_cast<ADNAtom*>(n);
    atomList.addReferenceTarget(a);
  }
  
  return atomList;
}

NucleotideGroup ADNBackbone::GetGroupType()
{
  return getType();
}

ADNPointer<ADNNucleotide> ADNBackbone::GetNucleotide() const
{
  auto nt = static_cast<ADNNucleotide*>(getParent());
  return ADNPointer<ADNNucleotide>(nt);
}

ADNSidechain::ADNSidechain(const ADNSidechain & other)
{
  *this = other;
}

ADNSidechain & ADNSidechain::operator=(const ADNSidechain & other)
{
  PositionableSB::operator =(other);
  SBSideChain::operator =(other);

  return *this;
}

bool ADNSidechain::AddAtom(ADNPointer<ADNAtom> atom)
{
  return addChild(atom());
}

bool ADNSidechain::DeleteAtom(ADNPointer<ADNAtom> atom)
{
  return removeChild(atom());
}

CollectionMap<ADNAtom> ADNSidechain::GetAtoms()
{
  CollectionMap<ADNAtom> atomList;

  auto children = *getChildren();
  SB_FOR(SBStructuralNode* n, children) {
    ADNAtom* a = static_cast<ADNAtom*>(n);
    atomList.addReferenceTarget(a);
  }

  return atomList;
}

NucleotideGroup ADNSidechain::GetGroupType()
{
  return getType();
}

ADNPointer<ADNNucleotide> ADNSidechain::GetNucleotide() const
{
  auto nt = static_cast<ADNNucleotide*>(getParent());
  return ADNPointer<ADNNucleotide>(nt);
}

PositionableSB::PositionableSB(const PositionableSB & other)
{
  *this = other;
}

PositionableSB & PositionableSB::operator=(const PositionableSB & other)
{
  if (&other == this) {
    return *this;
  }

  centerAtom_ = other.GetCenterAtom();

  return *this;
}

void PositionableSB::SetPosition(Position3D pos)
{
  centerAtom_->setPosition(pos);
}

Position3D PositionableSB::GetPosition() const
{
  return centerAtom_->getPosition();
}

ADNPointer<ADNAtom> PositionableSB::GetCenterAtom() const
{
  return centerAtom_;
}

void PositionableSB::SetCenterAtom(ADNPointer<ADNAtom> centerAtom)
{
  centerAtom_ = centerAtom;
}
