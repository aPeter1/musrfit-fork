/***************************************************************************

  PSkipper.hpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  Based on Joel de Guzman example on a mini_c,
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

#ifndef _PSKIPPER_HPP_
#define _PSKIPPER_HPP_

#include <boost/spirit/include/qi.hpp>

namespace mupp { namespace parser
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    ///////////////////////////////////////////////////////////////////////////////
    //  The skipper grammar
    ///////////////////////////////////////////////////////////////////////////////
    template <typename Iterator>
    struct PSkipper : qi::grammar<Iterator>
    {
        PSkipper() : PSkipper::base_type(start)
        {
            qi::char_type char_;
            qi::lit_type lit;
            qi::eol_type eol;
            ascii::space_type space;
            ascii::print_type print;

            single_line_comment = (lit('%') | lit('#') | lit("//")) >> *print >> eol;

            start =
                    space                               // tab/space/cr/lf
                |   "/*" >> *(char_ - "*/") >> "*/"     // C-style comments
                |   single_line_comment
                ;
        }

        qi::rule<Iterator> single_line_comment;
        qi::rule<Iterator> start;
    };
}}

#endif // _PSKIPPER_HPP_


