#include <hptt/kernels/macro_kernel_trans.h>

#include <hptt/types.h>
#include <hptt/util/util.h>
#include <hptt/arch/compat.h>
#include <hptt/kernels/micro_kernel_trans.h>

namespace hptt {

/*
 * Implementation for class MacroTrans
 */
template <typename MicroKernel,
          TensorUInt SIZE_IN_INLD,
          TensorUInt SIZE_IN_OUTLD>
MacroTrans<MicroKernel, SIZE_IN_INLD, SIZE_IN_OUTLD>::MacroTrans()
    : kernel_(),
      kn_width_(MicroKernel::KN_WIDTH) {
}


template <typename MicroKernel,
          TensorUInt SIZE_IN_INLD,
          TensorUInt SIZE_IN_OUTLD>
TensorUInt
MacroTrans<MicroKernel, SIZE_IN_INLD, SIZE_IN_OUTLD>::get_cont_len() const {
  return SIZE_IN_INLD * this->kn_width_;
}


template <typename MicroKernel,
          TensorUInt SIZE_IN_INLD,
          TensorUInt SIZE_IN_OUTLD>
TensorUInt
MacroTrans<MicroKernel, SIZE_IN_INLD, SIZE_IN_OUTLD>::get_ncont_len() const {
  return SIZE_IN_OUTLD * this->kn_width_;
}


template <typename MicroKernel,
          TensorUInt SIZE_IN_INLD,
          TensorUInt SIZE_IN_OUTLD>
void MacroTrans<MicroKernel, SIZE_IN_INLD, SIZE_IN_OUTLD>::set_coef(
    const DeducedFloatType<typename MicroKernel::Float> alpha,
    const DeducedFloatType<typename MicroKernel::Float> beta) {
  this->kernel_.set_coef(alpha, beta);
}


template <typename MicroKernel,
          TensorUInt SIZE_IN_INLD,
          TensorUInt SIZE_IN_OUTLD>
void MacroTrans<MicroKernel, SIZE_IN_INLD, SIZE_IN_OUTLD>::exec(
    const typename MicroKernel::Float *data_in,
    typename MicroKernel::Float *data_out, const TensorIdx stride_in_outld,
    const TensorIdx stride_out_inld) const {
  // Prefetching
  using FloatType = typename MicroKernel::Float;
  constexpr TensorUInt NUM_IN_OUTLD = MicroKernel::KN_WIDTH * SIZE_IN_OUTLD;
  auto USE_STREAMING = not MicroKernel::UPDATE and
      hptt::check_aligned<FloatType>(data_out) and
      MicroKernel::check_stream(NUM_IN_OUTLD);

  if (USE_STREAMING) {
    // Create aligned buffer, macro HPTT_MEM_ALIGN is defined in
    // hptt/arch/compat.h
    constexpr TensorUInt NUM_IN_INLD = MicroKernel::KN_WIDTH * SIZE_IN_INLD;
    FloatType buffer_out[NUM_IN_INLD * NUM_IN_OUTLD] HPTT_MEM_ALIGN;

    this->tile_inld_(DualCounter<SIZE_IN_INLD, SIZE_IN_OUTLD>(), data_in,
        buffer_out, stride_in_outld, NUM_IN_OUTLD);

    for (TensorUInt idx_inld = 0; idx_inld < NUM_IN_INLD; ++idx_inld)
      for (TensorUInt idx_outld = 0; idx_outld < NUM_IN_OUTLD;
          idx_outld += MicroKernel::KN_WIDTH)
        MicroKernel::sstore(data_out + idx_inld * stride_out_inld + idx_outld,
            buffer_out + idx_inld * NUM_IN_OUTLD + idx_outld);
  }
  else
    this->tile_inld_(DualCounter<SIZE_IN_INLD, SIZE_IN_OUTLD>(), data_in,
        data_out, stride_in_outld, stride_out_inld);
}


template <typename MicroKernel,
          TensorUInt SIZE_IN_INLD,
          TensorUInt SIZE_IN_OUTLD>
template <TensorUInt IN_INLD,
         TensorUInt IN_OUTLD>
void MacroTrans<MicroKernel, SIZE_IN_INLD, SIZE_IN_OUTLD>::tile_inld_(
    DualCounter<IN_INLD, IN_OUTLD>, const typename MicroKernel::Float *data_in,
    typename MicroKernel::Float *data_out, const TensorIdx stride_in_outld,
    const TensorIdx stride_out_inld) const {
  this->tile_inld_(DualCounter<IN_INLD - 1, IN_OUTLD>(), data_in, data_out,
      stride_in_outld, stride_out_inld);
  this->tile_outld_(DualCounter<IN_INLD - 1, IN_OUTLD - 1>(), data_in, data_out,
      stride_in_outld, stride_out_inld);
}


template <typename MicroKernel,
          TensorUInt SIZE_IN_INLD,
          TensorUInt SIZE_IN_OUTLD>
template <TensorUInt IN_OUTLD>
void MacroTrans<MicroKernel, SIZE_IN_INLD, SIZE_IN_OUTLD>::tile_inld_(
    DualCounter<0, IN_OUTLD>, const typename MicroKernel::Float *data_in,
    typename MicroKernel::Float *data_out, const TensorIdx stride_in_outld,
    const TensorIdx stride_out_inld) const {
}


template <typename MicroKernel,
          TensorUInt SIZE_IN_INLD,
          TensorUInt SIZE_IN_OUTLD>
template <TensorUInt IN_INLD,
         TensorUInt IN_OUTLD>
void MacroTrans<MicroKernel, SIZE_IN_INLD, SIZE_IN_OUTLD>::tile_outld_(
    DualCounter<IN_INLD, IN_OUTLD>, const typename MicroKernel::Float *data_in,
    typename MicroKernel::Float *data_out, const TensorIdx stride_in_outld,
    const TensorIdx stride_out_inld) const {
  this->tile_outld_(DualCounter<IN_INLD, IN_OUTLD - 1>(), data_in, data_out,
      stride_in_outld, stride_out_inld);
  this->kernel_.exec(data_in + IN_INLD * this->kn_width_
          + IN_OUTLD * this->kn_width_ * stride_in_outld,
      data_out + IN_OUTLD * this->kn_width_
          + IN_INLD * this->kn_width_ * stride_out_inld,
      stride_in_outld, stride_out_inld);
}


template <typename MicroKernel,
          TensorUInt SIZE_IN_INLD,
          TensorUInt SIZE_IN_OUTLD>
template <TensorUInt IN_INLD>
void MacroTrans<MicroKernel, SIZE_IN_INLD, SIZE_IN_OUTLD>::tile_outld_(
    DualCounter<IN_INLD, 0>, const typename MicroKernel::Float *data_in,
    typename MicroKernel::Float *data_out, const TensorIdx stride_in_outld,
    const TensorIdx stride_out_inld) const {
  this->kernel_.exec(data_in + IN_INLD * this->kn_width_,
      data_out + IN_INLD * this->kn_width_ * stride_out_inld, stride_in_outld,
      stride_out_inld);
}


/*
 * Implementation for class MacroTransLinear
 */
template <typename FloatType,
          bool UPDATE_OUT>
MacroTransLinear<FloatType, UPDATE_OUT>::MacroTransLinear()
    : kernel_(),
      stride_in_inld_(1), stride_in_outld_(1), stride_out_inld_(1),
      stride_out_outld_(1), size_kn_inld_(1), size_kn_outld_(1) {
}


template <typename FloatType,
          bool UPDATE_OUT>
void MacroTransLinear<FloatType, UPDATE_OUT>::set_coef(
    const DeducedFloatType<FloatType> alpha,
    const DeducedFloatType<FloatType> beta) {
  this->kernel_.set_coef(alpha, beta);
}


template <typename FloatType,
          bool UPDATE_OUT>
void MacroTransLinear<FloatType, UPDATE_OUT>::set_wrapper_loop(
    const TensorIdx stride_in_inld, const TensorIdx stride_in_outld,
    const TensorIdx stride_out_inld, const TensorIdx stride_out_outld,
    const TensorUInt size_kn_inld, const TensorUInt size_kn_outld) {
  this->stride_in_inld_ = stride_in_inld;
  this->stride_in_outld_ = stride_in_outld;
  this->stride_out_inld_ = stride_out_inld;
  this->stride_out_outld_ = stride_out_outld;
  this->size_kn_inld_ = size_kn_inld > 0 ? size_kn_inld : 1;
  this->size_kn_outld_ = size_kn_outld > 0 ? size_kn_outld : 1;
}


template <typename FloatType,
          bool UPDATE_OUT>
void MacroTransLinear<FloatType, UPDATE_OUT>::exec(
    const FloatType * RESTRICT data_in, FloatType * RESTRICT data_out,
    const TensorIdx size_trans, const TensorIdx size_pad) const {
  for (TensorUInt out_idx = 0; out_idx < this->size_kn_outld_; ++out_idx) {
    const TensorIdx offset_in = this->stride_in_outld_ * out_idx;
    const TensorIdx offset_out = this->stride_out_outld_ * out_idx;

    for (TensorUInt in_idx = 0; in_idx < this->size_kn_inld_; ++in_idx)
      this->kernel_.exec(data_in + offset_in + this->stride_in_inld_ * in_idx,
          data_out + offset_out + this->stride_out_inld_ * in_idx,
          size_trans, 0);
  }
}


/*
 * Implementation for class MacroTransScalar
 */
template <typename FloatType,
          bool UPDATE_OUT>
void MacroTransScalar<FloatType, UPDATE_OUT>::set_coef(
    const DeducedFloatType<FloatType> alpha,
    const DeducedFloatType<FloatType> beta) {
  this->alpha_ = alpha, this->beta_ = beta;
}


template <typename FloatType,
          bool UPDATE_OUT>
void MacroTransScalar<FloatType, UPDATE_OUT>::set_wrapper_loop(
    const TensorIdx stride_in_inld, const TensorIdx stride_in_outld,
    const TensorIdx stride_out_inld, const TensorIdx stride_out_outld,
    const TensorUInt size_kn_inld, const TensorUInt size_kn_outld) {
  this->stride_in_inld_ = stride_in_inld;
  this->stride_in_outld_ = stride_in_outld;
  this->stride_out_inld_ = stride_out_inld;
  this->stride_out_outld_ = stride_out_outld;
  this->size_kn_inld_ = size_kn_inld > 0 ? size_kn_inld : 1;
  this->size_kn_outld_ = size_kn_outld > 0 ? size_kn_outld : 1;
}


template <typename FloatType,
          bool UPDATE_OUT>
void MacroTransScalar<FloatType, UPDATE_OUT>::exec(
    const FloatType * RESTRICT data_in, FloatType * RESTRICT data_out,
    const TensorIdx, const TensorIdx) const {
  if (UPDATE_OUT) {
    for (TensorUInt out_idx = 0; out_idx < this->size_kn_outld_; ++out_idx) {
      const TensorIdx offset_in = this->stride_in_outld_ * out_idx;
      const TensorIdx offset_out = this->stride_out_outld_ * out_idx;

      for (TensorUInt in_idx = 0; in_idx < this->size_kn_inld_; ++in_idx)
        *(data_out + offset_in + this->stride_out_inld_ * in_idx) = this->alpha_
            * *(data_in + offset_in + this->stride_in_inld_ * in_idx)
            + this->beta_
            * *(data_out + offset_out + this->stride_out_inld_ * in_idx);
    }
  }
  else {
    for (TensorUInt out_idx = 0; out_idx < this->size_kn_outld_; ++out_idx) {
      const TensorIdx offset_in = this->stride_in_outld_ * out_idx;
      const TensorIdx offset_out = this->stride_out_outld_ * out_idx;

      for (TensorUInt in_idx = 0; in_idx < this->size_kn_inld_; ++in_idx)
        *(data_out + offset_in + this->stride_out_inld_ * in_idx) = this->alpha_
            * *(data_in + offset_in + this->stride_in_inld_ * in_idx);
    }
  }
}


/*
 * Explicit template instantiation for class MacroTrans
 */
template class MacroTrans<KernelTransFull<float, true>, 4, 4>;
template class MacroTrans<KernelTransFull<float, true>, 4, 3>;
template class MacroTrans<KernelTransFull<float, true>, 4, 2>;
template class MacroTrans<KernelTransFull<float, true>, 4, 1>;
template class MacroTrans<KernelTransFull<float, true>, 3, 4>;
template class MacroTrans<KernelTransFull<float, true>, 3, 3>;
template class MacroTrans<KernelTransFull<float, true>, 3, 2>;
template class MacroTrans<KernelTransFull<float, true>, 3, 1>;
template class MacroTrans<KernelTransFull<float, true>, 2, 4>;
template class MacroTrans<KernelTransFull<float, true>, 2, 3>;
template class MacroTrans<KernelTransFull<float, true>, 2, 2>;
template class MacroTrans<KernelTransFull<float, true>, 2, 1>;
template class MacroTrans<KernelTransFull<float, true>, 1, 4>;
template class MacroTrans<KernelTransFull<float, true>, 1, 3>;
template class MacroTrans<KernelTransFull<float, true>, 1, 2>;
template class MacroTrans<KernelTransFull<float, true>, 1, 1>;

template class MacroTrans<KernelTransFull<double, true>, 4, 4>;
template class MacroTrans<KernelTransFull<double, true>, 4, 3>;
template class MacroTrans<KernelTransFull<double, true>, 4, 2>;
template class MacroTrans<KernelTransFull<double, true>, 4, 1>;
template class MacroTrans<KernelTransFull<double, true>, 3, 4>;
template class MacroTrans<KernelTransFull<double, true>, 3, 3>;
template class MacroTrans<KernelTransFull<double, true>, 3, 2>;
template class MacroTrans<KernelTransFull<double, true>, 3, 1>;
template class MacroTrans<KernelTransFull<double, true>, 2, 4>;
template class MacroTrans<KernelTransFull<double, true>, 2, 3>;
template class MacroTrans<KernelTransFull<double, true>, 2, 2>;
template class MacroTrans<KernelTransFull<double, true>, 2, 1>;
template class MacroTrans<KernelTransFull<double, true>, 1, 4>;
template class MacroTrans<KernelTransFull<double, true>, 1, 3>;
template class MacroTrans<KernelTransFull<double, true>, 1, 2>;
template class MacroTrans<KernelTransFull<double, true>, 1, 1>;

template class MacroTrans<KernelTransFull<FloatComplex, true>, 4, 4>;
template class MacroTrans<KernelTransFull<FloatComplex, true>, 4, 3>;
template class MacroTrans<KernelTransFull<FloatComplex, true>, 4, 2>;
template class MacroTrans<KernelTransFull<FloatComplex, true>, 4, 1>;
template class MacroTrans<KernelTransFull<FloatComplex, true>, 3, 4>;
template class MacroTrans<KernelTransFull<FloatComplex, true>, 3, 3>;
template class MacroTrans<KernelTransFull<FloatComplex, true>, 3, 2>;
template class MacroTrans<KernelTransFull<FloatComplex, true>, 3, 1>;
template class MacroTrans<KernelTransFull<FloatComplex, true>, 2, 4>;
template class MacroTrans<KernelTransFull<FloatComplex, true>, 2, 3>;
template class MacroTrans<KernelTransFull<FloatComplex, true>, 2, 2>;
template class MacroTrans<KernelTransFull<FloatComplex, true>, 2, 1>;
template class MacroTrans<KernelTransFull<FloatComplex, true>, 1, 4>;
template class MacroTrans<KernelTransFull<FloatComplex, true>, 1, 3>;
template class MacroTrans<KernelTransFull<FloatComplex, true>, 1, 2>;
template class MacroTrans<KernelTransFull<FloatComplex, true>, 1, 1>;

template class MacroTrans<KernelTransFull<DoubleComplex, true>, 4, 4>;
template class MacroTrans<KernelTransFull<DoubleComplex, true>, 4, 3>;
template class MacroTrans<KernelTransFull<DoubleComplex, true>, 4, 2>;
template class MacroTrans<KernelTransFull<DoubleComplex, true>, 4, 1>;
template class MacroTrans<KernelTransFull<DoubleComplex, true>, 3, 4>;
template class MacroTrans<KernelTransFull<DoubleComplex, true>, 3, 3>;
template class MacroTrans<KernelTransFull<DoubleComplex, true>, 3, 2>;
template class MacroTrans<KernelTransFull<DoubleComplex, true>, 3, 1>;
template class MacroTrans<KernelTransFull<DoubleComplex, true>, 2, 4>;
template class MacroTrans<KernelTransFull<DoubleComplex, true>, 2, 3>;
template class MacroTrans<KernelTransFull<DoubleComplex, true>, 2, 2>;
template class MacroTrans<KernelTransFull<DoubleComplex, true>, 2, 1>;
template class MacroTrans<KernelTransFull<DoubleComplex, true>, 1, 4>;
template class MacroTrans<KernelTransFull<DoubleComplex, true>, 1, 3>;
template class MacroTrans<KernelTransFull<DoubleComplex, true>, 1, 2>;
template class MacroTrans<KernelTransFull<DoubleComplex, true>, 1, 1>;

template class MacroTrans<KernelTransHalf<float, true>, 4, 1>;
template class MacroTrans<KernelTransHalf<float, true>, 3, 1>;
template class MacroTrans<KernelTransHalf<float, true>, 2, 1>;
template class MacroTrans<KernelTransHalf<float, true>, 1, 4>;
template class MacroTrans<KernelTransHalf<float, true>, 1, 3>;
template class MacroTrans<KernelTransHalf<float, true>, 1, 2>;
template class MacroTrans<KernelTransHalf<float, true>, 1, 1>;

template class MacroTrans<KernelTransHalf<double, true>, 4, 1>;
template class MacroTrans<KernelTransHalf<double, true>, 3, 1>;
template class MacroTrans<KernelTransHalf<double, true>, 2, 1>;
template class MacroTrans<KernelTransHalf<double, true>, 1, 4>;
template class MacroTrans<KernelTransHalf<double, true>, 1, 3>;
template class MacroTrans<KernelTransHalf<double, true>, 1, 2>;
template class MacroTrans<KernelTransHalf<double, true>, 1, 1>;

template class MacroTrans<KernelTransHalf<FloatComplex, true>, 4, 1>;
template class MacroTrans<KernelTransHalf<FloatComplex, true>, 3, 1>;
template class MacroTrans<KernelTransHalf<FloatComplex, true>, 2, 1>;
template class MacroTrans<KernelTransHalf<FloatComplex, true>, 1, 4>;
template class MacroTrans<KernelTransHalf<FloatComplex, true>, 1, 3>;
template class MacroTrans<KernelTransHalf<FloatComplex, true>, 1, 2>;
template class MacroTrans<KernelTransHalf<FloatComplex, true>, 1, 1>;

template class MacroTrans<KernelTransHalf<DoubleComplex, true>, 4, 1>;
template class MacroTrans<KernelTransHalf<DoubleComplex, true>, 3, 1>;
template class MacroTrans<KernelTransHalf<DoubleComplex, true>, 2, 1>;
template class MacroTrans<KernelTransHalf<DoubleComplex, true>, 1, 4>;
template class MacroTrans<KernelTransHalf<DoubleComplex, true>, 1, 3>;
template class MacroTrans<KernelTransHalf<DoubleComplex, true>, 1, 2>;
template class MacroTrans<KernelTransHalf<DoubleComplex, true>, 1, 1>;

template class MacroTrans<KernelTransFull<float, false>, 4, 4>;
template class MacroTrans<KernelTransFull<float, false>, 4, 3>;
template class MacroTrans<KernelTransFull<float, false>, 4, 2>;
template class MacroTrans<KernelTransFull<float, false>, 4, 1>;
template class MacroTrans<KernelTransFull<float, false>, 3, 4>;
template class MacroTrans<KernelTransFull<float, false>, 3, 3>;
template class MacroTrans<KernelTransFull<float, false>, 3, 2>;
template class MacroTrans<KernelTransFull<float, false>, 3, 1>;
template class MacroTrans<KernelTransFull<float, false>, 2, 4>;
template class MacroTrans<KernelTransFull<float, false>, 2, 3>;
template class MacroTrans<KernelTransFull<float, false>, 2, 2>;
template class MacroTrans<KernelTransFull<float, false>, 2, 1>;
template class MacroTrans<KernelTransFull<float, false>, 1, 4>;
template class MacroTrans<KernelTransFull<float, false>, 1, 3>;
template class MacroTrans<KernelTransFull<float, false>, 1, 2>;
template class MacroTrans<KernelTransFull<float, false>, 1, 1>;

template class MacroTrans<KernelTransFull<double, false>, 4, 4>;
template class MacroTrans<KernelTransFull<double, false>, 4, 3>;
template class MacroTrans<KernelTransFull<double, false>, 4, 2>;
template class MacroTrans<KernelTransFull<double, false>, 4, 1>;
template class MacroTrans<KernelTransFull<double, false>, 3, 4>;
template class MacroTrans<KernelTransFull<double, false>, 3, 3>;
template class MacroTrans<KernelTransFull<double, false>, 3, 2>;
template class MacroTrans<KernelTransFull<double, false>, 3, 1>;
template class MacroTrans<KernelTransFull<double, false>, 2, 4>;
template class MacroTrans<KernelTransFull<double, false>, 2, 3>;
template class MacroTrans<KernelTransFull<double, false>, 2, 2>;
template class MacroTrans<KernelTransFull<double, false>, 2, 1>;
template class MacroTrans<KernelTransFull<double, false>, 1, 4>;
template class MacroTrans<KernelTransFull<double, false>, 1, 3>;
template class MacroTrans<KernelTransFull<double, false>, 1, 2>;
template class MacroTrans<KernelTransFull<double, false>, 1, 1>;

template class MacroTrans<KernelTransFull<FloatComplex, false>, 4, 4>;
template class MacroTrans<KernelTransFull<FloatComplex, false>, 4, 3>;
template class MacroTrans<KernelTransFull<FloatComplex, false>, 4, 2>;
template class MacroTrans<KernelTransFull<FloatComplex, false>, 4, 1>;
template class MacroTrans<KernelTransFull<FloatComplex, false>, 3, 4>;
template class MacroTrans<KernelTransFull<FloatComplex, false>, 3, 3>;
template class MacroTrans<KernelTransFull<FloatComplex, false>, 3, 2>;
template class MacroTrans<KernelTransFull<FloatComplex, false>, 3, 1>;
template class MacroTrans<KernelTransFull<FloatComplex, false>, 2, 4>;
template class MacroTrans<KernelTransFull<FloatComplex, false>, 2, 3>;
template class MacroTrans<KernelTransFull<FloatComplex, false>, 2, 2>;
template class MacroTrans<KernelTransFull<FloatComplex, false>, 2, 1>;
template class MacroTrans<KernelTransFull<FloatComplex, false>, 1, 4>;
template class MacroTrans<KernelTransFull<FloatComplex, false>, 1, 3>;
template class MacroTrans<KernelTransFull<FloatComplex, false>, 1, 2>;
template class MacroTrans<KernelTransFull<FloatComplex, false>, 1, 1>;

template class MacroTrans<KernelTransFull<DoubleComplex, false>, 4, 4>;
template class MacroTrans<KernelTransFull<DoubleComplex, false>, 4, 3>;
template class MacroTrans<KernelTransFull<DoubleComplex, false>, 4, 2>;
template class MacroTrans<KernelTransFull<DoubleComplex, false>, 4, 1>;
template class MacroTrans<KernelTransFull<DoubleComplex, false>, 3, 4>;
template class MacroTrans<KernelTransFull<DoubleComplex, false>, 3, 3>;
template class MacroTrans<KernelTransFull<DoubleComplex, false>, 3, 2>;
template class MacroTrans<KernelTransFull<DoubleComplex, false>, 3, 1>;
template class MacroTrans<KernelTransFull<DoubleComplex, false>, 2, 4>;
template class MacroTrans<KernelTransFull<DoubleComplex, false>, 2, 3>;
template class MacroTrans<KernelTransFull<DoubleComplex, false>, 2, 2>;
template class MacroTrans<KernelTransFull<DoubleComplex, false>, 2, 1>;
template class MacroTrans<KernelTransFull<DoubleComplex, false>, 1, 4>;
template class MacroTrans<KernelTransFull<DoubleComplex, false>, 1, 3>;
template class MacroTrans<KernelTransFull<DoubleComplex, false>, 1, 2>;
template class MacroTrans<KernelTransFull<DoubleComplex, false>, 1, 1>;

template class MacroTrans<KernelTransHalf<float, false>, 4, 1>;
template class MacroTrans<KernelTransHalf<float, false>, 3, 1>;
template class MacroTrans<KernelTransHalf<float, false>, 2, 1>;
template class MacroTrans<KernelTransHalf<float, false>, 1, 4>;
template class MacroTrans<KernelTransHalf<float, false>, 1, 3>;
template class MacroTrans<KernelTransHalf<float, false>, 1, 2>;
template class MacroTrans<KernelTransHalf<float, false>, 1, 1>;

template class MacroTrans<KernelTransHalf<double, false>, 4, 1>;
template class MacroTrans<KernelTransHalf<double, false>, 3, 1>;
template class MacroTrans<KernelTransHalf<double, false>, 2, 1>;
template class MacroTrans<KernelTransHalf<double, false>, 1, 4>;
template class MacroTrans<KernelTransHalf<double, false>, 1, 3>;
template class MacroTrans<KernelTransHalf<double, false>, 1, 2>;
template class MacroTrans<KernelTransHalf<double, false>, 1, 1>;

template class MacroTrans<KernelTransHalf<FloatComplex, false>, 4, 1>;
template class MacroTrans<KernelTransHalf<FloatComplex, false>, 3, 1>;
template class MacroTrans<KernelTransHalf<FloatComplex, false>, 2, 1>;
template class MacroTrans<KernelTransHalf<FloatComplex, false>, 1, 4>;
template class MacroTrans<KernelTransHalf<FloatComplex, false>, 1, 3>;
template class MacroTrans<KernelTransHalf<FloatComplex, false>, 1, 2>;
template class MacroTrans<KernelTransHalf<FloatComplex, false>, 1, 1>;

template class MacroTrans<KernelTransHalf<DoubleComplex, false>, 4, 1>;
template class MacroTrans<KernelTransHalf<DoubleComplex, false>, 3, 1>;
template class MacroTrans<KernelTransHalf<DoubleComplex, false>, 2, 1>;
template class MacroTrans<KernelTransHalf<DoubleComplex, false>, 1, 4>;
template class MacroTrans<KernelTransHalf<DoubleComplex, false>, 1, 3>;
template class MacroTrans<KernelTransHalf<DoubleComplex, false>, 1, 2>;
template class MacroTrans<KernelTransHalf<DoubleComplex, false>, 1, 1>;


/*
 * Explicit template instantiation for class MacroTransLinear
 */
template class MacroTransLinear<float, true>;
template class MacroTransLinear<double, true>;
template class MacroTransLinear<FloatComplex, true>;
template class MacroTransLinear<DoubleComplex, true>;

template class MacroTransLinear<float, false>;
template class MacroTransLinear<double, false>;
template class MacroTransLinear<FloatComplex, false>;
template class MacroTransLinear<DoubleComplex, false>;


/*
 * Explicit template instantiation for class MacroTransScalar
 */
template class MacroTransScalar<float, true>;
template class MacroTransScalar<double, true>;
template class MacroTransScalar<FloatComplex, true>;
template class MacroTransScalar<DoubleComplex, true>;

template class MacroTransScalar<float, false>;
template class MacroTransScalar<double, false>;
template class MacroTransScalar<FloatComplex, false>;
template class MacroTransScalar<DoubleComplex, false>;

}
