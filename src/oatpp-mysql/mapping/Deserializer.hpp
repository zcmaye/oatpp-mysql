﻿#ifndef oatpp_mysql_mapping_Deserializer_hpp
#define oatpp_mysql_mapping_Deserializer_hpp

#include "oatpp/data/mapping/TypeResolver.hpp"
#include "oatpp/Types.hpp"

#ifdef _WIN32
    #include "mysql.h"
#else
    #include "mysql/mysql.h"
#endif // _WIN32


namespace oatpp { namespace mysql { namespace mapping {

/**
 * Mapper from mysql values to oatpp values.
 */
class Deserializer {
public:
  typedef oatpp::data::type::Type Type;
public:

  // Data structure to hold one column of data
  struct InData {

    InData(MYSQL_BIND* pBind, const std::shared_ptr<const data::mapping::TypeResolver>& pTypeResolver);

    MYSQL_BIND* bind;
    int col;
    int type;

    std::shared_ptr<const data::mapping::TypeResolver> typeResolver;

    int oid;
    bool isNull;

  };

public:
  typedef oatpp::Void (*DeserializerMethod)(const Deserializer*, const InData&, const Type*);
private:
  static v_int64 deInt(const InData& data);
private:
  std::vector<DeserializerMethod> m_methods;
public:

  Deserializer();

  void setDeserializerMethod(const data::type::ClassId& classId, DeserializerMethod method);

  oatpp::Void deserialize(const InData& data, const Type* type) const;

private:

  static oatpp::Void deserializeString(const Deserializer* _this, const InData& data, const Type* type);

  template<class IntWrapper>
  static oatpp::Void deserializeInt(const Deserializer* _this, const InData& data, const Type* type) {
    (void) _this;
    (void) type;

    if(data.isNull) {
      return IntWrapper();
    }
    auto value = deInt(data);
    return IntWrapper((typename IntWrapper::UnderlyingType) value);
  }

  static oatpp::Void deserializeFloat32(const Deserializer* _this, const InData& data, const Type* type);

  static oatpp::Void deserializeFloat64(const Deserializer* _this, const InData& data, const Type* type);

  static oatpp::Void deserializeAny(const Deserializer* _this, const InData& data, const Type* type);

  static oatpp::Void deserializeEnum(const Deserializer* _this, const InData& data, const Type* type);

};

}}}

#endif // oatpp_mysql_mapping_Deserializer_hpp
