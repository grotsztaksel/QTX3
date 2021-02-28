#include "txutils.h"
#include <stdexcept>
#include "general.h"

#include <string.h>

typedef std::string::size_type istr;

void txutils::handle_error(ReturnCode code) {
  if (code == INVALID_HANDLE) {
    throw std::runtime_error("Invalid Tixi Handle!");
  }
}

ReturnCode txutils::indexedPath(TixiDocumentHandle handle,
                                const char* xPathExpression,
                                int index,
                                char** ipath) {
  char* xPath;
  ReturnCode ret =
      tixiXPathExpressionGetXPath(handle, xPathExpression, index, &xPath);
  if (ret != ReturnCode::SUCCESS) {
    return ret;
  }

  // std::string offers way more tools to process the path than just char.

  // initialize the string to search but omit the root name and the first two
  // slashes.

  istr start = std::string(xPath, 1, std::string::npos).find_first_of("/") +
               2;  // +2 because: +1 - to compensate for omitting of first
                   // character ('/'); +1 - to exclude the second '/' from the
                   // string - want to split the string and dont want the first
                   // empty string in the vector:
                   // '/root/child/subchild...'
                   //        ^--- istart will be here, and the vector will be
                   //        (Python list notation): [ 'child', 'subchild',...]

  const std::string& path = std::string(xPath, start, std::string::npos);
  std::vector<std::string> elements = split(path, '/');

  // Root will always return /*[1], need to remove the first two elements (
  std::string iPath("/*[1]");

  for (std::string element : elements) {
    int n;
    const char* path = iPath.c_str();

    int I = txutils::elementNumber(element.c_str());
    const char* nextElementName = txutils::elementName(element.c_str());
    ReturnCode ret = tixiGetNumberOfChilds(handle, path, &n);
    if (ret != ReturnCode::SUCCESS)
      return ret;

    int i = 0;   // counter of elements of the required name
    int ic = 0;  // number of comment elements - these should not be counted in
                 // the number
    for (int j = 1; j <= n; j++) {
      char* elemName;
      ReturnCode ret = tixiGetChildNodeName(handle, path, j, &elemName);
      if (ret != ReturnCode::SUCCESS)
        return ret;
      if (std::string(elemName) == "#comment") {
        ic++;
        continue;
      }
      if (std::string(elemName) == std::string(nextElementName))
        i++;

      if (i == I) {
        iPath.append(std::string("/*[") + std::to_string(j - ic) +
                     std::string("]"));
        break;
      }
    }
  }

  *ipath = strdup(iPath.c_str());
  return ReturnCode::SUCCESS;
}

char* txutils::elementName(const char* xPathExpression) {
  const std::string path = std::string(uniqueName(xPathExpression));
  istr i = path.find_first_of("[");
  return strdup(path.substr(0, i).c_str());
}

int txutils::elementNumber(const char* xPathExpression) {
  const std::string path = std::string(uniqueName(xPathExpression));
  istr i = path.find_first_of("[");
  if (i == std::string::npos) {
    return 1;
  }
  istr n = path.find_first_of("]");
  return std::stoi(path.substr(i + 1, n - i - 1));
}

char* txutils::uniqueName(const char* xPathExpression) {
  const std::string path = std::string(xPathExpression);
  istr i = path.find_last_of("/") + 1;
  return strdup(path.substr(i).c_str());
}
