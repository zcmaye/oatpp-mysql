#include "Utils.hpp"

namespace oatpp { namespace mysql {

v_int64 Utils::getLastInsertRowId(const provider::ResourceHandle<orm::Connection>& connection) {
  auto c = std::static_pointer_cast<Connection>(connection.object);
  return mysql_insert_id(c->getHandle());
}

}}
