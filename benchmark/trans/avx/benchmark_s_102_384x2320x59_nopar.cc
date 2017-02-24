#include <hptc/config/config_trans.h>
#include <hptc/benchmark/benchmark_trans_avx.h>
#include <hptc/perf-test/test_perf_util.h>

using namespace hptc;


int main() {
  auto ref_func = [] (const float *input_data, float *output_data) {
      sTranspose_102_384x2320x59<384, 2320, 59>(
          input_data, output_data, ALPHA, BETA, nullptr, nullptr);
  };
  auto &ref_config = ref_trans_configs[8];

  compare_perf<float, decltype(ref_func), CoefUsageTrans::USE_BOTH, 3>(ref_func,
      ref_config);

  return 0;
}