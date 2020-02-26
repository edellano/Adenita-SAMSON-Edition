#include "ADNNanorobot.hpp"


ADNNanorobot::ADNNanorobot(const ADNNanorobot & n) : Nameable(n), Positionable(n), Orientable(n)
{
  *this = n;
}

ADNNanorobot & ADNNanorobot::operator=(const ADNNanorobot& other)
{
  Nameable::operator =(other);
  Positionable::operator =(other);
  Orientable::operator =(other);

  return *this;
}

CollectionMap<ADNSingleStrand> ADNNanorobot::GetSingleStrands() const
{
  CollectionMap<ADNSingleStrand> singleStrands;

  /*SBNodeIndexer nodeIndexer;
  SAMSON::getActiveDocument()->getNodes(nodeIndexer, (SBNode::GetClass() == std::string("ADNSingleStrand")) && (SBNode::GetElementUUID() == SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));

  SB_FOR(SBNode* n, nodeIndexer) {
    singleStrands.addReferenceTarget(static_cast<ADNSingleStrand*>(n));
  }*/

  SB_FOR(ADNPointer<ADNPart> p, partsIndex_) {
    auto pSingleStrands = p->GetSingleStrands();
    SB_FOR(ADNPointer<ADNSingleStrand> ss, pSingleStrands) {
      singleStrands.addReferenceTarget(ss());
    }
  }

  return singleStrands;
}

void ADNNanorobot::RegisterPart(ADNPointer<ADNPart> part)
{
  if (part->getName().empty()) {
    part->setName("Adenita component " + std::to_string(partId_));
    ++partId_;
  }

  partsIndex_.addReferenceTarget(part());
}

void ADNNanorobot::DeregisterPart(ADNPointer<ADNPart> part)
{
  partsIndex_.removeReferenceTarget(part());
}

int ADNNanorobot::GetNumberOfDoubleStrands()
{
  auto parts = GetParts();
  int count = 0;

  SB_FOR (ADNPointer<ADNPart> part, parts) {
    count += part->GetNumberOfDoubleStrands();
  }

  return count;
}

int ADNNanorobot::GetNumberOfBaseSegments()
{
  auto parts = GetParts();
  int count = 0;

  SB_FOR(ADNPointer<ADNPart> part, parts) {
    count += part->GetNumberOfBaseSegments();
  }

  return count;
}

int ADNNanorobot::GetNumberOfSingleStrands()
{
  auto parts = GetParts();
  int count = 0;

  SB_FOR(ADNPointer<ADNPart> part, parts) {
    count += part->GetNumberOfSingleStrands();
  }

  return count;
}

int ADNNanorobot::GetNumberOfNucleotides()
{
  auto parts = GetParts();
  int count = 0;

  SB_FOR(ADNPointer<ADNPart> part, parts) {
    count += part->GetNumberOfNucleotides();
  }

  return count;
}

CollectionMap<ADNPart> ADNNanorobot::GetParts() const
{
  //CollectionMap<ADNPart> parts;

  //SBNodeIndexer nodeIndexer;
  //SAMSON::getActiveDocument()->getNodes(nodeIndexer, (SBNode::GetClass() == std::string("ADNPart")) && (SBNode::GetElementUUID() == SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")) );

  //SB_FOR(SBNode* n, nodeIndexer) {
  //  if (!n->isErased()) parts.addReferenceTarget(static_cast<ADNPart*>(n));
  //}

  //return parts;

  return partsIndex_;
}

ADNPointer<ADNPart> ADNNanorobot::GetPart(ADNPointer<ADNSingleStrand> ss)
{
  SBNode* parent = ss->getParent()->getParent();  // first parent is the structural model root
  ADNPointer<ADNPart> part = static_cast<ADNPart*>(parent);
  return part;
}

ADNPointer<ADNPart> ADNNanorobot::GetPart(ADNPointer<ADNDoubleStrand> ds)
{
  SBNode* parent = ds->getParent()->getParent();  // first parent is the structural model root
  ADNPointer<ADNPart> part = static_cast<ADNPart*>(parent);
  return part;
}

CollectionMap<ADNNucleotide> ADNNanorobot::GetSelectedNucleotides()
{
  CollectionMap<ADNNucleotide> nts;

  SBDocument* doc = SAMSON::getActiveDocument();
  SBNodeIndexer nodes;
  doc->getNodes(nodes, (SBNode::GetClass() == std::string("ADNNucleotide")) && (SBNode::GetElementUUID() == SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));

  // only take one
  SB_FOR(SBNode* node, nodes) {
    if (node->isSelected()) {
      ADNPointer<ADNNucleotide> nt = static_cast<ADNNucleotide*>(node);
      nts.addReferenceTarget(nt());
    }
  }

  return nts;
}

CollectionMap<ADNPart> ADNNanorobot::GetSelectedParts()
{
  CollectionMap<ADNPart> parts;

  SBDocument* doc = SAMSON::getActiveDocument();
  SBNodeIndexer nodes;
  doc->getNodes(nodes, (SBNode::GetClass() == std::string("ADNPart")) && (SBNode::GetElementUUID() == SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));

  // only take one
  SB_FOR(SBNode* node, nodes) {
    if (node->isSelected()) {
      ADNPointer<ADNPart> part = static_cast<ADNPart*>(node);
      parts.addReferenceTarget(part());
    }
  }

  return parts;
}

CollectionMap<SBAtom> ADNNanorobot::GetHighlightedAtoms()
{
  CollectionMap<SBAtom> atoms;

  SBNodeIndexer atomIndexer;
  SAMSON::getActiveDocument()->getNodes(atomIndexer, SBNode::IsType(SBNode::Atom));

  // only take one
  SB_FOR(SBNode* node, atomIndexer) {
    if (node->isHighlighted()) {
      ADNPointer<SBAtom> a = static_cast<SBAtom*>(node);
      atoms.addReferenceTarget(a());
    }
  }

  return atoms;
}

CollectionMap<ADNNucleotide> ADNNanorobot::GetHighlightedNucleotides()
{
  CollectionMap<ADNNucleotide> nts;

  SBDocument* doc = SAMSON::getActiveDocument();
  SBNodeIndexer nodes;
  doc->getNodes(nodes, (SBNode::GetClass() == std::string("ADNNucleotide")) && (SBNode::GetElementUUID() == SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));

  // only take one
  SB_FOR(SBNode* node, nodes) {
    if (node->isHighlighted()) {
      ADNPointer<ADNNucleotide> nt = static_cast<ADNNucleotide*>(node);
      nts.addReferenceTarget(nt());
    }
  }

  return nts;
}

CollectionMap<ADNBaseSegment> ADNNanorobot::GetSelectedBaseSegmentsFromNucleotides()
{
  CollectionMap<ADNBaseSegment> bss;

  auto nts = GetSelectedNucleotides();

  std::vector<ADNPointer<ADNBaseSegment>> added;

  SB_FOR(ADNPointer<ADNNucleotide> nt, nts) {
    ADNPointer<ADNBaseSegment> bs = nt->GetBaseSegment();
    if (std::find(added.begin(), added.end(), bs) == added.end()) {
      bss.addReferenceTarget(bs());
      added.push_back(bs);
    }
  }

  //SBDocument* doc = SAMSON::getActiveDocument();
  //SBNodeIndexer nodes;
  //doc->getNodes(nodes, (SBNode::GetClass() == std::string("ADNBaseSegment")) && (SBNode::GetElementUUID() == SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));

  //// only take one
  //SB_FOR(SBNode* node, nodes) {
  //  if (node->isSelected()) {
  //    ADNPointer<ADNBaseSegment> bs = static_cast<ADNBaseSegment*>(node);
  //    bss.addReferenceTarget(bs());
  //  }
  //}

  return bss;
}

CollectionMap<ADNSingleStrand> ADNNanorobot::GetSelectedSingleStrands()
{
  CollectionMap<ADNSingleStrand> singleStrands;

  SBDocument* doc = SAMSON::getActiveDocument();
  SBNodeIndexer nodes;
  doc->getNodes(nodes, (SBNode::GetClass() == std::string("ADNSingleStrand")) && (SBNode::GetElementUUID() == SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));

  // only take one
  SB_FOR(SBNode* node, nodes) {
    if (node->isSelected()) {
      ADNPointer<ADNSingleStrand> ss = static_cast<ADNSingleStrand*>(node);
      singleStrands.addReferenceTarget(ss());
    }
  }

  return singleStrands;
}

CollectionMap<ADNDoubleStrand> ADNNanorobot::GetSelectedDoubleStrands()
{
  CollectionMap<ADNDoubleStrand> doubleStrands;

  SBDocument* doc = SAMSON::getActiveDocument();
  SBNodeIndexer nodes;
  doc->getNodes(nodes, (SBNode::GetClass() == std::string("ADNDoubleStrand")) && (SBNode::GetElementUUID() == SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));

  // only take one
  SB_FOR(SBNode* node, nodes) {
    if (node->isSelected()) {
      ADNPointer<ADNDoubleStrand> ds = static_cast<ADNDoubleStrand*>(node);
      doubleStrands.addReferenceTarget(ds());
    }
  }

  return doubleStrands;
}

CollectionMap<ADNDoubleStrand> ADNNanorobot::GetHighlightedDoubleStrands()
{
  CollectionMap<ADNDoubleStrand> doubleStrands;

  SBDocument* doc = SAMSON::getActiveDocument();
  SBNodeIndexer nodes;
  doc->getNodes(nodes, (SBNode::GetClass() == std::string("ADNDoubleStrand")) && (SBNode::GetElementUUID() == SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));

  // only take one
  SB_FOR(SBNode* node, nodes) {
    if (node->isHighlighted()) {
      ADNPointer<ADNDoubleStrand> ds = static_cast<ADNDoubleStrand*>(node);
      doubleStrands.addReferenceTarget(ds());
    }
  }

  return doubleStrands;
}

CollectionMap<ADNSingleStrand> ADNNanorobot::GetSingleStrands(ADNPointer<ADNPart> p)
{
  return p->GetSingleStrands();
}

void ADNNanorobot::RemoveSingleStrand(ADNPointer<ADNSingleStrand> ss)
{
  auto part = GetPart(ss);
  part->DeregisterSingleStrand(ss);
}

void ADNNanorobot::RemoveDoubleStrand(ADNPointer<ADNDoubleStrand> ds)
{
  auto part = GetPart(ds);
  part->DeregisterDoubleStrand(ds);
}

void ADNNanorobot::AddSingleStrand(ADNPointer<ADNSingleStrand> ss, ADNPointer<ADNPart> part)
{
  part->RegisterSingleStrand(ss);
}

CollectionMap<ADNSingleStrand> ADNNanorobot::GetScaffolds(ADNPointer<ADNPart> p)
{
  return p->GetScaffolds();
}

CollectionMap<ADNDoubleStrand> ADNNanorobot::GetDoubleStrands(ADNPointer<ADNPart> p)
{
  return p->GetDoubleStrands();
}

ADNPointer<ADNDoubleStrand> ADNNanorobot::GetDoubleStrand(ADNPointer<ADNNucleotide> nt)
{
  auto bs = nt->GetBaseSegment();
  return bs->GetDoubleStrand();
}

CollectionMap<ADNNucleotide> ADNNanorobot::GetSingleStrandNucleotides(ADNPointer<ADNSingleStrand> ss)
{
  return ss->GetNucleotides();
}

ADNPointer<ADNNucleotide> ADNNanorobot::GetSingleStrandFivePrime(ADNPointer<ADNSingleStrand> ss)
{
  return ss->GetFivePrime();
}

bool ADNNanorobot::IsScaffold(ADNPointer<ADNSingleStrand> ss)
{
  return ss->IsScaffold();
}

End ADNNanorobot::GetNucleotideEnd(ADNPointer<ADNNucleotide> nt)
{
  return nt->GetEnd();
}

ADNPointer<ADNNucleotide> ADNNanorobot::GetNucleotideNext(ADNPointer<ADNNucleotide> nt, bool circular)
{
  return nt->GetNext(circular);
}

ADNPointer<ADNNucleotide> ADNNanorobot::GetNucleotidePair(ADNPointer<ADNNucleotide> nt)
{
  return nt->GetPair();
}

SBPosition3 ADNNanorobot::GetNucleotidePosition(ADNPointer<ADNNucleotide> nt)
{
  return nt->GetPosition();
}

SBPosition3 ADNNanorobot::GetNucleotideBackbonePosition(ADNPointer<ADNNucleotide> nt)
{
  return nt->GetBackbonePosition();
}

SBPosition3 ADNNanorobot::GetNucleotideSidechainPosition(ADNPointer<ADNNucleotide> nt)
{
  return nt->GetSidechainPosition();
}

void ADNNanorobot::HideCenterAtoms(ADNPointer<ADNNucleotide> nt)
{
  nt->HideCenterAtoms();
}

CollectionMap<ADNConformation> ADNNanorobot::GetConformations()
{
  return conformationsIndex_;
}

CollectionMap<ADNConformation> ADNNanorobot::GetConformations(ADNPointer<ADNPart> part)
{
  CollectionMap<ADNConformation> confs;
  SB_FOR(ADNPointer<ADNConformation> conf, conformationsIndex_) {
    auto parent = conf->getParent();
    ADNPointer<ADNPart> p = static_cast<ADNPart*>(parent);
    if (p == part) confs.addReferenceTarget(conf());
  }

  return confs;
}

void ADNNanorobot::RegisterConformation(ADNPointer<ADNConformation> conformation)
{
  conformationsIndex_.addReferenceTarget(conformation());
}

SBPosition3 ADNNanorobot::GetNucleotideBackbonePosition(ADNConformation conformation, ADNPointer<ADNNucleotide> nt)
{
  SBPosition3 pos;
  auto at = nt->GetBackboneCenterAtom();
  conformation.getPosition(at(), pos);
  return pos;
}

SBPosition3 ADNNanorobot::GetNucleotideSidechainPosition(ADNConformation conformation, ADNPointer<ADNNucleotide> nt)
{
  SBPosition3 pos;
  auto at = nt->GetSidechainCenterAtom();
  conformation.getPosition(at(), pos);
  return pos;
}

CollectionMap<ADNBaseSegment> ADNNanorobot::GetHighlightedBaseSegmentsFromNucleotides()
{
  CollectionMap<ADNBaseSegment> bss;

  auto nts = GetHighlightedNucleotides();

  std::vector<ADNPointer<ADNBaseSegment>> added;

  SB_FOR(ADNPointer<ADNNucleotide> nt, nts) {
    ADNPointer<ADNBaseSegment> bs = nt->GetBaseSegment();
    if (std::find(added.begin(), added.end(), bs) == added.end()) {
      bss.addReferenceTarget(bs());
      added.push_back(bs);
    }
  }


  return bss;
}

CollectionMap<ADNBaseSegment> ADNNanorobot::GetHighlightedBaseSegments()
{
  CollectionMap<ADNBaseSegment> bss;

  SBDocument* doc = SAMSON::getActiveDocument();
  SBNodeIndexer nodes;
  doc->getNodes(nodes, (SBNode::GetClass() == std::string("ADNBaseSegment")) && (SBNode::GetElementUUID() == SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));

  // only take one
  SB_FOR(SBNode* node, nodes) {
    if (node->isHighlighted()) {
      ADNPointer<ADNBaseSegment> bs = static_cast<ADNBaseSegment*>(node);
      bss.addReferenceTarget(bs());
    }
  }

  return bss;
}

void ADNNanorobot::InitBoundingBox()
{
  auto maxVal = SBQuantity::picometer(std::numeric_limits<double>::max());
  minBox_ = SBPosition3(maxVal, maxVal, maxVal);
  maxBox_ = SBPosition3(-maxVal, -maxVal, -maxVal);
}

void ADNNanorobot::SetBoundingBox(ADNPointer<ADNPart> newPart)
{
  auto bb = newPart->GetBoundingBox();
  if (bb.first[0] < minBox_[0]) minBox_[0] = bb.first[0];
  if (bb.first[1] < minBox_[1]) minBox_[1] = bb.first[1];
  if (bb.first[2] < minBox_[2]) minBox_[2] = bb.first[2];
  if (bb.second[0] > maxBox_[0]) maxBox_[0] = bb.second[0];
  if (bb.second[1] > maxBox_[1]) maxBox_[1] = bb.second[1];
  if (bb.second[2] > maxBox_[2]) maxBox_[2] = bb.second[2];
}

std::pair<SBPosition3, SBPosition3> ADNNanorobot::GetBoundingBox()
{
  return std::pair<SBPosition3, SBPosition3>(minBox_, maxBox_);
}

std::pair<SBPosition3, SBPosition3> ADNNanorobot::GetBoundingBoxForSelection()
{
  auto maxVal = SBQuantity::picometer(std::numeric_limits<double>::max());
  auto minBox = SBPosition3(maxVal, maxVal, maxVal);
  auto maxBox = SBPosition3(-maxVal, -maxVal, -maxVal);
  std::pair<SBPosition3, SBPosition3> bb(minBox, maxBox);

  auto parts = GetSelectedParts();
  InitBoundingBox();
  SB_FOR(ADNPointer<ADNPart> part, parts) {
    auto bb = part->GetBoundingBox();
    if (bb.first[0] < minBox[0]) minBox[0] = bb.first[0];
    if (bb.first[1] < minBox[1]) minBox[1] = bb.first[1];
    if (bb.first[2] < minBox[2]) minBox[2] = bb.first[2];
    if (bb.second[0] > maxBox[0]) maxBox[0] = bb.second[0];
    if (bb.second[1] > maxBox[1]) maxBox[1] = bb.second[1];
    if (bb.second[2] > maxBox[2]) maxBox[2] = bb.second[2];
  }

  return bb;
}

void ADNNanorobot::ResetBoundingBox()
{
  auto parts = GetParts();
  InitBoundingBox();
  SB_FOR(ADNPointer<ADNPart> part, parts) {
    SetBoundingBox(part);
  }
}
