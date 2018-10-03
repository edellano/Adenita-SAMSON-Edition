#include "ADNDisplayHelper.hpp"

void ADNDisplayHelper::displayLine(SBPosition3 start, SBPosition3 end, std::string text) {

  unsigned int nLines = 1;
  unsigned int nPositions = 2;
  unsigned int* indexData = new unsigned int[2 * nLines];
  float *positionData = new float[3 * nPositions];
  unsigned int *capData = new unsigned int[2 * nLines];
  float *colorData = new float[4 * 2 * nLines];
  unsigned int *flagData = new unsigned int[2 * nLines];

  positionData[3 * 0 + 0] = start.v[0].getValue();
  positionData[3 * 0 + 1] = start.v[1].getValue();
  positionData[3 * 0 + 2] = start.v[2].getValue();
  positionData[3 * 1 + 0] = end.v[0].getValue();
  positionData[3 * 1 + 1] = end.v[1].getValue();
  positionData[3 * 1 + 2] = end.v[2].getValue();

  indexData[0] = 0;
  indexData[1] = 1;

  capData[0] = 1;
  capData[1] = 1;

  colorData[0] = 0;
  colorData[1] = 0;
  colorData[2] = 0;
  colorData[3] = 1;
  colorData[4] = 0;
  colorData[5] = 0;
  colorData[6] = 0;
  colorData[7] = 1;

  unsigned int flag = 0;
  flagData[0] = flag;
  flagData[1] = flag;

  SAMSON::displayLines(nLines, nPositions, indexData, positionData, colorData, flagData);
  displayLengthText(start, end, text);

  delete[] indexData;
  delete[] capData;
  delete[] colorData;
  delete[] flagData;
  delete[] positionData;

}

void ADNDisplayHelper::displayCylinder(SBPosition3 start, SBPosition3 end, std::string text /*= ""*/)
{
  SEConfig& config = SEConfig::GetInstance();
  
  unsigned int nCylinders = 1;
  unsigned int nPositions = 2 * nCylinders;
  unsigned int* indexData = new unsigned int[2 * nCylinders];
  float *positionData = new float[3 * nPositions];
  float *radiusData = new float[2 * nCylinders];
  unsigned int *capData = new unsigned int[2 * nCylinders];
  float *colorData = new float[4 * 2 * nCylinders];
  unsigned int *flagData = new unsigned int[2 * nCylinders];

  positionData[0 * 3 + 0] = start.v[0].getValue();
  positionData[0 * 3 + 1] = start.v[1].getValue();
  positionData[0 * 3 + 2] = start.v[2].getValue();
  positionData[1 * 3 + 0] = end.v[0].getValue();
  positionData[1 * 3 + 1] = end.v[1].getValue();
  positionData[1 * 3 + 2] = end.v[2].getValue();

  indexData[0] = 0;
  indexData[1] = 1;

  radiusData[0] = config.nucleotide_V_radius;
  radiusData[1] = config.nucleotide_V_radius;

  capData[0] = 1;
  capData[1] = 1;

  colorData[0] = config.nucleotide_E_Color[0];
  colorData[1] = config.nucleotide_E_Color[1];
  colorData[2] = config.nucleotide_E_Color[2];
  colorData[3] = config.nucleotide_E_Color[3] * 0.7f;
  colorData[4] = config.nucleotide_E_Color[0];
  colorData[5] = config.nucleotide_E_Color[1];
  colorData[6] = config.nucleotide_E_Color[2];
  colorData[7] = config.nucleotide_E_Color[3] * 0.7f;
  
  int flag = 0;
  flagData[0] = flag;
  flagData[1] = flag;

  SAMSON::displayCylinders(nCylinders, nPositions, indexData, positionData, radiusData, capData, colorData, flagData);

  delete[] indexData;
  delete[] radiusData;
  delete[] capData;
  delete[] colorData;
  delete[] flagData;
  delete[] positionData;


}

void ADNDisplayHelper::displayLine(ublas::vector<double> center, ublas::vector<double> dir, int length)
{
  ublas::vector<double> end = center + dir;
  end = end * length;

  SBPosition3 start = SBPosition3(
    SBQuantity::picometer(center[0]),
    SBQuantity::picometer(center[1]),
    SBQuantity::picometer(center[2])
    );

  SBPosition3 end2 = SBPosition3(
    SBQuantity::picometer(end2[0]),
    SBQuantity::picometer(end2[1]),
    SBQuantity::picometer(end2[2])
    );

  displayLine(start, end2);
}

void ADNDisplayHelper::displayLengthText(SBPosition3 start, SBPosition3 end, std::string text) {
  SBPosition3 center = (start + end) / 2;
  if (text == "") {
    double length = (end - start).norm().getValue();
    length /= 1000;

    stringstream stream;
    stream << fixed << setprecision(2) << length;
    string text = stream.str() + " nm";
  }

  float * color = new float[4];
  color[0] = 1.0f;
  color[1] = 1.0f;
  color[2] = 1.0f;
  color[3] = 1.0f;

  SAMSON::displayText(
    text,
    center,
    QFont(QString("Helvetica"), 60),
    color);
}

void ADNDisplayHelper::displayDirectedCylinder(SBPosition3 start, SBPosition3 end)
{

  unsigned int nCylinders = 1;
  unsigned int nPositions = 2 * nCylinders;
  unsigned int* indexData = new unsigned int[2 * nCylinders];
  float *positionData = new float[3 * nPositions];
  float *radiusData = new float[2 * nCylinders];
  unsigned int *capData = new unsigned int[2 * nCylinders];
  float *colorData = new float[4 * 2 * nCylinders];
  unsigned int *flagData = new unsigned int[2 * nCylinders];

  positionData[0 * 3 + 0] = start.v[0].getValue();
  positionData[0 * 3 + 1] = start.v[1].getValue();
  positionData[0 * 3 + 2] = start.v[2].getValue();
  positionData[1 * 3 + 0] = end.v[0].getValue();
  positionData[1 * 3 + 1] = end.v[1].getValue();
  positionData[1 * 3 + 2] = end.v[2].getValue();

  indexData[0] = 0;
  indexData[1] = 1;

  radiusData[0] = 100;
  radiusData[1] = 100;

  capData[0] = 1;
  capData[1] = 1;

  colorData[0] = 1.0f;
  colorData[1] = 1.0f;
  colorData[2] = 1.0f;
  colorData[3] = 1.0f;
  colorData[4] = 1.0f;
  colorData[5] = 0.0f;
  colorData[6] = 0.0f;
  colorData[7] = 1.0f;

  int flag = 0;
  flagData[0] = flag;
  flagData[1] = flag;

  SAMSON::displayCylinders(nCylinders, nPositions, indexData, positionData, radiusData, capData, colorData, flagData);

  delete[] indexData;
  delete[] radiusData;
  delete[] capData;
  delete[] colorData;
  delete[] flagData;
  delete[] positionData;

}

void ADNDisplayHelper::displayDirectedCylinder(SBPosition3 start, SBPosition3 end, float * color, int radius) {
  unsigned int nCylinders = 1;
  unsigned int nPositions = 2 * nCylinders;
  unsigned int* indexData = new unsigned int[2 * nCylinders];
  float *positionData = new float[3 * nPositions];
  float *radiusData = new float[2 * nCylinders];
  unsigned int *capData = new unsigned int[2 * nCylinders];
  float *colorData = new float[4 * 2 * nCylinders];
  unsigned int *flagData = new unsigned int[2 * nCylinders];

  positionData[0 * 3 + 0] = start.v[0].getValue();
  positionData[0 * 3 + 1] = start.v[1].getValue();
  positionData[0 * 3 + 2] = start.v[2].getValue();
  positionData[1 * 3 + 0] = end.v[0].getValue();
  positionData[1 * 3 + 1] = end.v[1].getValue();
  positionData[1 * 3 + 2] = end.v[2].getValue();

  indexData[0] = 0;
  indexData[1] = 1;

  radiusData[0] = radius;
  radiusData[1] = radius;

  capData[0] = 1;
  capData[1] = 1;

  colorData[0] = color[0];
  colorData[1] = color[1];
  colorData[2] = color[2];
  colorData[3] = color[3];
  colorData[4] = color[0];
  colorData[5] = color[1];
  colorData[6] = color[2];
  colorData[7] = color[3];

  int flag = 0;
  flagData[0] = flag;
  flagData[1] = flag;

  SAMSON::displayCylinders(nCylinders, nPositions, indexData, positionData, radiusData, capData, colorData, flagData);

  delete[] indexData;
  delete[] radiusData;
  delete[] capData;
  delete[] colorData;
  delete[] flagData;
}

void ADNDisplayHelper::displayPlane(SBVector3 vec, SBPosition3 shift) {
  //unsigned int nTriangles = 2;
  //unsigned int nVertices = 4;

  //unsigned int indexData[] = {
  //0, 1, 2,
  //2, 3, 0
  //};

  //float vertexPositions[] = {
  //  0, 0, 0,
  //  0, 1000, 0,
  //  1000, 1000, 0,
  //  1000, 0, 0};

  //float * colorData = new float[4 * nVertices];
  //unsigned int * flagData = new unsigned int[nVertices];
  //float * normalData = new float[3 * nVertices];

  //colorData[4 * 0 + 0] = 1.0f;
  //colorData[4 * 0 + 1] = 0.0f;
  //colorData[4 * 0 + 2] = 0.0f;
  //colorData[4 * 0 + 3] = 1.0f;

  //colorData[4 * 1 + 0] = 0.0f;
  //colorData[4 * 1 + 1] = 1.0f;
  //colorData[4 * 1 + 2] = 0.0f;
  //colorData[4 * 1 + 3] = 1.0f;

  //colorData[4 * 2 + 0] = 0.0f;
  //colorData[4 * 2 + 1] = 0.0f;
  //colorData[4 * 2 + 2] = 1.0f;
  //colorData[4 * 2 + 3] = 1.0f;

  //colorData[4 * 3 + 0] = 1.0f;
  //colorData[4 * 3 + 1] = 1.0f;
  //colorData[4 * 3 + 2] = 1.0f;
  //colorData[4 * 3 + 3] = 1.0f;

  //flagData[0] = 0;
  //flagData[1] = 0;
  //flagData[2] = 0;
  //flagData[3] = 0;

  //for (int i = 0; i < nTriangles; i++) {
  //  int idx1 = indexData[3 * i + 0];
  //  int idx2 = indexData[3 * i + 1];
  //  int idx3 = indexData[3 * i + 2];
  //
  //  ublas::vector<double> posA(3);
  //  posA[0] = vertexPositions[3 * idx1 + 0];
  //  posA[1] = vertexPositions[3 * idx1 + 1];
  //  posA[2] = vertexPositions[3 * idx1 + 2];
  //
  //  ublas::vector<double> posB(3);
  //  posB[0] = vertexPositions[3 * idx2 + 0];
  //  posB[1] = vertexPositions[3 * idx2 + 1];
  //  posB[2] = vertexPositions[3 * idx2 + 2];
  //
  //  ublas::vector<double> posC(3);
  //  posC[0] = vertexPositions[3 * idx3 + 0];
  //  posC[1] = vertexPositions[3 * idx3 + 1];
  //  posC[2] = vertexPositions[3 * idx3 + 2];
  //
  //  ublas::vector<double> dir = ADNVectorMath::CrossProduct(posB - posA, posC - posA);
  //
  //  ublas::vector<double> n = dir / ublas::norm_2(dir);
  //
  //  normalData[3 * idx1 + 0] = n[0];
  //  normalData[3 * idx1 + 1] = n[1];
  //  normalData[3 * idx1 + 2] = n[2];
  //
  //  ANTAuxiliary::logVector(n);
  //
  //}

  //SAMSON::displayTriangles(
  //  nTriangles,
  //  nVertices,
  //  indexData,
  //  vertexPositions,
  //  normalData,
  //  colorData,
  //  flagData
  //  );

  unsigned int nTriangles = 20;
  unsigned int nVertices = 12;

  unsigned int indexData[] = {
    2, 1, 0,
    3, 2, 0,
    4, 3, 0,
    5, 4, 0,
    1, 5, 0,
    11, 6, 7,
    11, 7, 8,
    11, 8, 9,
    11, 9, 10,
    11, 10, 6,
    1, 2, 6,
    2, 3, 7,
    3, 4, 8,
    4, 5, 9,
    5, 1, 10,
    2, 7, 6,
    3, 8, 7,
    4, 9, 8,
    5, 10, 9,
    1, 6, 10 };

  float vertexPositions[] = {
    0.000f*1000.0f, 0.000f*1000.0f, 1.000f*1000.0f,
    0.894f*1000.0f, 0.000f*1000.0f, 0.447f*1000.0f,
    0.276f*1000.0f, 0.851f*1000.0f, 0.447f*1000.0f,
    -0.724f*1000.0f, 0.526f*1000.0f, 0.447f*1000.0f,
    -0.724f*1000.0f, -0.526f*1000.0f, 0.447f*1000.0f,
    0.276f*1000.0f, -0.851f*1000.0f, 0.447f*1000.0f,
    0.724f*1000.0f, 0.526f*1000.0f, -0.447f*1000.0f,
    -0.276f*1000.0f, 0.851f*1000.0f, -0.447f*1000.0f,
    -0.894f*1000.0f, 0.000f*1000.0f, -0.447f*1000.0f,
    -0.276f*1000.0f, -0.851f*1000.0f, -0.447f*1000.0f,
    0.724f*1000.0f, -0.526f*1000.0f, -0.447f*1000.0f,
    0.000f*1000.0f, 0.000f*1000.0f, -1.000f*1000.0f };

  float * colorData = new float[4 * nVertices];
  unsigned int * flagData = new unsigned int[nVertices];
  float * normalData = new float[3 * nVertices];

  for (unsigned int i = 0; i < nVertices; i++) {
    colorData[4 * i + 0] = 1.0f;
    colorData[4 * i + 1] = 0.0f;
    colorData[4 * i + 2] = 0.0f;
    colorData[4 * i + 3] = 1.0f;

    flagData[i] = 0;

  }

  for (unsigned int i = 0; i < nTriangles; i++) {
    int idx1 = indexData[3 * i + 0];
    int idx2 = indexData[3 * i + 1];
    int idx3 = indexData[3 * i + 2];

    ublas::vector<double> posA(3);
    posA[0] = vertexPositions[3 * idx1 + 0];
    posA[1] = vertexPositions[3 * idx1 + 1];
    posA[2] = vertexPositions[3 * idx1 + 2];

    ublas::vector<double> posB(3);
    posB[0] = vertexPositions[3 * idx2 + 0];
    posB[1] = vertexPositions[3 * idx2 + 1];
    posB[2] = vertexPositions[3 * idx2 + 2];

    ublas::vector<double> posC(3);
    posC[0] = vertexPositions[3 * idx3 + 0];
    posC[1] = vertexPositions[3 * idx3 + 1];
    posC[2] = vertexPositions[3 * idx3 + 2];

    ublas::vector<double> dir = ADNVectorMath::CrossProduct(posB - posA, posC - posA);

    ublas::vector<double> n = dir / ublas::norm_2(dir);

    normalData[3 * idx1 + 0] = n[0];
    normalData[3 * idx1 + 1] = n[1];
    normalData[3 * idx1 + 2] = n[2];

    //ANTAuxiliary::logVector(n);

  }

  SAMSON::displayTriangles(
    nTriangles,
    nVertices,
    indexData,
    vertexPositions,
    normalData,
    colorData,
    flagData
    );

}


//void ADNDisplayHelper::displayTriangleMesh(ANTPolyhedron * p)
//{
//  unsigned int nTriangles = boost::numeric_cast<unsigned int>(p->GetNumFaces());
//  unsigned int nVertices = boost::numeric_cast<unsigned int>(p->GetNumFaces());
//  float * vertexPositions = new float[3 * nVertices];
//  float * colorData = new float[4 * nVertices];
//  unsigned int * flagData = new unsigned int[nVertices];
//  float * normalData = new float[3 * nVertices];
//  unsigned int * indexData = p->GetIndices();
//
//  for (unsigned int i = 0; i < nVertices; i++) {
//    vertexPositions[3 * i + 0] = p->GetVertexById(i)->GetSBPosition()[0].getValue();
//    vertexPositions[3 * i + 1] = p->GetVertexById(i)->GetSBPosition()[1].getValue();
//    vertexPositions[3 * i + 2] = p->GetVertexById(i)->GetSBPosition()[2].getValue();
//
//    colorData[4 * i + 0] = 1.0f;
//    colorData[4 * i + 1] = 0.0f;
//    colorData[4 * i + 2] = 0.0f;
//    colorData[4 * i + 3] = 1.0f;
//
//    flagData[i] = 0;
//
//    //normal
//    int idx1 = indexData[3 * i + 0];
//    int idx2 = indexData[3 * i + 1];
//    int idx3 = indexData[3 * i + 2];
//
//    ublas::vector<double> posA(3);
//    posA[0] = vertexPositions[3 * idx1 + 0];
//    posA[1] = vertexPositions[3 * idx1 + 1];
//    posA[2] = vertexPositions[3 * idx1 + 2];
//
//    ublas::vector<double> posB(3);
//    posB[0] = vertexPositions[3 * idx2 + 0];
//    posB[1] = vertexPositions[3 * idx2 + 1];
//    posB[2] = vertexPositions[3 * idx2 + 2];
//
//    ublas::vector<double> posC(3);
//    posC[0] = vertexPositions[3 * idx3 + 0];
//    posC[1] = vertexPositions[3 * idx3 + 1];
//    posC[2] = vertexPositions[3 * idx3 + 2];
//
//    ublas::vector<double> dir = ADNVectorMath::CrossProduct(posB - posA, posC - posA);
//
//    ublas::vector<double> n = dir / ublas::norm_2(dir);
//
//    normalData[3 * idx1 + 0] = n[0];
//    normalData[3 * idx1 + 1] = n[1];
//    normalData[3 * idx1 + 2] = n[2];
//
//  }
//
//  SAMSON::displayTriangles(
//    nTriangles,
//    nVertices,
//    indexData,
//    vertexPositions,
//    normalData,
//    colorData,
//    flagData
//    );
//}

//void ADNDisplayHelper::displayPolyhedron(ADNPart * part)
//{
//  unsigned int nCylinders = boost::numeric_cast<unsigned int>(part->GetNumberOfBaseSegments());
//  unsigned int nPositions = boost::numeric_cast<unsigned int>(part->GetNumberOfJoints());
//
//  ADNArray<float> positionData = ADNArray<float>(3, nPositions);
//  ADNArray<float> radiusData = ADNArray<float>(nPositions);
//  ADNArray<unsigned int> flagData = ADNArray<unsigned int>(nPositions);
//  ADNArray<float> colorData = ADNArray<float>(4, nPositions);
//  ADNArray<unsigned int> capData = ADNArray<unsigned int>(nPositions);
//
//  ADNArray<unsigned int> indexData = ADNArray<unsigned int>(nCylinders * 2);
//
//  for (auto const &ent : part->GetBaseSegments()) {
//
//    ADNBaseSegment* bs = ent.second;
//
//    unsigned int jid1 = bs->source_->id_;
//    unsigned int jid2 = bs->target_->id_;
//
//    unsigned int k = bs->id_;
//    unsigned int i = 2 * k;
//    indexData(i) = jid1;
//    indexData(i + 1) = jid2;
//  }
//
//  for (auto const &joint : part->GetSegmentJoints()) {
//
//    SBPosition3 pos3D = joint->GetSBPosition();
//
//    unsigned int jid = joint->id_;
//
//    positionData(jid, 0) = (float)pos3D.v[0].getValue();
//    positionData(jid, 1) = (float)pos3D.v[1].getValue();
//    positionData(jid, 2) = (float)pos3D.v[2].getValue();
//
//    colorData(jid, 0) = 0.9f;
//    colorData(jid, 1) = 0.9f;
//    colorData(jid, 2) = 0.9f;
//    colorData(jid, 3) = 1.0f;
//
//    radiusData(jid) = 1000;
//
//    flagData(jid) = 0;
//
//    capData(jid) = 1;
//
//  }
//
//  //SAMSON::displaySpheres(
//  //  nPositions,
//  //  positionData.GetArray(),
//  //  radiusData.GetArray(),
//  //  colorData.GetArray(),
//  //  flagData.GetArray());
//
//  SAMSON::displayCylinders(
//    nCylinders,
//    nPositions,
//    indexData.GetArray(),
//    positionData.GetArray(),
//    radiusData.GetArray(),
//    capData.GetArray(),
//    colorData.GetArray(),
//    flagData.GetArray());
//}

void ADNDisplayHelper::displayOrthoPlane(SBVector3 vec, SBPosition3 shift)
{

  unsigned int nTriangles = 4;
  unsigned int nVertices = 7;

  unsigned int indexData[] = {
    0,1,2,
    2,3,0,
    2,4,5,
    5,6,2
  };

  float vertexPositions[] = {
    0, 0,
    0, 200,
    100, 200,
    100, 0,
    100,200,
    200,200,
    200,100
  };

  float * colorData = new float[4 * nVertices];

  for (unsigned int i = 0; i < 4 * nVertices; i++) {
    colorData[i] = 1.0f;
    if (i % 3) colorData[i] = 0.0f;

    ++i;
  }
  /*
  colorData[0] = 1.0f;
  colorData[1] = 0.0f;
  colorData[2] = 0.0f;
  colorData[3] = 1.0f;

  colorData[4] = 0.0f;
  colorData[5] = 1.0f;
  colorData[6] = 0.0f;
  colorData[7] = 1.0f;

  colorData[8] = 0.0f;
  colorData[9] = 0.0f;
  colorData[10] = 1.0f;
  colorData[11] = 1.0f;

  colorData[12] = 1.0f;
  colorData[13] = 1.0f;
  colorData[14] = 1.0f;
  colorData[15] = 1.0f;

  colorData[16] = 1.0f;
  colorData[17] = 1.0f;
  colorData[18] = 1.0f;
  colorData[19] = 1.0f;

  colorData[20] = 1.0f;
  colorData[21] = 1.0f;
  colorData[22] = 1.0f;
  colorData[23] = 1.0f;

  colorData[24] = 1.0f;
  colorData[25] = 1.0f;
  colorData[26] = 1.0f;
  colorData[27] = 1.0f;*/

  SAMSON::displayTrianglesOrtho(
    nTriangles,
    nVertices,
    indexData,
    vertexPositions,
    colorData
    );

}


void ADNDisplayHelper::displaySphere(SBPosition3 pos, float radius)
{
  ADNArray<float> positions = ADNArray<float>(3);
  ADNArray<float> radiiV = ADNArray<float>(1);
  ADNArray<float> colorsV = ADNArray<float>(4);
  ADNArray<unsigned int> flags = ADNArray<unsigned int>(1);

  positions(0) = pos[0].getValue();
  positions(1) = pos[1].getValue();
  positions(2) = pos[2].getValue();

  radiiV(0) = radius;
  flags(0) = 0;

  colorsV(0) = 1;
  colorsV(1) = 0;
  colorsV(2) = 0;
  colorsV(3) = 1;

  SAMSON::displaySpheres(
    1,
    positions.GetArray(),
    radiiV.GetArray(),
    colorsV.GetArray(),
    flags.GetArray());
}

void ADNDisplayHelper::displayPart(ADNPointer<ADNPart> part)
{
  SEConfig& config = SEConfig::GetInstance();

  auto doubleStrands = part->GetDoubleStrands();
  unsigned int nPositions = part->GetNumberOfBaseSegments();

  ADNArray<float> positions = ADNArray<float>(3, nPositions);
  ADNArray<float> radiiV = ADNArray<float>(nPositions);
  ADNArray<unsigned int> flags = ADNArray<unsigned int>(nPositions);
  ADNArray<float> colorsV = ADNArray<float>(4, nPositions);
  ADNArray<unsigned int> nodeIndices = ADNArray<unsigned int>(nPositions);

  unsigned int index = 0;

  SB_FOR(auto doubleStrand, doubleStrands) {
    auto baseSegments = doubleStrand->GetBaseSegments();

    SB_FOR(auto baseSegment, baseSegments) {
      auto cell = baseSegment->GetCell();

      if (cell->GetType() == BasePair) {
        SBPosition3 pos = baseSegment->GetPosition();
        positions(index, 0) = (float)pos.v[0].getValue();
        positions(index, 1) = (float)pos.v[1].getValue();
        positions(index, 2) = (float)pos.v[2].getValue();
      }

      colorsV(index, 0) = config.double_strand_color[0];
      colorsV(index, 1) = config.double_strand_color[1];
      colorsV(index, 2) = config.double_strand_color[2];
      colorsV(index, 3) = 0.4f;

      radiiV(index) = config.base_pair_radius;

      flags(index) = baseSegment->getInheritedFlags();

      ++index;

    }
  }

  SAMSON::displaySpheres(
    nPositions,
    positions.GetArray(),
    radiiV.GetArray(),
    colorsV.GetArray(),
    flags.GetArray());
}

void ADNDisplayHelper::displayVector(SBVector3 vec, SBPosition3 shift)
{
  int length = 1000;
  vec *= length;

  SBPosition3 end = SBPosition3(
    SBQuantity::picometer(vec[0].getValue()),
    SBQuantity::picometer(vec[1].getValue()),
    SBQuantity::picometer(vec[2].getValue())
    );

  end += shift;

  displayDirectedCylinder(shift, end);
}

void ADNDisplayHelper::displayVector(SBVector3 vec, SBPosition3 shift, float * color, int length) {
  vec *= length;

  SBPosition3 end = SBPosition3(
    SBQuantity::picometer(vec[0].getValue()),
    SBQuantity::picometer(vec[1].getValue()),
    SBQuantity::picometer(vec[2].getValue())
    );

  end += shift;

  displayDirectedCylinder(shift, end, color, 10);
}

void ADNDisplayHelper::displayArrow(SBVector3 vec, SBPosition3 start)
{
  SBVector3 shaft = vec * 0.75;
  int length = 1000;
  vec *= length;

  //shaft[0] = shaft[0].

  SBPosition3 end = SBPosition3(
    SBQuantity::picometer(vec[0].getValue()),
    SBQuantity::picometer(vec[1].getValue()),
    SBQuantity::picometer(vec[2].getValue())
    );
  end += start;

  unsigned int nCylinders = 2;
  unsigned int nPositions = 2 * nCylinders;
  unsigned int* indexData = new unsigned int[2 * nCylinders];
  float *positionData = new float[3 * nPositions];
  float *radiusData = new float[2 * nCylinders];
  unsigned int *capData = new unsigned int[2 * nCylinders];
  float *colorData = new float[4 * 2 * nCylinders];
  unsigned int *flagData = new unsigned int[2 * nCylinders];

  positionData[0 * 3 + 0] = start.v[0].getValue();
  positionData[0 * 3 + 1] = start.v[1].getValue();
  positionData[0 * 3 + 2] = start.v[2].getValue();
  positionData[1 * 3 + 0] = shaft.v[0].getValue();
  positionData[1 * 3 + 1] = shaft.v[1].getValue();
  positionData[1 * 3 + 2] = shaft.v[2].getValue();
  positionData[2 * 3 + 0] = shaft.v[0].getValue();
  positionData[2 * 3 + 1] = shaft.v[1].getValue();
  positionData[2 * 3 + 2] = shaft.v[2].getValue();
  positionData[3 * 3 + 0] = end.v[0].getValue();
  positionData[3 * 3 + 1] = end.v[1].getValue();
  positionData[3 * 3 + 2] = end.v[2].getValue();

  indexData[0] = 0;
  indexData[1] = 1;
  indexData[2] = 2;
  indexData[3] = 3;

  radiusData[0] = 100;
  radiusData[1] = 100;
  radiusData[2] = 200;
  radiusData[3] = 000;

  capData[0] = 1;
  capData[1] = 1;
  capData[2] = 1;
  capData[3] = 1;

  colorData[0] = 1.0f;
  colorData[1] = 1.0f;
  colorData[2] = 1.0f;
  colorData[3] = 1.0f;
  colorData[4] = 1.0f;
  colorData[5] = 1.0f;
  colorData[6] = 1.0f;
  colorData[7] = 1.0f;
  colorData[8] = 1.0f;
  colorData[9] = 0.0f;
  colorData[10] = 0.0f;
  colorData[11] = 1.0f;
  colorData[12] = 1.0f;
  colorData[13] = 0.0f;
  colorData[14] = 0.0f;
  colorData[15] = 1.0f;

  int flag = 0;
  flagData[0] = flag;
  flagData[1] = flag;
  flagData[2] = flag;
  flagData[3] = flag;

  SAMSON::displayCylinders(nCylinders, nPositions, indexData, positionData, radiusData, capData, colorData, flagData);

  delete[] indexData;
  delete[] radiusData;
  delete[] capData;
  delete[] colorData;
  delete[] flagData;
}

void ADNDisplayHelper::displayArrow(SBPosition3 start, SBPosition3 end, unsigned int nodeIndex, float * color, bool selectable)
{
  SBPosition3 shaft = end - start;
  shaft *= 0.55f;
  shaft += start;

  unsigned int nCylinders = 2;
  unsigned int nPositions = 2 * nCylinders;
  unsigned int* indexData = new unsigned int[nPositions];
  float *positionData = new float[3 * nPositions];
  float *radiusData = new float[nPositions];
  unsigned int *capData = new unsigned int[nPositions];
  float *colorData = new float[4 * nPositions];
  unsigned int *flagData = new unsigned int[nPositions];
  unsigned int *nodeIndexData = new unsigned int[nCylinders];

  positionData[0 * 3 + 0] = start.v[0].getValue();
  positionData[0 * 3 + 1] = start.v[1].getValue();
  positionData[0 * 3 + 2] = start.v[2].getValue();
  positionData[1 * 3 + 0] = shaft.v[0].getValue();
  positionData[1 * 3 + 1] = shaft.v[1].getValue();
  positionData[1 * 3 + 2] = shaft.v[2].getValue();
  positionData[2 * 3 + 0] = shaft.v[0].getValue();
  positionData[2 * 3 + 1] = shaft.v[1].getValue();
  positionData[2 * 3 + 2] = shaft.v[2].getValue();
  positionData[3 * 3 + 0] = end.v[0].getValue();
  positionData[3 * 3 + 1] = end.v[1].getValue();
  positionData[3 * 3 + 2] = end.v[2].getValue();

  indexData[0] = 0;
  indexData[1] = 1;
  indexData[2] = 2;
  indexData[3] = 3;

  radiusData[0] = 60;
  radiusData[1] = 60;
  radiusData[2] = 100;
  radiusData[3] = 0;

  capData[0] = 1;
  capData[1] = 1;
  capData[2] = 1;
  capData[3] = 1;

  nodeIndexData[0] = nodeIndex;
  nodeIndexData[1] = nodeIndex;
  /*nodeIndexData[2] = nodeIndex;
  nodeIndexData[3] = nodeIndex;*/


  colorData[0] = color[0];
  colorData[1] = color[1];
  colorData[2] = color[2];
  colorData[3] = color[3];
  colorData[4] = color[0];
  colorData[5] = color[1];
  colorData[6] = color[2];
  colorData[7] = color[3];
  colorData[8] = color[0];
  colorData[9] = color[1];
  colorData[10] = color[2];
  colorData[11] = color[3];
  colorData[12] = color[0];
  colorData[13] = color[1];
  colorData[14] = color[2];
  colorData[15] = color[3];

  int flag = 0;
  flagData[0] = flag;
  flagData[1] = flag;
  flagData[2] = flag;
  flagData[3] = flag;

  SAMSON::displayCylinders(nCylinders, nPositions, indexData, positionData, radiusData, capData, colorData, flagData);

  if (selectable)
  {
    SAMSON::displayCylindersSelection(nCylinders, nPositions, indexData, positionData, radiusData, capData, nodeIndexData);
  }

  delete[] indexData;
  delete[] radiusData;
  delete[] capData;
  delete[] colorData;
  delete[] flagData;

}