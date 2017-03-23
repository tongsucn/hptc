#pragma once
#ifndef HPTC_BENCHMARK_BENCHMARK_TRANS_TCC_
#define HPTC_BENCHMARK_BENCHMARK_TRANS_TCC_

RefTransConfig::RefTransConfig(TensorUInt order, TensorUInt thread_num,
    const std::vector<TensorUInt> &perm, const std::vector<TensorIdx> &size)
    : order(order),
      thread_num(thread_num),
      perm(perm),
      size(size) {
}

#endif // HPTC_BENCHMARK_BENCHMARK_TRANS_TCC_
