#pragma once
#ifndef HPTC_UTIL_TCC_
#define HPTC_UTIL_TCC_

/*
 * Implementation for class DataWrapper
 */
template <typename FloatType>
DataWrapper<FloatType>::DataWrapper(const std::vector<TensorOrder> &size,
    bool randomize)
    : gen_(std::random_device()()),
      dist_(this->ele_lower_, this->ele_upper_),
      data_len_(std::accumulate(size.begin(), size.end(), 1,
          std::multiplies<TensorOrder>())) {
  // Allocate memory
  this->org_in_data = new FloatType [this->data_len_];
  this->org_out_data = new FloatType [this->data_len_];

  // Initialize content
  if (randomize)
    for (TensorIdx idx = 0; idx < this->data_len_; ++idx) {
      auto org_in_ptr = reinterpret_cast<Deduced_ *>(this->org_in_data + idx);
      auto org_out_ptr = reinterpret_cast<Deduced_ *>(this->org_out_data + idx);
      for (GenNumType in_idx = 0; in_idx < this->inner_; ++in_idx) {
        org_in_ptr[in_idx] = this->dist_(this->gen_);
        org_out_ptr[in_idx] = this->dist_(this->gen_);
      }
    }
}


template <typename FloatType>
DataWrapper<FloatType>::DataWrapper(const std::vector<TensorOrder> &size,
    const FloatType *input_data, const FloatType *output_data)
    : data_len_(std::accumulate(size.begin(), size.end(), 1,
          std::multiplies<TensorOrder>())) {
  this->org_in_data = new FloatType [this->data_len_];
  std::copy(input_data, input_data + this->data_len_, this->org_in_data);
  this->org_out_data = new FloatType [this->data_len_];
  std::copy(output_data, output_data + this->data_len_, this->org_out_data);
}


template <typename FloatType>
DataWrapper<FloatType>::~DataWrapper() {
  delete [] this->org_in_data;
  delete [] this->org_out_data;
}


/*
 * Implementation for class TimerWrapper
 */
template <typename Callable,
          typename... Args>
INLINE double TimerWrapper::operator()(Callable &target, Args&&... args) {
  using Duration = std::chrono::duration<double, std::milli>;

  if (0 == this->times_)
    return 0.0;

  auto start = std::chrono::high_resolution_clock::now();
  target(std::forward<Args>(args)...);
  auto diff = std::chrono::high_resolution_clock::now() - start;
  auto result = std::chrono::duration_cast<Duration>(diff);

  for (GenNumType idx = 1; idx < this->times_; ++idx) {
    start = std::chrono::high_resolution_clock::now();
    target(std::forward<Args>(args)...);
    diff = std::chrono::high_resolution_clock::now() - start;
    auto duration = std::chrono::duration_cast<Duration>(diff);
    if (duration < result)
      result = duration;
  }

  return result.count();
}


#endif // HPTC_UTIL_TCC_
