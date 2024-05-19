#ifndef oatpp_test_mysql_ql_template_ParserTest_hpp
#define oatpp_test_mysql_ql_template_ParserTest_hpp

#include "oatpp-test/UnitTest.hpp"

namespace oatpp { namespace test { namespace mysql { namespace ql_template {

class ParserTest : public UnitTest {
public:
  ParserTest() : UnitTest("TEST[mysql::ql_template::ParserTest]") {}
  void onRun() override;
};

}}}}

#endif // oatpp_test_mysql_ql_template_ParserTest_hpp
