/***************************************************************************

  PStatement.hpp

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

#ifndef _PSTATEMENT_HPP_
#define _PSTATEMENT_HPP_

#include "PExpression.hpp"

namespace mupp { namespace parser
{
  ///////////////////////////////////////////////////////////////////////////////
  //  The statement grammar
  ///////////////////////////////////////////////////////////////////////////////
  template <typename Iterator>
  struct PStatement : qi::grammar<Iterator, ast::statement_list(), PSkipper<Iterator> >
  {
    PStatement(PErrorHandler<Iterator>& error_handler);

    PExpression<Iterator> expr;
    qi::rule<Iterator, ast::statement_list(), PSkipper<Iterator> > statement_list;
    qi::rule<Iterator, ast::variable_declaration(), PSkipper<Iterator> > variable_declaration;
    qi::rule<Iterator, ast::assignment(), PSkipper<Iterator> > assignment;
    qi::rule<Iterator, std::string(), PSkipper<Iterator> > identifier;
  };
}}

#endif // _PSTATEMENT_HPP_


