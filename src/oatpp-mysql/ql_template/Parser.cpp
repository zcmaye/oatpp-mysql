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

#include "Parser.hpp"

#include "oatpp/data/stream/BufferStream.hpp"
#include "oatpp/utils/parser/ParsingError.hpp"

namespace oatpp { namespace mysql { namespace ql_template {

// create a variable which starts with ':' and ends with a non-alphanumeric character except '_' or '.'
// e.g. :my_var.val
data::share::StringTemplate::Variable Parser::parseIdentifier(utils::parser::Caret& caret) {
  data::share::StringTemplate::Variable result;
  result.posStart = caret.getPosition();
  if(caret.canContinueAtChar(':', 1)) {
    auto label = caret.putLabel();
    while(caret.canContinue()) {
      v_char8 a = *caret.getCurrData();
      // check if the character is allowed in identifier
      bool isAllowedChar = (a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z') || (a >= '0' && a <= '9') || (a == '_') || (a == '.');
      if(!isAllowedChar) {
        result.posEnd = caret.getPosition() - 1;
        result.name = label.toString();
        return result;
      }
      caret.inc();
    }
    result.name = label.toString();
  } else {
    caret.setError("Invalid identifier");
  }
  result.posEnd = caret.getPosition() - 1;
  return result;
}

// skip a string enclosed in quotes, e.g. "\'some string\'"
void Parser::skipStringInQuotes(utils::parser::Caret& caret) {

  bool opened = false;
  while(caret.canContinueAtChar('\'', 1)) {
    opened = true;
    if(caret.findChar('\'')) {
      caret.inc();
      opened = false;
    }
  }

  if(opened) {
    caret.setError("Invalid quote-enclosed string");
  }

}

// skip a string enclosed in dollars, e.g. "$some_string$"
void Parser::skipStringInDollars(utils::parser::Caret& caret) {

  if(caret.canContinueAtChar('$', 1)) {

    auto label = caret.putLabel();
    if(!caret.findChar('$')) {
      caret.setError("Invalid dollar-enclosed string");
      return;
    }
    caret.inc();
    auto term = label.toString();

    while(caret.canContinue()) {

      if(caret.findChar('$')) {
        caret.inc();
        if(caret.isAtText(term->data(), term->size(), true)) {
          return;
        }
      }

    }

  }

  caret.setError("Invalid dollar-enclosed string");

}

// find all variables in the given text and return a StringTemplate object
// e.g. "SELECT * FROM table WHERE id = :id AND name = 'John'" -> ':id' is a variable
data::share::StringTemplate Parser::parseTemplate(const oatpp::String& text) {
	utils::parser::Caret caret(text);

  std::vector<data::share::StringTemplate::Variable> variables;

  while(caret.canContinue()) {
    v_char8 c = *caret.getCurrData();

    switch(c) {

      case ':': {
        auto var = parseIdentifier(caret);
        if(var.name) {
          variables.push_back(var);
        }
      }
        break;

      case '\'': skipStringInQuotes(caret); break;
      case '$': skipStringInDollars(caret); break;

      default:
        caret.inc();

    }

  }

  if(caret.hasError()) {
    throw oatpp::utils::parser::ParsingError(caret.getErrorMessage(), caret.getErrorCode(), caret.getPosition());
  }

  return data::share::StringTemplate(text, std::move(variables));

}

}}}
