#include "ADNModel.hpp"

ADNAtom::ADNAtom(const ADNAtom & other) : SBAtom(other)
{
  *this = other;
}

ADNAtom & ADNAtom::operator=(const ADNAtom & other)
{
  SBAtom::operator =(other);

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

SBNode * ADNAtom::getNt() const
{
  return getParent()->getParent();
}

SBNode * ADNAtom::getNtGroup() const
{
  return getParent();
}

bool ADNAtom::IsInBackbone() 
{
  return isFromNucleicAcidBackbone();
}

ADNNucleotide::ADNNucleotide() : PositionableSB(), SBResidue(), Orientable()
{
  SetType(DNABlocks::DI);
  ADNPointer<ADNBackbone> bb = new ADNBackbone();
  bb->setName(getName() + " " + "Backbone");
  ADNPointer<ADNSidechain> sc = new ADNSidechain();
  sc->setName(getName() + " " + "Sidechain");
  addChild(bb());
  addChild(sc());
}

ADNNucleotide::ADNNucleotide(const ADNNucleotide & other) : PositionableSB(other), SBResidue(other), Orientable(other)
{
  *this = other;
}

ADNNucleotide & ADNNucleotide::operator=(const ADNNucleotide & other) {
  PositionableSB::operator =(other);
  Orientable::operator =(other);
  SBResidue::operator =(other);

  return *this;
}

void ADNNucleotide::SetType(DNABlocks t)
{
  setResidueType(t);
  SetName(ADNModel::GetResidueName(t) + std::to_string(getNodeIndex()));
}

DNABlocks ADNNucleotide::GetType()
{
  return getResidueType();
}

DNABlocks ADNNucleotide::getNucleotideType() const
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

SBNode* ADNNucleotide::getSingleStrand() const
{
  return getParent();
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

void ADNNucleotide::HideCenterAtoms()
{
  auto bb = GetBackbone();
  auto sc = GetSidechain();

  HideCenterAtom();
  bb->HideCenterAtom();
  sc->HideCenterAtom();
}

CollectionMap<ADNAtom> ADNNucleotide::GetAtomsByName(std::string name)
{
  CollectionMap<ADNAtom> res;
  /*SBNodeIndexer atoms;
  getNodes(atoms, SBNode::GetName() == name);

  SB_FOR(SBNode* a, atoms) {
    ADNPointer<ADNAtom> at = static_cast<ADNAtom*>(a);
    res.addReferenceTarget(at());
  }*/

  auto atoms = GetAtoms();
  SB_FOR(ADNPointer<ADNAtom> a, atoms) {
    if (a->GetName() == name) {
      res.addReferenceTarget(a());
    }
  }

  return res;
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

SBNode * ADNNucleotide::getBaseSegment() const
{
  return bs_();
}

std::string ADNNucleotide::getBaseSegmentType() const
{
  CellType t = bs_->GetCellType();
  return ADNModel::CellTypeToString(t);
}

End ADNNucleotide::GetEnd()
{
  return end_;
}

void ADNNucleotide::SetEnd(End e)
{
  end_ = e;
}

bool ADNNucleotide::IsEnd()
{
  bool isEnd = false;
  if (end_ != NotEnd) isEnd = true;

  return isEnd;
}

ADNPointer<ADNBackbone> ADNNucleotide::GetBackbone()
{
  auto bb = static_cast<ADNBackbone*>(getBackbone());
  return ADNPointer<ADNBackbone>(bb);
}

void ADNNucleotide::SetBackbone(ADNPointer<ADNBackbone> bb)
{
  auto bbOld = GetBackbone();
  bbOld = bb;
}

ADNPointer<ADNSidechain> ADNNucleotide::GetSidechain()
{
  auto sc = static_cast<ADNSidechain*>(getSideChain());
  return ADNPointer<ADNSidechain>(sc);
}

void ADNNucleotide::SetSidechain(ADNPointer<ADNSidechain> sc)
{
  auto scOld = GetSidechain();
  scOld = sc;
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
//  std::map<int, int> atomMap;
//
//  ADNPointer<ADNBackbone> bbO = GetBackbone();
//  auto bb0Atoms = bbO->GetAtoms();
//  SB_FOR(ADNPointer<ADNAtom> oat, bb0Atoms) {
//    ADNPointer<ADNAtom> atom(new ADNAtom(*oat));
//    bb->AddAtom(atom);
//    atomMap.insert(std::make_pair(oat->getNodeIndex(), atom->getNodeIndex()));
//  }
//  ADNPointer<ADNSidechain> scO = GetSidechain();
//  auto sc0Atoms = scO->GetAtoms();
//  SB_FOR(ADNPointer<ADNAtom> oat, sc0Atoms) {
//    ADNPointer<ADNAtom> atom(new ADNAtom(*oat));
//    sc->AddAtom(atom);
//    atomMap.insert(std::make_pair(oat->getNodeIndex(), atom->getNodeIndex()));
//  }
//
//  /*auto bonds = GetBonds();
//  for (auto b : bonds) {
//    ADNPointer<ADNAtom> b1 = b.first;
//    ADNPointer<ADNAtom> b2 = b.second;
//    auto newB1 = atomMap.at(b1);
//    auto newB2 = atomMap.at(b2);
//    target->AddBond(newB1, newB2);
//  }*/
//
//  target->SetBackbone(bb);
//  target->SetSidechain(sc);
//}

ADNSingleStrand::ADNSingleStrand(const ADNSingleStrand & other)
{
  *this = other;
}

ADNSingleStrand & ADNSingleStrand::operator=(const ADNSingleStrand & other) {

  SBChain::operator =(other);

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

SBNode * ADNSingleStrand::getFivePrime() const
{
  return fivePrime_();
}

ADNPointer<ADNNucleotide> ADNSingleStrand::GetThreePrime()
{
  return threePrime_;
}

SBNode * ADNSingleStrand::getThreePrime() const
{
  return threePrime_();
}

void ADNSingleStrand::SetFivePrime(ADNPointer<ADNNucleotide> nt)
{
  fivePrime_ = nt;
}

void ADNSingleStrand::SetThreePrime(ADNPointer<ADNNucleotide> nt)
{
  threePrime_ = nt;
}

void ADNSingleStrand::IsScaffold(bool b)
{
  isScaffold_ = b;
}

bool ADNSingleStrand::IsScaffold() const
{
  return isScaffold_;
}

bool ADNSingleStrand::getIsScaffold() const
{
  return IsScaffold();
}

void ADNSingleStrand::setIsScaffold(bool b)
{
  IsScaffold(b);
}

int ADNSingleStrand::getNumberOfNucleotides() const
{
  return boost::numeric_cast<int>(GetNucleotides().size());
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
  addChild(nt());
  if (threePrime_ != nullptr) {
    if (threePrime_->GetEnd() == FiveAndThreePrime) threePrime_->SetEnd(FivePrime);
    else threePrime_->SetEnd(NotEnd);
    nt->SetEnd(ThreePrime);
  }
  else {
    // nt is also fivePrime_
    fivePrime_ = nt;
    nt->SetEnd(FiveAndThreePrime);
  }
  auto test = nt->GetPrev();
  threePrime_ = nt;
}

void ADNSingleStrand::AddNucleotideFivePrime(ADNPointer<ADNNucleotide> nt)
{
  if (fivePrime_ != nullptr) {
    if (fivePrime_->GetEnd() == FiveAndThreePrime) fivePrime_->SetEnd(ThreePrime);
    else fivePrime_->SetEnd(NotEnd);
    nt->SetEnd(FivePrime);
  }
  else {
    // nt is also fivePrime_
    threePrime_ = nt;
    nt->SetEnd(FiveAndThreePrime);
  }
  addChild(nt(), fivePrime_());
  fivePrime_ = nt;
}

void ADNSingleStrand::AddNucleotide(ADNPointer<ADNNucleotide> nt, ADNPointer<ADNNucleotide> nextNt)
{
  if (nextNt == nullptr) return AddNucleotideThreePrime(nt);
  if (nextNt == GetFivePrime()) return AddNucleotideFivePrime(nt);

  addChild(nt(), nextNt());
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

std::string ADNSingleStrand::GetSequence() const
{
  std::string seq = "";
  ADNPointer<ADNNucleotide> nt = fivePrime_;
  while (nt != nullptr) {
    seq += ADNModel::GetResidueName(nt->GetType());
    nt = nt->GetNext();
  }
  return seq;
}

std::string ADNSingleStrand::getSequence() const
{
  return GetSequence();
}

double ADNSingleStrand::GetGCContent() const
{
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

double ADNSingleStrand::getGCContent() const
{
  return GetGCContent();
}

void ADNSingleStrand::SetSequence(std::string seq) {

  if (seq.empty()) return;

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
    if (isScaffold_ && nt->GetPair() != nullptr) {
      DNABlocks compType = ADNModel::GetComplementaryBase(type);
      nt->GetPair()->SetType(compType);
    }
    nt = nt->GetNext();
    ++count;
  }
}

void ADNSingleStrand::SetDefaultName() {
  std::string name = "Strand";
  if (isScaffold_) name = "Scaffold";
  setName(name);
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
  char n = name[0];
  if (name.size() > 0) n = name[1];

  if (n == 'I') n = 'N';

  return n;
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
  return t;
}

std::string ADNModel::CellTypeToString(CellType t)
{
  std::string typ = "Unknown";
  if (t == BasePair) {
    typ = "Base Pair";
  }
  else if (t == LoopPair) {
    typ = "Loop Pair";
  }
  else if (t == SkipPair) {
    typ = "Skip Pair";
  }
  return typ;
}

bool ADNModel::IsAtomInBackboneByName(std::string name)
{
  bool res = false;
  if (std::find(backbone_names_.begin(), backbone_names_.end(), name) != backbone_names_.end()) {
    res = true;
  }
  return res;

}

SBElement::Type ADNModel::GetElementType(std::string atomName)
{
  std::map<std::string, SBElement::Type> atomType = {
    { "P", SBElement::Phosphorus },
    { "OP1", SBElement::Oxygen },
    { "O1P", SBElement::Oxygen },
    { "OP2", SBElement::Oxygen },
    { "O2P", SBElement::Oxygen },
    { "O5'", SBElement::Oxygen },
    { "O4'", SBElement::Oxygen },
    { "O3'", SBElement::Oxygen },
    { "O6", SBElement::Oxygen },
    { "O4", SBElement::Oxygen },
    { "O2", SBElement::Oxygen },
    { "C5'", SBElement::Carbon },
    { "C4'", SBElement::Carbon },
    { "C3'", SBElement::Carbon },
    { "C2'", SBElement::Carbon },
    { "C1'", SBElement::Carbon },
    { "C8", SBElement::Carbon },
    { "C7", SBElement::Carbon },
    { "C6", SBElement::Carbon },
    { "C5", SBElement::Carbon },
    { "C4", SBElement::Carbon },
    { "C2", SBElement::Carbon },
    { "N9", SBElement::Nitrogen },
    { "N7", SBElement::Nitrogen },
    { "N6", SBElement::Nitrogen },
    { "N4", SBElement::Nitrogen },
    { "N3", SBElement::Nitrogen },
    { "N2", SBElement::Nitrogen },
    { "N1", SBElement::Nitrogen }
  };

  SBElement::Type t = SBElement::Type::Unknown;
  if (atomType.find(atomName) != atomType.end()) {
    t = atomType.at(atomName);
  }

  return t;
}

std::map<std::string, std::vector<std::string>> ADNModel::GetNucleotideBonds(DNABlocks t)
{
  std::map<std::string, std::vector<std::string>> res;
  // P
  std::vector<std::string> pConn = {"OP2", "OP1", "O5'"};
  res.insert(std::make_pair("P", pConn));
  // OP2
  std::vector<std::string> op2Conn = { "P" };
  res.insert(std::make_pair("OP2", op2Conn));
  // OP1
  std::vector<std::string> op1Conn = { "P" };
  res.insert(std::make_pair("OP1", op1Conn));
  // O5'
  std::vector<std::string> o5pConn = { "P",  "C5'"};
  res.insert(std::make_pair("O5'", o5pConn));
  // C5'
  std::vector<std::string> c5pConn = { "O5'",  "C4'" };
  res.insert(std::make_pair("C5'", c5pConn));
  // C4'
  std::vector<std::string> c4pConn = { "C5'",  "O4'", "C3'" };
  res.insert(std::make_pair("C4'", c4pConn));
  // C3'
  std::vector<std::string> c3pConn = { "C4'",  "O3'", "C2'" };
  res.insert(std::make_pair("C3'", c3pConn));
  // O3'
  std::vector<std::string> o3pConn = { "C3'" };
  res.insert(std::make_pair("O3'", o3pConn));
  // C2'
  std::vector<std::string> c2pConn = { "C3'",  "C1'" };
  res.insert(std::make_pair("C2'", c2pConn));
  // C1'
  std::vector<std::string> c1pConn = { "C2'",  "O4'" };
  res.insert(std::make_pair("C1'", c1pConn));
  // O4'
  std::vector<std::string> o4pConn = { "C4'",  "C1'" };
  res.insert(std::make_pair("O4'", o4pConn));

  if (t == DNABlocks::DA || t == DNABlocks::DI) {
    std::vector<std::string> c1pConn = { "C2'",  "O4'", "N9" };
    res["C1'"] = c1pConn;
    std::vector<std::string> c8Conn = { "N9",  "N7" };
    res.insert(std::make_pair("C8", c8Conn));
    std::vector<std::string> n7Conn = { "C8",  "C5" };
    res.insert(std::make_pair("N7", n7Conn));
    std::vector<std::string> c5Conn = { "C4",  "N7", "C6" };
    res.insert(std::make_pair("C5", c5Conn));
    std::vector<std::string> c4Conn = { "C5",  "N9", "N3" };
    res.insert(std::make_pair("C4", c4Conn));
    std::vector<std::string> n3Conn = { "C4",  "C2" };
    res.insert(std::make_pair("N3", n3Conn));
    std::vector<std::string> c2Conn = { "N3",  "N1" };
    res.insert(std::make_pair("C2", c2Conn));
    std::vector<std::string> n1Conn = { "C2",  "C6" };
    res.insert(std::make_pair("N1", n1Conn));
    std::vector<std::string> c6Conn = { "N1",  "C5", "N6" };
    res.insert(std::make_pair("C6", c6Conn));
  }
  else if (t == DNABlocks::DG) {
    std::vector<std::string> c1pConn = { "C2'",  "O4'", "N9" };
    res["C1'"] = c1pConn;
    std::vector<std::string> c8Conn = { "N9",  "N7" };
    res.insert(std::make_pair("C8", c8Conn));
    std::vector<std::string> n7Conn = { "C8",  "C5" };
    res.insert(std::make_pair("N7", n7Conn));
    std::vector<std::string> c5Conn = { "C4",  "N7", "C6" };
    res.insert(std::make_pair("C5", c5Conn));
    std::vector<std::string> c4Conn = { "C5",  "N9", "N3" };
    res.insert(std::make_pair("C4", c4Conn));
    std::vector<std::string> n3Conn = { "C4",  "C2" };
    res.insert(std::make_pair("N3", n3Conn));
    std::vector<std::string> c2Conn = { "N3",  "N2", "N1" };
    res.insert(std::make_pair("C2", c2Conn));
    std::vector<std::string> n1Conn = { "C2",  "C6" };
    res.insert(std::make_pair("N1", n1Conn));
    std::vector<std::string> c6Conn = { "N1",  "C5", "O6" };
    res.insert(std::make_pair("C6", c6Conn));
  }
  else if (t == DNABlocks::DT) {
    std::vector<std::string> c1pConn = { "C2'",  "O4'", "N1" };
    res["C1'"] = c1pConn;
    std::vector<std::string> c5Conn = { "C4",  "C6", "C7" };
    res.insert(std::make_pair("C5", c5Conn));
    std::vector<std::string> c4Conn = { "O4", "N3" };
    res.insert(std::make_pair("C4", c4Conn));
    std::vector<std::string> n3Conn = { "C2" };
    res.insert(std::make_pair("N3", n3Conn));
    std::vector<std::string> c2Conn = { "N1", "O2" };
    res.insert(std::make_pair("C2", c2Conn));
    std::vector<std::string> n1Conn = { "C6" };
    res.insert(std::make_pair("N1", n1Conn));
  }
  else if (t == DNABlocks::DC) {
    std::vector<std::string> c1pConn = { "C2'",  "O4'", "N1" };
    res["C1'"] = c1pConn;
    std::vector<std::string> c5Conn = { "C4",  "C6" };
    res.insert(std::make_pair("C5", c5Conn));
    std::vector<std::string> c4Conn = { "C5",  "N4", "N3" };
    res.insert(std::make_pair("C4", c4Conn));
    std::vector<std::string> n3Conn = { "C4",  "C2" };
    res.insert(std::make_pair("N3", n3Conn));
    std::vector<std::string> c2Conn = { "N3",  "N1", "O2" };
    res.insert(std::make_pair("C2", c2Conn));
    std::vector<std::string> n1Conn = { "C2",  "C6" };
    res.insert(std::make_pair("N1", n1Conn));
    std::vector<std::string> c6Conn = { "N1",  "C5" };
    res.insert(std::make_pair("C6", c6Conn));
  }
  return res;
}

ADNBaseSegment::ADNBaseSegment(CellType cellType) : PositionableSB(), Orientable(), SBStructuralGroup() {
  if (cellType == BasePair) {
    cell_ = new ADNBasePair();
  }
  else if (cellType == LoopPair) {
    cell_ = new ADNLoopPair();
  }
  else if (cellType == SkipPair) {
    cell_ = new ADNSkipPair();
  }
}

ADNBaseSegment::ADNBaseSegment(const ADNBaseSegment & other) : PositionableSB(other), Orientable(other), SBStructuralGroup(other)
{
  *this = other;
}

ADNBaseSegment & ADNBaseSegment::operator=(const ADNBaseSegment & other) {
  PositionableSB::operator =(other);
  Orientable::operator =(other);
  SBStructuralGroup::operator =(other);

  if (this != &other) {
    number_ = other.number_;
    //cell_ = other.GetCell();
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

ADNPointer<ADNBaseSegment> ADNBaseSegment::GetPrev() const
{
  auto p = static_cast<ADNBaseSegment*>(getPreviousStructuralNode());
  return ADNPointer<ADNBaseSegment>(p);
}

ADNPointer<ADNBaseSegment> ADNBaseSegment::GetNext() const
{
  auto p = static_cast<ADNBaseSegment*>(getNextStructuralNode());
  return ADNPointer<ADNBaseSegment>(p);
}

ADNPointer<ADNDoubleStrand> ADNBaseSegment::GetDoubleStrand()
{
  auto p = static_cast<ADNDoubleStrand*>(getParent());
  return ADNPointer<ADNDoubleStrand>(p);
}

CollectionMap<ADNNucleotide> ADNBaseSegment::GetNucleotides()
{
  return cell_->GetNucleotides();
}

void ADNBaseSegment::SetCell(ADNCell* c) {
  cell_ = ADNPointer<ADNCell>(c);
  std::string type = ADNModel::CellTypeToString(cell_->GetType());
  cell_->setName(type + " " + std::to_string(cell_->getNodeIndex()));
  addChild(cell_());
}

ADNPointer<ADNCell> ADNBaseSegment::GetCell() const {
  return cell_;
}

CellType ADNBaseSegment::GetCellType() const
{
  ADNPointer<ADNCell> cell = GetCell();
  return cell->GetType();
}

std::string ADNBaseSegment::getCellType() const
{
  auto t = GetCellType();
  return ADNModel::CellTypeToString(t);
}

void ADNBaseSegment::RemoveNucleotide(ADNPointer<ADNNucleotide> nt) {
  ADNPointer<ADNCell> cell = GetCell();
  cell->RemoveNucleotide(nt);
}

ADNDoubleStrand::ADNDoubleStrand(const ADNDoubleStrand & other) : SBStructuralGroup(other)
{
  *this = other;
}

ADNDoubleStrand & ADNDoubleStrand::operator=(const ADNDoubleStrand & other) {
  
  SBStructuralGroup::operator =(other);

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

double ADNDoubleStrand::getInitialTwistAngle() const
{
  return GetInitialTwistAngle();
}

int ADNDoubleStrand::GetLength() const
{
  return boost::numeric_cast<int>(GetBaseSegments().size());
}

int ADNDoubleStrand::getLength() const
{
  return GetLength();
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

ADNPointer<ADNBaseSegment> ADNDoubleStrand::GetFirstBaseSegment() const
{
  return start_;
}

SBNode * ADNDoubleStrand::getFirstBaseSegment() const
{
  return GetFirstBaseSegment()();
}

ADNPointer<ADNBaseSegment> ADNDoubleStrand::GetLastBaseSegment() const
{
  return end_;
}

SBNode * ADNDoubleStrand::getLastBaseSegment() const
{
  return GetLastBaseSegment()();
}

void ADNDoubleStrand::AddBaseSegmentBeginning(ADNPointer<ADNBaseSegment> bs)
{
  bs->setName("Base Segment " + std::to_string(bs->getNodeIndex()));
  int number = bs->GetNumber();
  if (number == -1) {
    // number not defined
    number = 0;
  }

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
  bs->setName("Base Segment " + std::to_string(bs->getNodeIndex()));
  int number = bs->GetNumber();
  if (number == -1) {
    // number not defined
    number = 0;
  }
  
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

SBNode* ADNBasePair::getLeft() const
{
  return left_();
}

void ADNBasePair::SetLeftNucleotide(ADNPointer<ADNNucleotide> nt) {
  left_ = nt;
  addChild(left_());
}

ADNPointer<ADNNucleotide> ADNBasePair::GetRightNucleotide() {
  return right_;
}

SBNode* ADNBasePair::getRight() const
{
  return right_();
}

void ADNBasePair::SetRightNucleotide(ADNPointer<ADNNucleotide> nt) {
  right_ = nt;
  addChild(right_());
}

void ADNBasePair::AddPair(ADNPointer<ADNNucleotide> left, ADNPointer<ADNNucleotide> right)
{
  SetLeftNucleotide(left);
  SetRightNucleotide(right);
  if (left != nullptr) left->SetPair(right);
  if (right_ != nullptr) right->SetPair(left);
}

void ADNBasePair::PairNucleotides()
{
  if (left_ != nullptr && right_ != nullptr) {
    left_->SetPair(right_);
    right_->SetPair(left_);
  }
}

void ADNBasePair::RemoveNucleotide(ADNPointer<ADNNucleotide> nt) {
  if (left_ == nt) {
    left_ = nullptr;
  }
  else if (right_ == nt) {
    right_ = nullptr;
  }
}

CollectionMap<ADNNucleotide> ADNBasePair::GetNucleotides()
{
  CollectionMap<ADNNucleotide> nts;
  if (left_ != nullptr) nts.addReferenceTarget(left_());
  if (right_ != nullptr) nts.addReferenceTarget(right_());
  return nts;
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

CollectionMap<ADNNucleotide> ADNLoopPair::GetNucleotides()
{
  CollectionMap<ADNNucleotide> nts;
  
  if (left_ != nullptr) {
    auto leftNts = left_->GetNucleotides();
    SB_FOR(ADNPointer<ADNNucleotide> n, leftNts) {
      nts.addReferenceTarget(n());
    }
  }
  
  if (right_ != nullptr) {
    auto rightNts = right_->GetNucleotides();
    SB_FOR(ADNPointer<ADNNucleotide> n, rightNts) {
      nts.addReferenceTarget(n());
    }
  }

  return nts;
}

void ADNLoop::SetStart(ADNPointer<ADNNucleotide> nt)
{
  startNt_ = nt;
}

ADNPointer<ADNNucleotide> ADNLoop::GetStart()
{
  return startNt_;
}

SBNode * ADNLoop::getStartNucleotide() const
{
  return startNt_();
}

void ADNLoop::SetEnd(ADNPointer<ADNNucleotide> nt)
{
  endNt_ = nt;
}

ADNPointer<ADNNucleotide> ADNLoop::GetEnd()
{
  return endNt_;
}

SBNode * ADNLoop::getEndNucleotide() const
{
  return endNt_();
}

int ADNLoop::getNumberOfNucleotides() const
{
  return boost::numeric_cast<int>(GetNucleotides().size());
}

std::string ADNLoop::getLoopSequence() const
{
  std::string seq = "";
  ADNPointer<ADNNucleotide> nt = startNt_;
  while(nt != endNt_->GetNext()) {
    DNABlocks t = nt->GetType();
    seq += ADNModel::GetResidueName(t);
    nt = nt->GetNext();
  }

  return seq;
}

void ADNLoop::SetBaseSegment(ADNPointer<ADNBaseSegment> bs, bool setPositions)
{
  auto nts = GetNucleotides();
  SB_FOR(ADNPointer<ADNNucleotide> nt, nts) {
    nt->SetBaseSegment(bs);
    if (setPositions) {
      nt->SetPosition(bs->GetPosition());
      nt->SetBackbonePosition(bs->GetPosition());
      nt->SetSidechainPosition(bs->GetPosition());
    }
  }
}

CollectionMap<ADNNucleotide> ADNLoop::GetNucleotides() const
{
  return nucleotides_;
}

void ADNLoop::AddNucleotide(ADNPointer<ADNNucleotide> nt) {
  addChild(nt());
  nucleotides_.addReferenceTarget(nt());
}

void ADNLoop::RemoveNucleotide(ADNPointer<ADNNucleotide> nt) {
  removeChild(nt());
  nucleotides_.removeReferenceTarget(nt());
}

bool ADNLoop::IsEmpty() {
  bool empty = true;
  if (GetNucleotides().size() > 0) empty = false;
  return empty;
}

ADNPointer<ADNLoop> ADNLoopPair::GetLeftLoop() {
  return left_;
}

SBNode* ADNLoopPair::getLeft() const
{
  return left_();
}

void ADNLoopPair::SetLeftLoop(ADNPointer<ADNLoop> lp) {
  left_ = lp;
  addChild(lp());
  lp->setName("Left Loop " + std::to_string(lp->getNodeIndex()));
}

ADNPointer<ADNLoop> ADNLoopPair::GetRightLoop() {
  return right_;
}

SBNode* ADNLoopPair::getRight() const
{
  return right_();
}

void ADNLoopPair::SetRightLoop(ADNPointer<ADNLoop> lp) {
  right_ = lp;
  addChild(lp());
  lp->setName("Right Loop " + std::to_string(lp->getNodeIndex()));
}

ADNBackbone::ADNBackbone() : PositionableSB(), SBBackbone()
{
  auto cA = GetCenterAtom();
  cA->setElementType(SBElement::Einsteinium);
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

int ADNBackbone::getNumberOfAtoms() const
{
  return boost::numeric_cast<int>(GetAtoms().size());
}

CollectionMap<ADNAtom> ADNBackbone::GetAtoms() const
{
  CollectionMap<ADNAtom> atomList;

  auto children = *getChildren();
  SB_FOR(SBStructuralNode* n, children) {
    if (n->getType() == SBNode::Atom) {
      ADNAtom* a = static_cast<ADNAtom*>(n);
      atomList.addReferenceTarget(a);
    }
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

ADNSidechain::ADNSidechain() : PositionableSB(), SBSideChain()
{
  auto cA = GetCenterAtom();
  cA->setElementType(SBElement::Fermium);
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

int ADNSidechain::getNumberOfAtoms() const
{
  return boost::numeric_cast<int>(GetAtoms().size());
}

CollectionMap<ADNAtom> ADNSidechain::GetAtoms() const
{
  CollectionMap<ADNAtom> atomList;

  auto children = *getChildren();
  SB_FOR(SBStructuralNode* n, children) {
    if (n->getType() == SBNode::Atom) {
      ADNAtom* a = static_cast<ADNAtom*>(n);
      atomList.addReferenceTarget(a);
    }
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

PositionableSB::PositionableSB()
{
  centerAtom_ = new ADNAtom();
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

void PositionableSB::HideCenterAtom()
{
  centerAtom_->setVisibilityFlag(false);
}
