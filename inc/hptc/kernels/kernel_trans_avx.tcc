#pragma once
#ifndef HPTC_KERNEL_KERNEL_TRANS_AVX_TCC_
#define HPTC_KERNEL_KERNEL_TRANS_AVX_TCC_


template <typename FloatType,
          uint32_t HEIGHT,
          uint32_t WIDTH>
inline void kernel_trans(std::shared_ptr<ParamTrans<FloatType>> &param) {
  const FloatType *input_data = &param->input_tensor[param->macro_loop_idx];
  FloatType *output_data = &param->output_tensor[param->macro_loop_idx];

  using DecayType = CoefficientType<FloatType>;
  const DecayType *decay_input
    = reinterpret_cast<const DecayType *>(input_data);
  DecayType *decay_output = reinterpret_cast<DecayType *>(output_data);

  *decay_output = *decay_input * param->alpha + *decay_output * param->beta;
  if (sizeof(FloatType) > sizeof(DecayType))
    *(decay_output + 1)
      = *(decay_input + 1) * param->alpha + *(decay_output + 1) * param->beta;
}


template <>
inline void kernel_trans<float, 0, 0>(
    std::shared_ptr<ParamTrans<float>> &param) {
  // Preparation
  const float *input_data = &param->input_tensor[param->macro_loop_idx];
  float *output_data = &param->output_tensor[param->macro_loop_idx];
  __m256 r_alpha = _mm256_set1_ps(param->alpha);
  __m256 r_beta = _mm256_set1_ps(param->beta);

  // Load input data into register
  __m256 r_input_row_0 = _mm256_load_ps(input_data);
  __m256 r_input_row_1 = _mm256_load_ps(input_data + param->input_offset);
  __m256 r_input_row_2 = _mm256_load_ps(input_data + 2 * param->input_offset);
  __m256 r_input_row_3 = _mm256_load_ps(input_data + 3 * param->input_offset);
  __m256 r_input_row_4 = _mm256_load_ps(input_data + 4 * param->input_offset);
  __m256 r_input_row_5 = _mm256_load_ps(input_data + 5 * param->input_offset);
  __m256 r_input_row_6 = _mm256_load_ps(input_data + 6 * param->input_offset);
  __m256 r_input_row_7 = _mm256_load_ps(input_data + 7 * param->input_offset);

  // Transpose Micro kernel
  __m256 r_unpklo_01 = _mm256_unpacklo_ps(r_input_row_0, r_input_row_1);
  __m256 r_unpkhi_01 = _mm256_unpacklo_ps(r_input_row_0, r_input_row_1);
  __m256 r_unpklo_23 = _mm256_unpacklo_ps(r_input_row_2, r_input_row_3);
  __m256 r_unpkhi_23 = _mm256_unpacklo_ps(r_input_row_2, r_input_row_3);
  __m256 r_unpklo_45 = _mm256_unpacklo_ps(r_input_row_4, r_input_row_5);
  __m256 r_unpkhi_45 = _mm256_unpacklo_ps(r_input_row_4, r_input_row_5);
  __m256 r_unpklo_67 = _mm256_unpacklo_ps(r_input_row_6, r_input_row_7);
  __m256 r_unpkhi_67 = _mm256_unpacklo_ps(r_input_row_6, r_input_row_7);

  __m256 r_shflo44_0123 = _mm256_shuffle_ps(r_unpklo_01, r_unpklo_23, 0x44);
  __m256 r_shfloee_0123 = _mm256_shuffle_ps(r_unpklo_01, r_unpklo_23, 0xEE);
  __m256 r_shfhi44_0123 = _mm256_shuffle_ps(r_unpkhi_01, r_unpkhi_23, 0x44);
  __m256 r_shfhiee_0123 = _mm256_shuffle_ps(r_unpkhi_01, r_unpkhi_23, 0xEE);
  __m256 r_shflo44_4567 = _mm256_shuffle_ps(r_unpklo_45, r_unpklo_67, 0x44);
  __m256 r_shfloee_4567 = _mm256_shuffle_ps(r_unpklo_45, r_unpklo_67, 0xEE);
  __m256 r_shfhi44_4567 = _mm256_shuffle_ps(r_unpkhi_45, r_unpkhi_67, 0x44);
  __m256 r_shfhiee_4567 = _mm256_shuffle_ps(r_unpkhi_45, r_unpkhi_67, 0xEE);

  r_input_row_0 = _mm256_permute2f128_ps(r_shflo44_4567, r_shflo44_0123, 0x2);
  r_input_row_1 = _mm256_permute2f128_ps(r_shfloee_4567, r_shfloee_0123, 0x2);
  r_input_row_2 = _mm256_permute2f128_ps(r_shfhi44_4567, r_shfhi44_0123, 0x2);
  r_input_row_3 = _mm256_permute2f128_ps(r_shfhiee_4567, r_shfhiee_0123, 0x2);
  r_input_row_4 = _mm256_permute2f128_ps(r_shflo44_4567, r_shflo44_0123, 0x13);
  r_input_row_5 = _mm256_permute2f128_ps(r_shfloee_4567, r_shfloee_0123, 0x13);
  r_input_row_6 = _mm256_permute2f128_ps(r_shfhi44_4567, r_shfhi44_0123, 0x13);
  r_input_row_7 = _mm256_permute2f128_ps(r_shfhiee_4567, r_shfhiee_0123, 0x13);

  // Rescale input data
  r_input_row_0 = _mm256_mul_ps(r_input_row_0, r_alpha);
  r_input_row_1 = _mm256_mul_ps(r_input_row_1, r_alpha);
  r_input_row_2 = _mm256_mul_ps(r_input_row_2, r_alpha);
  r_input_row_3 = _mm256_mul_ps(r_input_row_3, r_alpha);
  r_input_row_4 = _mm256_mul_ps(r_input_row_4, r_alpha);
  r_input_row_5 = _mm256_mul_ps(r_input_row_5, r_alpha);
  r_input_row_6 = _mm256_mul_ps(r_input_row_6, r_alpha);
  r_input_row_7 = _mm256_mul_ps(r_input_row_7, r_alpha);

  // Load output data into register
  __m256 r_output_row_0 = _mm256_load_ps(output_data);
  __m256 r_output_row_1 = _mm256_load_ps(output_data + param->output_offset);
  __m256 r_output_row_2 = _mm256_load_ps(
      output_data + 2 * param->output_offset);
  __m256 r_output_row_3 = _mm256_load_ps(
      output_data + 3 * param->output_offset);
  __m256 r_output_row_4 = _mm256_load_ps(
      output_data + 4 * param->output_offset);
  __m256 r_output_row_5 = _mm256_load_ps(
      output_data + 5 * param->output_offset);
  __m256 r_output_row_6 = _mm256_load_ps(
      output_data + 6 * param->output_offset);
  __m256 r_output_row_7 = _mm256_load_ps(
      output_data + 7 * param->output_offset);

  // Rescale output data
  r_output_row_0 = _mm256_mul_ps(r_output_row_0, r_beta);
  r_output_row_1 = _mm256_mul_ps(r_output_row_1, r_beta);
  r_output_row_2 = _mm256_mul_ps(r_output_row_2, r_beta);
  r_output_row_3 = _mm256_mul_ps(r_output_row_3, r_beta);
  r_output_row_4 = _mm256_mul_ps(r_output_row_4, r_beta);
  r_output_row_5 = _mm256_mul_ps(r_output_row_5, r_beta);
  r_output_row_6 = _mm256_mul_ps(r_output_row_6, r_beta);
  r_output_row_7 = _mm256_mul_ps(r_output_row_7, r_beta);

  // Add input and output data
  r_output_row_0 = _mm256_add_ps(r_output_row_0, r_input_row_0);
  r_output_row_0 = _mm256_add_ps(r_output_row_0, r_input_row_0);
  r_output_row_0 = _mm256_add_ps(r_output_row_0, r_input_row_0);
  r_output_row_0 = _mm256_add_ps(r_output_row_0, r_input_row_0);
  r_output_row_0 = _mm256_add_ps(r_output_row_0, r_input_row_0);
  r_output_row_0 = _mm256_add_ps(r_output_row_0, r_input_row_0);
  r_output_row_0 = _mm256_add_ps(r_output_row_0, r_input_row_0);
  r_output_row_0 = _mm256_add_ps(r_output_row_0, r_input_row_0);

  // Store output data back to memory
  _mm256_store_ps(output_data, r_output_row_0);
  _mm256_store_ps(output_data + param->output_offset, r_output_row_1);
  _mm256_store_ps(output_data + 2 * param->output_offset, r_output_row_2);
  _mm256_store_ps(output_data + 3 * param->output_offset, r_output_row_3);
  _mm256_store_ps(output_data + 4 * param->output_offset, r_output_row_4);
  _mm256_store_ps(output_data + 5 * param->output_offset, r_output_row_5);
  _mm256_store_ps(output_data + 6 * param->output_offset, r_output_row_6);
  _mm256_store_ps(output_data + 7 * param->output_offset, r_output_row_7);
}


template <>
inline void kernel_trans<double, 0, 0>(
    std::shared_ptr<ParamTrans<double>> &param) {
  // Preparation
  const double *input_data = &param->input_tensor[param->macro_loop_idx];
  double *output_data = &param->output_tensor[param->macro_loop_idx];
  __m256d r_alpha = _mm256_set1_pd(param->alpha);
  __m256d r_beta = _mm256_set1_pd(param->beta);

  // Load input data into register
  __m256d r_input_row_0 = _mm256_load_pd(input_data);
  __m256d r_input_row_1 = _mm256_load_pd(input_data + param->input_offset);
  __m256d r_input_row_2 = _mm256_load_pd(input_data + 2 * param->input_offset);
  __m256d r_input_row_3 = _mm256_load_pd(input_data + 3 * param->input_offset);

  // Transpose micro kernel
  __m256d r_shf3_01 = _mm256_shuffle_pd(r_input_row_0, r_input_row_1, 0x3);
  __m256d r_shfc_01 = _mm256_shuffle_pd(r_input_row_0, r_input_row_1, 0xc);
  __m256d r_shf3_23 = _mm256_shuffle_pd(r_input_row_2, r_input_row_3, 0x3);
  __m256d r_shfc_23 = _mm256_shuffle_pd(r_input_row_2, r_input_row_3, 0xc);

  r_input_row_0 = _mm256_permute2f128_pd(r_shfc_23, r_shfc_01, 0x2);
  r_input_row_1 = _mm256_permute2f128_pd(r_shf3_23, r_shf3_01, 0x2);
  r_input_row_2 = _mm256_permute2f128_pd(r_shf3_23, r_shf3_01, 0x13);
  r_input_row_3 = _mm256_permute2f128_pd(r_shfc_23, r_shfc_01, 0x13);

  // Rescale input data
  r_input_row_0 = _mm256_mul_pd(r_input_row_0, r_alpha);
  r_input_row_1 = _mm256_mul_pd(r_input_row_1, r_alpha);
  r_input_row_2 = _mm256_mul_pd(r_input_row_2, r_alpha);
  r_input_row_3 = _mm256_mul_pd(r_input_row_3, r_alpha);

  // Load output data into register
  __m256d r_output_row_0, r_output_row_1, r_output_row_2, r_output_row_3;
  r_output_row_0 = _mm256_load_pd(output_data);
  r_output_row_1 = _mm256_load_pd(output_data + param->output_offset);
  r_output_row_2 = _mm256_load_pd(output_data + 2 * param->output_offset);
  r_output_row_3 = _mm256_load_pd(output_data + 3 * param->output_offset);

  // Rescale output tensor
  r_output_row_0 = _mm256_mul_pd(r_output_row_0, r_beta);
  r_output_row_1 = _mm256_mul_pd(r_output_row_1, r_beta);
  r_output_row_2 = _mm256_mul_pd(r_output_row_2, r_beta);
  r_output_row_3 = _mm256_mul_pd(r_output_row_3, r_beta);

  // Add input and output data
  r_output_row_0 = _mm256_add_pd(r_output_row_0, r_input_row_0);
  r_output_row_1 = _mm256_add_pd(r_output_row_0, r_input_row_1);
  r_output_row_2 = _mm256_add_pd(r_output_row_0, r_input_row_2);
  r_output_row_3 = _mm256_add_pd(r_output_row_0, r_input_row_3);

  // Store output data back to memory
  _mm256_store_pd(output_data, r_output_row_0);
  _mm256_store_pd(output_data + param->output_offset, r_output_row_1);
  _mm256_store_pd(output_data + 2 * param->output_offset, r_output_row_2);
  _mm256_store_pd(output_data + 3 * param->output_offset, r_output_row_3);
}


template <>
inline void kernel_trans<FloatComplex, 0, 0>(
    std::shared_ptr<ParamTrans<FloatComplex>> &param) {
  // Preparation
  const FloatComplex *input_data = &param->input_tensor[param->macro_loop_idx];
  FloatComplex *output_data = &param->output_tensor[param->macro_loop_idx];
  __m256 r_alpha = _mm256_set1_ps(param->alpha);
  __m256 r_beta = _mm256_set1_ps(param->beta);

  // Load input data into register
  __m256 input_row_0 = _mm256_load_ps(
      reinterpret_cast<const float *>(input_data));
  __m256 input_row_1 = _mm256_load_ps(
      reinterpret_cast<const float *>(input_data + param->input_offset));
  __m256 input_row_2 = _mm256_load_ps(
      reinterpret_cast<const float *>(input_data + 2 * param->input_offset));
  __m256 input_row_3 = _mm256_load_ps(
      reinterpret_cast<const float *>(input_data + 3 * param->input_offset));

  // Transpose micro kernel
  __m256 r_shf44_23 = _mm256_shuffle_ps(input_row_2, input_row_3, 0x44);
  __m256 r_shf44_01 = _mm256_shuffle_ps(input_row_0, input_row_1, 0x44);
  __m256 r_shfee_23 = _mm256_shuffle_ps(input_row_2, input_row_3, 0xee);
  __m256 r_shfee_01 = _mm256_shuffle_ps(input_row_0, input_row_1, 0xee);

  input_row_0 = _mm256_permute2f128_ps(r_shf44_23, r_shf44_01, 0x2);
  input_row_1 = _mm256_permute2f128_ps(r_shfee_23, r_shfee_01, 0x2);
  input_row_2 = _mm256_permute2f128_ps(r_shf44_23, r_shf44_01, 0x13);
  input_row_3 = _mm256_permute2f128_ps(r_shfee_23, r_shfee_01, 0x13);

  // Rescale input data
  input_row_0 = _mm256_mul_ps(input_row_0, r_alpha);
  input_row_1 = _mm256_mul_ps(input_row_1, r_alpha);
  input_row_2 = _mm256_mul_ps(input_row_2, r_alpha);
  input_row_3 = _mm256_mul_ps(input_row_3, r_alpha);

  // Load output data into register
  __m256 output_row_0 = _mm256_load_ps(
      reinterpret_cast<float *>(output_data));
  __m256 output_row_1 = _mm256_load_ps(
      reinterpret_cast<float *>(output_data + param->input_offset));
  __m256 output_row_2 = _mm256_load_ps(
      reinterpret_cast<float *>(output_data + 2 * param->input_offset));
  __m256 output_row_3 = _mm256_load_ps(
      reinterpret_cast<float *>(output_data + 3 * param->input_offset));

  // Rescale output data
  output_row_0 = _mm256_mul_ps(output_row_0, r_beta);
  output_row_1 = _mm256_mul_ps(output_row_1, r_beta);
  output_row_2 = _mm256_mul_ps(output_row_2, r_beta);
  output_row_3 = _mm256_mul_ps(output_row_3, r_beta);

  // Add input and output data
  output_row_0 = _mm256_add_ps(output_row_0, input_row_0);
  output_row_1 = _mm256_add_ps(output_row_1, input_row_1);
  output_row_2 = _mm256_add_ps(output_row_2, input_row_2);
  output_row_3 = _mm256_add_ps(output_row_3, input_row_3);

  // Store output data back to memory
  _mm256_store_ps(reinterpret_cast<float *>(output_data), output_row_0);
  _mm256_store_ps(reinterpret_cast<float *>(output_data + param->input_offset),
      output_row_1);
  _mm256_store_ps(
      reinterpret_cast<float *>(output_data + 2 * param->input_offset),
      output_row_2);
  _mm256_store_ps(
      reinterpret_cast<float *>(output_data + 3 * param->input_offset),
      output_row_3);
}


template <>
inline void kernel_trans<DoubleComplex, 0, 0>(
    std::shared_ptr<ParamTrans<DoubleComplex>> &param) {
  // Preparation
  const DoubleComplex *input_data = &param->input_tensor[param->macro_loop_idx];
  DoubleComplex *output_data = &param->output_tensor[param->macro_loop_idx];
  __m256d r_alpha = _mm256_set1_pd(param->alpha);
  __m256d r_beta = _mm256_set1_pd(param->beta);

  // Load input data into register
  __m256d input_row_0 = _mm256_load_pd(
      reinterpret_cast<const double *>(input_data));
  __m256d input_row_1 = _mm256_load_pd(
      reinterpret_cast<const double *>(input_data + param->input_offset));

  // Transpose micro kernel
  __m256d r_perm02_10 = _mm256_permute2f128_pd(input_row_1, input_row_0, 0x2);
  __m256d r_perm13_10 = _mm256_permute2f128_pd(input_row_1, input_row_0, 0x13);

  input_row_1 = r_perm13_10;
  input_row_0 = r_perm02_10;

  // Rescale input data
  input_row_0 = _mm256_mul_pd(input_row_0, r_alpha);
  input_row_1 = _mm256_mul_pd(input_row_1, r_alpha);

  // Load output data into register
  __m256d output_row_0 = _mm256_load_pd(
      reinterpret_cast<double *>(output_data));
  __m256d output_row_1 = _mm256_load_pd(
      reinterpret_cast<double *>(output_data + param->input_offset));

  // Rescale putput data
  output_row_0 = _mm256_mul_pd(output_row_0, r_beta);
  output_row_1 = _mm256_mul_pd(output_row_1, r_beta);

  // Add input and output data
  output_row_0 = _mm256_add_pd(output_row_0, input_row_0);
  output_row_1 = _mm256_add_pd(output_row_1, input_row_1);

  // Store output data back to memory
  _mm256_store_pd(reinterpret_cast<double *>(output_data), output_row_0);
  _mm256_store_pd(reinterpret_cast<double *>(output_data + param->input_offset),
      output_row_1);
}

#endif // HPTC_KERNEL_KERNEL_TRANS_AVX_TCC_