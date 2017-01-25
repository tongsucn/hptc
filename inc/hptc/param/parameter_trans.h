#pragma once
#ifndef HPTC_PARAM_PARAMETER_TRANS_H_
#define HPTC_PARAM_PARAMETER_TRANS_H_

#include <array>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

#include <hptc/types.h>
#include <hptc/tensor.h>


namespace hptc {

enum class CoefUsage : GenNumType {
  USE_NONE  = 0x0,
  USE_ALPHA = 0x1,
  USE_BETA  = 0x2,
  USE_BOTH  = 0x3
};


template <typename FloatType,
          TensorOrder ORDER,
          MemLayout LAYOUT = MemLayout::COL_MAJOR>
class TensorMergedWrapper : public TensorWrapper<FloatType, ORDER, LAYOUT> {
public:
  TensorMergedWrapper() = delete;
  TensorMergedWrapper(const TensorWrapper<FloatType, ORDER, LAYOUT> &wrapper);

  INLINE FloatType &operator[](const TensorIdx * RESTRICT indices);
  INLINE const FloatType &operator[](const TensorIdx * RESTRICT indices) const;
  INLINE FloatType &operator[](TensorIdx **indices);
  INLINE const FloatType &operator[](const TensorIdx **indices) const;

  INLINE TensorOrder get_merged_order();
  void merge_idx(const std::unordered_set<TensorOrder> &merge_set);

private:
  TensorOrder merged_order_;
};


template <typename FloatType,
          TensorOrder ORDER,
          CoefUsage USAGE,
          MemLayout LAYOUT = MemLayout::COL_MAJOR>
struct ParamTrans {
  ParamTrans(const TensorWrapper<FloatType, ORDER, LAYOUT> &input_tensor,
      const TensorWrapper<FloatType, ORDER, LAYOUT> &output_tensor,
      const std::array<TensorOrder, ORDER> &perm,
      DeducedFloatType<FloatType> alpha, DeducedFloatType<FloatType> beta);

  constexpr static CoefUsage COEF_USAGE = USAGE;

  TensorWrapper<FloatType, ORDER, LAYOUT> org_input_tensor, org_output_tensor;
  TensorMergedWrapper<FloatType, ORDER, LAYOUT> input_tensor, output_tensor;
  DeducedFloatType<FloatType> alpha, beta;

  TensorOrder perm[ORDER];
  TensorIdx input_stride, output_stride;
  TensorOrder merged_order;

private:
  void merge_idx_(const std::array<TensorOrder, ORDER> &perm);
};


/*
 * Import implementation
 */
#include "parameter_trans.tcc"

}

#endif // HPTC_PARAM_PARAMETER_TRANS_H_
