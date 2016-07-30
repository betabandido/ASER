#ifndef UTIL_FACTORY_H_
#define UTIL_FACTORY_H_

#include <cassert>
#include <map>
#include <memory>
#include <string>

namespace aser {
namespace util {

// Adapted from http://stackoverflow.com/a/29154449/1135819

/** Generic factory.
 *
 * @tparam Base Base class for all the objects to be created by this factory.
 * @tparam Args Types of the arguments to pass to the constructor of objects
 *              created by this factory.
 */
template<typename Base, typename... Args>
class factory {
public:
  /** Returns the instance for a factory. */
  static factory& instance() {
    static factory instance_;
    return instance_;
  }

  /** Registers an instance creator for a given derived class.
   *
   * @tparam Derived The derived class.
   * @param id The identifier for the derived class.
   */
  template<typename Derived>
  void register_creator(const std::string& id) {
    auto res = creators_.emplace(id, std::make_unique<creator<Derived>>());
    assert(res.second);
  }

  /** Creates an instance.
   *
   * @param id The derived class identifier.
   * @param args The args to pass to the constructor.
   * @return A pointer to the instance.
   */
  std::unique_ptr<Base> create(const std::string& id, Args&&... args) {
    auto it = creators_.find(id);
    assert(it != end(creators_));
    return it->second->create(std::forward<Args>(args)...);
  }

private:
  struct creator_interface {
    virtual std::unique_ptr<Base> create(Args&&... args) = 0;
  };

  template<typename Derived>
  struct creator : public creator_interface {
    virtual std::unique_ptr<Base> create(Args&&... args) override {
      return std::make_unique<Derived>(std::forward<Args>(args)...);
    }
  };

  std::map<std::string, std::unique_ptr<creator_interface>> creators_;
};

/** Instantiating an object of this class registers a creator with the factory.
 *
 * @tparam Base The base class.
 * @tparam Derived The derived class.
 * @tparam Args Types of the arguments to pass to the constructor.
 * @param id The derived class text identifier.
 */
template<typename Base, typename Derived, typename... Args>
class registar {
public:
  registar(const std::string& id) {
    auto& instance = factory<Base, Args...>::instance();
    instance.template register_creator<Derived>(id);
  }
};


/** Creates an object using an object factory.
 *
 * @tparam Base The base class.
 * @tparam Args Types of the arguments to pass to the constructor.
 * @param id The derived class text identifier.
 * @param args The actual arguments to pass to the derived class' constructor.
 */
template<typename Base, typename... Args>
std::unique_ptr<Base> create(const std::string& id, Args&&... args) {
  return factory<Base, Args...>::instance().create(id, std::forward<Args>(args)...);
}

} // namespace util
} // namespace aser

#endif // UTIL_FACTORY_H_

