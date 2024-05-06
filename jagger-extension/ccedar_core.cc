#include <ccedar_core.h>

namespace ccedar {
  template <> size_t key_len <char> (const char *p) { return std::strlen (p); }

}