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

/**
  @brief Copies the tree content of a selected element to a new handle
 */
ReturnCode copy(TixiDocumentHandle handle,
                const char* elementPath,
                TixiDocumentHandle* clip);

/**
  @brief Pastes the tree content of a clip handle element to a target handle
 */
ReturnCode paste(TixiDocumentHandle handle,
                 const char* elementPath,
                 TixiDocumentHandle clip,
                 int index = 0,
                 const char* target_path = nullptr);
/**
  @brief indent text in all text elements satisfying the provided
  xPathExpression. If none is given, will indent all text elements
  @param[in]  handle handle as returned by ::tixiOpenDocument,
  ::tixiOpenDocumentRecursive, ::tixiOpenDocumentFromHTTP, ::tixiCreateDocument
  @param[in]  xPathExpression The XPath Expression to evaluate.

  NOTE: The function assumes, that the xPathExpression will only find text
  elements. Otherwise will return error. Use "text()" in the xPathExpression to
  filter out the text elements
*/
ReturnCode indentText(TixiDocumentHandle handle,
                      const char* xPathExpression = nullptr);
char* elementName(const char* xPathExpression);
int elementNumber(const char* xPathExpression);
char* uniqueName(const char* xPathExpression);

/**
  @brief Helper function returning the number of spaces per each indentation in
  tixi using pretty print

  @return Number of spaces per each indentation in tixi using pretty print
*/
int indentation();

};  // namespace txutils

#endif  // TIXI_UTILS_H
