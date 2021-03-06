#pragma once
#ifndef HPTT_ARCH_ARCH_H_
#define HPTT_ARCH_ARCH_H_

#include <cstdint>

#include <vector>
#include <string>
#include <unordered_map>


namespace hptt {

void hptt_cpuid(const uint32_t input, uint32_t output[4]);


class LibLoader {
public:
  // Delete copy/move constructors and operators
  LibLoader(const LibLoader &) = delete;
  LibLoader &operator=(const LibLoader &) = delete;
  LibLoader(LibLoader &&) = delete;
  LibLoader &operator=(LibLoader &&) = delete;

  static LibLoader &get_loader();
  void *dlsym(const char *symbol);

private:
  struct Arch_ {
    Arch_() = default;
    Arch_(bool found, const char *filename)
        : found(found), filename(filename) {}
    bool found;
    const std::string filename;
  };

  LibLoader();

  void init_cpu_();
  void select_arch_();
  void *load_(const std::string &filename);

  void *handler_;

  std::unordered_map<std::string, Arch_> intrin_sets_;
};

}

#endif // HPTT_ARCH_ARCH_H_
