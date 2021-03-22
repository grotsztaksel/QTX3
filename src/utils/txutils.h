#ifndef TXUTILS_H
#define TXUTILS_H

#include "tixi.h"

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
ReturnCode indexedPath(TixiDocumentHandle handle, const char *xPathExpression,
                       int index, char **ipath);

/**
  @brief Copies the tree content of a selected element to a new handle
 */
ReturnCode copy(TixiDocumentHandle handle, const char *elementPath,
                TixiDocumentHandle *clip);

/**
  @brief Pastes the tree content of a clip handle element to a target handle
 */
ReturnCode paste(TixiDocumentHandle handle, const char *elementPath,
                 TixiDocumentHandle clip, int index = 0,
                 const char *target_path_in = nullptr);
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
                      const char *xPathExpression = nullptr);
char *elementName(const char *xPathExpression);
int elementNumber(const char *xPathExpression);
char *uniqueName(const char *xPathExpression);

/**
  @brief Helper function returning the number of spaces per each indentation in
  tixi using pretty print
  @return Number of spaces per each indentation in tixi using pretty print
  @remark This is a bit expensive function. Use wisely.
*/
int indentation();
/**
 @brief sort elements that the xPathExpression resolves to

 @param[in]  handle handle as returned by ::tixiOpenDocument,
 ::tixiOpenDocumentRecursive, ::tixiOpenDocumentFromHTTP, ::tixiCreateDocument
 or ::tixiImportFromString
 @param[in] xPathExpression The XPath Expression to evaluate.
 @param[in] criterion name of the attribute which should be considered for
 sorting. If none given, the elemenents will by sorted by their name
 @param[in] ascending: if true, the elements will be sorted in ascending orter.
 Otherwise the elements will be sorted in descending order

 @remark The function uses TiXi swap() function.
 @remark The sorted elements do not have
 to have the same parent, but the xPathExpression must not resolve to elements
 that are the others' elements ascendants or descendants

 */
ReturnCode sort(TixiDocumentHandle h, const char *xPathExpression,
                const char *criterion = nullptr, bool ascending = true);

}; // namespace txutils

#endif // TXUTILS_H
