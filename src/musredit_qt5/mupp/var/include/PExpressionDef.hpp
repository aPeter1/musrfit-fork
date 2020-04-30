/***************************************************************************

  PExpressionDef.hpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  Based on Joel de Guzman example on calc7,
  see https://github.com/boostorg/spirit

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2020 by Andreas Suter                                   *
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

#include "PExpression.hpp"
#include "PErrorHandler.hpp"
#include "PAnnotation.hpp"
#include <boost/spirit/include/phoenix_function.hpp>

namespace mupp { namespace parser
{
  template <typename Iterator>
  PExpression<Iterator>::PExpression(PErrorHandler<Iterator>& error_handler)
      : PExpression::base_type(expr)
  {
    qi::_1_type _1;
    qi::_2_type _2;
    qi::_3_type _3;
    qi::_4_type _4;

    qi::char_type char_;
    qi::double_type double_;
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

    ///////////////////////////////////////////////////////////////////////
    // Tokens
    additive_op.add
        ("+", ast::op_plus)
        ("-", ast::op_minus)
        ;

    multiplicative_op.add
        ("*", ast::op_times)
        ("/", ast::op_divide)
        ;

    unary_op.add
        ("+", ast::op_positive)
        ("-", ast::op_negative)
        ;

    fun_tok.add
        ("max", ast::fun_max)
        ("min", ast::fun_min)
        ("abs", ast::fun_abs)
        ("sin", ast::fun_sin)
        ("cos", ast::fun_cos)
        ("tan", ast::fun_tan)
        ("exp", ast::fun_exp)
        ("log", ast::fun_log)
        ("ln",  ast::fun_ln)
        ;

    ///////////////////////////////////////////////////////////////////////
    // Grammar
    expr =
        additive_expr.alias()
        ;

    additive_expr =
        multiplicative_expr
        >> *(additive_op > multiplicative_expr)
           ;

    multiplicative_expr =
        unary_expr
        >> *(multiplicative_op > unary_expr)
           ;

    unary_expr =
        primary_expr
        |   (unary_op > primary_expr)
        ;

    primary_expr =
        double_
        |   identifier
        |   fun_tok > '(' > expr > ')'
        |   "pow(" > expr > ',' > expr > ')'
        |   '(' > expr > ')'
        ;

    identifier =
        raw[lexeme['$' >> *(alnum | '_')]]
        ;

    // name all the rules
    additive_expr.name("additive_expr");
    multiplicative_expr.name("multiplicative_expr");
    unary_expr.name("unary_expr");
    primary_expr.name("primary_expr");
    identifier.name("identifier");

    // Debugging and error handling and reporting support.
    BOOST_SPIRIT_DEBUG_NODES(
      (expr)
      (additive_expr)
      (multiplicative_expr)
      (unary_expr)
      (primary_expr)
      (identifier)
     );

    // Error handling: on error in expr, call error_handler.
    on_error<fail>(expr,
         error_handler_function(error_handler)(
         "**ERROR** Expecting ", _4, _3));

    // Annotation: on success in primary_expr, call annotation.
    on_success(primary_expr,
         annotation_function(error_handler.iters)(_val, _1));
  }
}}


