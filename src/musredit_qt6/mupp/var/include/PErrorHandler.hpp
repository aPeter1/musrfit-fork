/***************************************************************************

  PErrorHandler.hpp

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

#ifndef _PERROR_HANDLER_HPP_
#define _PERROR_HANDLER_HPP_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace mupp
{
  ///////////////////////////////////////////////////////////////////////////////
  //  The error handler
  ///////////////////////////////////////////////////////////////////////////////
  template <typename Iterator>
  struct PErrorHandler
  {
     template <typename, typename, typename>
     struct result { typedef void type; };

     PErrorHandler(Iterator first, Iterator last)
          : first(first), last(last) {}

     template <typename Message, typename What>
     void operator()(
       Message const& message,
       What const& what,
       Iterator err_pos) const
     {
       int line;
       Iterator line_start = get_pos(err_pos, line);
       const char *homeStr = getenv("HOME");
       char fln[1024];
       sprintf(fln, "%s/.musrfit/mupp/mupp_err.log", homeStr);
       std::ofstream fout(fln, std::ofstream::app);
       if (err_pos != last) {
         fout << message << what << ':' << std::endl;
         fout << get_line(line_start) << std::endl;
         for (; line_start != err_pos; ++line_start)
           fout << ' ';
         fout << "^~~" << std::endl;
       } else {
         fout << "**ERROR** Unexpected end of file. ";
         fout << message << what << " line " << line << std::endl;
       }
     }

     Iterator get_pos(Iterator err_pos, int& line) const
     {
       line = 1;
       Iterator i = first;
       Iterator line_start = first;
       while (i != err_pos) {
         bool eol = false;
         if (i != err_pos && *i == '\r') { // CR
           eol = true;
           line_start = ++i;
         }
         if (i != err_pos && *i == '\n') { // LF
            eol = true;
            line_start = ++i;
         }
         if (eol)
           ++line;
         else
           ++i;
       }
       return line_start;
    }

    std::string get_line(Iterator err_pos) const
    {
      Iterator i = err_pos;
      // position i to the next EOL
      while (i != last && (*i != '\r' && *i != '\n'))
        ++i;
      return std::string(err_pos, i);
    }

    Iterator first;
    Iterator last;
    std::vector<Iterator> iters;
  };
}

#endif //_PERROR_HANDLER_HPP_

