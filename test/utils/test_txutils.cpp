#include "gtest/gtest.h"
#include "txutils.h"

class TxUtilsTest : public ::testing::Test {
 protected:
  TxUtilsTest();
  //  ~UtilsTest() override;

 protected:
  const char* xml = R"(<?xml version="1.0"?>
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
                       </root>                                   )";
};

TxUtilsTest::TxUtilsTest() {}

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
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
