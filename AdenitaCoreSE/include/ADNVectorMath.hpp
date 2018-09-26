#pragma once

#include <vector>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <cmath>
#include "SAMSON.hpp"

namespace ublas = boost::numeric::ublas;

namespace ADNVectorMath {
  using namespace ublas;

  ublas::vector<double> CreateBoostVector(std::vector<double> vec);
  std::vector<double> CreateStdVector(ublas::vector<double> vec);
  ublas::matrix<double> CreateBoostMatrix(std::vector<std::vector<double>> vecovec);
  ublas::vector<double> CalculateCM(ublas::matrix<double> positions);
  ublas::vector<double> CalculateCM(ublas::matrix<double> weightedPositions, double totalMass);
  ublas::vector<double> CrossProduct(ublas::vector<double> v, ublas::vector<double> w);
  ublas::vector<double> DirectionVector(ublas::vector<double> p, ublas::vector<double> q);
  double DegToRad(double degree);
  ublas::matrix<double> MakeRotationMatrix(ublas::vector<double> dir, double angle);
  ublas::matrix<double> SkewMatrix(ublas::vector<double> v);
  ublas::vector<double> InitializeVector(size_t size);  // deprecated, use constructor
  ublas::matrix<double> InitializeMatrix(size_t sz_r, size_t sz_c);  // deprecated, use constructor
  ublas::matrix<double> InitializeMatrix(size_t sz);  // deprecated, use constructor
  ublas::matrix<double> Translate(ublas::matrix<double> input, ublas::vector<double> t_vector);
  ublas::matrix<double> Rotate(ublas::matrix<double> input, ublas::matrix<double> rot_matrix);  // deprecated, use ApplyTransformation
  ublas::matrix<double> CenterSystem(ublas::matrix<double> input);
  void AddRowToMatrix(ublas::matrix<double> &input, ublas::vector<double> r);
  ublas::vector<double> CalculatePlane(ublas::matrix<double> mat);
  ublas::matrix<double> FindOrthogonalSubspace(ublas::vector<double> z);
  ublas::matrix<double> InvertMatrix(const ublas::matrix<double>& input);
  double Determinant(ublas::matrix<double> mat);
  bool IsNearlyZero(double n);
  double CalculateVectorNorm(ublas::vector<double> v);
  /*!
    * Applies the transformation given by t_mat to a set of points
    * \param the transformation matrix
    * \param a matrix holding coordinates of points
    * \return a matrix with the coordinates after the transformation
  */
  ublas::matrix<double> ApplyTransformation(ublas::matrix<double> t_mat, ublas::matrix<double> points);

  ublas::vector<double> Spherical2Cartesian(ublas::vector<double> spher);

  // SAMSON types operations
  SBVector3 SBCrossProduct(SBVector3 v, SBVector3 w);
  double SBInnerProduct(SBVector3 v, SBVector3 w);
};