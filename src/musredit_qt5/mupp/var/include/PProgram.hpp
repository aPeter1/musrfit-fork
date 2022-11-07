/***************************************************************************

  PProgram.hpp

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

#ifndef _PPROGRAM_HPP_
#define _PPROGRAM_HPP_

#include <string>
#include <vector>
#include <map>

#include "PAst.hpp"
#include "PErrorHandler.hpp"

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/phoenix/core.hpp>
#include <boost/phoenix/function.hpp>
#include <boost/phoenix/operator.hpp>

namespace mupp { namespace prog {
    ///////////////////////////////////////////////////////////////////////////
    //  Variable Handler
    ///////////////////////////////////////////////////////////////////////////
    class PVarHandler
    {
      public:
        PVarHandler() : fName("") {}

        void SetName(std::string name) { fName = name; }
        void SetValue(std::vector<double> &dval) { fValue = dval; }
        void SetValue(double dval, unsigned idx);
        void SetError(std::vector<double> &dval) { fError = dval; }
        void SetError(double dval, unsigned idx);

        std::string GetName() { return fName; }
        unsigned int GetSize() { return (fValue.size() == fError.size()) ? fValue.size() : 0; }
        std::vector<double> GetValue() { return fValue; }
        double GetValue(unsigned int idx) { return (idx < fValue.size()) ? fValue[idx] : 0; }
        std::vector<double> GetError() { return fError; }
        double GetError(unsigned int idx) { return (idx < fError.size()) ? fError[idx] : 0; }

      private:
        std::string fName;
        std::vector<double> fValue;
        std::vector<double> fError;
    };

    ///////////////////////////////////////////////////////////////////////////
    //  Program Semantic Analysis
    ///////////////////////////////////////////////////////////////////////////
    struct PProgram
    {
        typedef bool result_type;

        template <typename PErrorHandler>
        PProgram(PErrorHandler& error_handler_)
        {
          using namespace boost::phoenix::arg_names;
          namespace phx = boost::phoenix;
          using boost::phoenix::function;
           error_handler = function<PErrorHandler>(error_handler_)(
              "**ERROR** ", _2, phx::cref(error_handler_.iters)[_1]);
        }

        bool operator()(ast::nil) { BOOST_ASSERT(0); return false; }
        bool operator()(double x);
        bool operator()(ast::assignment const &x);
        bool operator()(ast::expression const &x);
        bool operator()(ast::function const &x);
        bool operator()(ast::operation const &x);
        bool operator()(ast::power const &x);
        bool operator()(ast::statement const &x);
        bool operator()(ast::statement_list const &x);
        bool operator()(ast::unary const &x);
        bool operator()(ast::variable const &x);
        bool operator()(ast::variable_declaration const &x);

        void add_predef_var_values(const std::string &name,
                                   std::vector<double> &val,
                                   std::vector<double> &err);

        void add_var(std::string const& name);
        bool find_var(std::string const &name);
        bool find_var(std::string const &name, unsigned int &idx);
        std::string pos_to_var(std::string const &name, bool &ok);

        std::vector<PVarHandler> getVars() { return fVariable; }

      private:
        std::vector<PVarHandler> fVariable;
        std::map<int, std::string> fVarPos;

        boost::function<
            void(int tag, std::string const& what)>
        error_handler;
    };

    ///////////////////////////////////////////////////////////////////////////
    //  Program Evaluation
    ///////////////////////////////////////////////////////////////////////////
    struct PProgEval
    {
        typedef std::vector<double> result_type;

        PProgEval(std::vector<PVarHandler> var) : fVariable(var) {}

        std::vector<double> operator()(ast::nil);
        std::vector<double> operator()(double x);
        std::vector<double> operator()(ast::assignment const &x);
        std::vector<double> operator()(ast::expression const &x);
        std::vector<double> operator()(ast::function const &x);
        std::vector<double> operator()(ast::operation const &x, std::vector<double> lhs);
        std::vector<double> operator()(ast::power const &x);
        std::vector<double> operator()(ast::statement const &x);
        std::vector<double> operator()(ast::statement_list const &x);
        std::vector<double> operator()(ast::unary const &x);
        std::vector<double> operator()(ast::variable const &x);
        std::vector<double> operator()(ast::variable_declaration const &x);

        PVarHandler getVar(const std::string name, bool &ok);
        void print_result();

      private:
        std::vector<PVarHandler> fVariable;

        unsigned int find_var(std::string const &name);
    };
}}

#endif // _PPROGRAM_HPP_
