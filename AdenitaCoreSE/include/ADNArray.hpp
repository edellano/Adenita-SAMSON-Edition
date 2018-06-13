#pragma once

#define _CRTDBG_MAP_ALLOC

template <class T> class ADNArray {
public:
  ADNArray() = default;

  ADNArray(const ADNArray &obj) : ADNArray(obj.GetDim(), obj.GetNumElements()) {
    size_t total = dim_ * num_elements_;
    size_t i = 0;
    auto arr = obj.GetArray();
    for (i = 0; i < total; ++i) {
      array_[i] = arr[i];
    }
  }

  ADNArray(size_t dim, size_t num_elements) {
    dim_ = dim;
    num_elements_ = num_elements;
    size_t total = dim * num_elements;
    array_ = new T[total];
  }

  // For C++11: 
  ADNArray(size_t num_elements) : ADNArray(1, num_elements) {}

  ~ADNArray() {
    delete[] array_;
  }

  ADNArray<T>& operator=(ADNArray<T> other) {
    if (&other == this) {
      return *this;
    }
    // if array has same dim and num_elements we could try to save memory
    // here using other array space?
    dim_ = other.GetDim();
    num_elements_ = other.GetNumElements();
    size_t total = dim_ * num_elements_;
    array_ = new T[total];
    size_t i = 0;
    auto arr = other.GetArray();
    for (i = 0; i < total; ++i) {
      array_[i] = arr[i];
    }

    return *this;
  }

  T& operator()(std::size_t idx, std::size_t idy) {
    if (dim_ * idx + idy >= num_elements_*dim_) {
      std::string pos = std::to_string(dim_ * idx + idy);
      std::string total = std::to_string(num_elements_*dim_);
      std::string msg = "Trying to access position " + pos + " of array of size " + total;
      //ADNAuxiliary::log(msg);
      throw ERROR_OUT_OF_BOUNDS;
    }
    return array_[dim_ * idx + idy];
  }

  const T& operator()(std::size_t idx, std::size_t idy) const {
    if (dim_ * idx + idy >= num_elements_*dim_) {
      std::string pos = std::to_string(dim_ * idx + idy);
      std::string total = std::to_string(num_elements_*dim_);
      std::string msg = "Trying to access position " + pos + " of array of size " + total;
      //ADNAuxiliary::log(msg);
      throw ERROR_OUT_OF_BOUNDS;
    }
    return array_[dim_ * idx + idy];
  }

  T& operator()(std::size_t idx) {
    if (dim_ * idx >= num_elements_*dim_) {
      std::string pos = std::to_string(dim_ * idx);
      std::string total = std::to_string(num_elements_*dim_);
      std::string msg = "Trying to access position " + pos + " of array of size " + total;
      //ADNAuxiliary::log(msg);
      throw ERROR_OUT_OF_BOUNDS;
    }
    return array_[dim_ * idx];
  }

  const T& operator()(std::size_t idx) const {
    if (dim_ * idx >= num_elements_*dim_) {
      std::string pos = std::to_string(dim_ * idx);
      std::string total = std::to_string(num_elements_*dim_);
      std::string msg = "Trying to access position " + pos + " of array of size " + total;
      //ADNAuxiliary::log(msg);
      throw ERROR_OUT_OF_BOUNDS;
    }
    return array_[dim_ * idx];
  }

  T* GetArray() const { return array_; }
  size_t GetDim() const { return dim_; }
  size_t GetNumElements() const { return num_elements_; }
  /**
  * Returns a row as a 1-dim ANTArray
  * \return A 1-dim ANTArray with number of elements equal to this dim_
  */
  ADNArray<T> GetRow(std::size_t row) {
    ADNArray<T> arr(dim_);
    for (int i = 0; i < dim_; ++i) {
      arr(i) = this->operator ()(row, i);
    }
    return arr;
  }
  /**
  * Sets a row equal to the values contained in arr.
  * \param the row we want to set.
  * \param a 1-dim ANTArray with number of elements equal to the row length.
  */
  void SetRow(std::size_t row, ADNArray<T> arr) {
    if (arr.GetDim() != 1 || dim_ != arr.GetNumElements()) {
      std::string msg = "Dimension mismatch when assigning row.";
      //ADNAuxiliary::log(msg);
      throw ERROR_DIMENSION_MISMATCH;
    }
    else {
      for (int i = 0; i < dim_; ++i) {
        this->operator ()(row, i) = arr(i);
      }
    }
  }
  /**
  * Concatenates two ANTArrays and returns one
  */
  static ADNArray* Concatenate(ADNArray<T> v, ADNArray<T> w) {
    ADNArray* sol = nullptr;
    if (v.GetDim() == w.GetDim()) {
      sol = ADNArray(v.GetDim(), v.GetNumElements() + w.GetNumElements());
      size_t i = 0;
      for (i = 0; i < v.GetNumElements(); ++i) {
        auto row = v.GetRow(i);
        sol->SetRow(i, row);
      }
      for (size_t j = 0; j < w.GetNumElements(); ++j) {
        auto row = w.GetRow(j);
        sol->SetRow(i + j, row);
      }
    }
    else {
      std::string msg = "Dimension mismatch when assigning row.";
      //ADNAuxiliary::log(msg);
      throw ERROR_DIMENSION_MISMATCH;
    }
    return sol;
  }

private:
  T* array_ = nullptr;
  size_t dim_;
  size_t num_elements_;
  int ERROR_OUT_OF_BOUNDS = 30;
  int ERROR_DIMENSION_MISMATCH = 31;
};
