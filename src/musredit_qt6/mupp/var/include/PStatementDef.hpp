/***************************************************************************

  PStatementDef.hpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  Based on Joel de Guzman example on calc7,
  see https://github.com/boostorg/spirit

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2023 by Andreas Suter                                   *
 *   andreas.suter@psi.ch                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "PStatement.hpp"
#include "PErrorHandler.hpp"
#include "PAnnotation.hpp"

namespace mupp { namespace parser
{
  template <typename Iterator>
  PStatement<Iterator>::PStatement(PErrorHandler<Iterator>& error_handler)
      : PStatement::base_type(statement_list), expr(error_handler)
  {
    qi::_1_type _1;
    qi::_2_type _2;
    qi::_3_type _3;
    qi::_4_type _4;

    qi::_val_type _val;
    qi::raw_type raw;
    qi::lexeme_type lexeme;
    qi::alpha_type alpha;
    qi::alnum_type alnum;

    using qi::on_error;
    using qi::on_success;
    using qi::fail;
    using boost::phoenix::function;

    typedef function<mupp::PErrorHandler<Iterator> > error_handler_function;
    typedef function<mupp::PAnnotation<Iterator> > annotation_function;

    statement_list =
        +(variable_declaration | assignment)
        ;

    identifier =
        raw[lexeme[(alpha | '_') >> *(alnum | '_')]]
        ;

    variable_declaration =
        lexeme["var" >> !(alnum | '_')] // make sure we have whole words
        >   identifier
        >   -('=' > expr)
        ;

    assignment =
        identifier
        >   '='
        >   expr
        ;

    // name all the rules
    statement_list.name("statement_list");
    identifier.name("identifier");
    variable_declaration.name("variable_declaration");
    assignment.name("assignment");

    // Debugging and error handling and reporting support.
    BOOST_SPIRIT_DEBUG_NODES(
      (statement_list)
      (identifier)
      (variable_declaration)
      (assignment)
    );

    // Error handling: on error in statement_list, call error_handler.
    on_error<fail>(statement_list,
       error_handler_function(error_handler)(
       "**ERROR** Expecting ", _4, _3));

    // Annotation: on success in assignment, call annotation.
    on_success(assignment,
       annotation_function(error_handler.iters)(_val, _1));
  }
}}


