#include "QueryResult.hpp"
#include "oatpp/base/Log.hpp"

namespace oatpp { namespace mysql {

QueryResult::QueryResult(MYSQL_STMT* stmt,
                         const provider::ResourceHandle<orm::Connection>& connection,
                         const std::shared_ptr<mapping::ResultMapper>& resultMapper,
                         const std::shared_ptr<const data::mapping::TypeResolver>& typeResolver)
    :m_stmt(stmt)
    ,m_connection(connection)
    ,m_resultMapper(resultMapper)
    ,m_resultData(stmt,typeResolver)
{
	m_resultData.init();    // initialize the information of all columns
	if (mysql_stmt_errno(m_stmt) != 0) {
		m_errorMessage = "Error executing statement: " + std::string(mysql_stmt_error(m_stmt));
	}
	else {
		m_errorMessage = std::string(mysql_stmt_error(m_stmt));
	}
}

QueryResult::~QueryResult() {
	mysql_stmt_close(m_stmt);
	OATPP_LOGd("QueryResult", "QueryResult destroyed");
}

provider::ResourceHandle<orm::Connection> QueryResult::getConnection() const {
  return m_connection;
}

bool QueryResult::isSuccess() const {
  return m_resultData.isSuccess;
}

oatpp::String QueryResult::getErrorMessage() const {
  return m_errorMessage;
}

v_int64 QueryResult::getPosition() const {
  return m_resultData.rowIndex;
}

v_int64 QueryResult::getKnownCount() const {
  return m_resultMapper->getKnownCount(const_cast<oatpp::mysql::mapping::ResultMapper::ResultData *>(&m_resultData));
}

bool QueryResult::hasMoreToFetch() const {
  return m_resultData.hasMore;
}

oatpp::Void QueryResult::fetch(const oatpp::Type* const type, v_int64 count) {
  // OATPP_LOGd("QueryResult::fetch", "Fetching {} rows, type_id={}, type_name={}", count, type->classId.id, type->classId.name);
  return m_resultMapper->readRows(&m_resultData, type, count);
}

}}
