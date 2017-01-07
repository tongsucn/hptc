#pragma once
#ifndef HPTC_KERNELS_KERNEL_TRANS_H_
#define HPTC_KERNELS_KERNEL_TRANS_H_

#include <hptc/types.h>
#include <hptc/param/parameter_trans.h>
#include <hptc/kernels/kernel_trans_base.h>
#include <hptc/kernels/avx/kernel_trans_avx.h>


namespace hptc {

template <typename FloatType,
          CoefUsage USAGE>
using KernelTransFull
    = KernelTransAvx<FloatType, USAGE, KernelType::KERNEL_FULL>;


template <typename FloatType,
          CoefUsage USAGE>
using KernelTransHalf
    = KernelTransAvx<FloatType, USAGE, KernelType::KERNEL_HALF>;


template <typename FloatType>
using DeducedRegType = typename RegDeducerAvx<FloatType>::type;

}

#endif // HPTC_KERNELS_KERNEL_TRANS_H_
