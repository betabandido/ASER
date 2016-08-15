#include <memory>
#include <string>
#include <vector>

#include <core/benchmark.h>
#include <core/exec_manager.h>

namespace aser {

namespace util {
class process;
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
  typedef std::shared_ptr<util::process> process_ptr;

  std::vector<benchmark> benchs_;
  std::vector<process_ptr> processes_;

  void start_impl() final;

  void prepare_bench(const benchmark& bench);
};

} // namespace aser

