#include <memory>
#include <string>
#include <vector>

#include <core/benchmark.h>
#include <core/exec_manager.h>

namespace aser {

namespace util {
class sync_process;
}

/** Simple execution manager.
 * 
 * This execution manager executes a set of benchmarks, and waits until any
 * of the benchmarks finishes its execution. At that time, the other
 * benchmarks are killed, and the execution finishes.
 */
class simple_manager : public exec_manager {
public:
  /** Constructor.
   *
   * @param properties Configuration properties.
   */
  simple_manager(const boost::property_tree::ptree& properties);

private:
  using process_type = aser::util::sync_process;
  using process_ptr = std::shared_ptr<process_type>;

  std::vector<benchmark> benchs_;
  std::vector<process_ptr> processes_;

  void start_impl() final;

  void prepare_bench(const benchmark& bench);
};

} // namespace aser

