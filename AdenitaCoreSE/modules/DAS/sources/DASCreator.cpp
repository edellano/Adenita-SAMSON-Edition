#include "DASCreator.hpp"

//ADNPart * DASEditor::CreateTwoTubes(size_t length, SBPosition3 start, SBVector3 direction, SBVector3 sepDir) {
//  ADNPart* part = new ADNPart();
//
//  // direction in which the double helices would be separated
//  //SBVector3 sepDir = SBVector3(direction[1], -direction[0], SBQuantity::dimensionless(0.0));
//  SBQuantity::length sepDist = SBQuantity::angstrom(0.5*ADNConstants::DH_DIST + 5*ADNConstants::DH_DIAMETER);
//  SBPosition3 startFirst = start - sepDist * sepDir;
//  SBPosition3 startSecond = start + sepDist * sepDir + SBQuantity::nanometer(length * ADNConstants::BP_RISE) * direction;
//
//  AddDoubleStrandToANTPart(part, length, startFirst, direction);
//  AddDoubleStrandToANTPart(part, length, startSecond, -direction);
//
//  part->SetScaffold(0);
//  part->SetNtSegmentMap();
//  return part;
//}
//

//ANTPart * DASEditor::CreateCrippledWireframeCuboid(SBPosition3 topLeftFront, int xSize, int ySize, int zSize) {
//  ANTPart* part = new ANTPart();
//
//  SBVector3 xDir(1.0, 0.0, 0.0);
//  SBVector3 yDir(0.0, 1.0, 0.0);
//  SBVector3 zDir(0.0, 0.0, 1.0);
//
//  SBQuantity::nanometer xLength = SBQuantity::nanometer(ANTConstants::BP_RISE * xSize);
//  SBQuantity::nanometer yLength = SBQuantity::nanometer(ANTConstants::BP_RISE * ySize);
//  SBQuantity::nanometer zLength = SBQuantity::nanometer(ANTConstants::BP_RISE * zSize);
//
//  // create 12 double helices
//  AddCrippledDoubleStrandToANTPart(topLeftFront, xDir, xSize, *part);
//  AddCrippledDoubleStrandToANTPart(topLeftFront, -yDir, ySize, *part);
//  AddCrippledDoubleStrandToANTPart(topLeftFront, -zDir, zSize, *part);
//
//  SBPosition3 bottomRightFront = topLeftFront + (xLength*xDir - yLength*yDir);
//
//  AddCrippledDoubleStrandToANTPart(bottomRightFront, -xDir, xSize, *part);
//  AddCrippledDoubleStrandToANTPart(bottomRightFront, yDir, ySize, *part);
//  AddCrippledDoubleStrandToANTPart(bottomRightFront, -zDir, zSize, *part);
//
//  SBPosition3 bottomLeftBack = topLeftFront - (yLength*yDir + zLength*zDir);
//
//  AddCrippledDoubleStrandToANTPart(bottomLeftBack, xDir, xSize, *part);
//  AddCrippledDoubleStrandToANTPart(bottomLeftBack, yDir, ySize, *part);
//  AddCrippledDoubleStrandToANTPart(bottomLeftBack, zDir, zSize, *part);
//
//  SBPosition3 topRightBack = topLeftFront + (xLength*xDir - zLength*zDir);
//
//  AddCrippledDoubleStrandToANTPart(topRightBack, -xDir, xSize, *part);
//  AddCrippledDoubleStrandToANTPart(topRightBack, -yDir, ySize, *part);
//  AddCrippledDoubleStrandToANTPart(topRightBack, zDir, zSize, *part);
//
//  return part;
//}
//
//ANTPart * DASEditor::DebugCrossoversPart() {
//  ANTPart* part = new ANTPart();
//
//  SBPosition3 init_pos = SBPosition3();
//  SBVector3 init_dir = SBVector3(0.0, 0.0, 1.0);
//  size_t length = 42;
//
//  AddDoubleStrandToANTPart(part, length, init_pos, init_dir);
//
//  SBVector3 dirPlane = SBVector3(1.0, 0.0, 1.0);
//  SBPosition3 pos = init_pos + SBQuantity::nanometer(ANTConstants::DH_DIAMETER + 0.25) * dirPlane;
//  SBPosition3 posFinal = pos + SBQuantity::nanometer(ANTConstants::BP_RISE * length) * init_dir;
//
//  AddDoubleStrandToANTPart(part, length, posFinal, -init_dir);
//  AddDoubleStrandToANTPart(part, length, -pos, init_dir);
//
//  part->SetScaffold(0);
//  part->SetNtSegmentMap();
//
//  return part;
//}

// todo: calculate positions
ADNPointer<ADNDoubleStrand> DASCreator::CreateDoubleStrand(ADNPointer<ADNPart> part, int length, SBPosition3 start, SBVector3 direction, bool mock)
{
  ADNPointer<ADNDoubleStrand> ds = AddDoubleStrandToADNPart(part, length, start, direction, mock);

  return ds;
}

ADNPointer<ADNSingleStrand> DASCreator::CreateSingleStrand(ADNPointer<ADNPart> part, int length, SBPosition3 start, SBVector3 direction, bool mock)
{
  return AddSingleStrandToADNPart(part, length, start, direction);
}

ADNPointer<ADNLoop> DASCreator::CreateLoop(ADNPointer<ADNSingleStrand> ss, ADNPointer<ADNNucleotide> nextNt, std::string seq, ADNPointer<ADNPart> part)
{
  ADNPointer<ADNLoop> loop = new ADNLoop();

  for (size_t k = 0; k < seq.size(); ++k) {
    ADNPointer<ADNNucleotide> nt = new ADNNucleotide();
    if (part != nullptr) {
      part->RegisterNucleotide(ss, nt, nextNt);
    }else {
      ss->AddNucleotide(nt, nextNt);
    }
    nt->SetType(ADNModel::ResidueNameToType(seq[k]));
    loop->AddNucleotide(nt);
    if (k == 0) loop->SetStart(nt);
    if (k == seq.size() - 1) loop->SetEnd(nt);
  }

  return loop;
}

ADNPointer<ADNPart> DASCreator::CreateNanotube(SBQuantity::length radius, SBPosition3 center, SBVector3 direction, int length, bool mock)
{
  int minHeight = 1;
  int minNanotubes = 3;

  ADNPointer<ADNPart> nanorobot = nullptr;
  
  // so far create only a circle
  // transformation to global coordinates
  ublas::vector<double> dir = ublas::vector<double>(3);
  dir[0] = direction[0].getValue();
  dir[1] = direction[1].getValue();
  dir[2] = direction[2].getValue();
  ublas::matrix<double> subspace = ADNVectorMath::FindOrthogonalSubspace(dir);
  ADNVectorMath::AddRowToMatrix(subspace, dir);
  
  if (length < minHeight) {
    length = minHeight;
  }

  int num = 0;
  double theta = 0.0;
  double pi = atan(1.0) * 4.0;
  SBQuantity::length r = SBQuantity::nanometer(ADNConstants::DH_DIAMETER * 0.5);
  SBQuantity::length R;

  if (radius > SBQuantity::length(0.0)) {
    // number of double helices that fit into the circumpherence
    auto diameter = 2 * radius;
    R = radius;

    // in a circumpherence every double strand is going to take up the same space
    auto up = -r*r*(4 * R*R - r*r) + (2 * R*R - r*r)*(2 * R*R - r*r);
    auto down = r*r*(4 * R*R - r*r) + (2 * R*R - r*r)*(2 * R*R - r*r);
    auto cosTheta = up / down;
    theta = acos(cosTheta.getValue());
    num = ceil(2 * pi / theta);
  }

  if (num < minNanotubes) {
    num = minNanotubes;
    theta = ADNVectorMath::DegToRad(120.0);
    R = 2 * r / sqrt(3);
  }

  // recalculate the exact radius so num will fit
  auto newR = theta * num * R / (2 * pi);
  auto newTheta = 2 * pi / num;

  if (num > 0) {
    nanorobot = new ADNPart();
    // create dsDNA
    double t = 0.0;
    for (int j = 0; j < num; ++j) {
      //  // a and b are the coordinates on the plane
      auto a = newR*sin(t);
      auto b = newR*cos(t);
      ublas::vector<double> pos_p(3);
      pos_p[0] = a.getValue();
      pos_p[1] = b.getValue();
      pos_p[2] = 0.0;
      ublas::vector<double> trf = ublas::prod(ublas::trans(subspace), pos_p);
      SBPosition3 dsPosition = SBPosition3(SBQuantity::picometer(trf[0]), SBQuantity::picometer(trf[1]), SBQuantity::picometer(trf[2])) + center;
      AddDoubleStrandToADNPart(nanorobot, length, dsPosition, direction, mock);

      t += newTheta;
    }

    nanorobot->SetE1(ADNVectorMath::row(subspace, 0));
    nanorobot->SetE2(ADNVectorMath::row(subspace, 1));
    nanorobot->SetE3(ADNVectorMath::row(subspace, 2));
  }

  ADNLogger& logger = ADNLogger::GetLogger();
  logger.LogDebugDateTime();
  logger.LogDebug(std::string("-> Creating DNA nanotube"));
  logger.LogDebug(std::string("    * num of ds: ") + std::to_string(num));
  logger.LogDebug(std::string("    * bps per ds: ") + std::to_string(length));
  logger.LogDebug(std::string("    * total bps: ") + std::to_string(length*num));
 
  return nanorobot;
}

ADNPointer<ADNPart> DASCreator::CreateMockNanotube(SBQuantity::length radius, SBPosition3 center, SBVector3 direction, int length)
{
  return CreateNanotube(radius, center, direction, length, true);
}

ADNPointer<ADNPart> DASCreator::CreateDSRing(SBQuantity::length radius, SBPosition3 center, SBVector3 normal, bool mock)
{
  ADNPointer<ADNPart> part = new ADNPart();
  DASCreator::AddDSRingToADNPart(part, radius, center, normal);
  return part;
}

ADNPointer<ADNPart> DASCreator::CreateLinearCatenanes(SBQuantity::length radius, SBPosition3 center, SBVector3 normal, int number, bool mock)
{
  ADNPointer<ADNPart> part = new ADNPart();
  // calculate overlap
  SBQuantity::length dist = radius*0.8;
  // total distance spanning the catenanes
  SBQuantity::length totalLength = 2 * radius*number - dist*(number - 1);
  SBVector3 v = SBVector3(1.0, 0.0, 0.0);
  SBVector3 w = SBVector3(0.0, 1.0, 0.0);

  SBPosition3 start = center - (totalLength*0.5 + radius)*v;
  double pi = atan(1.0) * 4.0;
  double theta = pi*0.5*0.95;
  for (int i = 0; i < number; i++) {
    auto n = normal+cos(theta)*w;
    DASCreator::AddDSRingToADNPart(part, radius, start, n.normalizedVersion());
    // calculate next center and normal
    start = start + (2 * radius - dist)*v;
    w *= -1.0;
  }

  return part;
}

ADNPointer<ADNPart> DASCreator::CreateHexagonalCatenanes(SBQuantity::length radius, SBPosition3 center, SBVector3 normal, int rows, int cols, bool mock)
{
  ADNPointer<ADNPart> part = new ADNPart();

  double edgeDist = radius.getValue() * 2 * 0.8;
  DASLattice lattice = DASLattice(LatticeType::Honeycomb, edgeDist, rows, cols);

  double pi = atan(1.0) * 4.0;
  double w = 1.0;
  double theta = 0.1;
  // create a ring at every point of the lattice
  auto numRows = lattice.GetNumberRows();
  auto numCols = lattice.GetNumberCols();
  for (unsigned int i = 0; i < numRows; ++i) {
    for (unsigned int j = 0; j < numCols; ++j) {
      auto sigW = w;
      if ((i + j) % 2 == 0) sigW *= -1.0;
      LatticeCell cell = lattice.GetLatticeCell(i, j);
      SBPosition3 c = SBPosition3(SBQuantity::picometer(cell.x_), SBQuantity::picometer(cell.y_), SBQuantity::nanometer(0.0)) + center;

      ublas::vector<double> normal(3.0, 0.0);
      normal[0] = 1.0;
      normal[1] = theta;
      normal[2] = pi * 0.5 + sigW * pi * 0.5;
      SBVector3 n = ADNAuxiliary::UblasVectorToSBVector(ADNVectorMath::Spherical2Cartesian(normal));
      DASCreator::AddDSRingToADNPart(part, radius, c, n.normalizedVersion());
    }
  }

  return part;
}

ADNPointer<ADNDoubleStrand> DASCreator::AddDSRingToADNPart(ADNPointer<ADNPart> part, SBQuantity::length radius, SBPosition3 center, SBVector3 normal, bool mock)
{
  ADNPointer<ADNDoubleStrand> ds = new ADNDoubleStrand();
  ADNPointer<ADNSingleStrand> ssLeft = new ADNSingleStrand();
  ADNPointer<ADNSingleStrand> ssRight = new ADNSingleStrand();
  part->RegisterDoubleStrand(ds);
  if (!mock) {
    part->RegisterSingleStrand(ssLeft);
    part->RegisterSingleStrand(ssRight);
  }

  double pi = atan(1.0) * 4.0;
  auto circumpherence = 2.0 * pi * radius;
  // see how many nts fit in circumpherence using B-DNA
  int numNts = floor((circumpherence / SBQuantity::nanometer(ADNConstants::BP_RISE)).getValue());
  // calculate angle and new radius so all nts fit
  auto theta = 2.0 * pi / numNts;
  auto R = theta * numNts * radius / (2.0 * pi);

  // create coordinate system
  ublas::vector<double> n = ADNAuxiliary::SBVectorToUblasVector(normal);
  auto subspace = ADNVectorMath::FindOrthogonalSubspace(n);
  ADNVectorMath::AddRowToMatrix(subspace, n);
  ublas::vector<double> xVec = ublas::row(subspace, 0);
  ublas::vector<double> yVec = ublas::row(subspace, 1);

  double t = 0.0;
  for (int j = 0; j < numNts; ++j) {
    // a and b are the coordinates on the plane
    auto a = R*sin(t);
    auto b = R*cos(t);
    // calculate tangent
    ublas::vector<double> rVec(3, 0.0);
    rVec += sin(t)*xVec;
    rVec += cos(t)*yVec;
    //rVec[2] = 0.0;
    auto direction = ADNVectorMath::CrossProduct(ADNAuxiliary::SBVectorToUblasVector(normal), -rVec);

    ADNPointer<ADNBaseSegment> bs = new ADNBaseSegment();

    Position3D pos = center;
    pos += a*ADNAuxiliary::UblasVectorToSBVector(xVec);
    pos += b*ADNAuxiliary::UblasVectorToSBVector(yVec);
    bs->SetPosition(pos);
    bs->SetE3(direction);
    bs->SetE2(ADNAuxiliary::SBVectorToUblasVector(normal));
    bs->SetE1(rVec);
    bs->SetNumber(boost::numeric_cast<int>(j));

    ADNPointer<ADNBasePair> cell = new ADNBasePair();
    if (!mock) {
      // create nucleotides
      ADNPointer<ADNNucleotide> ntLeft = new ADNNucleotide();
      part->RegisterNucleotideThreePrime(ssLeft, ntLeft);
      cell->SetLeftNucleotide(ntLeft);
      ntLeft->SetPosition(bs->GetPosition());
      ntLeft->SetBackbonePosition(bs->GetPosition());
      ntLeft->SetSidechainPosition(bs->GetPosition());
      ntLeft->SetBaseSegment(bs);
      ntLeft->SetType(DNABlocks::DI);

      ADNPointer<ADNNucleotide> ntRight = new ADNNucleotide();
      part->RegisterNucleotideFivePrime(ssRight, ntRight);
      cell->SetRightNucleotide(ntRight);
      ntRight->SetPosition(bs->GetPosition());
      ntRight->SetBackbonePosition(bs->GetPosition());
      ntRight->SetSidechainPosition(bs->GetPosition());
      ntRight->SetBaseSegment(bs);
      ntRight->SetType(DNABlocks::DI);

      ntLeft->SetPair(ntRight);
      ntRight->SetPair(ntLeft);
    }

    bs->SetCell(cell());

    part->RegisterBaseSegmentEnd(ds, bs);

    t += theta;
  }

  ssLeft->SetDefaultName();
  ssRight->SetDefaultName();
  ssLeft->IsCircular(true);
  ssRight->IsCircular(true);
  ds->IsCircular(true);

  return ds;
}

ADNPointer<ADNDoubleStrand> DASCreator::AddDoubleStrandToADNPart(ADNPointer<ADNPart> part, size_t length, SBPosition3 start, SBVector3 direction, bool mock)
{
  SBPosition3 delt = SBQuantity::nanometer(ADNConstants::BP_RISE) * direction;
  SBPosition3 pos = start;

  ADNPointer<ADNDoubleStrand> ds = new ADNDoubleStrand();
  part->RegisterDoubleStrand(ds);

  ADNPointer<ADNSingleStrand> ssLeft = nullptr;
  ADNPointer<ADNSingleStrand> ssRight = nullptr;
  if (!mock) {
    ssLeft = new ADNSingleStrand();
    part->RegisterSingleStrand(ssLeft);
    ssRight = new ADNSingleStrand();
    part->RegisterSingleStrand(ssRight);

    ssLeft->SetDefaultName();
    ssRight->SetDefaultName();
  }

  for (size_t i = 0; i < length; ++i) {
    ADNPointer<ADNBaseSegment> bs = new ADNBaseSegment();

    bs->SetPosition(pos);
    bs->SetE3(ADNAuxiliary::SBVectorToUblasVector(direction));
    bs->SetNumber(boost::numeric_cast<int>(i));
    ADNPointer<ADNBasePair> cell = new ADNBasePair();

    if (!mock) {
      // create nucleotides
      ADNPointer<ADNNucleotide> ntLeft = new ADNNucleotide();
      part->RegisterNucleotideThreePrime(ssLeft, ntLeft);
      cell->SetLeftNucleotide(ntLeft);
      ntLeft->SetPosition(bs->GetPosition());
      ntLeft->SetBackbonePosition(bs->GetPosition());
      ntLeft->SetSidechainPosition(bs->GetPosition());
      ntLeft->SetBaseSegment(bs);
      ntLeft->SetType(DNABlocks::DI);

      ADNPointer<ADNNucleotide> ntRight = new ADNNucleotide();
      part->RegisterNucleotideFivePrime(ssRight, ntRight);
      cell->SetRightNucleotide(ntRight);
      ntRight->SetPosition(bs->GetPosition());
      ntRight->SetBackbonePosition(bs->GetPosition());
      ntRight->SetSidechainPosition(bs->GetPosition());
      ntRight->SetBaseSegment(bs);
      ntRight->SetType(DNABlocks::DI);

      ntLeft->SetPair(ntRight);
      ntRight->SetPair(ntLeft);
    }
    bs->SetCell(cell());

    part->RegisterBaseSegmentEnd(ds, bs);

    pos += delt;
  }

  return ds;
}

ADNPointer<ADNSingleStrand> DASCreator::AddSingleStrandToADNPart(ADNPointer<ADNPart> part, size_t length, SBPosition3 start, SBVector3 direction)
{
  SBPosition3 delt = SBQuantity::nanometer(ADNConstants::BP_RISE) * direction;
  SBPosition3 pos = start;

  ADNPointer<ADNDoubleStrand> ds = new ADNDoubleStrand();
  part->RegisterDoubleStrand(ds);
  // create nucleotides
  ADNPointer<ADNSingleStrand> ss = new ADNSingleStrand();
  part->RegisterSingleStrand(ss);

  for (size_t i = 0; i < length; ++i) {
    ADNPointer<ADNBaseSegment> bs = new ADNBaseSegment();
    bs->SetPosition(pos);
    bs->SetE3(ADNAuxiliary::SBVectorToUblasVector(direction));
    bs->SetNumber(boost::numeric_cast<int>(i));
    part->RegisterBaseSegmentEnd(ds, bs);

    ADNPointer<ADNBasePair> bp = new ADNBasePair();
    bs->SetCell(bp());

    ADNPointer<ADNNucleotide> nt = new ADNNucleotide();
    part->RegisterNucleotideThreePrime(ss, nt);
    nt->SetType(DNABlocks::DI);
    nt->SetPosition(bs->GetPosition());
    nt->SetBackbonePosition(bs->GetPosition());
    nt->SetSidechainPosition(bs->GetPosition());
    nt->SetBaseSegment(bs);
    bp->SetLeftNucleotide(nt);

    pos += delt;
  }

  ss->SetDefaultName();

  return ss;
}

void DASCreatorEditors::resetPositions(Positions& pos)
{
  pos.First = SBPosition3();
  pos.Second = SBPosition3();
  pos.Third = SBPosition3();
  pos.Fourth = SBPosition3();
  pos.Fifth = SBPosition3();
  pos.Sixth = SBPosition3();
  pos.cnt = 0;
}
