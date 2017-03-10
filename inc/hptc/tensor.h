#pragma once
#ifndef HPTC_TENSOR_H_
#define HPTC_TENSOR_H_

#include <array>
#include <vector>
#include <algorithm>
#include <initializer_list>

#include <hptc/types.h>


namespace hptc {

struct TensorRangeIdx {
  TensorRangeIdx(TensorIdx left_idx, TensorIdx right_idx)
    : left_idx(left_idx),
      right_idx(right_idx) {
  }

  TensorIdx left_idx;
  TensorIdx right_idx;
};

using TRI = TensorRangeIdx;


template <TensorOrder ORDER>
class TensorSize {
public:
  TensorSize();
  TensorSize(const std::array<TensorOrder, ORDER> &sizes);
  TensorSize(const std::vector<TensorOrder> &sizes);
  TensorSize(std::initializer_list<TensorOrder> sizes);

  bool operator==(const TensorSize<ORDER> &size_obj) const;

  INLINE TensorOrder &operator[](TensorOrder order);
  INLINE const TensorOrder &operator[](TensorOrder order) const;

private:
  TensorOrder size_[ORDER];
};


template <typename FloatType,
          TensorOrder ORDER,
          MemLayout LAYOUT = MemLayout::COL_MAJOR>
class TensorWrapper {
public:
  using FLOAT = FloatType;
  constexpr static auto TENSOR_ORDER = ORDER;

  TensorWrapper();

  TensorWrapper(const TensorSize<ORDER> &size_obj, FloatType *raw_data);
  TensorWrapper(const TensorSize<ORDER> &size_obj,
      const TensorSize<ORDER> &outer_size_obj,
      const std::array<TensorIdx, ORDER> &order_offset, FloatType *raw_data);

  template <MemLayout ACT_MAJOR>
  TensorWrapper(TensorWrapper<FloatType, ORDER, ACT_MAJOR> &wrapper);

  template <typename... Idx>
  INLINE FloatType &operator()(Idx... indices);

  INLINE FloatType &operator[](const TensorIdx * RESTRICT indices);
  INLINE const FloatType &operator[](const TensorIdx * RESTRICT indices) const;

  template <typename... Ranges>
  TensorWrapper<FloatType, ORDER, LAYOUT> slice(TRI range, Ranges... rest);
  TensorWrapper<FloatType, ORDER, LAYOUT> slice(
      const std::array<TRI, ORDER> &ranges);

  INLINE TensorSize<ORDER> &get_size();
  INLINE const TensorSize<ORDER> &get_size() const;
  INLINE TensorSize<ORDER> &get_outer_size();
  INLINE const TensorSize<ORDER> &get_outer_size() const;
  INLINE FloatType *get_data();
  INLINE const FloatType *get_data() const;
  INLINE void set_data(FloatType *new_data);
  INLINE const TensorIdx *get_offset() const;

protected:
  // Internal function member
  INLINE void init_offset_();
  INLINE void init_offset_(const std::array<TensorIdx, ORDER> &order_offset);

  template <typename... Idx>
  INLINE FloatType &get_element_(TensorOrder curr_order, TensorIdx curr_offset,
      TensorIdx next_idx, Idx... idx);
  INLINE FloatType &get_element_(TensorOrder curr_order, TensorIdx curr_offset);

  // Internal data member
  TensorSize<ORDER> size_;
  TensorSize<ORDER> outer_size_;
  FloatType *raw_data_;
  TensorIdx offsets_[ORDER];
  TensorIdx strides_[ORDER];
};


/*
 * Import implementation
 */
#include "tensor.tcc"

}

#endif // HPTC_TENSOR_H_
