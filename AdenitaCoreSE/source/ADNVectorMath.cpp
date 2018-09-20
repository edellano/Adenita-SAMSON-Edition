#include "ADNVectorMath.hpp"

namespace ADNVectorMath {

  ublas::vector<double> CreateBoostVector(std::vector<double> vec) {
    size_t sz = vec.size();
    ublas::vector<double> vex(sz);
    for (size_t i = 0; i < sz; ++i) {
      vex(i) = vec[i];
    }
    return vex;
  }

  std::vector<double> CreateStdVector(ublas::vector<double> vec) {
    size_t sz = vec.size();
    std::vector<double> vex(sz);
    for (size_t i = 0; i < sz; ++i) {
      vex[i] = vec(i);
    }
    return vex;
  }

  ublas::matrix<double> CreateBoostMatrix(std::vector<std::vector<double>> vecovec) {
    size_t num_rows = vecovec.size();
    size_t num_cols = 0;
    ublas::matrix<double> mat(num_rows, num_cols);
    if (num_rows > 0) {
      num_cols = vecovec[0].size();
      mat = ublas::matrix<double>(num_rows, num_cols);
      for (size_t i = 0; i < num_rows; ++i) {
        for (size_t j = 0; j < num_cols; ++j) {
          mat(i, j) = vecovec[i][j];
        }
      }
    }

    return mat;
  }

  ublas::vector<double> CalculateCM(ublas::matrix<double> positions) {
    ublas::vector<double> vec = InitializeVector(positions.size2());
    for (ublas::matrix<double>::iterator1 it1 = positions.begin1(); it1 != positions.end1(); ++it1) {
      // iteration over rows
      ublas::vector<double> r = ublas::row(positions, it1.index1());
      vec += r;
    }
    size_t n = positions.size1();
    vec *= (1.0 / n);
    return vec;
  }

  ublas::vector<double> CalculateCM(ublas::matrix<double> weightedPositions, double totalMass)
  {
    ublas::vector<double> vec = InitializeVector(weightedPositions.size2());
    for (ublas::matrix<double>::iterator1 it1 = weightedPositions.begin1(); it1 != weightedPositions.end1(); ++it1) {
      // iteration over rows
      ublas::vector<double> r = ublas::row(weightedPositions, it1.index1());
      vec += r;
    }
    size_t n = weightedPositions.size1();
    vec /= totalMass;
    return vec;
  }

  ublas::vector<double> CrossProduct(ublas::vector<double> v, ublas::vector<double> w) {
    size_t vsz = v.size();
    size_t wsz = w.size();
    if (vsz != wsz || vsz != 3) {
      std::exit(EXIT_FAILURE);
    }

    ublas::vector<double> k(3);
    k(0) = v(1)*w(2) - v(2)*w(1);
    k(1) = v(2)*w(0) - v(0)*w(2);
    k(2) = v(0)*w(1) - v(1)*w(0);

    return k;
  }

  ublas::matrix<double> SkewMatrix(ublas::vector<double> v) {
    std::vector<double> row1 = { 0.0, -v[2], v[1] };
    std::vector<double> row2 = { v[2], 0.0, -v[0] };
    std::vector<double> row3 = { -v[1], v[0], 0.0 };
    std::vector<std::vector<double>> skew = { row1, row2, row3 };
    return ADNVectorMath::CreateBoostMatrix(skew);
  }

  ublas::vector<double> DirectionVector(ublas::vector<double> p, ublas::vector<double> q) {
    ublas::vector<double> dif = (q - p);
    return dif / ublas::norm_2(dif);
  }

  double DegToRad(double degree) {
    double pi = atan(1.0) * 4;
    return degree * pi / 180.0;
  }

  ublas::matrix<double> MakeRotationMatrix(ublas::vector<double> dir, double angle) {
    // We use R = d*d^T + cos(a) * (I - d*d^T) - sin(a)*skew(d)
    ublas::matrix<double> ident = ublas::identity_matrix<double>(3);
    ublas::matrix<double> ddT = ublas::outer_prod(dir, dir);
    ublas::matrix<double> skew = SkewMatrix(dir);
    ublas::matrix<double> mtx = ddT + cos(angle) * (ident - ddT) - sin(angle) * skew;
    return mtx;
  }

  ublas::vector<double> InitializeVector(size_t size) {
    ublas::vector<double> vec(size, 0.0);
    return vec;
  }

  ublas::matrix<double> InitializeMatrix(size_t sz_r, size_t sz_c) {
    ublas::matrix<double> mat(sz_r, sz_c, 0.0);
    return mat;
  }

  ublas::matrix<double> InitializeMatrix(size_t sz) {
    return InitializeMatrix(sz, sz);
  }

  ublas::matrix<double> Translate(ublas::matrix<double> input, ublas::vector<double> t_vector) {
    ublas::vector<double> T = t_vector;
    ublas::matrix<double> trans_pos(input.size1(), input.size2());
    std::size_t N = input.size1();
    for (size_t rit = 0; rit < N; ++rit) {
      ublas::vector<double> coords = ublas::row(input, rit);
      ublas::vector<double> new_coords = coords + T;
      ublas::row(trans_pos, rit) = new_coords;
    }

    return trans_pos;
  }

  ublas::matrix<double> Rotate(ublas::matrix<double> input, ublas::matrix<double> rot_matrix) {
    ublas::matrix<double> rot_pos(input.size1(), input.size2());
    size_t N = input.size1();
    for (size_t rit = 0; rit < N; ++rit) {
      ublas::vector<double> coords = ublas::row(input, rit);
      ublas::vector<double> new_coords = ublas::prod(rot_matrix, coords);
      ublas::row(rot_pos, rit) = new_coords;
    }

    return rot_pos;
  }

  ublas::matrix<double> CenterSystem(ublas::matrix<double> input) {
    ublas::vector<double> cm = CalculateCM(input);
    return Translate(input, -cm);
  }

  void AddRowToMatrix(ublas::matrix<double> &input, ublas::vector<double> r) {
    size_t num_rows = input.size1();
    size_t num_cols = input.size2();
    input.resize(num_rows + 1, num_cols, true);
    ublas::row(input, num_rows) = r;
  }

  ublas::vector<double> CalculatePlane(ublas::matrix<double> mat) {
    size_t num_points = mat.size1();
    size_t dim = mat.size2();
    ublas::vector<double> sol(3);

    if (dim != 3) {
      throw std::invalid_argument("Points are not 3-dimensional ");
    }

    double XX = 0.0;
    double YY = 0.0;
    double XY = 0.0;
    double XZ = 0.0;
    double YZ = 0.0;
    double ZZ = 0.0;


    for (int i = 0; i < num_points; ++i) {
      ublas::vector<double> point = ublas::row(mat, i);
      XX += point[0] * point[0];
      YY += point[1] * point[1];
      XY += point[0] * point[1];
      XZ += point[0] * point[2];
      YZ += point[1] * point[2];
      ZZ += point[2] * point[2];
    }

    // N = (A, B, C)

    // Assume C = 1
    double detC = XX*YY - XY*XY;
    // Assume B = 1
    double detB = XX*ZZ - XZ*XZ;
    // Assume A = 1
    double detA = YY*ZZ - YZ*YZ;

    ublas::vector<double> dets(3);
    dets[0] = detA;
    dets[1] = detB;
    dets[2] = detC;

    size_t idx = ublas::index_norm_inf(dets);

    switch (idx) {
    case 0:
      sol[0] = 1.0;
      sol[1] = (YZ*XZ - XY*ZZ) / detA;
      sol[2] = (YZ*XY - XZ*YY) / detA;
      break;
    case 1:
      sol[0] = (XZ*YZ - XY*ZZ) / detB;
      sol[1] = 1.0;
      sol[2] = (XZ*XY - YZ*XX) / detB;
      break;
    case 2:
      sol[0] = (YZ*XY - XZ*YY) / detC;
      sol[1] = (XY*XZ - XX*YZ) / detC;
      sol[2] = 1.0;
      break;
    }

    return sol / ublas::norm_2(sol);
  }

  ublas::matrix<double> InvertMatrix(const ublas::matrix<double>& input) {
    typedef ublas::permutation_matrix<std::size_t> pmatrix;

    // create a working copy of the input
    ublas::matrix<double> A(input);

    // create a permutation matrix for the LU-factorization
    pmatrix pm(A.size1());

    // perform LU-factorization
    size_t res = ublas::lu_factorize(A, pm);
    if (res != 0) {
      throw std::invalid_argument("Could not perform LU factorization");
    }

    // create identity matrix of "inverse"
    ublas::matrix<double> inverse = ublas::identity_matrix<double>(A.size1());

    // backsubstitute to get the inverse
    lu_substitute(A, pm, inverse);

    return inverse;
  }

  double Determinant(ublas::matrix<double> mat) {
    size_t num_rows = mat.size1();
    size_t num_cols = mat.size2();
    if (num_rows != num_cols || num_rows != 3) {
      throw std::invalid_argument("Non-square 3-dimensional matrix");
    }

    double sol = mat.at_element(0, 0) * (mat.at_element(1, 1) * mat.at_element(2, 2) - mat.at_element(1, 2)*mat.at_element(2, 1)) - \
      mat.at_element(0, 1) * (mat.at_element(1, 0) * mat.at_element(2, 2) - mat.at_element(2, 0) * mat.at_element(1, 2)) + \
      mat.at_element(0, 2) * (mat.at_element(1, 0) * mat.at_element(2, 1) - mat.at_element(2, 0) * mat.at_element(1, 1));

    return sol;
  }

  bool IsNearlyZero(double n) {
    bool sol = false;

    double tol = 0.000000001;
    if (abs(n) < tol) {
      sol = true;
    }
    return sol;
  }

  double CalculateVectorNorm(ublas::vector<double> v)
  {
    double n = 0.0;
    size_t sz = v.size();
    for (size_t i = 0; i < sz; ++i) {
      n += v[i] * v[i];
    }
    n = sqrt(n);
    return n;
  }

  ublas::matrix<double> ApplyTransformation(ublas::matrix<double> t_mat, ublas::matrix<double> points) {
    size_t num_points = points.size1();
    size_t dim = points.size2();

    if (t_mat.size1() != dim || t_mat.size2() != dim) {
      throw std::invalid_argument("Transformation matrix does not have the right dimensions.");
    }

    ublas::matrix<double> coords(num_points, 3);
    for (int i = 0; i < num_points; ++i) {
      ublas::row(coords, i) = ublas::prod(t_mat, ublas::row(points, i));
    }

    return coords;
  }

  ublas::vector<double> Spherical2Cartesian(ublas::vector<double> spher)
  {
    double r = spher[0];
    double theta = spher[1];
    double phi = spher[2];
    ublas::vector<double> cart(3, 0.0);
    cart[0] = r * sin(theta) * cos(phi);  // x
    cart[1] = r * sin(theta) * sin(phi);  // y
    cart[2] = r * cos(theta);  // z
    return cart;
  }

  ublas::matrix<double> FindOrthogonalSubspace(ublas::vector<double> z) {
    z /= ublas::norm_2(z);
    ublas::vector<double> x(3, 0.0);
    if (z[0] != 0) {
      x[1] = 1;
      x[2] = 1;
      x[0] = (-z[1] - z[2]) / z[0];
    }
    else if (z[1] != 0) {
      x[0] = 1;
      x[2] = 1;
      x[1] = (-z[0] - z[2]) / z[1];
    }
    else if (z[2] != 0) {
      x[0] = 1;
      x[1] = 1;
      x[2] = (-z[0] - z[1]) / z[2];
    }
    x /= ublas::norm_2(x);
    ublas::vector<double> y = CrossProduct(z, x);
    y /= ublas::norm_2(y);

    ublas::matrix<double> subspace(2, 3, 0.0);
    ublas::row(subspace, 0) = x;
    ublas::row(subspace, 1) = y;
    return subspace;
  }
};
