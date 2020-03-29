/*
  This file is part of the Cutelee template system.

  Copyright (c) 2009,2010 Stephen Kelly <steveire@gmail.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either version
  2.1 of the Licence, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef CUTELEE_EXCEPTION_H
#define CUTELEE_EXCEPTION_H

#include "cutelee_templates_export.h"

#include <QtCore/QString>

#include <exception>

namespace Cutelee
{

/**
  Types of errors that can occur while using %Cutelee
*/
enum Error {
  NoError,
  EmptyVariableError,
  EmptyBlockTagError,
  InvalidBlockTagError,
  UnclosedBlockTagError,
  UnknownFilterError,
  TagSyntaxError,
  //   VariableSyntaxError,

  VariableNotInContext,
  ObjectReturnTypeInvalid,
  CompileFunctionError
};

/// @headerfile exception.h cutelee/exception.h

/**
  @brief An exception for use when implementing template tags.

  The **%Exception** class can be used when implementing
  AbstractNodeFactory::getNode. An exception can be thrown to indicate that
  the syntax of a particular tag is invalid.

  For example, the following template markup should throw an error because the
  include tag should have exactly one argument:

  @code
    <div>
      {% include %}
    </div>
  @endcode

  The corresponding implementation of IncludeNodeFactory::getNode is

  @code
    QStringList tagContents = smartSplit( tagContent );

    if ( tagContents.size() != 2 )
      throw Cutelee::Exception( TagSyntaxError,
        "Error: Include tag takes exactly one argument" );

    // The item at index 0 in the list is the tag name, "include"
    QString includeName = tagContents.at( 1 );
  @endcode

  @author Stephen Kelly <steveire@gmail.com>
*/
class CUTELEE_TEMPLATES_EXPORT Exception
{
public:
  /**
    Creates an exception for the error @p errorCode and the verbose
    message @p what
  */
  Exception(Error errorCode, const QString &what)
      : m_errorCode(errorCode), m_what(what)
  {
  }

  virtual ~Exception() throw() {}

#ifndef Q_QDOC
  /**
    @internal

    Returns the verbose message for the exception.
  */
  const QString what() const throw() { return m_what; }

  /**
    @internal

    Returns the error code for the exception.
  */
  Error errorCode() const { return m_errorCode; }
#endif

private:
  Error m_errorCode;
  QString m_what;
};
}

#endif
