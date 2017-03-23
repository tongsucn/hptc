#pragma once
#ifndef HPTC_UTIL_UTIL_TRANS_H_
#define HPTC_UTIL_UTIL_TRANS_H_

#include <array>
#include <vector>
#include <algorithm>

#include <hptc/types.h>


namespace hptc {

/*
 * Transpose coefficients types
 */
enum class CoefUsageTrans : TensorUInt {
  USE_NONE  = 0x0,
  USE_ALPHA = 0x1,
  USE_BETA  = 0x2,
  USE_BOTH  = 0x3
};


/*
 * Transpose kernel types
 */
enum class KernelTypeTrans : TensorUInt {
  KERNEL_FULL = 0,
  KERNEL_HALF = 1,
  KERNEL_LINE = 2
};


template <typename FloatType,
          KernelTypeTrans TYPE,
          typename Enable = void>
struct RegTypeDeducer {
};


template <typename FloatType,
          KernelTypeTrans TYPE>
using DeducedRegType = typename RegTypeDeducer<FloatType, TYPE>::type;


template <TensorUInt ORDER>
struct LoopParamTrans {
  LoopParamTrans();

  INLINE void set_pass(TensorUInt order);
  INLINE void set_disable();
  INLINE bool is_disabled() const;

  TensorIdx loop_begin[ORDER];
  TensorIdx loop_end[ORDER];
  TensorIdx loop_step[ORDER];
};


template <TensorUInt ORDER>
using LoopOrderTrans = std::array<TensorUInt, ORDER>;


template <TensorUInt ORDER>
using ParaStrategyTrans = std::array<TensorUInt, ORDER>;


template <typename FloatType,
          CoefUsageTrans USAGE>
double calc_tp_trans(const std::vector<TensorIdx> &size, double time_ms);


/*
 * Import implementation
 */
#include "util_trans.tcc"

}

#endif // HPTC_UTIL_UTIL_TRANS_H_