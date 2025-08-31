
#ifndef oatpp_mysql_Utils_hpp
#define oatpp_mysql_Utils_hpp

#include "QueryResult.hpp"

namespace oatpp { namespace mysql {

/**
 * Util methods.
 */
class Utils {
public:

  /**
   * Get row id following the last insert operation on the connection.
   * @param connection - &id:oatpp::mysql::Connection;.
   * @return
   */
  static v_int64 getLastInsertRowId(const provider::ResourceHandle<orm::Connection>& connection);

};

}}

#endif // oatpp_mysql_Utils_hpp
