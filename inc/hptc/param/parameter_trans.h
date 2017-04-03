#pragma once
#ifndef HPTC_PARAM_PARAMETER_TRANS_H_
#define HPTC_PARAM_PARAMETER_TRANS_H_

#include <array>
#include <utility>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

#include <hptc/types.h>
#include <hptc/tensor.h>
#include <hptc/arch/compat.h>
#include <hptc/util/util_trans.h>
#include <hptc/kernels/kernel_trans.h>


namespace hptc {

template <typename FloatType,
          TensorUInt ORDER>
class TensorMergedWrapper
    : public TensorWrapper<FloatType, ORDER, MemLayout::COL_MAJOR> {
public:
  TensorMergedWrapper() = delete;

  template <MemLayout ACT_MAJOR>
  TensorMergedWrapper(const TensorWrapper<FloatType, ORDER, ACT_MAJOR> &tensor,
      const std::unordered_set<TensorUInt> &merge_set);

  HPTC_INL FloatType &operator[](const TensorIdx * RESTRICT indices);
  HPTC_INL const FloatType &operator[](
      const TensorIdx * RESTRICT indices) const;
  HPTC_INL FloatType &operator[](TensorIdx **indices);
  HPTC_INL const FloatType &operator[](const TensorIdx **indices) const;

private:
  TensorUInt begin_order_idx_, merged_order_;

  TensorUInt merge_idx_(const std::unordered_set<TensorUInt> &merge_set);
};


template <typename TensorType>
struct ParamTrans {
  // Type alias and constant values
  using Float = typename TensorType::Float;
  using Deduced = DeducedFloatType<Float>;
  using KernelPack = KernelPackTrans<Float>;

  static constexpr auto ORDER = TensorType::TENSOR_ORDER;

  ParamTrans(const TensorType &input_tensor, TensorType &output_tensor,
      const std::array<TensorUInt, ORDER> &perm, const Deduced alpha,
      const Deduced beta);

  HPTC_INL bool is_common_leading() const;
  HPTC_INL void set_coef(const Deduced alpha, const Deduced beta);
  HPTC_INL const KernelPack &get_kernel() const;
  void set_lin_wrapper_loop(const TensorUInt size_kn_inld,
      const TensorUInt size_kn_outld);

  void reset_data(const Float *data_in, Float *data_out);

private:
  TensorUInt merge_idx_(const std::array<TensorUInt, ORDER> &perm);

  // They need to be initialized before merging
  std::unordered_set<TensorUInt> input_merge_set_, output_merge_set_;
  KernelPackTrans<Float> kn_;

public:
  std::array<TensorUInt, ORDER> perm;
  Deduced alpha, beta;
  TensorIdx stride_in_inld, stride_in_outld, stride_out_inld, stride_out_outld;
  TensorUInt begin_order_idx;
  const TensorUInt merged_order;

  // Put the merged tensors here, they must be initialized after merging
  TensorMergedWrapper<Float, ORDER> input_tensor, output_tensor;
};


/*
 * Import implementation
 */
#include "parameter_trans.tcc"

}

#endif // HPTC_PARAM_PARAMETER_TRANS_H_
