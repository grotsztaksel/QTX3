#include <regex>
#include "gtest/gtest.h"
#include "txutils.h"

class TxUtilsTest : public ::testing::Test {
 protected:
  TxUtilsTest();
  ~TxUtilsTest() override;

 protected:
  const char* xml_raw = R"(<?xml version="1.0"?>
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
  char* xml;
};

TxUtilsTest::TxUtilsTest() {
  // Need to get rid of fake text elements from the string

  std::string rawstring(xml_raw);
  std::regex re(R"(>\s*<)");
  std::string stripped = std::regex_replace(rawstring, re, "><");
  xml = strdup(stripped.c_str());
}
TxUtilsTest::~TxUtilsTest() {
  delete xml;
}

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
  char* ipath;
  txutils::indexedPath(
      handle, "/root/child_2[1]/child_2[1]/node_3[1]/node_4[3]", 1, &ipath);

  EXPECT_STREQ("/*[1]/*[2]/*[1]/*[1]/*[4]", ipath);

  delete[] ipath;
}

TEST_F(TxUtilsTest, test_copy) {
  TixiDocumentHandle handle;
  tixiImportFromString(this->xml, &handle);

  ASSERT_EQ(SUCCESS, txutils::indentText(handle));
  const char* expected_clip_raw = R"(<?xml version="1.0"?>
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
  const char* expected_stripped = strdup(stripped.c_str());
  char* expected_clip;
  char* actual_clip;
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

TEST_F(TxUtilsTest, test_indentText) {
  const char* original_text =
      "\n                                                           Text O\n   "
      "                                    ";
  TixiDocumentHandle handle;
  ASSERT_EQ(SUCCESS, tixiImportFromString(this->xml, &handle));
  char* text_element;
  ASSERT_EQ(SUCCESS, tixiGetTextElement(
                         handle, "/root/child_2[1]/child_2[1]/node_3/node_5[1]",
                         &text_element));
  ASSERT_STREQ(original_text, text_element);

  const char* expected_text = "\n          Text O\n        ";

  ASSERT_EQ(SUCCESS,
            txutils::indentText(
                handle, "/root/child_2[1]/child_2[1]/node_3/node_5[1]"));

  ASSERT_EQ(SUCCESS, tixiGetTextElement(
                         handle, "/root/child_2[1]/child_2[1]/node_3/node_5[1]",
                         &text_element));

  ASSERT_STREQ(expected_text, text_element);
}

TEST_F(TxUtilsTest, test_indentation) {
  ASSERT_EQ(2, txutils::indentation());
}
