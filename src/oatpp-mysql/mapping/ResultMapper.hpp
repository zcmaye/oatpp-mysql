#ifndef oatpp_mysql_mapping_ResultMapper_hpp
#define oatpp_mysql_mapping_ResultMapper_hpp

#include "Deserializer.hpp"
#include "oatpp/data/mapping/TypeResolver.hpp"
#include "oatpp/Types.hpp"

#ifdef _WIN32
    #include "mysql.h"
#else
    #include "mysql/mysql.h"
#endif // _WIN32

namespace oatpp { namespace mysql { namespace mapping {

/**
 * Mapper from mysql result to oatpp objects.
 */
class ResultMapper {
public:

  /**
   * Result data. Get data row by row.
   */
  struct ResultData {

    /**
     * Constructor.
     * @param pStmt
     * @param pTypeResolver
     */
    ResultData(MYSQL_STMT* pStmt, const std::shared_ptr<const data::mapping::TypeResolver>& pTypeResolver);

    /**
     * Destructor. Free mysql resources.
     */
    ~ResultData();

    /**
     * mysql statement.
     */
    MYSQL_STMT* stmt;

    /**
     * &id:oatpp::data::mapping::TypeResolver;.
     */
    std::shared_ptr<const data::mapping::TypeResolver> typeResolver;

    /**
     * Names of columns.
     */
    std::vector<oatpp::String> colNames;

    /**
     * Column indices.
     */
    std::unordered_map<data::share::StringKeyLabel, v_int32> colIndices;

    /**
     * Column count.
     */
    v_int64 colCount;

    /**
     * Current row index.
     */
    v_int64 rowIndex;

    /**
     * Has more to read.
     */
    bool hasMore;

    /**
     * Is success.
     */
    bool isSuccess;

    /**
     * Bind results for cache.
     */
    std::vector<MYSQL_BIND> bindResults;

    /**
     * Meta results, the null represents that it is no result.
     */
    MYSQL_RES* metaResults;

  public:

    /**
     * Initialize column names and indices.
     */
    void init();

    /**
     * Move to next row.
     */
    void next();

    /**
     * Bind results for cache.
     */
    void bindResultsForCache();

  };

private:
  typedef oatpp::data::type::Type Type;
  typedef oatpp::Void (*ReadOneRowMethod)(ResultMapper*, ResultData*, const Type*);
  typedef oatpp::Void (*ReadRowsMethod)(ResultMapper*, ResultData*, const Type*, v_int64);
private:

  // Read one row methods
  static oatpp::Void readOneRowAsCollection(ResultMapper* _this, ResultData* dbData, const Type* type);
  static oatpp::Void readOneRowAsMap(ResultMapper* _this, ResultData* dbData, const Type* type);
  static oatpp::Void readOneRowAsObject(ResultMapper* _this, ResultData* dbData, const Type* type);

  // Read rows methods
  static oatpp::Void readRowsAsCollection(ResultMapper* _this, ResultData* dbData, const Type* type, v_int64 count);

private:
  Deserializer m_deserializer;
  std::vector<ReadOneRowMethod> m_readOneRowMethods;
  std::vector<ReadRowsMethod> m_readRowsMethods;
public:

  /**
   * Default constructor.
   */
  ResultMapper();

  /**
   * Set "read one row" method for class id.
   * @param classId
   * @param method
   */
  void setReadOneRowMethod(const data::type::ClassId& classId, ReadOneRowMethod method);

  /**
   * Set "read rows" method for class id.
   * @param classId
   * @param method
   */
  void setReadRowsMethod(const data::type::ClassId& classId, ReadRowsMethod method);

  /**
   * Read one row to oatpp object or collection. <br>
   * Allowed output type classes are:
   *
   * - &id:oatpp::Vector;
   * - &id:oatpp::List;
   * - &id:oatpp::UnorderedSet;
   * - &id:oatpp::Fields;
   * - &id:oatpp::UnorderedFields;
   * - &id:oatpp::Object;
   *
   * @param dbData
   * @param type
   * @return
   */
  oatpp::Void readOneRow(ResultData* dbData, const Type* type);

  /**
   * Read `count` of rows to oatpp collection. <br>
   * Allowed collections to store rows are:
   *
   * - &id:oatpp::Vector;
   * - &id:oatpp::List;
   * - &id:oatpp::UnorderedSet;.
   *
   * @param dbData
   * @param type
   * @param count
   * @return
   */
  oatpp::Void readRows(ResultData* dbData, const Type* type, v_int64 count);

  /**
   * Get result entries count in the case it's known.
   * @param dbData
   * @return - `[0..N]` - in case known. `-1` - otherwise.
   */
  v_int64 getKnownCount(ResultData* dbData)const;
};

}}}

#endif //oatpp_mysql_mapping_ResultMapper_hpp
