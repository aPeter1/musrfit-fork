/***************************************************************************

  PAnnotation.hpp

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

#ifndef _PANNOTATION_HPP_
#define _PANNOTATION_HPP_

#include <map>
#include <boost/variant/apply_visitor.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/mpl/bool.hpp>
#include "PAst.hpp"

namespace mupp
{
  ///////////////////////////////////////////////////////////////////////////////
  //  The annotation handler links the AST to a map of iterator positions
  //  for the purpose of subsequent semantic error handling when the
  //  program is being compiled.
  ///////////////////////////////////////////////////////////////////////////////
  template <typename Iterator>
  struct PAnnotation
  {
    template <typename, typename>
    struct result { typedef void type; };

    std::vector<Iterator>& iters;
    PAnnotation(std::vector<Iterator>& iters) : iters(iters) {}

    struct set_id
    {
      typedef void result_type;

      int id;
      set_id(int id) : id(id) {}

      template <typename T>
      void operator()(T& x) const
      {
        this->dispatch(x, boost::is_base_of<ast::tagged, T>());
      }

      // This will catch all nodes except those inheriting from ast::tagged
      template <typename T>
      void dispatch(T& x, boost::mpl::false_) const
      {
        // (no-op) no need for tags
      }

      // This will catch all nodes inheriting from ast::tagged
      template <typename T>
      void dispatch(T& x, boost::mpl::true_) const
      {
        x.id = id;
      }
    };

    void operator()(ast::operand& ast, Iterator pos) const
    {
      int id = iters.size();
      iters.push_back(pos);
      boost::apply_visitor(set_id(id), ast);
    }

    void operator()(ast::assignment& ast, Iterator pos) const
    {
      int id = iters.size();
      iters.push_back(pos);
      ast.lhs.id = id;
    }
  };
}

#endif // _PANNOTATION_HPP_

