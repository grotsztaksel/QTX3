#ifndef TIXI_UTILS_H
#define TIXI_UTILS_H

#include <tixi.h>

// Collection of general-purpose tixi & xml path utilities
namespace txutils {

// Throw error when INVALID_HANDLE
void handle_error(ReturnCode code);

/**
  @brief Evaluates a XPath expression and the xPath for the i-th result and
  converts it to a format composed only of indexes: "/*[1]/*[i]/*[j]/*[k]..."

  @param[in]  handle handle as returned by ::tixiOpenDocument,
  ::tixiOpenDocumentRecursive, ::tixiOpenDocumentFromHTTP, ::tixiCreateDocument
  or ::tixiImportFromString
  @param[in]  xPathExpression The XPath Expression to evaluate.
  @param[in]  index The index of the result to query, with 1 <= index <= number,
  and number queried with ::tixiXPathEvaluateNodeNumber
  @param[out] xPath The xPath of the search result in the "index" format.

  @return
    - SUCCESS if successfully retrieve the xPath of the search.
    - INVALID_HANDLE if the handle is not valid, i.e.  does not or no longer
  exist.
    - FAILED  if an internal error occured.
 */
ReturnCode indexedPath(TixiDocumentHandle handle,
                       const char* xPathExpression,
                       int index,
                       char** ipath);

char* elementName(const char* xPathExpression);
int elementNumber(const char* xPathExpression);
char* uniqueName(const char* xPathExpression);

};  // namespace txutils

#endif  // TIXI_UTILS_H
