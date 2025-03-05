/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************************/

#include "Serializer.hpp"
#include "oatpp/base/Log.hpp"


namespace oatpp { namespace mysql { namespace mapping {

Serializer::Serializer() {

  m_methods.resize(data::type::ClassId::getClassCount(), nullptr);

  setSerializerMethod(data::type::__class::String::CLASS_ID, &Serializer::serializeString);
  setSerializerMethod(data::type::__class::Any::CLASS_ID, nullptr);

  setSerializerMethod(data::type::__class::Boolean::CLASS_ID, &Serializer::serializeBoolean);
  setSerializerMethod(data::type::__class::Int8::CLASS_ID, &Serializer::serializeInt8);
  setSerializerMethod(data::type::__class::UInt8::CLASS_ID, &Serializer::serializeUInt8);

  setSerializerMethod(data::type::__class::Int16::CLASS_ID, &Serializer::serializeInt16);
  setSerializerMethod(data::type::__class::UInt16::CLASS_ID, &Serializer::serializeUInt16);

  setSerializerMethod(data::type::__class::Int32::CLASS_ID, &Serializer::serializeInt32);
  setSerializerMethod(data::type::__class::UInt32::CLASS_ID, &Serializer::serializeUInt32);

  setSerializerMethod(data::type::__class::Int64::CLASS_ID, &Serializer::serializeInt64);
  setSerializerMethod(data::type::__class::UInt64::CLASS_ID, &Serializer::serializeUInt64);

  setSerializerMethod(data::type::__class::Float32::CLASS_ID, &Serializer::serializeFloat32);
  setSerializerMethod(data::type::__class::Float64::CLASS_ID, &Serializer::serializeFloat64);

  setSerializerMethod(data::type::__class::AbstractObject::CLASS_ID, nullptr);
  setSerializerMethod(data::type::__class::AbstractEnum::CLASS_ID, &Serializer::serializeEnum);

  setSerializerMethod(data::type::__class::AbstractVector::CLASS_ID, nullptr);
  setSerializerMethod(data::type::__class::AbstractList::CLASS_ID, nullptr);
  setSerializerMethod(data::type::__class::AbstractUnorderedSet::CLASS_ID, nullptr);

  setSerializerMethod(data::type::__class::AbstractPairList::CLASS_ID, nullptr);
  setSerializerMethod(data::type::__class::AbstractUnorderedMap::CLASS_ID, nullptr);

}

Serializer::~Serializer() {
  // free memory allocated for is_null pointers
  for(auto& bindParam : m_bindParams) {
    if(bindParam.is_null) {
      free(bindParam.is_null);
      bindParam.is_null = nullptr;
    }
  }
}

void Serializer::setSerializerMethod(const data::type::ClassId& classId, SerializerMethod method) {
  const v_uint32 id = classId.id;
  if(id >= m_methods.size()) {
    m_methods.resize(id + 1, nullptr);
  }
  m_methods[id] = method;
}

void Serializer::serialize(MYSQL_STMT* stmt, v_uint32 paramIndex, const oatpp::Void& polymorph) const {
  auto id = polymorph.getValueType()->classId.id;
  auto& method = m_methods[id];

  OATPP_LOGd("Serializer::serialize()", "classId={}, className={}, paramIndex={}, method={}", 
    id, polymorph.getValueType()->classId.name, paramIndex, method);

  if(method) {
    (*method)(this, stmt, paramIndex, polymorph);
  } else {
    throw std::runtime_error("[oatpp::mysql::mapping::Serializer::serialize()]: "
                             "Error. No serialize method for type '" + std::string(polymorph.getValueType()->classId.name) +
                             "'");
  }
}

std::vector<MYSQL_BIND>& Serializer::getBindParams() const {
  return m_bindParams;
}

void Serializer::setBindParam(MYSQL_BIND& bind, v_uint32 paramIndex) const {
  if (paramIndex >= m_bindParams.size()) {
    m_bindParams.resize(paramIndex + 1);
  }
  m_bindParams[paramIndex] = bind;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Serializer functions

void Serializer::serializeString(const Serializer* _this, MYSQL_STMT* stmt, v_uint32 paramIndex, const oatpp::Void& polymorph) {
  MYSQL_BIND bindParam;
  std::memset(&bindParam, 0, sizeof(bindParam));
  bindParam.buffer_type = MYSQL_TYPE_STRING;

  if(polymorph) {
    std::string *buff = static_cast<std::string*>(polymorph.get());

    bindParam.buffer = static_cast<void*>(const_cast<char*>(buff->c_str()));
    bindParam.buffer_length = static_cast<unsigned long>(buff->size());
    bindParam.is_null = 0;

    OATPP_LOGd("Serializer::serializeString()", "value='{}'", buff->c_str());
  } else {
    bindParam.is_null = static_cast<bool*>(malloc(sizeof(bool)));
    *bindParam.is_null = 1;
    OATPP_LOGd("Serializer::serializeString()", "null");
  }

  _this->setBindParam(bindParam, paramIndex);
}


void Serializer::serializeBoolean(const Serializer* _this, MYSQL_STMT* stmt, v_uint32 paramIndex, const oatpp::Void& polymorph)
{
  MYSQL_BIND bindParam;
  std::memset(&bindParam, 0, sizeof(bindParam));
  bindParam.buffer_type = MYSQL_TYPE_TINY;

  if(polymorph) {
    auto v = polymorph.cast<oatpp::Boolean>();

    bindParam.buffer = v.get();
    bindParam.buffer_length = 0;
    bindParam.is_null = 0;
  } else {
    bindParam.is_null = static_cast<bool*>(malloc(sizeof(bool)));
    *bindParam.is_null = 1;
  }

  _this->setBindParam(bindParam, paramIndex);
}

void Serializer::serializeInt8(const Serializer* _this, MYSQL_STMT* stmt, v_uint32 paramIndex, const oatpp::Void& polymorph) {
  MYSQL_BIND bindParam;
  std::memset(&bindParam, 0, sizeof(bindParam));
  bindParam.buffer_type = MYSQL_TYPE_TINY;

  if(polymorph) {
    auto v = polymorph.cast<oatpp::Int8>();

    bindParam.buffer = v.get();
    bindParam.buffer_length = 0;
    bindParam.is_null = 0;
  } else {
    bindParam.is_null = static_cast<bool*>(malloc(sizeof(bool)));
    *bindParam.is_null = 1;
  }

  _this->setBindParam(bindParam, paramIndex);
}

void Serializer::serializeUInt8(const Serializer* _this, MYSQL_STMT* stmt, v_uint32 paramIndex, const oatpp::Void& polymorph) {
  MYSQL_BIND bindParam;
  std::memset(&bindParam, 0, sizeof(bindParam));
  bindParam.buffer_type = MYSQL_TYPE_TINY;

  if(polymorph) {
    auto v = polymorph.cast<oatpp::UInt8>();

    bindParam.buffer = v.get();
    bindParam.buffer_length = 0;
    bindParam.is_null = 0;
  } else {
    bindParam.is_null = static_cast<bool*>(malloc(sizeof(bool)));
    *bindParam.is_null = 1;
  }

  _this->setBindParam(bindParam, paramIndex);
}

void Serializer::serializeInt16(const Serializer* _this, MYSQL_STMT* stmt, v_uint32 paramIndex, const oatpp::Void& polymorph) {
  MYSQL_BIND bindParam;
  std::memset(&bindParam, 0, sizeof(bindParam));
  bindParam.buffer_type = MYSQL_TYPE_SHORT;

  if(polymorph) {
    auto v = polymorph.cast<oatpp::Int16>();

    bindParam.buffer = v.get();
    bindParam.buffer_length = 0;
    bindParam.is_null = 0;
  } else {
    bindParam.is_null = static_cast<bool*>(malloc(sizeof(bool)));
    *bindParam.is_null = 1;
  }

  _this->setBindParam(bindParam, paramIndex);
}

void Serializer::serializeUInt16(const Serializer* _this, MYSQL_STMT* stmt, v_uint32 paramIndex, const oatpp::Void& polymorph) {
  MYSQL_BIND bindParam;
  std::memset(&bindParam, 0, sizeof(bindParam));
  bindParam.buffer_type = MYSQL_TYPE_SHORT;

  if(polymorph) {
    auto v = polymorph.cast<oatpp::UInt16>();

    bindParam.buffer = v.get();
    bindParam.buffer_length = 0;
    bindParam.is_null = 0;
  } else {
    bindParam.is_null = static_cast<bool*>(malloc(sizeof(bool)));
    *bindParam.is_null = 1;
  }

  _this->setBindParam(bindParam, paramIndex);
}

void Serializer::serializeInt32(const Serializer* _this, MYSQL_STMT* stmt, v_uint32 paramIndex, const oatpp::Void& polymorph) {
  MYSQL_BIND bindParam;
  std::memset(&bindParam, 0, sizeof(bindParam));
  bindParam.buffer_type = MYSQL_TYPE_LONG;

  if(polymorph) {
    auto v = polymorph.cast<oatpp::Int32>();

    bindParam.buffer = v.get();
    bindParam.buffer_length = 0;
    bindParam.is_null = 0;
  } else {
    bindParam.is_null = static_cast<bool*>(malloc(sizeof(bool)));
    *bindParam.is_null = 1;
  }

  _this->setBindParam(bindParam, paramIndex);
}

void Serializer::serializeUInt32(const Serializer* _this, MYSQL_STMT* stmt, v_uint32 paramIndex, const oatpp::Void& polymorph) {
  MYSQL_BIND bindParam;
  std::memset(&bindParam, 0, sizeof(bindParam));
  bindParam.buffer_type = MYSQL_TYPE_LONG;

  if(polymorph) {
    auto v = polymorph.cast<oatpp::UInt32>();

    bindParam.buffer = v.get();
    bindParam.buffer_length = 0;
    bindParam.is_null = 0;
  } else {
    bindParam.is_null = static_cast<bool*>(malloc(sizeof(bool)));
    *bindParam.is_null = 1;
  }

  _this->setBindParam(bindParam, paramIndex);
}

void Serializer::serializeInt64(const Serializer* _this, MYSQL_STMT* stmt, v_uint32 paramIndex, const oatpp::Void& polymorph) {
  MYSQL_BIND bindParam;
  std::memset(&bindParam, 0, sizeof(bindParam));
  bindParam.buffer_type = MYSQL_TYPE_LONGLONG;

  if(polymorph) {
    auto v = polymorph.cast<oatpp::Int64>();

    bindParam.buffer = v.get();
    bindParam.buffer_length = 0;
    bindParam.is_null = 0;
  } else {
    bindParam.is_null = static_cast<bool*>(malloc(sizeof(bool)));
    *bindParam.is_null = 1;
  }

  _this->setBindParam(bindParam, paramIndex);
}

void Serializer::serializeUInt64(const Serializer* _this, MYSQL_STMT* stmt, v_uint32 paramIndex, const oatpp::Void& polymorph) {
  MYSQL_BIND bindParam;
  std::memset(&bindParam, 0, sizeof(bindParam));
  bindParam.buffer_type = MYSQL_TYPE_LONGLONG;

  if(polymorph) {
    auto v = polymorph.cast<oatpp::UInt64>();

    bindParam.buffer = v.get();
    bindParam.buffer_length = 0;
    bindParam.is_null = 0;
  } else {
    bindParam.is_null = static_cast<bool*>(malloc(sizeof(bool)));
    *bindParam.is_null = 1;
  }

  _this->setBindParam(bindParam, paramIndex);
}

void Serializer::serializeFloat32(const Serializer* _this, MYSQL_STMT* stmt, v_uint32 paramIndex, const oatpp::Void& polymorph) {
  MYSQL_BIND bindParam;
  std::memset(&bindParam, 0, sizeof(bindParam));
  bindParam.buffer_type = MYSQL_TYPE_FLOAT;

  if(polymorph) {
    auto v = polymorph.cast<oatpp::Float32>();

    bindParam.buffer = v.get();
    bindParam.buffer_length = 0;
    bindParam.is_null = 0;
  } else {
    bindParam.is_null = static_cast<bool*>(malloc(sizeof(bool)));
    *bindParam.is_null = 1;
  }

  _this->setBindParam(bindParam, paramIndex);
}

void Serializer::serializeFloat64(const Serializer* _this, MYSQL_STMT* stmt, v_uint32 paramIndex, const oatpp::Void& polymorph) {
  MYSQL_BIND bindParam;
  std::memset(&bindParam, 0, sizeof(bindParam));
  bindParam.buffer_type = MYSQL_TYPE_DOUBLE;

  if(polymorph) {
    auto v = polymorph.cast<oatpp::Float64>();

    bindParam.buffer = v.get();
    bindParam.buffer_length = 0;
    bindParam.is_null = 0;
  } else {
    bindParam.is_null = static_cast<bool*>(malloc(sizeof(bool)));
    *bindParam.is_null = 1;
  }

  _this->setBindParam(bindParam, paramIndex);
}

void Serializer::serializeEnum(const Serializer* _this, MYSQL_STMT* stmt, v_uint32 paramIndex, const oatpp::Void& polymorph) {

  auto polymorphicDispatcher = static_cast<const data::type::__class::AbstractEnum::PolymorphicDispatcher*>(
    polymorph.getValueType()->polymorphicDispatcher
  );

  data::type::EnumInterpreterError e = data::type::EnumInterpreterError::OK;
  const auto& enumInterpretation = polymorphicDispatcher->toInterpretation(polymorph,true, e);

  if(e == data::type::EnumInterpreterError::OK) {
    _this->serialize(stmt, paramIndex, enumInterpretation);
    return;
  }

  switch(e) {
    case data::type::EnumInterpreterError::CONSTRAINT_NOT_NULL:
      throw std::runtime_error("[oatpp::mysql::mapping::Serializer::serializeEnum()]: Error. Enum constraint violated - 'NotNull'.");
    default:
      throw std::runtime_error("[oatpp::mysql::mapping::Serializer::serializeEnum()]: Error. Can't serialize Enum.");
  }

}

}}}
