#include "tixi_utils.h"
#include <stdexcept>

void tixi_utils::handle_error(ReturnCode code) {
  if (code == INVALID_HANDLE) {
    throw std::runtime_error("Invalid Tixi Handle!");
  }
}
