/***************************************************************************

  PAst.hpp

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

#ifndef _PAST_HPP_
#define _PAST_HPP_

#include <boost/config/warning_disable.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/optional.hpp>
#include <list>

namespace mupp { namespace ast
{
    ///////////////////////////////////////////////////////////////////////////
    //  The AST
    ///////////////////////////////////////////////////////////////////////////
    struct tagged
    {
      int id; // Used to annotate the AST with the iterator position.
              // This id is used as a key to a map<int, Iterator>
              // (not really part of the AST.)
    };

    enum optoken
    {
      op_plus,
      op_minus,
      op_times,
      op_divide,
      op_positive,
      op_negative,
    };

    enum funid
    {
      fun_max,
      fun_min,
      fun_abs,
      fun_sin,
      fun_cos,
      fun_tan,
      fun_sinh,
      fun_cosh,
      fun_tanh,
      fun_asin,
      fun_acos,
      fun_atan,
      fun_exp,
      fun_log,
      fun_ln,
      fun_sqrt
    };

    struct nil {};
    struct unary;
    struct expression;
    struct function;
    struct power;

    struct variable : tagged
    {
      variable(std::string const& name = "") : name(name) {}
      std::string name;
    };

    typedef boost::variant<
            nil
          , double
          , variable
          , boost::recursive_wrapper<function>
          , boost::recursive_wrapper<power>
          , boost::recursive_wrapper<unary>
          , boost::recursive_wrapper<expression>
        >
    operand;

    struct unary
    {
      optoken operator_;
      operand operand_;
    };

    struct operation
    {
      optoken operator_;
      operand operand_;
    };

    struct expression
    {
      operand first;
      std::list<operation> rest;
    };

    struct function
    {
      funid func_id;
      expression arg;
    };

    struct power
    {
      expression base;
      expression pow;
    };

    struct assignment
    {
      variable lhs;
      expression rhs;
    };

    struct variable_declaration
    {
      variable lhs;
      boost::optional<expression> rhs;
    };

    typedef boost::variant<
            variable_declaration
          , assignment>
    statement;

    typedef std::list<statement> statement_list;

    // print functions for debugging
    inline std::ostream& operator<<(std::ostream& out, nil) { out << "nil"; return out; }
    inline std::ostream& operator<<(std::ostream& out, variable const& var) { out << var.name; return out; }
}}

BOOST_FUSION_ADAPT_STRUCT(
    mupp::ast::unary,
    (mupp::ast::optoken, operator_)
    (mupp::ast::operand, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
    mupp::ast::operation,
    (mupp::ast::optoken, operator_)
    (mupp::ast::operand, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
    mupp::ast::expression,
    (mupp::ast::operand, first)
    (std::list<mupp::ast::operation>, rest)
)

BOOST_FUSION_ADAPT_STRUCT(
    mupp::ast::function,
    (mupp::ast::funid, func_id)
    (mupp::ast::expression, arg)
)

BOOST_FUSION_ADAPT_STRUCT(
    mupp::ast::power,
    (mupp::ast::expression, base)
    (mupp::ast::expression, pow)
)

BOOST_FUSION_ADAPT_STRUCT(
    mupp::ast::variable_declaration,
    (mupp::ast::variable, lhs)
    (boost::optional<mupp::ast::expression>, rhs)
)

BOOST_FUSION_ADAPT_STRUCT(
    mupp::ast::assignment,
    (mupp::ast::variable, lhs)
    (mupp::ast::expression, rhs)
)

#endif // _PAST_HPP_
