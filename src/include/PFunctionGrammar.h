/***************************************************************************

  PFunctionGrammer.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007 by Andreas Suter                                   *
 *   andreas.suter@psi.c                                                   *
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

#ifndef _PFUNCTIONGRAMMAR_H_
#define _PFUNCTIONGRAMMAR_H_

#include <iostream>
using namespace std;

//#define BOOST_SPIRIT_DEBUG

#include <boost/version.hpp>

#if BOOST_VERSION >= 103800
#  include <boost/spirit/include/classic_core.hpp>
#  include <boost/spirit/include/classic_ast.hpp>
   using namespace BOOST_SPIRIT_CLASSIC_NS;
#else
#  include <boost/spirit/core.hpp>
#  include <boost/spirit/tree/ast.hpp>
   using namespace boost::spirit;
#endif

typedef char const* iterator_t;
typedef tree_match<iterator_t> parse_tree_match_t;
typedef parse_tree_match_t::tree_iterator iter_t;

//--------------------------------------------------------------------------
/**
 *
 */
struct PFunctionGrammar : public grammar<PFunctionGrammar>
{
    static const int realID         = 1;
    static const int constPiID      = 2;
    static const int constGammaMuID = 3;
    static const int funLabelID     = 4;
    static const int parameterID    = 5;
    static const int mapID          = 6;
    static const int functionID     = 7;
    static const int factorID       = 8;
    static const int termID         = 9;
    static const int expressionID   = 10;
    static const int assignmentID   = 11;

    template <typename ScannerT>
    struct definition
    {
        definition(PFunctionGrammar const& /*self*/)
        {
            //  Start grammar definition
            real           =   leaf_node_d[ real_p ];

            const_pi       =   leaf_node_d[ str_p("PI") ];

            const_gamma_mu =   leaf_node_d[ str_p("GAMMA_MU") ];

            fun_label      =   leaf_node_d[ ( lexeme_d[ "FUN" >> +digit_p ] ) ];

            parameter      =   leaf_node_d[ ( lexeme_d[ "PAR" >> +digit_p ] ) |
                                            ( lexeme_d[ "-PAR" >> +digit_p ] ) ];

            map            =   leaf_node_d[ ( lexeme_d[ "MAP" >> +digit_p ] ) ];

            function       =   lexeme_d[ root_node_d[ str_p("COS")   ] >> ch_p('(') ] >> expression >> ch_p(')')
                           |   lexeme_d[ root_node_d[ str_p("SIN")   ] >> ch_p('(') ] >> expression >> ch_p(')')
                           |   lexeme_d[ root_node_d[ str_p("TAN")   ] >> ch_p('(') ] >> expression >> ch_p(')')
                           |   lexeme_d[ root_node_d[ str_p("COSH")  ] >> ch_p('(') ] >> expression >> ch_p(')')
                           |   lexeme_d[ root_node_d[ str_p("SINH")  ] >> ch_p('(') ] >> expression >> ch_p(')')
                           |   lexeme_d[ root_node_d[ str_p("TANH")  ] >> ch_p('(') ] >> expression >> ch_p(')')
                           |   lexeme_d[ root_node_d[ str_p("ACOS")  ] >> ch_p('(') ] >> expression >> ch_p(')')
                           |   lexeme_d[ root_node_d[ str_p("ASIN")  ] >> ch_p('(') ] >> expression >> ch_p(')')
                           |   lexeme_d[ root_node_d[ str_p("ATAN")  ] >> ch_p('(') ] >> expression >> ch_p(')')
                           |   lexeme_d[ root_node_d[ str_p("ACOSH") ] >> ch_p('(') ] >> expression >> ch_p(')')
                           |   lexeme_d[ root_node_d[ str_p("ASINH") ] >> ch_p('(') ] >> expression >> ch_p(')')
                           |   lexeme_d[ root_node_d[ str_p("ATANH") ] >> ch_p('(') ] >> expression >> ch_p(')')
                           |   lexeme_d[ root_node_d[ str_p("LOG")   ] >> ch_p('(') ] >> expression >> ch_p(')')
                           |   lexeme_d[ root_node_d[ str_p("LN")    ] >> ch_p('(') ] >> expression >> ch_p(')')
                           |   lexeme_d[ root_node_d[ str_p("EXP")   ] >> ch_p('(') ] >> expression >> ch_p(')')
                           ;

            factor         =   real
                           |   const_pi
                           |   const_gamma_mu
                           |   parameter
                           |   map
                           |   function
                           |   inner_node_d[ch_p('(') >> expression >> ch_p(')')]
                           ;

            term           =   factor >>
                               *(  (root_node_d[ch_p('*')] >> factor)
                                 | (root_node_d[ch_p('/')] >> factor)
                               );

            expression     =   term >>
                               *(  (root_node_d[ch_p('+')] >> term)
                                 | (root_node_d[ch_p('-')] >> term)
                               );

            assignment     =   (fun_label >> ch_p('=') >> expression);
            //  End grammar definition

            // turn on the debugging info.
            BOOST_SPIRIT_DEBUG_RULE(real);
            BOOST_SPIRIT_DEBUG_RULE(const_pi);
            BOOST_SPIRIT_DEBUG_RULE(const_gamma_mu);
            BOOST_SPIRIT_DEBUG_RULE(fun_label);
            BOOST_SPIRIT_DEBUG_RULE(parameter);
            BOOST_SPIRIT_DEBUG_RULE(map);
            BOOST_SPIRIT_DEBUG_RULE(function);
            BOOST_SPIRIT_DEBUG_RULE(factor);
            BOOST_SPIRIT_DEBUG_RULE(term);
            BOOST_SPIRIT_DEBUG_RULE(expression);
            BOOST_SPIRIT_DEBUG_RULE(assignment);
        }

        rule<ScannerT, parser_context<>, parser_tag<assignmentID> >   assignment;
        rule<ScannerT, parser_context<>, parser_tag<expressionID> >   expression;
        rule<ScannerT, parser_context<>, parser_tag<termID> >         term;
        rule<ScannerT, parser_context<>, parser_tag<factorID> >       factor;
        rule<ScannerT, parser_context<>, parser_tag<functionID> >     function;
        rule<ScannerT, parser_context<>, parser_tag<mapID> >          map;
        rule<ScannerT, parser_context<>, parser_tag<parameterID> >    parameter;
        rule<ScannerT, parser_context<>, parser_tag<funLabelID> >     fun_label;
        rule<ScannerT, parser_context<>, parser_tag<constGammaMuID> > const_gamma_mu;
        rule<ScannerT, parser_context<>, parser_tag<constPiID> >      const_pi;
        rule<ScannerT, parser_context<>, parser_tag<realID> >         real;

        rule<ScannerT, parser_context<>, parser_tag<assignmentID> > const&
        start() const { return assignment; }
    };
};

#endif // _PFUNCTIONGRAMMAR_H_
