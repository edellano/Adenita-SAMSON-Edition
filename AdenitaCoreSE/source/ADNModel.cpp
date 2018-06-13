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

char ADNNucleotide::GetName() {
  return ADNModel::GetResidueName(GetType());
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
  SB_FOR(ADNAtom* a, scAtoms) atoms.addReferenceTarget(a);

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

void ADNNucleotide::SetSidechainPosition(ublas::vector<double> pos)
{
  auto sc = GetSidechain();
  sc->SetPosition(pos);
}

ublas::vector<double> ADNNucleotide::GetSidechainPosition()
{
  auto sc = GetSidechain();
  return sc->GetPosition();
}

void ADNNucleotide::SetBackbonePosition(ublas::vector<double> pos)
{
  auto bb = GetBackbone();
  bb->SetPosition(pos);
}

ublas::vector<double> ADNNucleotide::GetBackbonePosition()
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

ADNSingleStrand::ADNSingleStrand(const ADNSingleStrand & other) : Identifiable(other)
{
  isScaffold_ = other.isScaffold_;
  fivePrime_ = other.fivePrime_;
  threePrime_ = other.threePrime_;
}

ADNSingleStrand & ADNSingleStrand::operator=(const ADNSingleStrand & other) {
  ADNSingleStrand* ss = new ADNSingleStrand(other);
  return *ss;
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
  return GetCollection();
}

ADNPointer<ADNNucleotide> ADNSingleStrand::GetNucleotide(int id) const
{
  return GetElement(id);
}

void ADNSingleStrand::AddNucleotideThreePrime(ADNPointer<ADNNucleotide> nt)
{
  nt->SetNext(ADNPointer<ADNNucleotide>());
  nt->SetPrev(threePrime_);
  if (threePrime_ != nullptr) {
    threePrime_->SetNext(nt);
    threePrime_->SetEnd(NotEnd);
  }
  else {
    // nt is also fivePrime_
    fivePrime_ = nt;
  }
  nt->SetEnd(ThreePrime);
  if (nt->GetId() == -1) nt->SetId(GetLastKey() + 1);
  threePrime_ = nt;
  AddElement(nt, nt->GetId());
  nt->SetStrand(shared_from_this());
}

void ADNSingleStrand::AddNucleotideFivePrime(ADNPointer<ADNNucleotide> nt)
{
  nt->SetPrev(ADNPointer<ADNNucleotide>());
  nt->SetNext(fivePrime_);
  if (fivePrime_ != nullptr) {
    fivePrime_->SetPrev(nt);
    fivePrime_->SetEnd(NotEnd);
  }
  else {
    // nt is also fivePrime_
    threePrime_ = nt;
  }
  nt->SetEnd(FivePrime);
  if (nt->GetId() == -1) nt->SetId(GetLastKey() + 1);
  fivePrime_ = nt;
  AddElement(nt, nt->GetId());
  nt->SetStrand(shared_from_this());
}

//SBPointer<SBChain> ANTSingleStrand::CreateSBChain(bool use_btta) {
//  SBPointer<SBChain> chain = new SBChain();
//  chain_ = chain;
//  chain->setName(chainName_);
//
//  auto nucleotides = nucleotides_;
//  ANTNucleotide* nt_first = fivePrime_;
//  ANTNucleotide* nt = nt_first;
//  ANTNucleotide* nt_prev = nullptr;
//  do {
//    SBPointer<SBResidue> res = nt->CreateSBResidue();
//    // link residues (if all-atom model was produced!)
//    // todo: deal better with non-paired regions!!
//    if (nt_prev != nullptr && use_btta && nt->atomList_.size() > 0 && nt_prev->atomList_.size() > 0) {
//      // previous O3'
//      ANTAtom* o3_prime = nt_prev->GetAtom("O3'");
//      ANTAtom* p = nt->GetAtom("P");
//      SBPointer<SBBond> link = new SBBond(o3_prime->sbAtom_(), p->sbAtom_());
//      SBBackbone* bb = res->getBackbone();
//      bb->addChild(link());
//    }
//    chain->addChild(res());
//    nt_prev = nt;
//    nt = nt->next_;
//  } while (nt != nullptr);
//
//  return chain;
//}

void ADNSingleStrand::ShiftStart(ADNPointer<ADNNucleotide> nt, bool shiftSeq) {
  if (nt == fivePrime_) return;

  std::string seq = GetSequence();
  threePrime_ = nt->GetPrev();
  nt->SetPrev(ADNPointer<ADNNucleotide>());
  ADNPointer<ADNNucleotide> nuc = nt;
  int i = 0;
  // move sequence
  do {
    if (shiftSeq) {
      nuc->SetType(residue_names_.right.at(seq[i]));
      if (nuc->GetPair() != nullptr) nuc->GetPair()->SetType(ADNModel::GetComplementaryBase(nuc->GetType()));
    }
    if (nuc->GetNext() == nullptr) {
      nuc->SetNext(fivePrime_);
      fivePrime_->SetPrev(nuc);
      nuc = fivePrime_;
    }
    else if (nuc->GetNext() == nt) {
      nuc->SetNext(ADNPointer<ADNNucleotide>());
      nuc = nt;
    }
    else {
      nuc = nuc->GetNext();
    }
    ++i;
  } while (nuc != nt);
  fivePrime_ = nt;
}

std::string ADNSingleStrand::GetSequence() {
  std::string seq = "";
  ADNPointer<ADNNucleotide> nt = fivePrime_;
  do {
    seq += residue_names_.left.at(nt->GetType());
    nt = nt->GetNext();
  } while (nt != nullptr);
  return seq;
}

double ADNSingleStrand::GetGCContent() {
  double gcCont = 0.0;
  auto nucleotides = GetNucleotides();

  for (auto &nt : nucleotides) {
    if (nt.second->GetType() == DNABlocks::DC || nt.second->GetType() == DNABlocks::DG) {
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
    DNABlocks type = residue_names_.right.at(seq[count]);
    nt->SetType(type);
    std::string name = seq[count] + std::to_string(nt->GetId());
    //nt->SetName(name);
    if (isScaffold_ && nt->GetPair() != nullptr) {
      DNABlocks compType = ADNModel::GetComplementaryBase(type);
      nt->GetPair()->SetType(compType);
      std::string namePair = std::string(1, residue_names_.left.at(compType)) + std::to_string(nt->GetPair()->GetId());
      //nt->pair_->SetName(namePair);
    }
    nt = nt->GetNext();
    ++count;
  }
}

void ADNSingleStrand::SetDefaultName() {
  std::string name = "Strand";
  if (isScaffold_) name = "Scaffold";
  SetName(name + " " + std::to_string(GetId()));
}

//void ADNSingleStrand::WriteToJson(Writer<StringBuffer>& writer) {
//  std::string key = std::to_string(id_);
//  writer.Key(key.c_str());
//  writer.StartObject();
//
//  writer.Key("id");
//  writer.Int(id_);
//
//  writer.Key("chainName");
//  writer.String(chainName_.c_str());
//
//  writer.Key("isScaffold");
//  writer.Bool(isScaffold_);
//
//  writer.Key("fivePrimeId");
//  writer.Int(fivePrime_->id_);
//
//  writer.Key("threePrimeId");
//  writer.Int(threePrime_->id_);
//
//  writer.Key("nucleotides");
//  writer.StartObject();
//  for (auto &pair : nucleotides_) {
//    ADNNucleotide* nt = pair.second;
//    nt->WriteToJson(writer);
//  }
//  writer.EndObject();
//
//  writer.EndObject();
//}

DNABlocks ADNModel::GetComplementaryBase(DNABlocks base) {
  DNABlocks comp_base = DN;
  if (base == DA) {
    comp_base = DT;
  }
  else if (base == DT) {
    comp_base = DA;
  }
  else if (base == DG) {
    comp_base = DC;
  }
  else if (base == DC) {
    comp_base = DG;
  }
  return comp_base;
}

char ADNModel::GetResidueName(DNABlocks t)
{
  std::string name = SBResidue::getResidueTypeString(t);
  return name[0];
}

//void ANTJoint::WriteToJson(Writer<StringBuffer>& writer) {
//  std::string key = std::to_string(id_);
//  writer.Key(key.c_str());
//  writer.StartObject();
//
//  writer.Key("id");
//  writer.Int(id_);
//
//  writer.Key("edge");
//  if (edge_ != nullptr) {
//    writer.Int(edge_->id_);
//  }
//  else {
//    writer.Int(-1);
//  }
//
//  //writer.Key("position");
//  //writer.String(ADNAuxiliary::SBPosition3ToString(position_).c_str());
//
//  //writer.Key("position2D");
//  //writer.String(ADNAuxiliary::SBPosition3ToString(position2D_).c_str());
//
//  //writer.Key("position1D");
//  //writer.String(ADNAuxiliary::SBPosition3ToString(position1D_).c_str());
//
//  //writer.EndObject();
//}

//SBPosition3 ANTJoint::GetSBPosition() const {
//  return position_;
//}
//
//void ANTJoint::SetCoordinates(SBPosition3 coordinates) {
//  position_ = coordinates;
//}

//ANTJoint::ANTJoint(SBPosition3 position) {
//  SetCoordinates(position);
//}

ADNBaseSegment::ADNBaseSegment(const ADNBaseSegment & other) : Identifiable(other), Positionable(other), Orientable(other)
{
  *this = other;
}

ADNBaseSegment & ADNBaseSegment::operator=(const ADNBaseSegment & other) {
  Positionable::operator =(other);
  Orientable::operator =(other);
  Identifiable::operator =(other);

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

int ADNBaseSegment::GetNumber()
{
  return number_;
}

void ADNBaseSegment::SetPrev(ADNPointer<ADNBaseSegment> bs)
{
  previous_ = ADNWeakPointer<ADNBaseSegment>(bs);
}

ADNPointer<ADNBaseSegment> ADNBaseSegment::GetPrev()
{
  return previous_;
}

void ADNBaseSegment::SetNext(ADNPointer<ADNBaseSegment> bs)
{
  next_ = ADNWeakPointer<ADNBaseSegment>(bs);
}

ADNPointer<ADNBaseSegment> ADNBaseSegment::GetNext()
{
  return next_;
}

void ADNBaseSegment::SetDoubleStrand(ADNPointer<ADNDoubleStrand> ds)
{
  doubleStrand_ = ADNWeakPointer<ADNDoubleStrand>(ds);
}

ADNPointer<ADNDoubleStrand> ADNBaseSegment::GetDoubleStrand()
{
  return doubleStrand_;
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

void ADNBaseSegment::SetCell(ADNPointer<ADNCell> c) {
  cell_ = c;
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

//void ADNBaseSegment::WriteToJson(Writer<StringBuffer>& writer) {
//  std::string key = std::to_string(id_);
//  writer.Key(key.c_str());
//  writer.StartObject();
//
//  writer.Key("id");
//  writer.Int(id_);
//
//  writer.Key("number");
//  writer.Int(number_);
//
//  //writer.Key("target");
//  //writer.Int(target_->id_);
//
//  //writer.Key("source");
//  //writer.Int(source_->id_);
//
//  writer.Key("pair");
//  if (pair_ != nullptr) writer.Int(pair_->id_);
//  else writer.Int(-1);
//
//  //writer.Key("halfedge");
//  //if (halfEdge_ != nullptr) writer.Int(halfEdge_->id_);
//  //else writer.Int(-1);
//
//  //writer.Key("face");
//  //if (face_ != nullptr) writer.Int(face_->id_);
//  //else writer.Int(-1);
//
//  writer.Key("next");
//  if (next_ != nullptr) writer.Int(next_->id_);
//  else writer.Int(-1);
//
//  writer.Key("previous");
//  if (previous_ != nullptr) writer.Int(previous_->id_);
//  else writer.Int(-1);
//
//  writer.Key("cell");
//  writer.StartObject();
//  cell_->WriteToJson(writer);
//  writer.EndObject();
//
//  //writer.Key("normal");
//  //writer.String(ADNAuxiliary::SBVector3ToString(normal_).c_str());
//
//  //writer.Key("direction");
//  //writer.String(ADNAuxiliary::SBVector3ToString(direction_).c_str());
//
//  //writer.Key("u");
//  //writer.String(ADNAuxiliary::SBVector3ToString(u_).c_str());
//
//  writer.Key("double_strand");
//  writer.Int(doubleStrand_->id_);
//
//  writer.EndObject();
//}

ADNDoubleStrand::ADNDoubleStrand(int numBases) : Identifiable(), Collection<ADNBaseSegment>()
{
  for (int i = 0; i < numBases; ++i) {
    ADNPointer<ADNBaseSegment> bs = ADNPointer<ADNBaseSegment>(new ADNBaseSegment());;
    bs->SetId(GetLastKey() + 1);
    AddBaseSegmentEnd(bs);
  }
}

ADNDoubleStrand::ADNDoubleStrand(std::vector<ADNPointer<ADNBaseSegment>> bss)
{
  for (auto bs : bss) {
    bs->SetId(GetLastKey() + 1);
    AddBaseSegmentEnd(bs);
  }
}

ADNDoubleStrand::ADNDoubleStrand(const ADNDoubleStrand & other) : Identifiable(other), Collection<ADNBaseSegment>(other)
{
  *this = other;
}

ADNDoubleStrand & ADNDoubleStrand::operator=(const ADNDoubleStrand & other) {
  
  Collection<ADNBaseSegment>::operator =(other);
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
  return GetCollection();
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
  bs->SetPrev(ADNPointer<ADNBaseSegment>());
  bs->SetNext(start_);
  int number = 0;
  if (start_!= nullptr) {
    number = start_->GetNumber() - 1;
  }
  bs->SetNumber(number);
  if (start_ != nullptr) {
    start_->SetPrev(bs);
  }
  else {
    end_ = bs;
  }
  if (bs->GetId() == -1) bs->SetId(GetLastKey() + 1);
  start_ = bs;
  AddElement(bs, bs->GetId());
  bs->SetDoubleStrand(shared_from_this());
}

void ADNDoubleStrand::AddBaseSegmentEnd(ADNPointer<ADNBaseSegment> bs)
{
  bs->SetNext(ADNPointer<ADNBaseSegment>());
  bs->SetPrev(end_);
  int number = 0;
  if (end_ != nullptr) {
    number = end_->GetNumber() + 1;
  }
  bs->SetNumber(number);
  if (end_ != nullptr) {
    end_->SetNext(bs);
  }
  else {
    // nt is also fivePrime_
    start_ = bs;
  }
  if (bs->GetId() < 0) bs->SetId(GetLastKey() + 1);
  end_ = bs;
  AddElement(bs, bs->GetId());
  bs->SetDoubleStrand(shared_from_this());
}

//void ANTDoubleStrand::WriteToJson(Writer<StringBuffer>& writer) {
//  std::string key = std::to_string(id_);
//  writer.Key(key.c_str());
//  writer.StartObject();
//
//  writer.Key("id");
//  writer.Int(id_);
//
//  writer.Key("size");
//  writer.Int(size_);
//
//  writer.Key("bsStartId");
//  writer.Int(start_->id_);
//
//  writer.Key("initialTwistAngle");
//  writer.Double(initialTwistAngle_);
//
//  writer.EndObject();
//}

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

void ADNLoop::SetStrand(ADNPointer<ADNSingleStrand> ss)
{
  strand_ = ADNWeakPointer<ADNSingleStrand>(ss);
}

ADNPointer<ADNSingleStrand> ADNLoop::GetStrand()
{
  return strand_;
}

CollectionMap<ADNNucleotide> ADNLoop::GetNucleotides() const
{
  return GetCollection();
}

void ADNLoop::AddNucleotide(ADNPointer<ADNNucleotide> nt) {
  if (nt->GetId() < 0) {
    int id = GetLastKey() + 1;
    nt->SetId(id);
  }
  AddElement(nt, nt->GetId());
}

void ADNLoop::RemoveNucleotide(ADNPointer<ADNNucleotide> nt) {
  DeleteElement(nt->GetId());
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

//void ANTLoop::WriteToJson(Writer<StringBuffer>& writer) {
//  int pair_id = -1;
//  int start_nt_id = -1;
//  int start_nt_strand_id = -1;
//  int end_nt_id = -1;
//  int end_nt_strand_id = -1;
//  int strand_id = -1;
//
//  int id_left = id_;
//  if (pair_ != nullptr) pair_id = pair_->id_;
//  if (startNt_ != nullptr) {
//    start_nt_id = startNt_->id_;
//    start_nt_strand_id = startNt_->strand_->id_;
//  }
//  if (endNt_ != nullptr) {
//    end_nt_id = endNt_->id_;
//    end_nt_strand_id = endNt_->strand_->id_;
//  }
//  if (strand_ != nullptr) strand_id = strand_->id_;
//
//  writer.Key("id");
//  writer.Int(id_left);
//
//  writer.Key("pair_id");
//  writer.Int(pair_id);
//
//  writer.Key("strand_id");
//  writer.Int(strand_id);
//
//  writer.Key("start_nt");
//  writer.StartObject();
//
//  writer.Key("nt_id");
//  writer.Int(start_nt_id);
//
//  writer.Key("strand_id");
//  writer.Int(start_nt_strand_id);
//
//  writer.EndObject();
//
//  writer.Key("end_nt");
//  writer.StartObject();
//
//  writer.Key("nt_id");
//  writer.Int(end_nt_id);
//
//  writer.Key("strand_id");
//  writer.Int(end_nt_strand_id);
//
//  writer.EndObject();
//
//  writer.Key("nucleotides_list");
//  writer.StartObject();
//  for (auto &n : nucleotides_) {
//    int ntId = n.second->id_;
//    int ssId = n.second->strand_->id_;
//    writer.Key(std::to_string(ntId).c_str());
//    writer.StartObject();
//    writer.Key("nt_id");
//    writer.Int(ntId);
//
//    writer.Key("strand_id");
//    writer.Int(ssId);
//    writer.EndObject();
//  }
//  writer.EndObject();
//}

bool ADNLoop::IsEmpty() {
  bool empty = true;
  if (GetCollection().size() > 0) empty = false;
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

  auto children = getChildren();
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

  auto children = getChildren();
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
