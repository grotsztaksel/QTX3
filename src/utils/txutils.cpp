#include <stdexcept>
#include "txutils.h"

void txutils::handle_error(ReturnCode code) {
  if (code == INVALID_HANDLE) {
    throw std::runtime_error("Invalid Tixi Handle!");
  }
}
