#include "txutils.h"
#include "gtest/gtest.h"
#include <regex>

class TxUtilsTest : public ::testing::Test {
protected:
  TxUtilsTest();
  ~TxUtilsTest() override;

protected:
  const char *xml_raw = R"(<?xml version="1.0"?>
                       <root>
                           <child_1>
                               <child/>
                           </child_1>
                           <child_2 attr="foo" name="bar">
                               <child_2 attr="9">
                                   <node_3>
                                       <node_4/>
                                       <!-- some comment -->
                                       <node_4 attr="good"/>
                                       <node_5>
                                                           Text O
                                       </node_5>
                                       <node_4 name="node4">
                                                           Text
                                       </node_4>
                                   </node_3>
                                   <node_3 name="none"/>
                               </child_2>
                               <child_2/>
                               <node_3>
                                   <node_4/>
                               </node_3>
                           </child_2>
                           <child_2>
                               <!-- extra comment -->
                               <node_3>
                                   <node_4/>
                                   <node_4/>
                                   <!-- another comment -->
                                   <node_5/>
                               </node_3>
                           </child_2>
                       </root>)";
  char *xml;
};

TxUtilsTest::TxUtilsTest() {
  // Need to get rid of fake text elements from the string

  std::string rawstring(xml_raw);
  std::regex re(R"(>\s*<)");
  std::string stripped = std::regex_replace(rawstring, re, "><");
  xml = strdup(stripped.c_str());
}
TxUtilsTest::~TxUtilsTest() { delete xml; }

TEST_F(TxUtilsTest, test_uniqueName) {
  EXPECT_STREQ(
      "node_4[3]",
      txutils::uniqueName("/root/child_2[1]/child_2[1]/node_3[1]/node_4[3]"));
}

TEST_F(TxUtilsTest, test_elementName) {
  EXPECT_STREQ(
      "node_4",
      txutils::elementName("/root/child_2[1]/child_2[1]/node_3[1]/node_4[3]"));
  EXPECT_STREQ("node_4", txutils::elementName(
                             "/root/child_2[1]/child_2[1]/node_3[1]/node_4"));
}

TEST_F(TxUtilsTest, test_parentPath) {
  char *path =
      txutils::parentPath("/root/child_2[1]/child_2[1]/node_3[1]/node_4[3]");
  ASSERT_STREQ(path, "/root/child_2[1]/child_2[1]/node_3[1]");
}

TEST_F(TxUtilsTest, test_elementNumber) {
  EXPECT_EQ(3, txutils::elementNumber(
                   "/root/child_2[1]/child_2[1]/node_3[1]/node_4[3]"));
  EXPECT_EQ(1, txutils::elementNumber(
                   "/root/child_2[1]/child_2[1]/node_3[1]/node_4"));
}

TEST_F(TxUtilsTest, test_indexedPath) {
  TixiDocumentHandle handle;
  ASSERT_EQ(SUCCESS, tixiImportFromString(xml, &handle));
  char *ipath;
  txutils::indexedPath(
      handle, "/root/child_2[1]/child_2[1]/node_3[1]/node_4[3]", 1, &ipath);

  EXPECT_STREQ("/*[1]/*[2]/*[1]/*[1]/*[4]", ipath);

  delete[] ipath;
}

TEST_F(TxUtilsTest, test_copy) {
  TixiDocumentHandle handle;
  ASSERT_EQ(SUCCESS, tixiImportFromString(xml, &handle));

  ASSERT_EQ(SUCCESS, txutils::indentText(handle));
  const char *expected_clip_raw = R"(<?xml version="1.0"?>
<child_2 attr="9">
    <node_3>
        <node_4/>
        <node_4 attr="good"/>
        <node_5>
                                                           Text O
                                       </node_5>
        <node_4 name="node4">
                                                           Text
                                       </node_4>
    </node_3>
    <node_3 name="none"/>
</child_2>
)";
  std::string rawstring(expected_clip_raw);
  std::regex re(R"(>\s*<)");
  std::string stripped = std::regex_replace(rawstring, re, "><");
  const char *expected_stripped = strdup(stripped.c_str());
  char *expected_clip;
  char *actual_clip;
  TixiDocumentHandle expected;
  ASSERT_EQ(SUCCESS, tixiImportFromString(expected_stripped, &expected));
  TixiDocumentHandle clip;
  ASSERT_EQ(SUCCESS,
            txutils::copy(handle, "/root/child_2[1]/child_2[1]", &clip));
  ASSERT_EQ(SUCCESS, txutils::indentText(expected));
  ASSERT_EQ(SUCCESS, txutils::indentText(clip));
  ASSERT_EQ(SUCCESS, tixiExportDocumentAsString(expected, &expected_clip));
  ASSERT_EQ(SUCCESS, tixiExportDocumentAsString(clip, &actual_clip));

  ASSERT_STREQ(expected_clip, actual_clip);
}

TEST_F(TxUtilsTest, test_paste) {
  TixiDocumentHandle handle;
  ASSERT_EQ(SUCCESS, tixiImportFromString(xml, &handle));
  TixiDocumentHandle clip;
  ASSERT_EQ(SUCCESS,
            txutils::copy(handle, "/root/child_2[1]/child_2[1]", &clip));

  ASSERT_EQ(SUCCESS, txutils::paste(
                         handle, "/root/child_2[2]/node_3[1]/node_4[2]", clip));
  ASSERT_EQ(SUCCESS, txutils::paste(handle, "/root/child_2[2]", clip, 1));

  char *path;
  int i = 1;
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root", path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_1", path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_1/child", path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[1]", path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[1]/child_2[1]", path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[1]/child_2[1]/node_3[1]", path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[1]/child_2[1]/node_3[1]/node_4[1]", path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[1]/child_2[1]/node_3[1]/node_4[2]", path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[1]/child_2[1]/node_3[1]/node_5", path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[1]/child_2[1]/node_3[1]/node_4[3]", path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[1]/child_2[1]/node_3[2]", path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[1]/child_2[2]", path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[1]/node_3", path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[1]/node_3/node_4", path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[2]", path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[2]/child_2", path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[2]/node_3", path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[2]/node_3/node_4[1]", path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[2]/node_3/node_4[2]", path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[2]/node_3/node_4[2]/child_2", path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[2]/node_3/node_4[2]/child_2/node_3[1]", path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[2]/node_3/node_4[2]/child_2/node_3[1]/node_4[1]",
               path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[2]/node_3/node_4[2]/child_2/node_3[1]/node_4[2]",
               path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[2]/node_3/node_4[2]/child_2/node_3[1]/node_5",
               path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[2]/node_3/node_4[2]/child_2/node_3[1]/node_4[3]",
               path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[2]/node_3/node_4[2]/child_2/node_3[2]", path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[2]/node_3/node_5", path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[2]/node_3/node_3[1]", path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[2]/node_3/node_3[1]/node_4[1]", path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[2]/node_3/node_3[1]/node_4[2]", path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[2]/node_3/node_3[1]/node_5", path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[2]/node_3/node_3[1]/node_4[3]", path);
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(handle, "//*", i++, &path));
  ASSERT_STREQ("/root/child_2[2]/node_3/node_3[2]", path);
}

TEST_F(TxUtilsTest, test_indentText) {
  const char *original_text =
      "\n                                                           Text O\n   "
      "                                    ";
  TixiDocumentHandle handle;
  ASSERT_EQ(SUCCESS, tixiImportFromString(xml, &handle));
  char *text_element;
  ASSERT_EQ(SUCCESS, tixiGetTextElement(
                         handle, "/root/child_2[1]/child_2[1]/node_3/node_5[1]",
                         &text_element));
  ASSERT_STREQ(original_text, text_element);

  const char *expected_text = "\n          Text O\n        ";

  ASSERT_EQ(SUCCESS,
            txutils::indentText(
                handle, "/root/child_2[1]/child_2[1]/node_3/node_5[1]"));

  ASSERT_EQ(SUCCESS, tixiGetTextElement(
                         handle, "/root/child_2[1]/child_2[1]/node_3/node_5[1]",
                         &text_element));

  ASSERT_STREQ(expected_text, text_element);
}

TEST_F(TxUtilsTest, test_indentation) { ASSERT_EQ(2, txutils::indentation()); }

TEST_F(TxUtilsTest, test_sort) {
  std::string str_original = R"(<?xml version="1.0" encoding="utf-8"?>
                                <root>
                                  <child>
                                    <celem attr="9"/>
                                    <elem attr="6"/>
                                    <elem noattr="not included in sorting"/>
                                    <elem attr="5"/>
                                    <alem/>
                                    <elem attr="3"/>
                                    <elem attr="1"/>
                                    <helem/>
                                    <elem attr="8"/>
                                    <elem attr="7"/>
                                    <belem/>
                                    <elem attr="2"/>
                                    <elem attr="4"/>
                                  </child>
                                </root>)";

  std::string str_byAttribute = R"(<?xml version="1.0" encoding="utf-8"?>
                                   <root>
                                     <child>
                                       <celem attr="9"/>
                                       <elem attr="1"/>
                                       <elem noattr="not included in sorting"/>
                                       <elem attr="2"/>
                                       <alem/>
                                       <elem attr="3"/>
                                       <elem attr="4"/>
                                       <helem/>
                                       <elem attr="5"/>
                                       <elem attr="6"/>
                                       <belem/>
                                       <elem attr="7"/>
                                       <elem attr="8"/>
                                     </child>
                                   </root>)";
  std::string str_byName = R"(<?xml version="1.0" encoding="utf-8"?>
                              <root>
                                <child>
                                  <alem/>
                                  <belem/>
                                  <celem attr="9"/>
                                  <elem attr="6"/>
                                  <elem noattr="not included in sorting"/>
                                  <elem attr="5"/>
                                  <elem attr="3"/>
                                  <elem attr="1"/>
                                  <elem attr="8"/>
                                  <elem attr="7"/>
                                  <elem attr="2"/>
                                  <elem attr="4"/>
                                  <helem/>
                                </child>
                              </root>)";

  std::string str_alt_names_by_attribute =
      R"(<?xml version="1.0" encoding="utf-8"?>
         <root>
           <child>
             <elem attr="1"/>
             <elem attr="2"/>
             <elem noattr="not included in sorting"/>
             <elem attr="3"/>
             <alem/>
             <elem attr="4"/>
             <elem attr="5"/>
             <helem/>
             <elem attr="6"/>
             <elem attr="7"/>
             <belem/>
             <elem attr="8"/>
             <celem attr="9"/>
           </child>
         </root>)";

  std::regex re(R"(>\s*<)");
  std::string stripped = std::regex_replace(str_original, re, "><");
  char *xml = strdup(stripped.c_str());

  TixiDocumentHandle handle;
  TixiDocumentHandle handle2;
  TixiDocumentHandle handle3;
  ASSERT_EQ(tixiImportFromString(xml, &handle), SUCCESS);
  ASSERT_EQ(tixiImportFromString(xml, &handle2), SUCCESS);
  ASSERT_EQ(tixiImportFromString(xml, &handle3), SUCCESS);

  stripped = std::regex_replace(str_byAttribute, re, "><");
  xml = strdup(stripped.c_str());
  TixiDocumentHandle byAttr;
  ASSERT_EQ(tixiImportFromString(xml, &byAttr), SUCCESS);

  stripped = std::regex_replace(str_byName, re, "><");
  xml = strdup(stripped.c_str());
  TixiDocumentHandle byName;
  ASSERT_EQ(tixiImportFromString(xml, &byName), SUCCESS);

  // Make sure that there are no text elements
  int n = 0;
  ASSERT_EQ(tixiXPathEvaluateNodeNumber(handle, "//*[text()]", &n), FAILED);
  ASSERT_EQ(tixiXPathEvaluateNodeNumber(byName, "//*[text()]", &n), FAILED);
  ASSERT_EQ(tixiXPathEvaluateNodeNumber(byAttr, "//*[text()]", &n), FAILED);

  char *expected;

  // 1. xPath resolves to 0 elements - should not sort

  ASSERT_EQ(tixiExportDocumentAsString(handle, &expected), SUCCESS);
  ASSERT_EQ(txutils::sort(handle, "/root/child/elem[@otherattr]", "attr"),
            FAILED);
  ASSERT_EQ(tixiExportDocumentAsString(handle, &xml), SUCCESS);
  ASSERT_STREQ(xml, expected);

  // 2. sort by attribute
  ASSERT_EQ(tixiExportDocumentAsString(byAttr, &expected), SUCCESS);

  ASSERT_EQ(txutils::sort(handle, "/root/child/elem", "attr"), SUCCESS);
  ASSERT_EQ(tixiExportDocumentAsString(handle, &xml), SUCCESS);

  ASSERT_STREQ(xml, expected);

  // 3. sort by name

  ASSERT_EQ(tixiExportDocumentAsString(byName, &expected), SUCCESS);
  ASSERT_EQ(txutils::sort(handle2, "/root/child/*"), SUCCESS);
  ASSERT_EQ(tixiExportDocumentAsString(handle2, &xml), SUCCESS);

  ASSERT_STREQ(xml, expected);

  // 4. sort by attribute elements with different names

  stripped = std::regex_replace(str_alt_names_by_attribute, re, "><");
  xml = strdup(stripped.c_str());
  TixiDocumentHandle altNames_byAttr;

  ASSERT_EQ(tixiImportFromString(xml, &altNames_byAttr), SUCCESS);
  ASSERT_EQ(tixiXPathEvaluateNodeNumber(altNames_byAttr, "//*[text()]", &n),
            FAILED);
  ASSERT_EQ(tixiExportDocumentAsString(altNames_byAttr, &expected), SUCCESS);
  ASSERT_EQ(txutils::sort(handle3, "/root/child/*[self::elem or self::celem]",
                          "attr"),
            SUCCESS);
  ASSERT_EQ(tixiExportDocumentAsString(handle3, &xml), SUCCESS);
  tixiSaveCompleteDocument(handle3, "actual.xml");
  tixiSaveCompleteDocument(altNames_byAttr, "expected.xml");
  ASSERT_STREQ(xml, expected);
}

TEST_F(TxUtilsTest, test_sort_multiple_branches) {
  std::string EXP = R"(<?xml version="1.0"?>
                       <root>
                         <node attr="A">
                           <subnode attr="1">
                             <sortee attr="a"/>
                             <sortee attr="b"/>
                           </subnode>
                           <subnode attr="2">
                             <sortee attr="c"/>
                             <sortee attr="d"/>
                             <sortee attr="e"/>
                           </subnode>
                           <subnode attr="3">
                             <sortee attr="f"/>
                             <sortee attr="g"/>
                             <sortee attr="h"/>
                           </subnode>
                         </node>
                         <node attr="B">
                           <subnode attr="4">
                             <sortee attr="i"/>
                             <sortee attr="j"/>
                           </subnode>
                           <subnode  attr="5">
                             <sortee attr="k"/>
                             <sortee attr="l"/>
                             <sortee attr="m"/>
                           </subnode>
                           <subnode attr="6">
                             <sortee attr="n"/>
                             <sortee attr="o"/>
                           </subnode>
                         </node>
                       </root>)";

  std::regex re(R"(>\s*<)");
  std::string stripped = std::regex_replace(EXP, re, "><");
  char *exp = strdup(stripped.c_str());
  TixiDocumentHandle h_expected = -1;
  ASSERT_EQ(SUCCESS, tixiImportFromString(exp, &h_expected));
  std::string INI = R"(<?xml version="1.0"?>
                       <root>
                         <node attr="B">
                           <subnode attr="4">
                             <sortee attr="j"/>
                             <sortee attr="i"/>
                           </subnode>
                           <subnode attr="6">
                             <sortee attr="o"/>
                             <sortee attr="n"/>
                           </subnode>
                           <subnode  attr="5">
                             <sortee attr="l"/>
                             <sortee attr="k"/>
                             <sortee attr="m"/>
                           </subnode>
                         </node>
                         <node attr="A">
                           <subnode attr="3">
                             <sortee attr="g"/>
                             <sortee attr="h"/>
                             <sortee attr="f"/>
                           </subnode>
                           <subnode attr="2">
                             <sortee attr="d"/>
                             <sortee attr="e"/>
                             <sortee attr="c"/>
                           </subnode>
                           <subnode attr="1">
                             <sortee attr="b"/>
                             <sortee attr="a"/>
                           </subnode>
                         </node>
                       </root>)";

  stripped = std::regex_replace(EXP, re, "><");
  char *ini = strdup(stripped.c_str());
  TixiDocumentHandle h = -1;
  ASSERT_EQ(SUCCESS, tixiImportFromString(ini, &h));

  txutils::sort(h, "/root/node[1]/subnode[1]/sortee", "attr");
  txutils::sort(h, "/root/node[1]/subnode[2]/sortee", "attr");
  txutils::sort(h, "/root/node[1]/subnode[3]/sortee", "attr");
  txutils::sort(h, "/root/node[1]/subnode", "attr");
  txutils::sort(h, "/root/node[2]/subnode[1]/sortee", "attr");
  txutils::sort(h, "/root/node[2]/subnode[2]/sortee", "attr");
  txutils::sort(h, "/root/node[2]/subnode[3]/sortee", "attr");
  txutils::sort(h, "/root/node[2]", "attr");
  txutils::sort(h, "/root/node", "attr");

  char *actual;
  char *expected;
  ASSERT_EQ(SUCCESS, tixiExportDocumentAsString(h, &actual));
  ASSERT_EQ(SUCCESS, tixiExportDocumentAsString(h_expected, &expected));
  ASSERT_STREQ(actual, expected);
}

TEST_F(TxUtilsTest, test_expectCode_string) {
  ASSERT_EQ(txutils::expectCode(INVALID_HANDLE, "Test Error Message",
                                {SUCCESS, INVALID_HANDLE}),
            INVALID_HANDLE);

  try {
    txutils::expectCode(FAILED, "Test Error Message",
                        {SUCCESS, INVALID_HANDLE});
  } catch (std::runtime_error &e) {
    ASSERT_STREQ(e.what(), "Test Error Message\nTiXi ERROR CODE: 1");
  }
}

TEST_F(TxUtilsTest, test_expectCode_rc_specific_case) {
  TixiDocumentHandle h;
  tixiCreateDocument("root", &h);
  ASSERT_EQ(tixiCreateElement(h, "/root", "node1"), SUCCESS);
  ASSERT_EQ(tixiCreateElement(h, "/root", "directories"), SUCCESS);
  ASSERT_EQ(tixiCreateElement(h, "/root", "node2"), SUCCESS);
  ASSERT_EQ(tixiCreateElement(h, "/root", "node3"), SUCCESS);

  const char *basepath = "/root/directories/path[@abspath]";
  int n = 0;
  ReturnCode res = tixiXPathEvaluateNodeNumber(h, basepath, &n);
  ASSERT_EQ(res, FAILED);
  // The problem was that the argument list was shifted - expected message comes
  // before expected result list (here the empty "")
  ASSERT_EQ(txutils::expectCode(res, "", {SUCCESS, FAILED}), FAILED);
}

TEST_F(TxUtilsTest, test_expectCode_int) {
  ASSERT_EQ(txutils::expectCode(INVALID_HANDLE, 39, {SUCCESS, INVALID_HANDLE}),
            INVALID_HANDLE);

  try {
    txutils::expectCode(FAILED, 39, {SUCCESS, INVALID_HANDLE});
    ASSERT_TRUE(false);
  } catch (std::runtime_error &e) {
    ASSERT_STREQ(e.what(),
                 "Unhandled TiXi error in line 39\nTiXi ERROR CODE: 1");
  }
}

TEST_F(TxUtilsTest, test_excludeCode_string) {
  ASSERT_EQ(txutils::excludeCode(FAILED, "Test Error Message",
                                 {SUCCESS, INVALID_HANDLE}),
            FAILED);

  try {
    txutils::excludeCode(FAILED, "Test Error Message",
                         {SUCCESS, INVALID_HANDLE});
  } catch (std::runtime_error &e) {
    ASSERT_STREQ(e.what(), "Test Error Message\nTiXi ERROR CODE: 1");
  }
}

TEST_F(TxUtilsTest, test_excludeCode_int) {
  ASSERT_EQ(txutils::excludeCode(FAILED, 39, {SUCCESS, INVALID_HANDLE}),
            FAILED);

  try {
    txutils::excludeCode(SUCCESS, 39, {SUCCESS, INVALID_HANDLE});
  } catch (std::runtime_error &e) {
    ASSERT_STREQ(e.what(),
                 "Unhandled TiXi error in line 39\nTiXi ERROR CODE: 0");
  }
}

TEST_F(TxUtilsTest, test_cleanElementPath) {
  ASSERT_STREQ(txutils::cleanElementPath("/root/directories[2]/path[18]"),
               "/root/directories/path");

  ASSERT_STREQ(txutils::cleanElementPath("/root/directories/path"),
               "/root/directories/path");
  // This is an illegal use of the function
  try {
    txutils::cleanElementPath("/root/directories[@attr]");
    ASSERT_TRUE(false);
  } catch (std::runtime_error &e) {
    ASSERT_STREQ(e.what(), "cleanElementPath(): Function does not accep XPath "
                           "expressions! A full XML path is required"
                           "input argument: \"/root/directories[@attr]\"");
  }
}

TEST_F(TxUtilsTest, test_getElementNumber) {
  TixiDocumentHandle handle;
  ASSERT_EQ(SUCCESS, tixiImportFromString(xml, &handle));

  std::map<std::string, int> m{
      {"/root", 1},
      {"/root/child_1", 1},
      {"/root/child_1/child", 1},
      {"/root/child_2[1]", 2},
      {"/root/child_2[1]/child_2[1]", 1},
      {"/root/child_2[1]/child_2[1]/node_3[1]", 1},
      {"/root/child_2[1]/child_2[1]/node_3[1]/node_4[1]", 1},
      {"/root/child_2[1]/child_2[1]/node_3[1]/node_4[2]", 2},
      {"/root/child_2[1]/child_2[1]/node_3[1]/node_5", 3},
      {"/root/child_2[1]/child_2[1]/node_3[1]/node_4[3]", 4},
      {"/root/child_2[1]/child_2[1]/node_3[2]", 2},
      {"/root/child_2[1]/child_2[2]", 2},
      {"/root/child_2[1]/node_3", 3},
      {"/root/child_2[1]/node_3/node_4", 1},
      {"/root/child_2[2]", 3},
      {"/root/child_2[2]/node_3", 1},
      {"/root/child_2[2]/node_3/node_4[1]", 1},
      {"/root/child_2[2]/node_3/node_4[2]", 2},
      {"/root/child_2[2]/node_3/node_5", 3}};

  for (const auto &[key, expected] : m) {
    int actual = 0;
    auto ret = txutils::getElementNumber(handle, key.c_str(), &actual);
    ASSERT_EQ(SUCCESS, ret);
    ASSERT_EQ(expected, actual);
  }
}

TEST_F(TxUtilsTest, DISABLED_test_tixiXpath_by_attribute_relations) {
  TixiDocumentHandle h = -1;
  ASSERT_EQ(SUCCESS, tixiCreateDocument("root", &h));
  ASSERT_EQ(SUCCESS, tixiCreateElement(h, "/root", "child"));
  ASSERT_EQ(SUCCESS, tixiCreateElement(h, "/root", "child"));
  ASSERT_EQ(SUCCESS, tixiCreateElement(h, "/root", "child"));
  ASSERT_EQ(SUCCESS, tixiCreateElement(h, "/root", "child"));
  ASSERT_EQ(SUCCESS, tixiAddTextAttribute(h, "/root/child[1]", "attr", "a"));
  ASSERT_EQ(SUCCESS, tixiAddTextAttribute(h, "/root/child[2]", "attr", "b"));
  ASSERT_EQ(SUCCESS, tixiAddTextAttribute(h, "/root/child[3]", "attr", "c"));
  ASSERT_EQ(SUCCESS, tixiAddTextAttribute(h, "/root/child[4]", "attr", "d"));

  char *xPath;
  // This doesn't work with Tixi 3.1.1
  ASSERT_EQ(SUCCESS,
            tixiXPathExpressionGetXPath(h, "//child[@attr>\"b\"]", 1, &xPath));
}

TEST_F(TxUtilsTest, text_tixiXpathSearchOrder) {
  /* NOT A TXUTILS TEST
   *
   * This actually checks how Tixi behaves with XPathExpressions - if the order
   * of possible element names influences the order of search
   */
  TixiDocumentHandle h = -1;
  ASSERT_EQ(SUCCESS, tixiCreateDocument("root", &h));
  ASSERT_EQ(SUCCESS, tixiCreateElement(h, "/root", "child"));
  ASSERT_EQ(SUCCESS, tixiCreateElement(h, "/root/child", "subchild"));
  ASSERT_EQ(SUCCESS,
            tixiCreateElement(h, "/root/child/subchild", "grandchild"));

  int n = -1;
  char *path;

  const char *xpath = "//*[self::grandchild or self::subchild or self::child]";

  ASSERT_EQ(SUCCESS, tixiXPathEvaluateNodeNumber(h, xpath, &n));
  ASSERT_EQ(n, 3);

  // Apparently the order of the element names does not influence the order of
  // paths:
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(h, xpath, 1, &path));
  ASSERT_STREQ(path, "/root/child");
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(h, xpath, 2, &path));
  ASSERT_STREQ(path, "/root/child/subchild");
  ASSERT_EQ(SUCCESS, tixiXPathExpressionGetXPath(h, xpath, 3, &path));
  ASSERT_STREQ(path, "/root/child/subchild/grandchild");
}

TEST_F(TxUtilsTest, test_findInheritedAttribute) {
  TixiDocumentHandle h = -1;

  char *path;
  ASSERT_EQ(
      INVALID_HANDLE,
      txutils::findInheritedAttribute(
          h, "/root/child_2[1]/child_2[1]/node_3[1]/node_4[2]", "attr", &path));

  ASSERT_EQ(SUCCESS, tixiImportFromString(xml, &h));

  ASSERT_EQ(SUCCESS, txutils::findInheritedAttribute(
                         h, "/root/child_2[1]/child_2[1]/node_3[1]/node_4[2]",
                         "attr", &path));
  ASSERT_STREQ(path, "/root/child_2[1]/child_2[1]/node_3[1]/node_4[2]");
  ASSERT_EQ(ATTRIBUTE_NOT_FOUND,
            txutils::findInheritedAttribute(
                h, "/root/child_2[1]/child_2[1]/node_3[1]/node_4[2]",
                "nonexistent_attr", &path));

  ASSERT_EQ(SUCCESS, txutils::findInheritedAttribute(
                         h, "/root/child_2[1]/node_3/node_4", "attr", &path));
  ASSERT_STREQ(path, "/root/child_2[1]");

  ASSERT_EQ(INVALID_XPATH, txutils::findInheritedAttribute(
                               h, "this is't a valid path", "attr", &path));

  ASSERT_EQ(SUCCESS, txutils::findInheritedAttribute(
                         h, "/root/child_2[1]/child_2[1]/node_3[1]/node_4[1]",
                         "attr", &path));
  ASSERT_STREQ(path, "/root/child_2[1]/child_2[1]");
  ASSERT_EQ(
      ELEMENT_PATH_NOT_UNIQUE,
      txutils::findInheritedAttribute(
          h, "/root/child_2[1]/child_2[1]/node_3[1]/node_4", "attr", &path));

  ASSERT_EQ(ATTRIBUTE_NOT_FOUND, txutils::findInheritedAttribute(
                                     h, "/root/child_1/child", "attr", &path));

  ASSERT_EQ(ELEMENT_NOT_FOUND,
            txutils::findInheritedAttribute(h, "/root/child_1/mild_child",
                                            "attr", &path));
}

TEST_F(TxUtilsTest, test_getInheritedAttribute) {
  TixiDocumentHandle h = -1;
  ASSERT_EQ(SUCCESS, tixiImportFromString(xml, &h));

  char *attr;
  ASSERT_EQ(SUCCESS, txutils::getInheritedAttribute(
                         h, "/root/child_2[1]/child_2[1]/node_3[1]/node_4[2]",
                         "attr", &attr));
  ASSERT_STREQ(attr, "good");
  ASSERT_EQ(SUCCESS, txutils::getInheritedAttribute(
                         h, "/root/child_2[1]/node_3/node_4", "attr", &attr));
  ASSERT_STREQ(attr, "foo");
  ASSERT_EQ(SUCCESS, txutils::getInheritedAttribute(
                         h, "/root/child_2[1]/child_2[1]/node_3[1]/node_4[1]",
                         "attr", &attr));
  ASSERT_STREQ(attr, "9");
  ASSERT_EQ(ATTRIBUTE_NOT_FOUND, txutils::getInheritedAttribute(
                                     h, "/root/child_1/child", "attr", &attr));
}

TEST_F(TxUtilsTest, test_removeComments) {
  TixiDocumentHandle h = -1;
  ASSERT_EQ(SUCCESS, tixiImportFromString(xml, &h));
  char *name;
  ASSERT_EQ(SUCCESS, tixiGetChildNodeName(
                         h, "/root/child_2[1]/child_2[1]/node_3[1]", 2, &name));
  ASSERT_STREQ(name, "#comment");
  ASSERT_EQ(SUCCESS, tixiGetChildNodeName(h, "/root/child_2[2]", 1, &name));
  ASSERT_STREQ(name, "#comment");
  ASSERT_EQ(SUCCESS,
            tixiGetChildNodeName(h, "/root/child_2[2]/node_3", 3, &name));
  ASSERT_STREQ(name, "#comment");
  txutils::removeComments(h);
  ASSERT_EQ(SUCCESS, tixiGetChildNodeName(
                         h, "/root/child_2[1]/child_2[1]/node_3[1]", 2, &name));
  ASSERT_STRNE(name, "#comment");
  ASSERT_EQ(SUCCESS, tixiGetChildNodeName(h, "/root/child_2[2]", 1, &name));
  ASSERT_STRNE(name, "#comment");
  ASSERT_EQ(SUCCESS,
            tixiGetChildNodeName(h, "/root/child_2[2]/node_3", 3, &name));
  ASSERT_STRNE(name, "#comment");
}

TEST_F(TxUtilsTest, test_sortAttributes) {
  TixiDocumentHandle h = -1;
  ASSERT_EQ(SUCCESS, tixiCreateDocument("root", &h));
  ASSERT_EQ(SUCCESS, tixiCreateElement(h, "/root", "node"));

  const char *xpath = "/root/node";
  int i = 0;
  for (std::string attr : {"b", "e", "c", "a", "d"}) {
    ASSERT_EQ(SUCCESS, tixiAddTextAttribute(h, xpath, attr.c_str(),
                                            std::to_string(++i).c_str()));
  }

  ASSERT_EQ(SUCCESS, txutils::sortAttributes(h, xpath));

  char *attrName;
  char *attrValue;

  i = 0;
  ASSERT_EQ(SUCCESS, tixiGetAttributeName(h, xpath, ++i, &attrName));
  ASSERT_EQ(SUCCESS, tixiGetTextAttribute(h, xpath, attrName, &attrValue));
  ASSERT_STREQ("a", attrName);
  ASSERT_STREQ("4", attrValue);

  ASSERT_EQ(SUCCESS, tixiGetAttributeName(h, xpath, ++i, &attrName));
  ASSERT_EQ(SUCCESS, tixiGetTextAttribute(h, xpath, attrName, &attrValue));
  ASSERT_STREQ("b", attrName);
  ASSERT_STREQ("1", attrValue);

  ASSERT_EQ(SUCCESS, tixiGetAttributeName(h, xpath, ++i, &attrName));
  ASSERT_EQ(SUCCESS, tixiGetTextAttribute(h, xpath, attrName, &attrValue));
  ASSERT_STREQ("c", attrName);
  ASSERT_STREQ("3", attrValue);

  ASSERT_EQ(SUCCESS, tixiGetAttributeName(h, xpath, ++i, &attrName));
  ASSERT_EQ(SUCCESS, tixiGetTextAttribute(h, xpath, attrName, &attrValue));
  ASSERT_STREQ("d", attrName);
  ASSERT_STREQ("5", attrValue);

  ASSERT_EQ(SUCCESS, tixiGetAttributeName(h, xpath, ++i, &attrName));
  ASSERT_EQ(SUCCESS, tixiGetTextAttribute(h, xpath, attrName, &attrValue));
  ASSERT_STREQ("e", attrName);
  ASSERT_STREQ("2", attrValue);
}
