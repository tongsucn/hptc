#pragma once
#ifndef HPTT_ARCH_IBM_KERNEL_TRANS_IBM_TCC_
#define HPTT_ARCH_IBM_KERNEL_TRANS_IBM_TCC_

/*
 * Intrinsics wrappers
 */
template <typename FloatType,
          KernelTypeTrans TYPE,
          typename Selected = void>
struct IntrinImpl {
};

template <typename FloatType,
          KernelTypeTrans TYPE>
struct IntrinImpl<FloatType, TYPE,
    Enable<TypeSelector<FloatType, TYPE>::fhl_sc or
        TypeSelector<FloatType, TYPE>::fhl_dz>> {
  using Deduced = DeducedFloatType<FloatType>;
  using Reg = RegType<FloatType, TYPE>;
  constexpr TensorUInt REG_CAP = 1;

  static HPTT_INL Reg<FloatType, TYPE> set_reg(const Deduced coef) {
    return coef;
  }
};

template <typename FloatType,
          KernelTypeTrans TYPE>
struct IntrinImpl<FloatType, TYPE,
    Enable<TypeSelector<FloatType, TYPE>::f_d>> {
  using Deduced = DeducedFloatType<FloatType>;
  using Reg = RegType<FloatType, TYPE>;
  constexpr TensorUInt REG_CAP = SIZE_REG / sizeof(FloatType);

  static constexpr Reg rule[4] = {
    static_cast<vector4double>({ 2.0, 3.0, 2.5, 3.5 });
    static_cast<vector4double>({ 2.25, 3.25, 2.75, 3.75 });
    static_cast<vector4double>({ 2.0, 2.25, 3.0, 3.25 });
    static_cast<vector4double>({ 2.5, 2.75, 3.5, 3.75 });
  };

  static HPTT_INL Reg set_reg(const Deduced coef) {
    return static_cast<Reg>({ coef, coef, coef, coef });
  }
  static HPTT_INL Reg load(const FloatType * RESTRICT target) {
    return vec_lda(0,
        const_cast<Deduced *>(reinterpret_cast<const Deduced *>(target)));
  }
  static HPTT_INL void store(FloatType * RESTRICT target, const Reg &reg) {
    vec_sta(reg, 0, reinterpret_cas<Deduced *>(target));
  }
  static HPTT_INL Reg madd(const Reg &reg_out, const Reg &reg_coef,
      const Reg &reg_in) {
    return vec_madd(reg_out, reg_coef, reg_in);
  }
  static HPTT_INL Reg mul(const Reg &reg_a, const Reg &reg_b) {
    return vec_mul(reg_a, reg_b);
  }
};


/*
 * Implementation of class KernelTransData
 */
template <typename FloatType,
          KernelTypeTrans TYPE>
KernelTransData<FloatType, TYPE>::KernelTransData()
    : reg_alpha_(), reg_beta_(), alpha_(), beta_() {
}


template <typename FloatType,
          KernelTypeTrans TYPE>
void KernelTransData<FloatType, TYPE>::sstore(FloatType *data_out,
    const FloatType *buffer) {
}


template <typename FloatType,
          KernelTypeTrans TYPE>
bool KernelTransData<FloatType, TYPE>::check_stream(TensorUInt) {
  return false;
}


template <typename FloatType,
          KernelTypeTrans TYPE>
void KernelTransData<FloatType, TYPE>::set_coef(
    const DeducedFloatType<FloatType> alpha,
    const DeducedFloatType<FloatType> beta) {
  this->alpha_ = alpha, this->beta_ = beta;
  this->reg_alpha_ = IntrinImpl<FloatType, TYPE>::set_reg(this->alpha_);
  this->reg_beta_ = IntrinImpl<FloatType, TYPE>::set_reg(this->beta_);
}


/*
 * Specialization of class KernelTrans
 */
template <bool UPDATE_OUT>
class KernelTrans<double, KernelTypeTrans::KERNEL_FULL, UPDATE_OUT>
    : public KernelTransData<double, KernelTypeTrans::KERNEL_FULL> {
public:
  KernelTrans();

  void exec(const double * RESTRICT data_in, double * RESTRICT data_out,
      const TensorIdx stride_in_outld, const TensorIdx stride_out_inld) const;
};


/*
 * Specialization of class KernelTrans, linear kernel, used for common leading
 */
template <typename FloatType,
          bool UPDATE_OUT>
class KernelTrans<FloatType, KernelTypeTrans::KERNEL_LINE, UPDATE_OUT>
    : public KernelTransData<FloatType, KernelTypeTrans::KERNEL_LINE> {
public:
  KernelTrans();

  void exec(const FloatType * RESTRICT data_in, FloatType * RESTRICT data_out,
      const TensorIdx size_trans, const TensorIdx) const;
};


/*
 * Explicit template instantiation declaration for class KernelTrans
 */
extern template class KernelTrans<float, KernelTypeTrans::KERNEL_FULL, true>;
extern template class KernelTrans<double, KernelTypeTrans::KERNEL_FULL, true>;
extern template class KernelTrans<FloatComplex, KernelTypeTrans::KERNEL_FULL,
    true>;
extern template class KernelTrans<DoubleComplex, KernelTypeTrans::KERNEL_FULL,
    true>;

extern template class KernelTrans<float, KernelTypeTrans::KERNEL_HALF, true>;
extern template class KernelTrans<double, KernelTypeTrans::KERNEL_HALF, true>;
extern template class KernelTrans<FloatComplex, KernelTypeTrans::KERNEL_HALF,
    true>;
extern template class KernelTrans<DoubleComplex, KernelTypeTrans::KERNEL_HALF,
    true>;

extern template class KernelTrans<float, KernelTypeTrans::KERNEL_LINE, true>;
extern template class KernelTrans<double, KernelTypeTrans::KERNEL_LINE, true>;
extern template class KernelTrans<FloatComplex, KernelTypeTrans::KERNEL_LINE,
    true>;
extern template class KernelTrans<DoubleComplex, KernelTypeTrans::KERNEL_LINE,
    true>;

extern template class KernelTrans<float, KernelTypeTrans::KERNEL_FULL, false>;
extern template class KernelTrans<double, KernelTypeTrans::KERNEL_FULL, false>;
extern template class KernelTrans<FloatComplex, KernelTypeTrans::KERNEL_FULL,
    false>;
extern template class KernelTrans<DoubleComplex, KernelTypeTrans::KERNEL_FULL,
    false>;

extern template class KernelTrans<float, KernelTypeTrans::KERNEL_HALF, false>;
extern template class KernelTrans<double, KernelTypeTrans::KERNEL_HALF, false>;
extern template class KernelTrans<FloatComplex, KernelTypeTrans::KERNEL_HALF,
    false>;
extern template class KernelTrans<DoubleComplex, KernelTypeTrans::KERNEL_HALF,
    false>;

extern template class KernelTrans<float, KernelTypeTrans::KERNEL_LINE, false>;
extern template class KernelTrans<double, KernelTypeTrans::KERNEL_LINE, false>;
extern template class KernelTrans<FloatComplex, KernelTypeTrans::KERNEL_LINE,
    false>;
extern template class KernelTrans<DoubleComplex, KernelTypeTrans::KERNEL_LINE,
    false>;

#endif // HPTT_ARCH_IBM_KERNEL_TRANS_IBM_TCC_
