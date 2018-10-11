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

  SBNodeIndexer nodeIndexer;
  SAMSON::getActiveDocument()->getNodes(nodeIndexer, (SBNode::GetClass() == std::string("ADNSingleStrand")) && (SBNode::GetElementUUID() == SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));

  SB_FOR(SBNode* n, nodeIndexer) {
    singleStrands.addReferenceTarget(static_cast<ADNSingleStrand*>(n));
  }

  return singleStrands;
}

void ADNNanorobot::RegisterPart(ADNPointer<ADNPart> part)
{
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

CollectionMap<ADNBaseSegment> ADNNanorobot::GetSelectedBaseSegments()
{
  CollectionMap<ADNBaseSegment> bss;

  SBDocument* doc = SAMSON::getActiveDocument();
  SBNodeIndexer nodes;
  doc->getNodes(nodes, (SBNode::GetClass() == std::string("ADNBaseSegment")) && (SBNode::GetElementUUID() == SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));

  // only take one
  SB_FOR(SBNode* node, nodes) {
    if (node->isSelected()) {
      ADNPointer<ADNBaseSegment> bs = static_cast<ADNBaseSegment*>(node);
      bss.addReferenceTarget(bs());
    }
  }

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

CollectionMap<ADNSingleStrand> ADNNanorobot::GetSingleStrands(ADNPointer<ADNPart> p)
{
  return p->GetSingleStrands();
}

void ADNNanorobot::RemoveSingleStrand(ADNPointer<ADNSingleStrand> ss)
{
  auto part = GetPart(ss);
  part->DeregisterSingleStrand(ss);
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

ADNPointer<ADNNucleotide> ADNNanorobot::GetNucleotideNext(ADNPointer<ADNNucleotide> nt)
{
  return nt->GetNext();
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

void ADNNanorobot::RegisterConformation(ADNPointer<ADNConformation> conformation)
{
  conformationsIndex_.addReferenceTarget(conformation());
}
