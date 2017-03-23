#pragma once
#ifndef HPTC_HPTC_TRANS_H_
#define HPTC_HPTC_TRANS_H_

#include <array>
#include <vector>
#include <memory>
#include <numeric>
#include <functional>

#include <hptc/types.h>
#include <hptc/tensor.h>
#include <hptc/compat.h>
#include <hptc/util/util_trans.h>
#include <hptc/plan/plan_trans.h>
#include <hptc/cgraph/cgraph_trans.h>
#include <hptc/param/parameter_trans.h>


namespace hptc {

// Type alias for describing tensor outer size and sub tensor's offset
template <TensorOrder ORDER>
using OuterSize = std::pair<std::vector<TensorOrder>,
      std::array<TensorOrder, ORDER>>;

/**
 * \brief Function for creating a tensor transpose.
 *
 * \details This function is used to create computational a graph for
 *     transposing a tensor. For the flexibility, the transpose has following
 *     form:
 *
 * $B_{perm(i1,i2,..., iN)} = \alpha \cdot A_{i_1,i_2,..., i_N} +
 *     \beta \cdot B_{perm(i_1,i_2,..., i_N)}$
 *
 * where A is an input tensor, B is the output destination. There are two
 * template parameters serving for this function: The transpose strategy is
 * either based on static rule, or from heuristics and auto-tuning. See more
 * from the function parameters.
 *
 * - FloatType is the data type, four floating types are support: single float
 *   (float), double float (double), single complex (FloatComplex), double
 *   complex (DoubleComplex).
 * - ORDER is the tensor's order, it must be larger than 1.
 *
 * \param[in] in_data Raw input tensor data, the tensor to be transposed;
 * \param[in,out] out_data Raw output tensor data, the destination for storing
 *     the transpose result.
 * \param[in] in_size Vector used for describing size of each order in input
 *     tensor.
 * \param[in] perm Vector used for describing permutation (e.g., perm={1,0}
 *     denotes a matrix transpose).
 * \param[in] alpha Coefficient for scaling all the elements in input tensor.
 * \param[in] beta Coefficient for updating output tensor.
 * \param[in] thread_num Thread number for transpose, unsigned integer. When
 *     setting to 0, the OpenMP's OMP_NUM_THREADS will be used.
 * \param[in] in_outer_size An std::pair for describing input tensor outer size,
 *     First is the outer size vector, second is offset for each order. Optional
 *     parameter, default's first is an empty std::initializer_list;
 * \param[in] out_outer_size An std::pair for describing output tensor outer
 *     size, First is the outer size vector, second is offset for each order.
 *     Optional parameter, default's first is an empty std::initializer_list;
 * \param[in] max_num_cand Number of candidate implementations that should be
 *     evaluated; this is an optional parameter (default value is 0). Its
 *     input value could be:
 *     - max_num_cand < 0: Tune all generated transpose candidate. Caution:
 *       although tuning all possible results could deliver best transpose
 *       performance, it will cost plenty of time before getting the solution as
 *       well, especially when ORDER $n$ is large. The number of candidates
 *       increases with $n!$.
 *     - max_num_cand == 0: No tuning will be performed and a suitable candidate
 *       according to the performance model will be selected automatically.
 *     - max_num_cand > 0: max_num_cand candidates will be measured and
 *       compared.
 *
 * \return A pointer to the computational graph for this transpose. It can be
 *     used like this:
 * \code{.cpp}
 *   auto graph = create_cgraph_trans(...);  // Create graph
 *   if (nullptr != graph)
 *     graph->exec();   // call cgraph's member function exec()
 *   // ...
 *   delete graph;
 *   graph = nullptr;
 * \endcode
 *
 * The returned pointer will be null, when:
 * 1. template argument ORDER is less than or equal to 1.
 * 2. at least one of the two data pointers in function parameter list is null.
 * 3. the content of input permutation array is not valid.
 * 4. at least one of the tensor size vectors is incorrect, i.e. in_size's
 *    size is not ORDER; in_outer_size's or out_outer_size's size is neither 0
 *    nor ORDER.
 * 5. at least one of the tensor size vectors has incorrect value, i.e.
 *    zero-value or the outer size is less than the inner size.
 * 6. any offset + inner size is larger than the outer size.
 *
 * The computational graph must be released by user to avoid memory leak.
 *
 * \sa CGraphTrans
 */
template <typename FloatType,
          TensorOrder ORDER>
CGraphTrans<ParamTrans<TensorWrapper<FloatType, ORDER>>> *
create_cgraph_trans(const FloatType *in_data, FloatType *out_data,
    const std::vector<TensorOrder> &in_size,
    const std::array<TensorOrder, ORDER> &perm,
    const DeducedFloatType<FloatType> alpha,
    const DeducedFloatType<FloatType> beta, const GenNumType thread_num,
    OuterSize<ORDER> in_outer_size
        = OuterSize<ORDER>({}, std::array<TensorOrder, ORDER>()),
    OuterSize<ORDER> out_outer_size
        = OuterSize<ORDER>({}, std::array<TensorOrder, ORDER>()),
    TensorIdx max_num_cand = 0);


/*
 * Import implementations
 */
#include "hptc_trans.tcc"

}

#endif // HPTC_HPTC_TRANS_H_
