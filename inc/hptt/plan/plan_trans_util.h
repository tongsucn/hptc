#pragma once
#ifndef HPTT_PLAN_PLAN_TRANS_UTIL_H_
#define HPTT_PLAN_PLAN_TRANS_UTIL_H_

#include <cmath>

#include <vector>
#include <stack>
#include <queue>
#include <memory>
#include <utility>
#include <numeric>
#include <algorithm>
#include <functional>
#include <unordered_set>

#include <omp.h>

#include <hptt/types.h>
#include <hptt/util/util.h>
#include <hptt/util/util_trans.h>


namespace hptt {

/*
 * Forward declaration
 */
template <typename ParamType>
class CGraphTrans;


/*
 * Definition for class PlanTransOptimizer
 */
template <typename ParamType>
class PlanTransOptimizer {
public:
  using Descriptor = typename CGraphTrans<ParamType>::Descriptor;
  static constexpr auto ORDER = ParamType::ORDER;

  PlanTransOptimizer(const std::shared_ptr<ParamType> &param,
      const TensorUInt num_threads, const TensorInt tune_loop_num,
      const TensorInt tune_para_num, const TensorInt heur_loop_num,
      const TensorInt heur_para_num);

  std::vector<Descriptor> get_optimal() const;

private:
  struct LoopParaStrategy_ {
    LoopParaStrategy_(TensorIdx size, TensorUInt th_num, TensorUInt loop_idx)
        : size(size), th_num(th_num), loop_idx(loop_idx) {}
    TensorIdx size;
    TensorUInt th_num;
    TensorUInt loop_idx;
  };

  void init_(TensorInt tune_loop_num, TensorInt tune_para_num,
      TensorInt heur_loop_num, TensorInt heur_para_num);
  void init_config_();
  void init_loop_evaluator_param_();
  void init_parallel_evaluator_param_();

  void init_loop_rule_();
  void init_loop_heur_common_leading_(const TensorInt tune_num,
      const TensorInt heur_num);
  void init_loop_heur_general_(const TensorInt tune_num,
      const TensorInt heur_num);

  void init_threads_();

  void init_vec_general_();
  void init_vec_deploy_kernels_(const KernelTypeTrans kn_type,
      const TensorUInt kn_cont_size, const TensorUInt kn_ncont_size,
      const TensorUInt cont_begin_pos, const TensorUInt ncont_begin_pos,
      const TensorUInt cont_offset_size, const TensorUInt ncont_offset_size,
      const TensorUInt offset = 0);
  void init_vec_common_leading_();

  void init_parallel_rule_general_();
  void init_parallel_rule_common_leading_();
  void init_parallel_heur_(const TensorInt tune_num, const TensorInt heur_num);

  double heur_loop_evaluator_(
      const LoopOrderTrans<ORDER> &target_loop_order) const;
  double heur_parallel_evaluator_(
      const ParaStrategyTrans<ORDER> &target_para) const;

  std::vector<Descriptor> gen_candidates_() const;


  std::shared_ptr<ParamType> param_;
  TensorUInt threads_;
  const TensorUInt in_ld_idx_, out_ld_idx_;
  std::unordered_map<TensorUInt, TensorUInt> th_factor_map_;
  std::array<TensorIdx, ORDER> avail_parallel_;

  std::vector<LoopOrderTrans<ORDER>> loop_order_candidates_;
  std::vector<ParaStrategyTrans<ORDER>> parallel_strategy_candidates_;
  Descriptor template_descriptor_;

  // Parameters for loop order heuristics
  double heur_loop_penalty_begin, heur_loop_penalty_step;
  double heur_loop_importance_begin, heur_loop_importance_scale;
  double heur_loop_input_penalty_factor, heur_loop_output_penalty_factor;
  double heur_loop_in_ld_award, heur_loop_out_ld_award;

  // Parameters for parallelization heuristics
  double heur_para_penalty_factor_cl, heur_para_penalty_factor_inld,
         heur_para_penalty_factor_outld, heur_para_cost_begin;
  TensorUInt heur_para_max_penalty_threads;
};


/*
 * Import implementation
 */
#include "plan_trans_util.tcc"

}

#endif // HPTT_PLAN_PLAN_TRANS_UTIL_H_
