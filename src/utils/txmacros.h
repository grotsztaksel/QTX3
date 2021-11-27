

#ifndef TXMACROS_H
#define TXMACROS_H
#include <string.h>

/** @brief A collection of macros
 *
 */

//! @brief From the function, where this macro is used, return ReturnCode
//! returned by the function called in this macro.

#define RET_ERR(function)                                                      \
  {                                                                            \
    ReturnCode ret = function;                                                 \
    if (ret != SUCCESS)                                                        \
      return ret;                                                              \
  }

// Use #define ERRMSG in the source file before #include "txmacros.h" and then
// declare own errmsg function
#ifndef ERRMSG
#define ERRMSG
std::string errmsg(const char *fnc, int line) {
  return "Error in " + std::string(__FILE__) + ", function " +
         std::string(fnc) + "(), line " + std::to_string(line);
}
#endif

#define CHK_ERR(function, expected_codes)                                      \
  txutils::expectCode(function, errmsg(__func__, __LINE__),                    \
                      std::list<ReturnCode> expected_codes);

#endif // TXMACROS_H
