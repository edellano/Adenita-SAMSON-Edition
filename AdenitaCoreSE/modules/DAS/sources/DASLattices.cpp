#include "DASLattices.hpp"


void DASLattice::CreateSquareLattice(int maxRows, int maxCols)
{
  mat_ = ublas::matrix<LatticeCell>(maxRows, maxCols);

  for (unsigned row = 0; row < mat_.size1(); ++row) {
    for (unsigned column = 0; column < mat_.size2(); ++column) {
      //todo
      LatticeCell lc;
      lc.x_ = column * edgeDistance_;
      lc.y_ = row * edgeDistance_;
      mat_(row, column) = lc;
    }
  }
}

void DASLattice::CreateHoneycombLattice(int maxRows, int maxCols)
{
  mat_ = ublas::matrix<LatticeCell>(maxRows, maxCols);
  double angle = 120.0;  // deg
  double delt_y = edgeDistance_ * sin(ADNVectorMath::DegToRad(angle - 90)) * 0.5;
  double delt_x = edgeDistance_ * cos(ADNVectorMath::DegToRad(angle - 90));
  double offset = delt_y;

  for (unsigned int row = 0; row < mat_.size1(); ++row) {
    for (unsigned int column = 0; column < mat_.size2(); ++column) {
      double off = offset;
      if ((row + column) % 2 == 0) off *= -1;
      LatticeCell lc;
      lc.x_ = column * delt_x;
      lc.y_ = row * (edgeDistance_ + 2 * delt_y) + off;
      mat_(row, column) = lc;
    }
  }
}

LatticeCell DASLattice::GetLatticeCell(unsigned int row, unsigned int column)
{
  size_t rSize = mat_.size1();
  size_t cSize = mat_.size2();
  if (row >= rSize || column >= cSize) {
    std::string msg = "Lattice overflow. Probably worng lattice type was selected.";
    ADNLogger::LogError(msg);
  }
  return mat_(row, column);
}

size_t DASLattice::GetNumberRows()
{
  return mat_.size1();
}

size_t DASLattice::GetNumberCols()
{
  return mat_.size2();
}

DASLattice::DASLattice(LatticeType type, double edgeDistance, int maxRows, int maxCols)
{
  edgeDistance_ = edgeDistance;
  if (type == LatticeType::Honeycomb) {
    CreateHoneycombLattice(maxRows, maxCols);
  }
  else {
    CreateSquareLattice(maxRows, maxCols);
  }
}
