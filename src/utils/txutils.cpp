#include "txutils.h"
#include <regex>
#include <stdexcept>
#include "general.h"

#include <string.h>
#define CHECK_ERR(function)    \
  {                            \
    ReturnCode ret = function; \
    if (ret != SUCCESS)        \
      return ret;              \
  }

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
  CHECK_ERR(
      tixiXPathExpressionGetXPath(handle, xPathExpression, index, &xPath));

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
    CHECK_ERR(tixiGetNumberOfChilds(handle, path, &n));

    int i = 0;   // counter of elements of the required name
    int ic = 0;  // number of comment elements - these should not be counted in
                 // the number
    for (int j = 1; j <= n; j++) {
      char* elemName;
      CHECK_ERR(tixiGetChildNodeName(handle, path, j, &elemName));
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

ReturnCode txutils::copy(TixiDocumentHandle handle,
                         const char* elementPath,
                         TixiDocumentHandle* clip) {
  // Get the toplevel node name
  char* source_path;
  CHECK_ERR(tixiXPathExpressionGetXPath(handle, elementPath, 1, &source_path));

  char* name = elementName(source_path);

  CHECK_ERR(tixiCreateDocument(name, clip));
  // Need some dummy namespace to enable xPathExpression functions on the clip
  CHECK_ERR(tixiRegisterNamespace(*clip, "/local", "clp"));
  const char* target_path = strdup(std::string(name).insert(0, 1, '/').c_str());

  // The function paste does exactly the same thing but from clip to handle
  return txutils::paste(handle, elementPath, *clip, 0, target_path);
}

ReturnCode txutils::paste(TixiDocumentHandle handle_to_be_clip,
                          const char* target_path_to_be,
                          TixiDocumentHandle handle,
                          int index,
                          const char* elementPath) {
  ReturnCode ret;

  int na;
  CHECK_ERR(
      tixiGetNumberOfAttributes(handle_to_be_clip, target_path_to_be, &na));

  // copy attributes first
  for (int i = 1; i <= na; i++) {
    char* attrName;
    char* attrValue;
    CHECK_ERR(tixiGetAttributeName(handle_to_be_clip, target_path_to_be, i,
                                   &attrName));
    CHECK_ERR(tixiGetTextAttribute(handle_to_be_clip, target_path_to_be,
                                   attrName, &attrValue));
    CHECK_ERR(tixiAddTextAttribute(handle, elementPath, attrName, attrValue));
  }

  // recursively copy elements from one handle to another

  int ne;
  CHECK_ERR(tixiGetNumberOfChilds(handle_to_be_clip, target_path_to_be, &ne));
  int ic = 0;
  for (int i = 1; i <= ne; i++) {
    char* elementName;
    CHECK_ERR(tixiGetChildNodeName(handle_to_be_clip, target_path_to_be, i,
                                   &elementName));
    if (std::string(elementName) == "#comment") {
      // skipping comments, count them first.
      ic++;
      continue;
    } else if (std::string(elementName) == "#text") {
      // Need to copy the text from the element and create
      char* text;
      CHECK_ERR(
          tixiGetTextElement(handle_to_be_clip, target_path_to_be, &text));
      // The targetPath is already the path to the element, that has been
      // created in the previous recursion
      CHECK_ERR(tixiUpdateTextElement(handle, elementPath, text));

    } else {
      // 1. Create new element in the target
      // 2. Append its name to the source and target paths
      // 3. Run the function recursively with updated paths
      if (index == 0) {
        CHECK_ERR(tixiCreateElement(handle, elementPath, elementName));
      } else {
        CHECK_ERR(
            tixiCreateElementAtIndex(handle, elementPath, elementName, index));
      }
      ReturnCode res;
      char* sourcePath;
      char* xPath = strdup((std::string(target_path_to_be) + "/*").c_str());
      res = tixiXPathExpressionGetXPath(handle_to_be_clip, xPath, i - ic,
                                        &sourcePath);
      delete xPath;
      CHECK_ERR(res);

      int n;
      xPath = strdup((std::string(elementPath) + "/*").c_str());
      res = tixiXPathEvaluateNodeNumber(handle, xPath, &n);
      delete xPath;
      CHECK_ERR(res);

      char* newTargetPath;

      CHECK_ERR(tixiXPathExpressionGetXPath(handle, xPath, n, &newTargetPath));
      CHECK_ERR(paste(handle_to_be_clip, sourcePath, handle, 0, newTargetPath));
    }
  }
  return SUCCESS;
}

ReturnCode txutils::indentText(TixiDocumentHandle handle,
                               const char* xPathExpression) {
  if (!xPathExpression) {
    xPathExpression = "//*[text()]";
  }

  int n;
  CHECK_ERR(tixiXPathEvaluateNodeNumber(handle, xPathExpression, &n));
  int ind = indentation();
  std::regex nline(
      R"(\s*\n\s*)");            // use this regex to strip newlines from spaces
  std::regex lspace(R"(^\s+)");  // Remove leading spaces with that
  std::regex tspace(R"(\s+$)");  // Remove trailing spaces with that
  for (int i = 1; i <= n; i++) {
    char* path;
    char* text;
    CHECK_ERR(tixiXPathExpressionGetXPath(handle, xPathExpression, i, &path));
    CHECK_ERR(tixiGetTextElement(handle, path, &text));

    std::string s(text);
    std::string Path(path);
    int slashes =
        std::count(Path.begin(), Path.end(),
                   '/');  // this is how many times the text should be indented

    s = std::regex_replace(s, lspace, "");
    s = std::regex_replace(s, tspace, "");
    s = std::regex_replace(s, nline, "\n");

    if (s.empty() || s.at(0) != '\n')
      s.insert(0, "\n");
    if (s.back() != '\n')
      s.append("\n");

    std::string newline("\n");
    for (i = 0; i < slashes * ind; i++) {
      newline.append(" ");
    }
    s = std::regex_replace(s, nline, newline);

    // now strip the last line, so that the closing tag is nicely aligned
    for (int i = 0; i < ind; i++) {
      s.pop_back();
    }
    CHECK_ERR(tixiUpdateTextElement(handle, path, s.c_str()));
  }
  return SUCCESS;
}

int txutils::indentation() {
  // Export a simple XML as string
  TixiDocumentHandle tmp;
  CHECK_ERR(tixiImportFromString("<?xml version=\"1.0\"?><r><c/></r>", &tmp));
  char* tmp_txt;
  CHECK_ERR(tixiExportDocumentAsString(tmp, &tmp_txt));
  std::string tmp_str(tmp_txt);

  // Find the XML tags <r> and <c/> and count spaces between right side of <r>
  // (hence +4) and left side ot <c/>
  std::string::size_type pos_c = tmp_str.find(R"(<c/>)");
  std::string::size_type pos_r = tmp_str.find(R"(<r>)") + 4;
  return (int)pos_c - pos_r;
}
