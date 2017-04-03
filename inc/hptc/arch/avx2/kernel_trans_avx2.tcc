#pragma once
#ifndef HPTC_ARCH_AVX2_KERNEL_TRANS_AVX2_TCC_
#define HPTC_ARCH_AVX2_KERNEL_TRANS_AVX2_TCC_

/*
 * Specializations of class KernelTrans
 */
template <>
void KernelTrans<float, KernelTypeTrans::KERNEL_FULL>::exec(
    const float * RESTRICT data_in, float * RESTRICT data_out,
    const TensorIdx stride_in_outld, const TensorIdx stride_out_inld) const;
template <>
void KernelTrans<double, KernelTypeTrans::KERNEL_FULL>::exec(
    const double * RESTRICT data_in, double * RESTRICT data_out,
    const TensorIdx stride_in_outld, const TensorIdx stride_out_inld) const;
template <>
void KernelTrans<FloatComplex, KernelTypeTrans::KERNEL_FULL>::exec(
    const FloatComplex * RESTRICT data_in, FloatComplex * RESTRICT data_out,
    const TensorIdx stride_in_outld, const TensorIdx stride_out_inld) const;
template <>
void KernelTrans<DoubleComplex, KernelTypeTrans::KERNEL_FULL>::exec(
    const DoubleComplex * RESTRICT data_in, DoubleComplex * RESTRICT data_out,
    const TensorIdx stride_in_outld, const TensorIdx stride_out_inld) const;


template <>
void KernelTrans<float, KernelTypeTrans::KERNEL_HALF>::exec(
    const float * RESTRICT data_in, float * RESTRICT data_out,
    const TensorIdx stride_in_outld, const TensorIdx stride_out_inld) const;
template <>
void KernelTrans<double, KernelTypeTrans::KERNEL_HALF>::exec(
    const double * RESTRICT data_in, double * RESTRICT data_out,
    const TensorIdx stride_in_outld, const TensorIdx stride_out_inld) const;
template <>
void KernelTrans<FloatComplex, KernelTypeTrans::KERNEL_HALF>::exec(
    const FloatComplex * RESTRICT data_in, FloatComplex * RESTRICT data_out,
    const TensorIdx stride_in_outld, const TensorIdx stride_out_inld) const;
template <>
void KernelTrans<DoubleComplex, KernelTypeTrans::KERNEL_HALF>::exec(
    const DoubleComplex * RESTRICT data_in, DoubleComplex * RESTRICT data_out,
    const TensorIdx stride_in_outld, const TensorIdx stride_out_inld) const;


/*
 * Specialization of class KernelTrans, linear kernel, used for common leading
 */
template <typename FloatType>
class KernelTrans<FloatType, KernelTypeTrans::KERNEL_LINE>
    : public KernelTransData<FloatType, KernelTypeTrans::KERNEL_LINE> {
public:
  static constexpr TensorUInt LOOP_MAX = 10;

  KernelTrans();

  void set_wrapper_loop(const TensorIdx stride_in_inld,
      const TensorIdx stride_in_outld, const TensorIdx stride_out_inld,
      const TensorIdx stride_out_outld, const TensorUInt size_kn_inld,
      const TensorUInt size_kn_outld);

  void exec(const FloatType * RESTRICT data_in, FloatType * RESTRICT data_out,
      const TensorIdx size_trans, const TensorIdx size_pad) const;

private:
  TensorIdx stride_in_inld_, stride_in_outld_, stride_out_inld_,
      stride_out_outld_;
  TensorUInt size_kn_inld_, size_kn_outld_;
};


/*
 * Explicit template instantiation declaration for class KernelTransData
 */
extern template class KernelTransData<float, KernelTypeTrans::KERNEL_FULL>;
extern template class KernelTransData<double, KernelTypeTrans::KERNEL_FULL>;
extern template class KernelTransData<FloatComplex,
    KernelTypeTrans::KERNEL_FULL>;
extern template class KernelTransData<DoubleComplex,
    KernelTypeTrans::KERNEL_FULL>;

extern template class KernelTransData<float, KernelTypeTrans::KERNEL_HALF>;
extern template class KernelTransData<double, KernelTypeTrans::KERNEL_HALF>;
extern template class KernelTransData<FloatComplex,
    KernelTypeTrans::KERNEL_HALF>;
extern template class KernelTransData<DoubleComplex,
    KernelTypeTrans::KERNEL_HALF>;

extern template class KernelTransData<float, KernelTypeTrans::KERNEL_LINE>;
extern template class KernelTransData<double, KernelTypeTrans::KERNEL_LINE>;
extern template class KernelTransData<FloatComplex,
    KernelTypeTrans::KERNEL_LINE>;
extern template class KernelTransData<DoubleComplex,
    KernelTypeTrans::KERNEL_LINE>;


/*
 * Explicit template instantiation declaration for class KernelTrans
 */
extern template class KernelTrans<float, KernelTypeTrans::KERNEL_FULL>;
extern template class KernelTrans<double, KernelTypeTrans::KERNEL_FULL>;
extern template class KernelTrans<FloatComplex, KernelTypeTrans::KERNEL_FULL>;
extern template class KernelTrans<DoubleComplex, KernelTypeTrans::KERNEL_FULL>;

extern template class KernelTrans<float, KernelTypeTrans::KERNEL_HALF>;
extern template class KernelTrans<double, KernelTypeTrans::KERNEL_HALF>;
extern template class KernelTrans<FloatComplex, KernelTypeTrans::KERNEL_HALF>;
extern template class KernelTrans<DoubleComplex, KernelTypeTrans::KERNEL_HALF>;

extern template class KernelTrans<float, KernelTypeTrans::KERNEL_LINE>;
extern template class KernelTrans<double, KernelTypeTrans::KERNEL_LINE>;
extern template class KernelTrans<FloatComplex, KernelTypeTrans::KERNEL_LINE>;
extern template class KernelTrans<DoubleComplex, KernelTypeTrans::KERNEL_LINE>;

#endif // HPTC_ARCH_AVX2_KERNEL_TRANS_AVX2_TCC_
