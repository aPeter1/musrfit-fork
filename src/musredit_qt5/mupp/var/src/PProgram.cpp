/***************************************************************************

  PProgram.cpp

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

#include "PProgram.hpp"

#include <boost/foreach.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <set>

namespace mupp { namespace prog {
    void PVarHandler::SetValue(double dval, unsigned idx)
    {
       if (idx >= fValue.size()) {
         std::cerr << "**ERROR** SetValue: idx=" << idx << " is out-of-range." << std::endl;
         return;
       }
       fValue[idx] = dval;
    }

    void PVarHandler::SetError(double dval, unsigned idx)
    {
       if (idx >= fError.size()) {
         std::cerr << "**ERROR** SetError: idx=" << idx << " is out-of-range." << std::endl;
         return;
       }
       fError[idx] = dval;
    }

    bool PProgram::operator()(double x)
    {
      return true;
    }

    bool PProgram::operator()(ast::assignment const &x)
    {
      // check that the variable exists
      if (!find_var(x.lhs.name)) {
        error_handler(0, "Undeclared variable: " + x.lhs.name);
        return false;
      }

      if (!(*this)(x.rhs)) {
        std::cout << "**SEM ERROR** in var assignment of var: " << x.lhs.name << std::endl;
        std::cout << "              rhs is missing or has an error." << std::endl;
        return false;
      }

      return true;
    }

    bool PProgram::operator()(ast::expression const &x)
    {
      if (!boost::apply_visitor(*this, x.first))
        return false;

      BOOST_FOREACH(ast::operation const& oper, x.rest) {
        if (!(*this)(oper))
          return false;
      }

      return true;
    }

    bool PProgram::operator()(ast::function const &x)
    {
      switch (x.func_id) {
        case ast::fun_max:
        case ast::fun_min:
        case ast::fun_abs:
        case ast::fun_sin:
        case ast::fun_cos:
        case ast::fun_tan:
        case ast::fun_sinh:
        case ast::fun_cosh:
        case ast::fun_tanh:
        case ast::fun_asin:
        case ast::fun_acos:
        case ast::fun_atan:
        case ast::fun_exp:
        case ast::fun_log:
        case ast::fun_ln:
        case ast::fun_sqrt:
          break;
        default:
          BOOST_ASSERT(0);
          return false;
      }

      if (!(*this)(x.arg)) {
        std::cout << "**SEM ERROR** in function: arg is missing or has an error." << std::endl;
        return false;
      }

      return true;
    }

    bool PProgram::operator()(ast::operation const &x)
    {
      if (!boost::apply_visitor(*this, x.operand_))
        return false;

      switch (x.operator_) {
        case ast::op_plus:
          break;
        case ast::op_minus:
          break;
        case ast::op_times:
          break;
        case ast::op_divide:
          break;
        case ast::op_positive:
          break;
        case ast::op_negative:
          break;
        default:
          BOOST_ASSERT(0);
          return false;
      }

      return true;
    }

    bool PProgram::operator()(ast::power const &x)
    {
      if (!(*this)(x.base)) {
        std::cout << "**SEM ERROR** in power: base argument is missing or has an error." << std::endl;
        return false;
      }

      if (!(*this)(x.pow)) {
        std::cout << "**SEM ERROR** in power: power argument is missing or has an error." << std::endl;
        return false;
      }

      return true;
    }

    bool PProgram::operator()(ast::statement const &x)
    {
      return boost::apply_visitor(*this, x);
    }

    bool PProgram::operator()(ast::statement_list const &x)
    {
      BOOST_FOREACH(ast::statement const& s, x) {
        if (!(*this)(s))
          return false;
      }
      return true;
    }

    bool PProgram::operator()(ast::unary const &x)
    {
      if (!boost::apply_visitor(*this, x.operand_))
        return false;

      switch (x.operator_) {
        case ast::op_negative:
          break;
        case ast::op_positive:
          break;
        default:
          BOOST_ASSERT(0);
          return false;
      }

      return true;
    }

    bool PProgram::operator()(ast::variable const &x)
    {
      bool ok(false);
      std::string var_name;

      var_name = pos_to_var(x.name, ok);
      if (!ok) {
        error_handler(0, "Position variable out-of-range: " + x.name);
        return false;
      }

      if (!find_var(var_name)) {
        error_handler(0, "Undeclared variable: " + var_name);
        return false;
      }

      return true;
    }

    bool PProgram::operator()(ast::variable_declaration const &x)
    {
      if (find_var(x.lhs.name)) {
        error_handler(0, "Duplicate variable: " + x.lhs.name);
        return false;
      }

      if (x.rhs) { // if there's a RHS initializer
        bool r = (*this)(*x.rhs);
        if (r) { // don't add the variable if the RHS fails
          add_var(x.lhs.name);
        } else {
          std::cout << "**SEM ERROR** rhs of var decl, name: " << x.lhs.name << ", failed" << std::endl;
          return r;
        }
      } else {
        add_var(x.lhs.name);
      }

      return true;
    }

    void PProgram::add_predef_var_values(const std::string &name,
                                         std::vector<double> &val,
                                         std::vector<double> &err)
    {
      unsigned int idx;
      if (!find_var(name, idx)) {
        std::cerr << "**ERROR** couldn't find pre-def variable : " << name << std::endl;
        return;
      }

      fVariable[idx].SetValue(val);
      fVariable[idx].SetError(err);
    }

    void PProgram::add_var(std::string const& name)
    {
      PVarHandler var;
      var.SetName(name);
      fVariable.push_back(var);
      // add map var <-> pos
      std::size_t n = fVarPos.size();
      fVarPos[n] = name;
    }

    bool PProgram::find_var(std::string const &name)
    {
      unsigned int idx;
      return find_var(name, idx);
    }

    bool PProgram::find_var(std::string const &name, unsigned int &idx)
    {
      bool result=false;

      // remove leading '$' of identifier
      std::string id("");
      if (name[0] == '$')
        id = name.substr(1);
      else
        id = name;

      for (unsigned int i=0; i<fVariable.size(); i++) {
        if (id == fVariable[i].GetName()) {
          idx = i;
          result = true;
          break;
        }
      }

      return result;
    }

    std::string PProgram::pos_to_var(std::string const &name, bool &ok)
    {
      std::string result("??");
      ok = true;

      // remove leading '$' if present
      if (name[0] == '$')
        result = name.substr(1);
      else
        result = name;

      // check if number
      char *p;
      int num = strtol(result.c_str(), &p, 10);
      if (*p == 0) {
        if (num >= fVarPos.size())
          ok = false;
        else
          result = fVarPos[num];
      }

      return result;
    }


    std::vector<double> PProgEval::operator()(ast::nil)
    {
      std::vector<double> result;
      result.resize(fVariable[0].GetSize());

      for (unsigned int i=0; i<result.size(); i++)
        result[i] = 0.0;

      BOOST_ASSERT(0);
      return result;
    }

    std::vector<double> PProgEval::operator()(double x)
    {
      std::vector<double> result;
      result.resize(fVariable[0].GetSize());

      for (unsigned int i=0; i<result.size(); i++)
        result[i] = x;

      return result;
    }

    std::vector<double> PProgEval::operator()(ast::assignment const &x)
    {
      std::vector<double> result;
      result.resize(fVariable[0].GetSize());

      result = (*this)(x.rhs);

      // keep all data under <varName> not <varName>Err.
      // <varName>Err data go to <varName> error, whereas
      // <varName> data go to <varName> value
      std::string name = x.lhs.name;
      bool errorVal(false);

      // check for error variable
      std::size_t pos = name.find("Err");
      if (pos != std::string::npos) { // it is an error variable
        name = name.substr(0, pos);
        errorVal = true;
      }

      unsigned int idx = find_var(name);
      if (errorVal)
        fVariable[idx].SetError(result);
      else
        fVariable[idx].SetValue(result);

      return result;
    }

    std::vector<double> PProgEval::operator()(ast::expression const &x)
    {
      std::vector<double> vec = boost::apply_visitor(*this, x.first);

      BOOST_FOREACH(ast::operation const& oper, x.rest) {
        vec = (*this)(oper, vec);
      }
      return vec;
    }

    std::vector<double> PProgEval::operator()(ast::function const &x)
    {
      std::vector<double> vec = (*this)(x.arg);

      double dval;
      switch(x.func_id) {
        case ast::fun_max:
          dval = -1.0e10;
          for (unsigned int i=0; i<vec.size(); i++) {
            if (vec[i] > dval)
              dval = vec[i];
          }
          for (unsigned int i=0; i<vec.size(); i++)
            vec[i] = dval;
          break;
        case ast::fun_min:
          dval = 1.0e10;
          for (unsigned int i=0; i<vec.size(); i++) {
            if (vec[i] < dval)
              dval = vec[i];
          }
          for (unsigned int i=0; i<vec.size(); i++)
            vec[i] = dval;
          break;
        case ast::fun_abs:
          for (unsigned int i=0; i<vec.size(); i++)
            vec[i] = fabs(vec[i]);
          break;
        case ast::fun_sin:
          for (unsigned int i=0; i<vec.size(); i++)
            vec[i] = sin(vec[i]);
          break;
        case ast::fun_cos:
          for (unsigned int i=0; i<vec.size(); i++)
            vec[i] = cos(vec[i]);
          break;
        case ast::fun_tan:
          for (unsigned int i=0; i<vec.size(); i++)
            vec[i] = tan(vec[i]);
          break;
        case ast::fun_sinh:
          for (unsigned int i=0; i<vec.size(); i++)
            vec[i] = sinh(vec[i]);
          break;
        case ast::fun_cosh:
          for (unsigned int i=0; i<vec.size(); i++)
            vec[i] = cosh(vec[i]);
          break;
        case ast::fun_tanh:
          for (unsigned int i=0; i<vec.size(); i++)
            vec[i] = tanh(vec[i]);
          break;
        case ast::fun_asin:
          for (unsigned int i=0; i<vec.size(); i++)
            vec[i] = asin(vec[i]);
          break;
        case ast::fun_acos:
          for (unsigned int i=0; i<vec.size(); i++)
            vec[i] = acos(vec[i]);
          break;
        case ast::fun_atan:
          for (unsigned int i=0; i<vec.size(); i++)
            vec[i] = atan(vec[i]);
          break;
        case ast::fun_exp:
          for (unsigned int i=0; i<vec.size(); i++)
            vec[i] = exp(vec[i]);
          break;
        case ast::fun_log:
          for (unsigned int i=0; i<vec.size(); i++) {
            if (vec[i] <= 0.0) {
              BOOST_ASSERT(0);
              break;
            } else {
              vec[i] = log10(vec[i]);
            }
          }
          break;
        case ast::fun_ln:
          for (unsigned int i=0; i<vec.size(); i++) {
            if (vec[i] <= 0.0) {
              BOOST_ASSERT(0);
              break;
            } else {
              vec[i] = log(vec[i]);
            }
          }
          break;
        case ast::fun_sqrt:
          for (unsigned int i=0; i<vec.size(); i++) {
            if (vec[i] <= 0.0) {
              BOOST_ASSERT(0);
              break;
            } else {
              vec[i] = sqrt(vec[i]);
            }
          }
          break;
        default:
          BOOST_ASSERT(0);
          break;
      }

      return vec;
    }

    std::vector<double> PProgEval::operator()(ast::operation const &x, std::vector<double> lhs)
    {
      std::vector<double> rhs = boost::apply_visitor(*this, x.operand_);

      // make sure both vectors have the same size
      if (lhs.size() != rhs.size()) {
        BOOST_ASSERT(0);
        return lhs;
      }

      switch (x.operator_) {
        case ast::op_plus:
          for (unsigned int i=0; i<lhs.size(); i++)
            lhs[i] = lhs[i] + rhs[i];
          break;
        case ast::op_minus:
          for (unsigned int i=0; i<lhs.size(); i++)
            lhs[i] = lhs[i] - rhs[i];
          break;
        case ast::op_times:
          for (unsigned int i=0; i<lhs.size(); i++)
            lhs[i] = lhs[i] * rhs[i];
          break;
        case ast::op_divide:
          for (unsigned int i=0; i<lhs.size(); i++) {
            if (rhs[i] == 0.0) {
              BOOST_ASSERT(0);
              return lhs;
            } else {
              lhs[i] = lhs[i] / rhs[i];
            }
          }
          break;
        default:
          BOOST_ASSERT(0);
          return lhs;
      }

      return lhs;
    }

    std::vector<double> PProgEval::operator()(ast::power const &x)
    {
      std::vector<double> baseV = (*this)(x.base);
      std::vector<double> powV = (*this)(x.pow);

      if (baseV.size() != powV.size()) {
        BOOST_ASSERT(0);
        return baseV;
      }

      std::vector<double> result;
      result.resize(baseV.size());
      for (unsigned int i=0; i<baseV.size(); i++)
        result[i] = pow(baseV[i], powV[i]);

      return result;
    }

    std::vector<double> PProgEval::operator()(ast::statement const &x)
    {
      // as35: not-yet-implemented
      std::vector<double> result;
      result.resize(fVariable[0].GetSize());

      for (unsigned int i=0; i<result.size(); i++)
        result[i] = 0.0;

      boost::apply_visitor(*this, x);

      return result;
    }

    std::vector<double> PProgEval::operator()(ast::statement_list const &x)
    {
      // as35: not-yet-implemented
      std::vector<double> result;
      result.resize(fVariable[0].GetSize());

      for (unsigned int i=0; i<result.size(); i++)
        result[i] = 0.0;

      BOOST_FOREACH(ast::statement const& s, x) {
        (*this)(s);
      }

      return result;
    }

    std::vector<double> PProgEval::operator()(ast::unary const &x)
    {
      std::vector<double> vec = boost::apply_visitor(*this, x.operand_);

      if (x.operator_ == ast::op_negative) {
        for (unsigned int i=0; i<vec.size(); i++) {
          vec[i] *= -1.0;
        }
      }
      return vec;
    }

    std::vector<double> PProgEval::operator()(ast::variable const &x)
    {
      std::string name = x.name;
      bool errorVal(false);

      // check for injected error variable
      std::size_t pos = name.find("Err");
      if (pos != std::string::npos) { // it is an error variable
        name = name.substr(0, pos);
        errorVal = true;
      }

      unsigned idx = find_var(name);

      if (errorVal)
        return fVariable[idx].GetError();
      else
        return fVariable[idx].GetValue();
    }

    std::vector<double> PProgEval::operator()(ast::variable_declaration const &x)
    {
      std::vector<double> result;
      result.resize(fVariable[0].GetSize());

      // keep all data under <varName> not <varName>Err.
      // <varName>Err data go to <varName> error, whereas
      // <varName> data go to <varName> value
      std::string name = x.lhs.name;
      bool errorVal(false);

      // check for error variable
      std::size_t pos = name.find("Err");
      if (pos != std::string::npos) { // it is an error variable
        name = name.substr(0, pos);
        errorVal = true;
      }

      unsigned int idx = find_var(name);

      if (x.rhs) { // if there's a RHS initializer
        result = (*this)(*x.rhs);
      } else {
        // injected variable content
        if (errorVal)
          result = fVariable[idx].GetError();
        else
          result = fVariable[idx].GetValue();
      }

      if (errorVal)
        fVariable[idx].SetError(result);
      else
        fVariable[idx].SetValue(result);

      return result;
    }

    unsigned int PProgEval::find_var(std::string const &name)
    {
      // remove leading '$' of identifier
      std::string id("");
      if (name[0] == '$')
        id = name.substr(1);
      else
        id = name;

      unsigned idx = 0;
      for (unsigned int i=0; i<fVariable.size(); i++) {
        if (id == fVariable[i].GetName()) {
          idx = i;
          break;
        }
      }

      return idx;
    }

    PVarHandler PProgEval::getVar(const std::string name, bool &ok)
    {
      PVarHandler var = fVariable[0];
      ok = false;

      for (unsigned int i=0; i<fVariable.size(); i++) {
        if (fVariable[i].GetName() == name) {
          var = fVariable[i];
          ok = true;
          break;
        }
      }

      return var;
    }

    void PProgEval::print_result()
    {
      std::vector<double> val, err;
      for (unsigned int i=0; i<fVariable.size(); i++) {
        if (!strstr(fVariable[i].GetName().c_str(), "Err")) {
          std::cout << "+++++" << std::endl;
          std::cout << "var Name: " << fVariable[i].GetName() << std::endl;
          val = fVariable[i].GetValue();
          err = fVariable[i].GetError();
          for (unsigned int j=0; j<val.size(); j++) {
            if (j<err.size())
              std::cout << "  " << j << ": " << val[j] << " +- " << err[j] << std::endl;
            else
              std::cout << "  " << j << ": " << val[j] << std::endl;
          }
        }
      }
      std::cout << "+++++" << std::endl;
    }
}}
