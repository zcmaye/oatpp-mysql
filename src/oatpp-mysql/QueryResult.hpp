#ifndef oatpp_mysql_QueryResult_hpp
#define oatpp_mysql_QueryResult_hpp

#include "ConnectionProvider.hpp"
#include "mapping/Deserializer.hpp"
#include "mapping/ResultMapper.hpp"
#include "oatpp/orm/QueryResult.hpp"

namespace oatpp { namespace mysql {

/**
 * Implementation of &id:oatpp::orm::QueryResult;. for mysql.
 */
class QueryResult : public orm::QueryResult {
private:
  MYSQL_STMT* m_stmt;
  provider::ResourceHandle<orm::Connection> m_connection;
  std::shared_ptr<mapping::ResultMapper> m_resultMapper;
  mapping::ResultMapper::ResultData m_resultData;
  oatpp::String m_errorMessage;
public:

  QueryResult(MYSQL_STMT* stmt,
              const provider::ResourceHandle<orm::Connection>& connection,
              const std::shared_ptr<mapping::ResultMapper>& resultMapper,
              const std::shared_ptr<const data::mapping::TypeResolver>& typeResolver);

  ~QueryResult();

  provider::ResourceHandle<orm::Connection> getConnection() const override;

  bool isSuccess() const override;

  oatpp::String getErrorMessage() const override;

  v_int64 getPosition() const override;

  v_int64 getKnownCount() const override;

  bool hasMoreToFetch() const override;

  oatpp::Void fetch(const oatpp::Type* const type, v_int64 count) override;

};

}}

#endif //oatpp_mysql_QueryResult_hpp
