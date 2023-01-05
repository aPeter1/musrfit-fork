/***************************************************************************

  PExpression.hpp

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

#ifndef _PEXPRESSION_HPP_
#define _PEXPRESSION_HPP_

///////////////////////////////////////////////////////////////////////////////
// Spirit v2.5 allows you to suppress automatic generation
// of predefined terminals to speed up complation. With
// BOOST_SPIRIT_NO_PREDEFINED_TERMINALS defined, you are
// responsible in creating instances of the terminals that
// you need (e.g. see qi::uint_type uint_ below).
#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Uncomment this if you want to enable debugging
// #define BOOST_SPIRIT_QI_DEBUG
///////////////////////////////////////////////////////////////////////////////

#include <boost/spirit/include/qi.hpp>
#include "PAst.hpp"
#include "PErrorHandler.hpp"
#include "PSkipper.hpp"
#include <vector>

namespace mupp { namespace parser
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    ///////////////////////////////////////////////////////////////////////////////
    //  The expression grammar
    ///////////////////////////////////////////////////////////////////////////////
    template <typename Iterator>
    struct PExpression : qi::grammar<Iterator, ast::expression(), PSkipper<Iterator> >
    {
        PExpression(PErrorHandler<Iterator>& error_handler);

        qi::symbols<char, ast::optoken> additive_op, multiplicative_op, unary_op;
        qi::symbols<char, ast::funid> fun_tok;

        qi::rule<Iterator, ast::expression(), PSkipper<Iterator> > expr;
        qi::rule<Iterator, ast::expression(), PSkipper<Iterator> > additive_expr;
        qi::rule<Iterator, ast::expression(), PSkipper<Iterator> > multiplicative_expr;
        qi::rule<Iterator, ast::operand(), PSkipper<Iterator> > unary_expr;
        qi::rule<Iterator, ast::operand(), PSkipper<Iterator> > primary_expr;
        qi::rule<Iterator, std::string(), PSkipper<Iterator> > identifier;
    };
}}

#endif // _PEXPRESSION_HPP_


