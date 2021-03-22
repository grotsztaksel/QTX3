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
                               <node_3>
                                   <node_4/>
                                   <node_4/>
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

TEST_F(TxUtilsTest, test_handle_error) {
  ReturnCode ret = ReturnCode::ALREADY_SAVED;
  try {
    // should not throw any error
    txutils::handle_error(ret);
    ASSERT_TRUE(true);
  } catch (std::runtime_error) {
    ASSERT_TRUE(false);
  }

  ret = ReturnCode::INVALID_HANDLE;
  try {
    // should throw runtime_error
    txutils::handle_error(ret);
    ASSERT_TRUE(false);
  } catch (std::runtime_error) {
    ASSERT_TRUE(true);
  }
}

TEST_F(TxUtilsTest, test_uniqueName) {
  EXPECT_STREQ(
      "node_4[3]",
      txutils::uniqueName("/root/child_2[1]/child_2[1]/node_3[1]/node_4[3]"));
}

TEST_F(TxUtilsTest, test_elementName) {
  EXPECT_STREQ(
      "node_4",
      txutils::elementName("/root/child_2[1]/child_2[1]/node_3[1]/node_4[3]"));
}

TEST_F(TxUtilsTest, test_elementNumber) {
  EXPECT_EQ(3, txutils::elementNumber(
                   "/root/child_2[1]/child_2[1]/node_3[1]/node_4[3]"));
}

TEST_F(TxUtilsTest, test_indexedPath) {
  TixiDocumentHandle handle;
  tixiImportFromString(this->xml, &handle);
  char *ipath;
  txutils::indexedPath(
      handle, "/root/child_2[1]/child_2[1]/node_3[1]/node_4[3]", 1, &ipath);

  EXPECT_STREQ("/*[1]/*[2]/*[1]/*[1]/*[4]", ipath);

  delete[] ipath;
}

TEST_F(TxUtilsTest, test_copy) {
  TixiDocumentHandle handle;
  tixiImportFromString(this->xml, &handle);

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
  tixiImportFromString(expected_stripped, &expected);
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
  tixiImportFromString(this->xml, &handle);
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
  ASSERT_EQ(SUCCESS, tixiImportFromString(this->xml, &handle));
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
                                    <celem/>
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
                                       <celem/>
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
                                  <celem/>
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

  std::regex re(R"(>\s*<)");
  std::string stripped = std::regex_replace(str_original, re, "><");
  char *xml = strdup(stripped.c_str());

  TixiDocumentHandle handle;
  TixiDocumentHandle handle2;
  ASSERT_EQ(tixiImportFromString(xml, &handle), SUCCESS);
  ASSERT_EQ(tixiImportFromString(xml, &handle2), SUCCESS);
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

  ASSERT_EQ(txutils::sort(handle, "/root/child/*", "attr"), SUCCESS);
  ASSERT_EQ(tixiExportDocumentAsString(handle, &xml), SUCCESS);

  ASSERT_STREQ(xml, expected);

  // 3. sort by name

  ASSERT_EQ(tixiExportDocumentAsString(byName, &expected), SUCCESS);
  ASSERT_EQ(txutils::sort(handle2, "/root/child/*"), SUCCESS);
  ASSERT_EQ(tixiExportDocumentAsString(handle2, &xml), SUCCESS);

  ASSERT_STREQ(xml, expected);
}
