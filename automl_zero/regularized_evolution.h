// Copyright 2020 The Google Research Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef THIRD_PARTY_GOOGLE_RESEARCH_GOOGLE_RESEARCH_AUTOML_ZERO_REGULARIZED_EVOLUTION_H_
#define THIRD_PARTY_GOOGLE_RESEARCH_GOOGLE_RESEARCH_AUTOML_ZERO_REGULARIZED_EVOLUTION_H_

#include <memory>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "algorithm.h"
#include "definitions.h"
#include "evaluator.h"
#include "generator.h"
#include "mutator.h"
#include "random_generator.h"
#include "testing/production_stub/public/gunit_prod.h"
#include "absl/flags/flag.h"
#include "absl/time/time.h"

namespace brain {
namespace evolution {
namespace amlz {

class RegularizedEvolution {
 public:
  RegularizedEvolution(
      // The compute cost of evaluating one individual.
      RandomGenerator* rand_gen,
      // Runs up to this many total individuals.
      IntegerT population_size,
      IntegerT tournament_size,
      // Number of training examples for each model.
      // Number of mutations to apply to each seed individual before inserting.
      IntegerT init_mutations,
      // How frequently to print progress reports.
      IntegerT progress_every,
      bool progress_every_by_time,
      Generator* generator,
      Evaluator* evaluator,
      // The mutator to use to perform all mutations.
      Mutator* mutator);
  RegularizedEvolution(
      const RegularizedEvolution& other) = delete;
  RegularizedEvolution& operator=(
      const RegularizedEvolution& other) = delete;

  // Initializes the algorithm. Returns the number of individuals evaluated in
  // this call.
  IntegerT Init();

  // Runs for a given amount of time (rounded up to the nearest generation) or
  // for a certain number of individuals (rounded up to the nearest generation),
  // whichever is first. Assumes that Init has been called. Returns the number
  // of individuals evaluated in this call.
  IntegerT Run(IntegerT max_individuals, IntegerT max_nanos);

  // Returns the CUs/number of individuals evaluated so far. Returns an exact
  // number.
  IntegerT NumIndividuals() const;

  // Returns a random serialized Algorithm in the population and its fitness.
  std::shared_ptr<const Algorithm> Get(double* fitness);

  // Returns the best serialized Algorithm in the population and its worker
  // fitness.
  std::shared_ptr<const Algorithm> GetBest(double* fitness);

  IntegerT PopulationSize() const;

  void PopulationStats(
      double* pop_mean, double* pop_stdev,
      std::shared_ptr<const Algorithm>* pop_best_algorithm,
      double* pop_best_fitness) const;

 private:
  FRIEND_TEST(RegularizedEvolutionTest, TimesCorrectly);

  friend IntegerT PutsInPosition(
      const Algorithm&, RegularizedEvolution*);
  friend IntegerT EvaluatesAndPutsInPosition(
      const Algorithm&, RegularizedEvolution*);
  friend bool PopulationsEq(
      const RegularizedEvolution&,
      const RegularizedEvolution&);

  void InitAlgorithm(std::shared_ptr<const Algorithm>* algorithm);
  double Execute(std::shared_ptr<const Algorithm> algorithm);
  std::shared_ptr<const Algorithm> BestFitnessTournament();
  void SingleParentSelect(std::shared_ptr<const Algorithm>* algorithm);
  void MaybePrintProgress();

  Evaluator* evaluator_;
  RandomGenerator* rand_gen_;
  const IntegerT start_secs_;
  IntegerT epoch_secs_;
  IntegerT epoch_secs_last_progress_;
  IntegerT num_individuals_last_progress_;
  const IntegerT init_mutations_;
  const IntegerT tournament_size_;
  const IntegerT progress_every_;
  const bool progress_every_by_time_;
  bool initialized_;
  Generator* generator_;
  Mutator* mutator_;

  // Serializable components.
  const IntegerT population_size_;
  std::vector<std::shared_ptr<const Algorithm>> algorithms_;
  std::vector<double> fitnesses_;
  IntegerT num_individuals_;
  IntegerT num_individuals_since_put_;
};

}  // namespace amlz
}  // namespace evolution
}  // namespace brain

#endif  // THIRD_PARTY_GOOGLE_RESEARCH_GOOGLE_RESEARCH_AUTOML_ZERO_REGULARIZED_EVOLUTION_H_
