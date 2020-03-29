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

#ifndef CUTELEE_UTIL_P_H
#define CUTELEE_UTIL_P_H

#include "context.h"
#include "cutelee_templates_export.h"
#include "safestring.h"

#include <QtCore/QVariant>

/// @file util.h Utility functions used throughout %Cutelee.

namespace Cutelee
{

/**
  Returns whether the @p variant is evaluated to true.

  @see @ref truthiness
*/
CUTELEE_TEMPLATES_EXPORT bool variantIsTrue(const QVariant &variant);

/**
  Converts the @p input into its unescaped form.

  Double slashes are converted to single slashes, \\&quot is converted to &quot
  etc.
*/
CUTELEE_TEMPLATES_EXPORT QString unescapeStringLiteral(const QString &input);

/**
  Marks the @p input as safe.
*/
CUTELEE_TEMPLATES_EXPORT Cutelee::SafeString
markSafe(const Cutelee::SafeString &input);

/**
  Marks the @p input as requiring escaping.
*/
CUTELEE_TEMPLATES_EXPORT Cutelee::SafeString
markForEscaping(const Cutelee::SafeString &input);

/**
  Retrieves and returns a SafeString from the @p input.
*/
CUTELEE_TEMPLATES_EXPORT Cutelee::SafeString
getSafeString(const QVariant &input);

/**
  Returns whether @p input contains a SafeString.
*/
CUTELEE_TEMPLATES_EXPORT bool isSafeString(const QVariant &input);

/**
  Returns true if the type of @p input can be inserted into a rendered template
  directly.

  Note that lists, hashes and QObject*s can not be.
*/
CUTELEE_TEMPLATES_EXPORT bool supportedOutputType(const QVariant &input);

/**
  Compares @p lhs and @p rhs for equality. SafeStrings are compared as raw
  QStrings. Their safeness is not part of the comparison.

  @see QVariant::operator==
*/
CUTELEE_TEMPLATES_EXPORT bool equals(const QVariant &lhs, const QVariant &rhs);

#ifndef Q_QDOC
/**
  @internal
  Returns the @p list as a formatted string. This is for debugging purposes
  only.
*/
CUTELEE_TEMPLATES_EXPORT Cutelee::SafeString
toString(const QVariantList &list);
#endif
}

#endif
