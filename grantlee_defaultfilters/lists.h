/*
  This file is part of the Grantlee template system.

  Copyright (c) 2009 Stephen Kelly <steveire@gmail.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License version 3 only, as published by the Free Software Foundation.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License version 3 for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef LISTFILTERS_H
#define LISTFILTERS_H

//krazy:excludeall=includes

#include "filter.h"

using namespace Grantlee;

class GRANTLEE_EXPORT JoinFilter : public Filter
{
public:
  JoinFilter();

  QVariant doFilter( const QVariant &input, const QVariant &argument = QVariant(), bool autoescape = false ) const;

  bool isSafe() const {
    return true;
  }

  bool needsAutoescape() const {
    return true;
  }
};

class GRANTLEE_EXPORT LengthFilter : public Filter
{
public:
  LengthFilter();

  QVariant doFilter( const QVariant &input, const QVariant &argument = QVariant(), bool autoescape = false ) const;

  bool isSafe() const {
    return true;
  }
};


class GRANTLEE_EXPORT LengthIsFilter : public Filter
{
public:
  LengthIsFilter();

  QVariant doFilter( const QVariant &input, const QVariant &argument = QVariant(), bool autoescape = false ) const;

  bool isSafe() const {
    return true;
  }
};

class GRANTLEE_EXPORT FirstFilter : public Filter
{
public:
  FirstFilter();

  QVariant doFilter( const QVariant &input, const QVariant &argument = QVariant(), bool autoescape = false ) const;
};

class GRANTLEE_EXPORT LastFilter : public Filter
{
public:
  LastFilter();

  QVariant doFilter( const QVariant &input, const QVariant &argument = QVariant(), bool autoescape = false ) const;
};

class GRANTLEE_EXPORT RandomFilter : public Filter
{
public:
  RandomFilter();

  QVariant doFilter( const QVariant &input, const QVariant &argument = QVariant(), bool autoescape = false ) const;

  bool isSafe() const {
    return true;
  }
};


class GRANTLEE_EXPORT SliceFilter : public Filter
{
public:
  SliceFilter();

  QVariant doFilter( const QVariant &input, const QVariant &argument = QVariant(), bool autoescape = false ) const;

  bool isSafe() const {
    return true;
  }
};

class GRANTLEE_EXPORT MakeListFilter : public Filter
{
public:

  QVariant doFilter( const QVariant &input, const QVariant &argument = QVariant(), bool autoescape = false ) const;

  bool isSafe() const {
    return true;
  }
};

class GRANTLEE_EXPORT UnorderedListFilter : public Filter
{
public:
  QVariant doFilter( const QVariant &input, const QVariant &argument = QVariant(), bool autoescape = false ) const;

  bool isSafe() const {
    return true;
  }

  bool needsAutoescape() const
  {
    return true;
  }
protected:
  SafeString processList( const QVariantList &list, int tabs, bool autoescape ) const;

};


#endif

