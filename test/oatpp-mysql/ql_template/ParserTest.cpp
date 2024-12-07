#include "ParserTest.hpp"

#include "oatpp-mysql/ql_template/Parser.hpp"

namespace oatpp { namespace test { namespace mysql { namespace ql_template {

namespace {

typedef oatpp::mysql::ql_template::Parser Parser;

}

void ParserTest::onRun() {

  {
    oatpp::String text = "SELECT * FROM table WHERE id = :id AND name = :name;";
    auto result = Parser::parseTemplate(text);

    OATPP_LOGd(TAG, "--- case1 ---");
    OATPP_LOGd(TAG, "sql='{}'", text->c_str());

    OATPP_ASSERT(result.getTemplateVariables().size() == 2);

    auto vars = result.getTemplateVariables();
    OATPP_LOGd(TAG, "variables:");
    OATPP_LOGd(TAG, "{}: [{} -> {}]", vars[0].name->c_str(), vars[0].posStart, vars[0].posEnd);
    OATPP_ASSERT(vars[0].posStart == 31);
    OATPP_ASSERT(vars[0].posEnd == 33);
    OATPP_ASSERT(vars[0].name == "id");

    OATPP_LOGd(TAG, "{}: [{} -> {}]", vars[1].name->c_str(), vars[1].posStart, vars[1].posEnd);
    OATPP_ASSERT(vars[1].posStart == 46);
    OATPP_ASSERT(vars[1].posEnd == 50);
    OATPP_ASSERT(vars[1].name == "name");
  }

  {
    // CASE 2: skip escaped single quotes
    oatpp::String text = "SELECT \'* FROM table WHERE id = :id\' AND name = :name;";
    auto result = Parser::parseTemplate(text);

    OATPP_LOGd(TAG, "--- case2 \'\' ---");
    OATPP_LOGd(TAG, "sql='{}'", text->c_str());

    OATPP_ASSERT(result.getTemplateVariables().size() == 1);

    auto vars = result.getTemplateVariables();
    OATPP_LOGd(TAG, "variables:");

    OATPP_LOGd(TAG, "{}: [{} -> {}]", vars[0].name->c_str(), vars[0].posStart, vars[0].posEnd);
    OATPP_ASSERT(vars[0].posStart == 48);
    OATPP_ASSERT(vars[0].posEnd == 52);
    OATPP_ASSERT(vars[0].name == "name");
  }

  {
    // CASE 3: skip escaped dollar signs and not parse variables inside dollar signs
    oatpp::String text = "SELECT * FROM table WHERE id = :id AND $:name_d$ $:name_d$ = :name;";
    auto result = Parser::parseTemplate(text);

    OATPP_LOGd(TAG, "--- case3 $$ ---");
    OATPP_LOGd(TAG, "sql='{}'", text->c_str());

    OATPP_ASSERT(result.getTemplateVariables().size() == 2);

    auto vars = result.getTemplateVariables();
    OATPP_LOGd(TAG, "variables:");
    OATPP_LOGd(TAG, "{}: [{} -> {}]", vars[0].name->c_str(), vars[0].posStart, vars[0].posEnd);
    OATPP_ASSERT(vars[0].posStart == 31);
    OATPP_ASSERT(vars[0].posEnd == 33);
    OATPP_ASSERT(vars[0].name == "id");

    OATPP_LOGd(TAG, "{}: [{} -> {}]", vars[1].name->c_str(), vars[1].posStart, vars[1].posEnd);
    OATPP_ASSERT(vars[1].posStart == 61);
    OATPP_ASSERT(vars[1].posEnd == 65);
    OATPP_ASSERT(vars[1].name == "name");
  }

}

}}}}
