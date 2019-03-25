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

void ADNAtom::serialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber) const
{
  SBAtom::serialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);
}

void ADNAtom::unserialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber)
{
  SBAtom::unserialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);
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

void ADNNucleotide::serialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber) const
{
  SBResidue::serialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);

  serializer->writeIntElement("end", end_);

  SBPosition3 pos = GetPosition();
  serializer->writeStartElement("position");
  serializer->writeFloatElement("x", pos[0].getValue());
  serializer->writeFloatElement("y", pos[1].getValue());
  serializer->writeFloatElement("z", pos[2].getValue());
  serializer->writeEndElement();

  serializer->writeUnsignedIntElement("pair", nodeIndexer.getIndex(pair_()));
  serializer->writeUnsignedIntElement("base_segment", nodeIndexer.getIndex(bs_()));
}

void ADNNucleotide::unserialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber)
{
  SBResidue::unserialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);

  SetEnd(End(serializer->readIntElement()));

  SBPosition3 pos;
  serializer->readStartElement();
  pos[0] = SBQuantity::picometer(serializer->readFloatElement());
  pos[1] = SBQuantity::picometer(serializer->readFloatElement());
  pos[2] = SBQuantity::picometer(serializer->readFloatElement());
  serializer->readEndElement();
  SetPosition(pos);

  unsigned int pIdx = serializer->readUnsignedIntElement();
  unsigned int bsIdx = serializer->readUnsignedIntElement();
  SBNode* pNode = nodeIndexer.getNode(pIdx);
  SBNode* bsNode = nodeIndexer.getNode(bsIdx);
  ADNPointer<ADNNucleotide> p = static_cast<ADNNucleotide*>(pNode);
  ADNPointer<ADNBaseSegment> bs = static_cast<ADNBaseSegment*>(bsNode);
  SetPair(p);
  SetBaseSegment(bs);
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

ADNPointer<ADNNucleotide> ADNNucleotide::GetPair() const
{
  return pair_;
}

SBNode * ADNNucleotide::getPair() const
{
  return GetPair()();
}

ADNPointer<ADNNucleotide> ADNNucleotide::GetPrev(bool checkCircular) const
{
  ADNPointer<ADNNucleotide> p = static_cast<ADNNucleotide*>(getPreviousNucleicAcid());

  if (checkCircular) {
    auto strand = GetStrand();
    if (strand->IsCircular() && end_ == FivePrime) {
      p = strand->GetThreePrime();
    }
  }

  return p;
}

SBNode * ADNNucleotide::getPrev() const
{
  return GetPrev(true)();
}

ADNPointer<ADNNucleotide> ADNNucleotide::GetNext(bool checkCircular) const
{
  ADNPointer<ADNNucleotide> p = static_cast<ADNNucleotide*>(getNextNucleicAcid());
  
  if (checkCircular) {
    auto strand = GetStrand();
    if (strand->IsCircular() && end_ == ThreePrime) {
      p = strand->GetFivePrime();
    }
  }

  return p;
}

SBNode * ADNNucleotide::getNext() const
{
  return GetNext(true)();
}

ADNPointer<ADNSingleStrand> ADNNucleotide::GetStrand() const
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

ADNPointer<ADNAtom> ADNNucleotide::GetBackboneCenterAtom()
{
  auto bb = GetBackbone();
  return bb->GetCenterAtom();
}

ADNPointer<ADNAtom> ADNNucleotide::GetSidechainCenterAtom()
{
  auto sc = GetSidechain();
  return sc->GetCenterAtom();
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

std::string ADNNucleotide::getEndType() const
{
  std::string s;
  if (end_ == FivePrime) {
    s = "5'";
  }
  else if (end_ == ThreePrime) {
    s = "3'";
  }
  else if (end_ == FiveAndThreePrime) {
    s = "5' and 3'";
  }
  else if (end_ == NotEnd) {
    s = "Not end";
  }
  return s;
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

void ADNNucleotide::Init()
{
  SetType(DNABlocks::DI);
  ADNPointer<ADNBackbone> bb = new ADNBackbone();
  bb->setName(getName() + " " + "Backbone");
  ADNPointer<ADNSidechain> sc = new ADNSidechain();
  sc->setName(getName() + " " + "Sidechain");
  addChild(bb());
  addChild(sc());
}

ADNPointer<ADNBackbone> ADNNucleotide::GetBackbone() const
{
  auto bb = static_cast<ADNBackbone*>(getBackbone());
  return ADNPointer<ADNBackbone>(bb);
}

void ADNNucleotide::SetBackbone(ADNPointer<ADNBackbone> bb)
{
  auto bbOld = GetBackbone();
  bbOld = bb;
}

ADNPointer<ADNSidechain> ADNNucleotide::GetSidechain() const
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

bool ADNNucleotide::IsLeft()
{
  return bs_->IsLeft(this);
}

bool ADNNucleotide::IsRight()
{
  return bs_->IsRight(this);
}

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

void ADNSingleStrand::serialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber) const
{
  SBChain::serialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);

  serializer->writeBoolElement("isScaffold", IsScaffold());
  serializer->writeBoolElement("isCircular", IsCircular());
  serializer->writeUnsignedIntElement("fivePrime", nodeIndexer.getIndex(fivePrime_()));
  serializer->writeUnsignedIntElement("threePrime", nodeIndexer.getIndex(threePrime_()));
}

void ADNSingleStrand::unserialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber)
{
  SBChain::unserialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);

  bool isScaffold = serializer->readBoolElement();
  bool isCircular = serializer->readBoolElement();
  unsigned int fPrimeIdx = serializer->readUnsignedIntElement();
  unsigned int tPrimeIdx = serializer->readUnsignedIntElement();
  IsScaffold(isScaffold);
  IsCircular(isCircular);
  SBNode* fPrime = nodeIndexer.getNode(fPrimeIdx);
  ADNPointer<ADNNucleotide> fp = static_cast<ADNNucleotide*>(fPrime);
  SBNode* tPrime = nodeIndexer.getNode(tPrimeIdx);
  ADNPointer<ADNNucleotide> tp = static_cast<ADNNucleotide*>(tPrime);
  fivePrime_ = fp;
  threePrime_ = tp;
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

ADNPointer<ADNNucleotide> ADNSingleStrand::GetNthNucleotide(int n)
{
  ADNPointer<ADNNucleotide> nt = nullptr;
  if (n <= getNumberOfNucleotides()) {
    nt = fivePrime_;
    for (int i = 0; i < n; ++i) {
      nt = nt->GetNext();
    }
  }
  
  return nt;
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

void ADNSingleStrand::IsCircular(bool c)
{
  isCircular_ = c;
}

bool ADNSingleStrand::IsCircular() const
{
  return isCircular_;
}

bool ADNSingleStrand::getIsCircular() const
{
  return IsCircular();
}

void ADNSingleStrand::setIsCircular(bool b)
{
  IsCircular(b);
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

  std::string seq = GetSequence();
  auto origThreePrime = threePrime_;
  auto loopNt = origThreePrime;
  auto stopNt = nt->GetPrev();

  while (loopNt != stopNt) {
    auto cpNt = loopNt;
    loopNt = loopNt->GetPrev();
    removeChild(cpNt());
    AddNucleotideFivePrime(cpNt);
  }
  
  stopNt->SetEnd(ThreePrime);
  threePrime_ = stopNt;

  if (shiftSeq) {
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
    DNABlocks type = DNABlocks::DI;
    if (seq.size() > count) type = ADNModel::ResidueNameToType(seq[count]);
    nt->SetType(type);
    if (nt->GetPair() != nullptr) {
      DNABlocks compType = ADNModel::GetComplementaryBase(type);
      nt->GetPair()->SetType(compType);
    }
    nt = nt->GetNext();
    ++count;
  }
}

void ADNSingleStrand::setSequence(std::string seq)
{
  SetSequence(seq);
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
    SetCell(new ADNBasePair());
  }
  else if (cellType == LoopPair) {
    SetCell(new ADNLoopPair());
  }
  else if (cellType == SkipPair) {
    SetCell(new ADNSkipPair());
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

void ADNBaseSegment::serialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber) const
{
  SBStructuralGroup::serialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);
  
  serializer->writeIntElement("number", GetNumber());
  serializer->writeUnsignedIntElement("cell", nodeIndexer.getIndex(cell_()));
}

void ADNBaseSegment::unserialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber)
{
  SBStructuralGroup::unserialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);
  
  SetNumber(serializer->readIntElement());
  SBNode* cNode = nodeIndexer.getNode(serializer->readUnsignedIntElement());
  ADNPointer<ADNCell> cell = static_cast<ADNCell*>(cNode);
  SetCell(cell());
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

ADNPointer<ADNBaseSegment> ADNBaseSegment::GetPrev(bool checkCircular) const
{
  ADNPointer<ADNBaseSegment> p = static_cast<ADNBaseSegment*>(getPreviousStructuralNode());

  if (checkCircular) {
    auto ds = GetDoubleStrand();
    if (ds->IsCircular() && GetNumber() == 0) {
      // is the first bs
      p = ds->GetLastBaseSegment();
    }
  }
  return p;
}

ADNPointer<ADNBaseSegment> ADNBaseSegment::GetNext(bool checkCircular) const
{
  ADNPointer<ADNBaseSegment> p = static_cast<ADNBaseSegment*>(getNextStructuralNode());

  if (checkCircular) {
    auto ds = GetDoubleStrand();
    if (ds->IsCircular() && this == ds->GetLastBaseSegment()()) {
      // is the last bs
      p = ds->GetFirstBaseSegment();
    }
  }
  return p;
}

bool ADNBaseSegment::IsEnd()
{
  bool e = IsFirst() || IsLast();

  return e;
}

bool ADNBaseSegment::IsFirst()
{
  bool e = false;
  auto ds = GetDoubleStrand();
  if (ds->GetFirstBaseSegment() == this) e = true;

  return e;
}

bool ADNBaseSegment::IsLast()
{
  bool e = false;
  auto ds = GetDoubleStrand();
  if (ds->GetLastBaseSegment() == this) e = true;

  return e;
}

ADNPointer<ADNDoubleStrand> ADNBaseSegment::GetDoubleStrand() const
{
  auto p = static_cast<ADNDoubleStrand*>(getParent());
  return ADNPointer<ADNDoubleStrand>(p);
}

SBNode * ADNBaseSegment::getDoubleStrand() const
{
  return GetDoubleStrand()();
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

bool ADNBaseSegment::IsLeft(ADNPointer<ADNNucleotide> nt)
{
  return cell_->IsLeft(nt);
}

bool ADNBaseSegment::IsRight(ADNPointer<ADNNucleotide> nt)
{
  return cell_->IsRight(nt);
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

void ADNDoubleStrand::serialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber) const
{
  SBStructuralGroup::serialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);
  
  serializer->writeBoolElement("isCircular", IsCircular());
  serializer->writeDoubleElement("twistAngle", GetInitialTwistAngle());
  serializer->writeUnsignedIntElement("start", nodeIndexer.getIndex(start_()));
  serializer->writeUnsignedIntElement("end", nodeIndexer.getIndex(end_()));
}

void ADNDoubleStrand::unserialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber)
{
  SBStructuralGroup::unserialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);
  
  IsCircular(serializer->readBoolElement());
  SetInitialTwistAngle(serializer->readDoubleElement());
  unsigned int sIdx = serializer->readUnsignedIntElement();
  unsigned int eIdx = serializer->readUnsignedIntElement();
  SBNode* sNode = nodeIndexer.getNode(sIdx);
  SBNode* eNode = nodeIndexer.getNode(eIdx);
  start_ = static_cast<ADNBaseSegment*>(sNode);
  end_ = static_cast<ADNBaseSegment*>(eNode);
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

void ADNDoubleStrand::IsCircular(bool c)
{
  isCircular_ = c;
}

bool ADNDoubleStrand::IsCircular() const
{
  return isCircular_;
}

bool ADNDoubleStrand::getIsCircular() const
{
  return IsCircular();
}

void ADNDoubleStrand::setIsCircular(bool b)
{
  IsCircular(b);
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

void ADNBasePair::serialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber) const
{
  ADNCell::serialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);

  serializer->writeUnsignedIntElement("left", nodeIndexer.getIndex(left_()));
  serializer->writeUnsignedIntElement("right", nodeIndexer.getIndex(right_()));
}

void ADNBasePair::unserialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber)
{
  ADNCell::unserialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);

  unsigned int lIdx = serializer->readUnsignedIntElement();
  unsigned int rIdx = serializer->readUnsignedIntElement();

  SBNode* lNode = nodeIndexer.getNode(lIdx);
  SetLeftNucleotide(static_cast<ADNNucleotide*>(lNode));

  SBNode* rNode = nodeIndexer.getNode(rIdx);
  SetRightNucleotide(static_cast<ADNNucleotide*>(rNode));
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
}

void ADNBasePair::SetRemainingNucleotide(ADNPointer<ADNNucleotide> nt)
{
  if (left_ != nullptr && right_ == nullptr) {
    SetRightNucleotide(nt);
  }
  else if (left_ == nullptr && right_ != nullptr) {
    SetLeftNucleotide(nt);
  }
  PairNucleotides();
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

bool ADNBasePair::IsLeft(ADNPointer<ADNNucleotide> nt)
{
  bool s = false;
  if (left_ == nt) s = true;

  return s;
}

bool ADNBasePair::IsRight(ADNPointer<ADNNucleotide> nt)
{
  bool s = false;
  if (right_ == nt) s = true;

  return s;
}

void ADNSkipPair::serialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber) const
{
  ADNCell::serialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);
}

void ADNSkipPair::unserialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber)
{
  ADNCell::unserialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);
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

bool ADNLoopPair::IsRight(ADNPointer<ADNNucleotide> nt)
{
  bool s = false;
  auto nts = right_->GetNucleotides();
  SB_FOR(ADNPointer<ADNNucleotide> c, nts) {
    if (c == nt) {
      s = true;
      break;
    }
  }

  return s;
}

bool ADNLoopPair::IsLeft(ADNPointer<ADNNucleotide> nt)
{
  bool s = false;
  auto nts = left_->GetNucleotides();
  SB_FOR(ADNPointer<ADNNucleotide> c, nts) {
    if (c == nt) {
      s = true;
      break;
    }
  }

  return s;
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

void ADNLoop::serialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber) const
{
  SBStructuralGroup::serialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);
  
  serializer->writeUnsignedIntElement("startNt", nodeIndexer.getIndex(startNt_()));
  serializer->writeUnsignedIntElement("endNt", nodeIndexer.getIndex(endNt_()));
  serializer->writeUnsignedIntElement("numNt", getNumberOfNucleotides());
  serializer->writeStartElement("nucleotides");

  SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides_) {
    serializer->writeUnsignedIntElement("id", nodeIndexer.getIndex(nt()));
  }

  serializer->writeEndElement();
}

void ADNLoop::unserialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber)
{
  SBStructuralGroup::unserialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);
  
  unsigned int sIdx = serializer->readUnsignedIntElement();
  unsigned int eIdx = serializer->readUnsignedIntElement();
  unsigned int numNt = serializer->readUnsignedIntElement();
  serializer->readStartElement();

  for (unsigned int i = 0; i < numNt; ++i) {
    unsigned int idx = serializer->readUnsignedIntElement();
    SBNode* node = nodeIndexer.getNode(idx);
    ADNPointer<ADNNucleotide> nt = static_cast<ADNNucleotide*>(node);
    AddNucleotide(nt);
  }

  serializer->readEndElement();

  SBNode* sNode = nodeIndexer.getNode(sIdx);
  SetStart(static_cast<ADNNucleotide*>(sNode));
  SBNode* eNode = nodeIndexer.getNode(eIdx);
  SetEnd(static_cast<ADNNucleotide*>(eNode));
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
  nucleotides_.addReferenceTarget(nt());
}

void ADNLoop::RemoveNucleotide(ADNPointer<ADNNucleotide> nt) {
  nucleotides_.removeReferenceTarget(nt());
}

bool ADNLoop::IsEmpty() {
  bool empty = true;
  if (GetNucleotides().size() > 0) empty = false;
  return empty;
}

void ADNLoopPair::serialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber) const
{
  ADNCell::serialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);
  
  serializer->writeUnsignedIntElement("left", nodeIndexer.getIndex(left_()));
  serializer->writeUnsignedIntElement("right", nodeIndexer.getIndex(right_()));
}

void ADNLoopPair::unserialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber)
{
  ADNCell::unserialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);
  
  unsigned int lIdx = serializer->readUnsignedIntElement();
  unsigned int rIdx = serializer->readUnsignedIntElement();
  ADNPointer<ADNLoop> lp = static_cast<ADNLoop*>(nodeIndexer.getNode(lIdx));
  ADNPointer<ADNLoop> rp = static_cast<ADNLoop*>(nodeIndexer.getNode(rIdx));
  SetLeftLoop(lp);
  SetRightLoop(rp);
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

void ADNBackbone::serialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber) const
{
  SBBackbone::serialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);

  ADNPointer<ADNAtom> at = GetCenterAtom();
  serializer->writeUnsignedIntElement("centerAtom", nodeIndexer.getIndex(at()));
}

void ADNBackbone::unserialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber)
{
  SBBackbone::unserialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);

  unsigned int idx = serializer->readUnsignedIntElement();
  ADNPointer<ADNAtom> at = (ADNAtom*)nodeIndexer.getNode(idx);
  SetCenterAtom(at);
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

void ADNSidechain::serialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber) const
{
  SBSideChain::serialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);

  ADNPointer<ADNAtom> at = GetCenterAtom();
  serializer->writeUnsignedIntElement("centerAtom", nodeIndexer.getIndex(at()));
}

void ADNSidechain::unserialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber)
{
  SBSideChain::unserialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);

  unsigned int idx = serializer->readUnsignedIntElement();
  ADNPointer<ADNAtom> at = (ADNAtom*)nodeIndexer.getNode(idx);
  SetCenterAtom(at);
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

void ADNCell::serialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber) const
{
  SBStructuralGroup::serialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);
}

void ADNCell::unserialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber)
{
  SBStructuralGroup::unserialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);
}
