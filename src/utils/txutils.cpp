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
  const std::string& path = std::string(xPath);
  std::vector<std::string> elements = split(path, '/');

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
