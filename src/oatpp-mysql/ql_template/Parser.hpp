﻿/***************************************************************************
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

#ifndef oatpp_mysql_ql_template_Parser_hpp
#define oatpp_mysql_ql_template_Parser_hpp

#include "oatpp/data/share/StringTemplate.hpp"
#include "oatpp/utils/parser/Caret.hpp"

namespace oatpp { namespace mysql { namespace ql_template {

/**
 * Query template parser.
 */
class Parser {
public:

  /**
   * Template extra info.
   */
  struct TemplateExtra {

    /**
     * Template name.
     */
    oatpp::String templateName;

    /**
     * Template text with parameters substituted to mysql parameter placeholders.
     */
    oatpp::String preparedTemplate;

    /**
     * Use prepared statement for this query.
     */
    bool prepare;
  };

private:
  static data::share::StringTemplate::Variable parseIdentifier(utils::parser::Caret& caret);
  static void skipStringInQuotes(utils::parser::Caret& caret);
  static void skipStringInDollars(utils::parser::Caret& caret);
public:

  /**
   * Parse query template.
   * @param text
   * @return - &id:oatpp::data::share::StringTemplate;.
   */
  static data::share::StringTemplate parseTemplate(const oatpp::String& text);

};

}}}

#endif // oatpp_mysql_ql_template_Parser_hpp
