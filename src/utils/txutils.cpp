#include "txutils.h"
#include "general.h"
#include <regex>
#include <stdexcept>

#include <map>
#include <string.h>

#include "txmacros.h"

typedef std::string::size_type istr;

ReturnCode txutils::indexedPath(const TixiDocumentHandle handle,
                                const char *xPathExpression, int index,
                                char **ipath) {
  char *xPath;
  RET_ERR(tixiXPathExpressionGetXPath(handle, xPathExpression, index, &xPath));

  // std::string offers way more tools to process the path than just char.

  // initialize the string to search but omit the root name and the first two
  // slashes.

  istr start = std::string(xPath, 1, std::string::npos).find_first_of("/") +
               2; // +2 because: +1 - to compensate for omitting of first
                  // character ('/'); +1 - to exclude the second '/' from the
                  // string - want to split the string and dont want the first
                  // empty string in the vector:
                  // '/root/child/subchild...'
                  //        ^--- istart will be here, and the vector will be
                  //        (Python list notation): [ 'child', 'subchild',...]

  const std::string &path = std::string(xPath, start, std::string::npos);
  std::vector<std::string> elements = split(path, '/');

  // Root will always return /*[1], need to remove the first two elements (
  std::string iPath("/*[1]");

  for (const std::string &element : elements) {
    int n;
    const char *path = iPath.c_str();

    int I = txutils::elementNumber(element.c_str());
    const char *nextElementName = txutils::elementName(element.c_str());
    RET_ERR(tixiGetNumberOfChilds(handle, path, &n));

    int i = 0;  // counter of elements of the required name
    int ic = 0; // number of comment elements - these should not be counted in
                // the number
    for (int j = 1; j <= n; j++) {
      char *elemName;
      RET_ERR(tixiGetChildNodeName(handle, path, j, &elemName));
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

char *txutils::elementName(const char *xPathExpression) {
  const std::string path = std::string(uniqueName(xPathExpression));
  istr i = path.find_first_of("[");
  return strdup(path.substr(0, i).c_str());
}

int txutils::elementNumber(const char *xPathExpression) {
  const std::string path = std::string(uniqueName(xPathExpression));
  istr i = path.find_first_of("[");
  if (i == std::string::npos) {
    return 1;
  }
  istr n = path.find_first_of("]");
  return std::stoi(path.substr(i + 1, n - i - 1));
}

char *txutils::uniqueName(const char *xPathExpression) {
  const std::string path = std::string(xPathExpression);
  istr i = path.find_last_of("/") + 1;
  return strdup(path.substr(i).c_str());
}

ReturnCode txutils::copy(const TixiDocumentHandle handle,
                         const char *elementPath, TixiDocumentHandle *clip) {
  // Get the toplevel node name
  char *source_path;
  RET_ERR(tixiXPathExpressionGetXPath(handle, elementPath, 1, &source_path));

  char *name = elementName(source_path);

  RET_ERR(tixiCreateDocument(name, clip));
  // Need some dummy namespace to enable xPathExpression functions on the clip
  RET_ERR(tixiRegisterNamespace(*clip, "/local", "clp"));
  const char *target_path = strdup(std::string(name).insert(0, 1, '/').c_str());

  // The function paste does exactly the same thing but from clip to handle
  // we want to paste from the handle (Document) to the clip ("clipboard" tixi)
  return txutils::paste(*clip, target_path, handle, 0, elementPath);
}

ReturnCode txutils::paste(TixiDocumentHandle handle, const char *elementPath,
                          TixiDocumentHandle clip, int index,
                          const char *target_path_in) {
  char *target_path;
  if (target_path_in) {
    target_path = strdup(target_path_in);
  } else {
    target_path = strdup("/");
  }
  int na;
  RET_ERR(tixiGetNumberOfAttributes(clip, target_path, &na));

  // copy attributes first
  for (int i = 1; i <= na; i++) {
    char *attrName;
    char *attrValue;
    RET_ERR(tixiGetAttributeName(clip, target_path, i, &attrName));
    RET_ERR(tixiGetTextAttribute(clip, target_path, attrName, &attrValue));
    RET_ERR(tixiAddTextAttribute(handle, elementPath, attrName, attrValue));
  }

  // recursively copy elements from one handle to another

  int ne;
  RET_ERR(tixiGetNumberOfChilds(clip, target_path, &ne));
  int ic = 0;
  for (int i = 1; i <= ne; i++) {
    char *elementName;
    RET_ERR(tixiGetChildNodeName(clip, target_path, i, &elementName));
    if (std::string(elementName) == "#comment") {
      // skipping comments, count them first.
      ic++;
      continue;
    } else if (std::string(elementName) == "#text") {
      // Need to copy the text from the element and create
      char *text;
      RET_ERR(tixiGetTextElement(clip, target_path, &text));
      // The targetPath is already the path to the element, that has been
      // created in the previous recursion
      RET_ERR(tixiUpdateTextElement(handle, elementPath, text));

    } else {
      // 1. Create new element in the target
      // 2. Append its name to the source and target paths
      // 3. Run the function recursively with updated paths
      if (index == 0) {
        RET_ERR(tixiCreateElement(handle, elementPath, elementName));
      } else {
        RET_ERR(
            tixiCreateElementAtIndex(handle, elementPath, elementName, index));
      }
      ReturnCode res;
      char *sourcePath;
      char *xPath = strdup((std::string(target_path) + "/*").c_str());
      res = tixiXPathExpressionGetXPath(clip, xPath, i - ic, &sourcePath);
      free(xPath);
      RET_ERR(res);

      int n;
      xPath = strdup((std::string(elementPath) + "/*").c_str());
      res = tixiXPathEvaluateNodeNumber(handle, xPath, &n);
      RET_ERR(res);

      char *newTargetPath;

      RET_ERR(tixiXPathExpressionGetXPath(handle, xPath, n, &newTargetPath));
      RET_ERR(paste(handle, newTargetPath, clip, 0, sourcePath));
    }
  }
  return SUCCESS;
}

ReturnCode txutils::indentText(const TixiDocumentHandle handle,
                               const char *xPathExpression) {
  if (!xPathExpression) {
    xPathExpression = "//*[text()]";
  }

  int n;
  RET_ERR(tixiXPathEvaluateNodeNumber(handle, xPathExpression, &n));
  int ind = indentation();
  std::regex nline(
      R"(\s*\n\s*)");           // use this regex to strip newlines from spaces
  std::regex lspace(R"(^\s+)"); // Remove leading spaces with that
  std::regex tspace(R"(\s+$)"); // Remove trailing spaces with that
  for (int i = 1; i <= n; i++) {
    char *path;
    char *text;
    RET_ERR(tixiXPathExpressionGetXPath(handle, xPathExpression, i, &path));
    RET_ERR(tixiGetTextElement(handle, path, &text));

    std::string s(text);
    std::string Path(path);
    int slashes =
        std::count(Path.begin(), Path.end(),
                   '/'); // this is how many times the text should be indented

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
    RET_ERR(tixiUpdateTextElement(handle, path, s.c_str()));
  }
  return SUCCESS;
}

int txutils::indentation() {
  // Export a simple XML as string
  TixiDocumentHandle tmp;
  RET_ERR(tixiImportFromString("<?xml version=\"1.0\"?><r><c/></r>", &tmp));
  char *tmp_txt;
  RET_ERR(tixiExportDocumentAsString(tmp, &tmp_txt));
  std::string tmp_str(tmp_txt);

  // Find the XML tags <r> and <c/> and count spaces between right side of <r>
  // (hence +4) and left side ot <c/>
  std::string::size_type pos_c = tmp_str.find(R"(<c/>)");
  std::string::size_type pos_r = tmp_str.find(R"(<r>)") + 4;
  return (int)pos_c - pos_r;
}

ReturnCode txutils::sort(const TixiDocumentHandle h,
                         const char *xPathExpression, const char *criterion,
                         bool ascending) {

  char *xpath;
  const char *msg = "Error while sorting XML structure";
  if (criterion) {
    // append the attribute name to the xPath
    std::string xPath(xPathExpression);
    xPath.append("[@" + std::string(criterion) + "]");
    xpath = strdup(xPath.c_str());
  } else {
    xpath = strdup(xPathExpression);
  }
  int n;
  ReturnCode res =
      expectCode(tixiXPathEvaluateNodeNumber(h, xpath, &n),
                 msg + std::to_string(__LINE__), {SUCCESS, FAILED});
  if (res == FAILED) {
    // xPathExpression resolved to 0 elements
    free(xpath);
    return res;
  }

  // implementing bubble sort

  char *path_i;
  char *path_i1;
  char *val_i;
  char *val_i1;

  while (n > 1) {
    for (int i = 1; i < n; i++) {
      // get the paths
      expectCode(tixiXPathExpressionGetXPath(h, xpath, i, &path_i),
                 msg + std::to_string(__LINE__));
      expectCode(tixiXPathExpressionGetXPath(h, xpath, i + 1, &path_i1),
                 msg + std::to_string(__LINE__));

      if (criterion) {
        expectCode(tixiGetTextAttribute(h, path_i, criterion, &val_i),
                   msg + std::to_string(__LINE__));
        expectCode(tixiGetTextAttribute(h, path_i1, criterion, &val_i1),
                   msg + std::to_string(__LINE__));
      } else {
        val_i = elementName(path_i);
        val_i1 = elementName(path_i1);
      }
      std::string Val_i = std::string(val_i);
      int cmp = Val_i.compare(val_i1);

      if ((cmp > 0 && ascending) || (cmp < 0 && !ascending)) {
        expectCode(tixiSwapElements(h, path_i, path_i1),
                   msg + std::to_string(__LINE__));
      }
    }
    n--;
  }
  free(xpath);
  return SUCCESS;
}

ReturnCode txutils::expectCode(ReturnCode tixiresult, std::string message,
                               std::list<ReturnCode> acceptedCodes) {
  bool found;
  if (acceptedCodes.empty()) {
    found = (tixiresult == SUCCESS);
  } else {
    found = (std::find(acceptedCodes.begin(), acceptedCodes.end(),
                       tixiresult) != acceptedCodes.end());
  }
  if (!found) {
    throw(std::runtime_error(
        message + "\nTiXi ERROR CODE: " + std::to_string(tixiresult)));
  }
  return tixiresult;
}

ReturnCode txutils::expectCode(ReturnCode tixiresult, const int &line,
                               std::list<ReturnCode> acceptedCodes) {
  return expectCode(tixiresult,
                    std::string("Unhandled TiXi error in line ") +
                        std::to_string(line),
                    acceptedCodes);
}

ReturnCode txutils::excludeCode(ReturnCode tixiresult, std::string message,
                                std::list<ReturnCode> unacceptedCodes) {
  bool found = (std::find(unacceptedCodes.begin(), unacceptedCodes.end(),
                          tixiresult) != unacceptedCodes.end());
  if (found) {
    throw(std::runtime_error(
        message + "\nTiXi ERROR CODE: " + std::to_string(tixiresult)));
  }
  return tixiresult;
}

ReturnCode txutils::excludeCode(ReturnCode tixiresult, const int &line,
                                std::list<ReturnCode> unacceptedCodes) {
  return excludeCode(tixiresult,
                     std::string("Unhandled TiXi error in line ") +
                         std::to_string(line),
                     unacceptedCodes);
}

char *txutils::cleanElementPath(const char *xmlPath) {
  std::string xml(xmlPath);

  for (const std::string &illegal : {"@", "*", "="}) {
    if (xml.find(illegal) != std::string::npos) {
      std::string error("cleanElementPath(): Function does not accep XPath "
                        "expressions! A full XML path is required"
                        "input argument: \"");
      error.append(xmlPath).append("\"");
      throw(std::runtime_error(error));
    }
  }

  std::regex index(R"(\[\s*\d+\s*\])");
  xml = std::regex_replace(xml, index, "");

  return strdup(xml.c_str());
}

ReturnCode txutils::getInheritedAttribute(const TixiDocumentHandle handle,
                                          const char *elementPath,
                                          const char *attributeName,
                                          char **text) {
  char *path;
  RET_ERR(findInheritedAttribute(handle, elementPath, attributeName, &path));
  return tixiGetTextAttribute(handle, path, attributeName, text);
}

ReturnCode txutils::findInheritedAttribute(const TixiDocumentHandle handle,
                                           const char *elementPath,
                                           const char *attributeName,
                                           char **path) {

  // use Tixi function to check if everything is fine (tixiCheckAttribute
  // returns the same errors as this function)
  ReturnCode res = tixiCheckAttribute(handle, elementPath, attributeName);
  if (res != SUCCESS && res != ATTRIBUTE_NOT_FOUND) {
    return res;
  }

  std::string xPath = std::string(elementPath) + "/ancestor-or-self::*[@" +
                      std::string(attributeName) + "]";
  int n = -1;
  res = CHK_ERR(tixiXPathEvaluateNodeNumber(handle, xPath.c_str(), &n),
                ({SUCCESS, FAILED}));
  if (res == FAILED) {
    return ATTRIBUTE_NOT_FOUND;
  }
  return txutils::expectCode(
      tixiXPathExpressionGetXPath(handle, xPath.c_str(), n, path),
      errmsg(__func__, __LINE__));
}

char *txutils::parentPath(const char *elementPath) {
  std::string epath(elementPath);
  return strdup(epath.substr(0, epath.find_last_of("/")).c_str());
}

ReturnCode txutils::removeComments(const TixiDocumentHandle handle) {
  const char *xpath = "//comment()";
  int n = -1;

  ReturnCode res = tixiXPathEvaluateNodeNumber(handle, xpath, &n);
  if (res == FAILED) {
    return SUCCESS;
  } else if (res != SUCCESS) {
    return res;
  }

  for (int i = n; i > 0; i--) {
    char *path;
    RET_ERR(tixiXPathExpressionGetXPath(handle, xpath, i, &path));
    RET_ERR(tixiRemoveElement(handle, path));
  }
  return SUCCESS;
}

ReturnCode txutils::sortAttributes(TixiDocumentHandle h,
                                   const char *xPathExpression) {
  int n = 0;
  ReturnCode res = tixiXPathEvaluateNodeNumber(h, xPathExpression, &n);
  if (res == FAILED) {
    return SUCCESS;
  }
  RET_ERR(res);

  for (int i = 1; i <= n; i++) {
    char *path;
    RET_ERR(tixiXPathExpressionGetXPath(h, xPathExpression, i, &path));

    int nattr = 0;
    RET_ERR(tixiGetNumberOfAttributes(h, path, &nattr));
    std::map<std::string, std::string> attrmap;
    char *attrName;
    char *attrValue;
    for (int j = nattr; j > 0; j--) {
      RET_ERR(tixiGetAttributeName(h, path, j, &attrName));
      RET_ERR(tixiGetTextAttribute(h, path, attrName, &attrValue));
      RET_ERR(tixiRemoveAttribute(h, path, attrName));
      attrmap.insert({attrName, attrValue});
    }
    for (std::map<std::string, std::string>::iterator itr = attrmap.begin();
         itr != attrmap.end(); itr++) {
      RET_ERR(tixiAddTextAttribute(h, path, itr->first.c_str(),
                                   itr->second.c_str()));
    }
  }
  return SUCCESS;
}
