﻿#include "NumericTest.hpp"

#include "oatpp-mysql/orm.hpp"
#include "oatpp/json/ObjectMapper.hpp"

#include <limits>
#include <cstdio>

namespace oatpp { namespace test { namespace mysql { namespace types {

namespace {

#include OATPP_CODEGEN_BEGIN(DTO)

class NumsRow : public oatpp::DTO {

  DTO_INIT(NumsRow, DTO);

  DTO_FIELD(Int64, f_number);
  DTO_FIELD(Float64, f_decimal);
  DTO_FIELD(UInt8, f_number_unchar);
  DTO_FIELD(String, f_date);
  DTO_FIELD(String, f_datetime);
  DTO_FIELD(String, f_string);

};

#include OATPP_CODEGEN_END(DTO)

#include OATPP_CODEGEN_BEGIN(DbClient)

class MyClient : public oatpp::orm::DbClient {
public:

  MyClient(const std::shared_ptr<oatpp::orm::Executor>& executor)
    : oatpp::orm::DbClient(executor)
  {
    // create tables
  }

  QUERY(insertNumValues,
        "INSERT INTO test_numerics "
        "(f_number, f_decimal, f_number_unchar, f_date, f_datetime, f_string) "
        "VALUES "
        "(:row.f_number, :row.f_decimal, :row.f_number_unchar, :row.f_date, :row.f_datetime, :row.f_string);",
        PARAM(oatpp::Object<NumsRow>, row))

  QUERY(deleteAllNums,
        "DELETE FROM test_numerics;")

  QUERY(selectAllNums, "SELECT * FROM test_numerics;")

};

#include OATPP_CODEGEN_END(DbClient)

}

void NumericTest::onRun() {
  oatpp::mysql::ConnectionOptions options;
  options.host = "172.17.0.3";
  options.port = 3306;
  options.username = "root";
  options.password = "root";
  options.database = "test";

  OATPP_LOGd(TAG, "Connect to database '{}' on '{}:{}'", options.database->c_str(), options.host->c_str(), options.port);

  auto connectionProvider = std::make_shared<oatpp::mysql::ConnectionProvider>(options);
  auto executor = std::make_shared<oatpp::mysql::Executor>(connectionProvider);

  auto client = MyClient(executor);

  // {
  //   auto res = client.selectAllNums();
  //   if(res->isSuccess()) {
  //     OATPP_LOGd(TAG, "OK, knownCount={}, hasMore={}", res->getKnownCount(), res->hasMoreToFetch());
  //   } else {
  //     auto message = res->getErrorMessage();
  //     OATPP_LOGd(TAG, "Error, message={}", message->c_str());
  //   }

  //   auto dataset = res->fetch<oatpp::Vector<oatpp::Object<NumsRow>>>();

  //   oatpp::parser::json::mapping::ObjectMapper om;
  //   om.getSerializer()->getConfig()->useBeautifier = true;
  //   om.getSerializer()->getConfig()->enabledInterpretations = {"mysql"};

  //   auto str = om.writeToString(dataset);

  //   OATPP_LOGd(TAG, "res={}", str->c_str());

  //   OATPP_ASSERT(dataset->size() == 4);

  //   {
  //     auto row = dataset[0];
  //     OATPP_ASSERT(row->f_number == nullptr);
  //     OATPP_ASSERT(row->f_decimal == nullptr);
  //     OATPP_ASSERT(row->f_number_unchar == nullptr);
  //     OATPP_ASSERT(row->f_date == nullptr);
  //     OATPP_ASSERT(row->f_datetime == nullptr);
  //     OATPP_ASSERT(row->f_string == nullptr);
  //   }

  //   {
  //     auto row = dataset[1];
  //     OATPP_ASSERT(row->f_number == 0);
  //     OATPP_ASSERT(row->f_decimal == 0);
  //     OATPP_ASSERT(row->f_number_unchar == 0);
  //     OATPP_ASSERT(row->f_date == "2020-09-03");
  //     OATPP_ASSERT(row->f_datetime == "2020-09-03 23:59:59");
  //     OATPP_ASSERT(row->f_string == "hello");
  //   }

  //   {
  //     auto row = dataset[2];
  //     OATPP_ASSERT(row->f_number == 1);
  //     OATPP_ASSERT(row->f_decimal == 1);
  //     OATPP_ASSERT(row->f_number_unchar == 1);
  //     OATPP_ASSERT(row->f_date == "2020-09-03");
  //     OATPP_ASSERT(row->f_datetime == "2020-09-03 23:59:59");
  //     OATPP_ASSERT(row->f_string == "world");
  //   }

  //   {
  //     auto row = dataset[3];
  //     OATPP_ASSERT(row->f_number == 1);
  //     OATPP_ASSERT(row->f_decimal == 3.14);
  //     OATPP_ASSERT(row->f_number_unchar == 1);
  //     OATPP_ASSERT(row->f_date == "2020-09-03");
  //     OATPP_ASSERT(row->f_datetime == "2020-09-03 23:59:59");
  //     OATPP_ASSERT(row->f_string == "foo");
  //   }

  // }

  {
    auto res = client.deleteAllNums();
    if (res->isSuccess()) {
      OATPP_LOGd(TAG, "OK, knownCount={}, hasMore={}", res->getKnownCount(), res->hasMoreToFetch());
    } else {
      auto message = res->getErrorMessage();
      OATPP_LOGd(TAG, "Error, message={}", message->c_str());
    }

    OATPP_ASSERT(res->isSuccess());
  }

  {
    auto connection = client.getConnection();
    {
      auto row = NumsRow::createShared();
      row->f_number = nullptr;
      row->f_decimal = nullptr;
      row->f_number_unchar = nullptr;
      row->f_date = nullptr;
      row->f_datetime = nullptr;
      row->f_string = nullptr;
      client.insertNumValues(row, connection);
    }

    {
      auto row = NumsRow::createShared();
      row->f_number = 10;
      row->f_decimal = 10;
      row->f_number_unchar = 1;
      row->f_date = "2020-09-04";
      row->f_datetime = "2020-09-04 00:00:00";
      row->f_string = "bar";
      client.insertNumValues(row, connection);
    }

    OATPP_LOGd(TAG, "Insert 2 rows successfully");
  }

  {
    auto res = client.selectAllNums();
    if(res->isSuccess()) {
      OATPP_LOGd(TAG, "OK, knownCount={}, hasMore={}", res->getKnownCount(), res->hasMoreToFetch());
    } else {
      auto message = res->getErrorMessage();
      OATPP_LOGd(TAG, "Error, message={}", message->c_str());
    }

    auto dataset = res->fetch<oatpp::Vector<oatpp::Object<NumsRow>>>();

    oatpp::json::ObjectMapper om;
    om.serializerConfig().json.useBeautifier = true;
	om.serializerConfig().mapper.enabledInterpretations = { "mysql" };

    auto str = om.writeToString(dataset);

    OATPP_LOGd(TAG, "res={}", str->c_str());

    OATPP_ASSERT(dataset->size() == 2);

    {
      auto row = dataset[0];
      OATPP_ASSERT(row->f_number == nullptr);
      OATPP_ASSERT(row->f_decimal == nullptr);
      OATPP_ASSERT(row->f_number_unchar == nullptr);
      OATPP_ASSERT(row->f_date == nullptr);
      OATPP_ASSERT(row->f_datetime == nullptr);
      OATPP_ASSERT(row->f_string == nullptr);
    }

    {
      auto row = dataset[1];
      OATPP_ASSERT(row->f_number == 10);
      OATPP_ASSERT(row->f_decimal == 10);
      OATPP_ASSERT(row->f_number_unchar == 1);
      OATPP_ASSERT(row->f_date == "2020-09-04");
      OATPP_ASSERT(row->f_datetime == "2020-09-04 00:00:00");
      OATPP_ASSERT(row->f_string == "bar");
    }
  }

}

}}}}
