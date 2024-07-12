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

#include "util.h"

#include "metaenumvariable_p.h"
#include "metatype.h"

#include <QtCore/QStringList>
#include <QDebug>

#include <cfloat>

QString Cutelee::unescapeStringLiteral(const QString &input)
{
  return input.mid(1, input.size() - 2)
      .replace(QStringLiteral("\\\'"), QChar::fromLatin1('\''))
      .replace(QStringLiteral("\\\""), QChar::fromLatin1('"'))
      .replace(QStringLiteral("\\\\"), QChar::fromLatin1('\\'));
}

bool Cutelee::variantIsTrue(const QVariant &variant)
{

  if (!variant.isValid())
    return false;
  switch (variant.userType()) {
  case QMetaType::Bool: {
    return variant.toBool();
  }
  case QMetaType::Int: {
    return variant.value<int>() > 0;
  }
  case QMetaType::UInt: {
    return variant.value<uint>() > 0;
  }
  case QMetaType::LongLong: {
    return variant.value<qlonglong>() > 0;
  }
  case QMetaType::ULongLong: {
    return variant.value<qulonglong>() > 0;
  }
  case QMetaType::Double: {
    return variant.value<double>() > 0;
  }
  case QMetaType::Float: {
    return variant.value<float>() > 0;
  }
  case QMetaType::Char: {
    return variant.value<char>() > 0;
  }
  case QMetaType::QObjectStar: {
    auto obj = variant.value<QObject *>();
    if (!obj)
      return false;

    if (obj->property("__true__").isValid()) {
      return obj->property("__true__").toBool();
    }
    return true;
  }
  case QMetaType::QVariantList: {
    return !variant.value<QVariantList>().isEmpty();
  }
  case QMetaType::QVariantHash: {
    return !variant.value<QVariantHash>().isEmpty();
  }
  case QMetaType::QVariantMap: {
    return !variant.value<QVariantMap>().isEmpty();
  }
  }

  return !getSafeString(variant).get().isEmpty();
}

Cutelee::SafeString Cutelee::markSafe(const Cutelee::SafeString &input)
{
  auto sret = input;
  sret.setSafety(Cutelee::SafeString::IsSafe);
  return sret;
}

Cutelee::SafeString
Cutelee::markForEscaping(const Cutelee::SafeString &input)
{
  auto temp = input;
  if (input.isSafe() || input.needsEscape())
    return input;

  temp.setNeedsEscape(true);
  return temp;
}

Cutelee::SafeString Cutelee::getSafeString(const QVariant &input)
{
  if (input.userType() == qMetaTypeId<Cutelee::SafeString>()) {
    return input.value<Cutelee::SafeString>();
  } else {
    return input.toString();
  }
}

bool Cutelee::isSafeString(const QVariant &input)
{
  const auto type = input.userType();
  return ((type == qMetaTypeId<Cutelee::SafeString>())
          || type == QMetaType::QString);
}

static QList<int> getPrimitives()
{
  QList<int> primitives;
  primitives << qMetaTypeId<Cutelee::SafeString>() << QMetaType::QString
             << QMetaType::Bool << QMetaType::Int << QMetaType::Double
             << QMetaType::Float << QMetaType::QDate << QMetaType::QTime
             << QMetaType::QDateTime;
  return primitives;
}

bool Cutelee::supportedOutputType(const QVariant &input)
{
  static const auto primitives = getPrimitives();
  return primitives.contains(input.userType());
}

bool Cutelee::equals(const QVariant &lhs, const QVariant &rhs)
{
  // TODO: Redesign...

  // QVariant doesn't use operator== to compare its held data, so we do it
  // manually instead for SafeString.
  auto equal = false;
  if (lhs.userType() == qMetaTypeId<Cutelee::SafeString>()) {
    if (rhs.userType() == qMetaTypeId<Cutelee::SafeString>()) {
      equal = (lhs.value<Cutelee::SafeString>()
               == rhs.value<Cutelee::SafeString>());
    } else if (rhs.userType() == QMetaType::QString) {
      equal = (lhs.value<Cutelee::SafeString>() == rhs.toString());
    }
  } else if (rhs.userType() == qMetaTypeId<Cutelee::SafeString>()
             && lhs.userType() == QMetaType::QString) {
    equal = (rhs.value<Cutelee::SafeString>() == lhs.toString());
  } else if (rhs.userType() == qMetaTypeId<MetaEnumVariable>()) {
    if (lhs.userType() == qMetaTypeId<MetaEnumVariable>()) {
      equal = (rhs.value<MetaEnumVariable>() == lhs.value<MetaEnumVariable>());
    } else if (lhs.userType() == qMetaTypeId<int>()) {
      equal = (rhs.value<MetaEnumVariable>() == lhs.value<int>());
    }
  } else if (lhs.userType() == qMetaTypeId<MetaEnumVariable>()) {
    if (rhs.userType() == qMetaTypeId<int>()) {
      equal = (lhs.value<MetaEnumVariable>() == rhs.value<int>());
    }
  } else {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    equal = ((lhs == rhs) && (lhs.userType() == rhs.userType()));
#else
    equal = lhs == rhs;
#endif
  }
  return equal;
}

bool Cutelee::gt(const QVariant& lhs, const QVariant& rhs)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    return lhs > rhs;
#else
    return QVariant::compare(lhs, rhs) == QPartialOrdering::Greater;
#endif
}

bool Cutelee::gte(const QVariant& lhs, const QVariant& rhs)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    return lhs >= rhs;
#else
    return equals(lhs, rhs) || gt(lhs, rhs);
#endif
}

bool Cutelee::lt(const QVariant &lhs, const QVariant &rhs)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    return lhs < rhs;
#else
    return QVariant::compare(lhs, rhs) == QPartialOrdering::Less;
#endif
}


bool Cutelee::lte(const QVariant &lhs, const QVariant &rhs)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    return lhs <= rhs;
#else
    return equals(lhs, rhs) || lt(lhs, rhs);
#endif
}


std::pair<qreal,QString> Cutelee::calcFileSize(qreal size, int unitSystem, qreal multiplier)
{
  std::pair<qreal,QString> ret;

  int _unitSystem = unitSystem;

  if ((_unitSystem != 2) && (_unitSystem != 10)) {
      qWarning("%s", "Unrecognized file size unit system. Falling back to decimal unit system.");
      _unitSystem = 10;
  }

  if (size == 0.0) {
      ret.first = 0.0;
      ret.second = QStringLiteral("bytes");
      return ret;
  } else if ((size == 1.0) || (size == -1.0)) {
      ret.first = 1.0;
      ret.second = QStringLiteral("byte");
      return ret;
  }

  qreal _size = size * multiplier;

  const bool positiveValue = (_size > 0);

  if (!positiveValue) {
      _size *= -1;
  }

  static const QStringList binaryUnits({
                                           QStringLiteral("bytes"),
                                           QStringLiteral("KiB"),
                                           QStringLiteral("MiB"),
                                           QStringLiteral("GiB"),
                                           QStringLiteral("TiB"),
                                           QStringLiteral("PiB"),
                                           QStringLiteral("EiB"),
                                           QStringLiteral("ZiB"),
                                           QStringLiteral("YiB")
                                       });

  static const QStringList decimalUnits({
                                           QStringLiteral("bytes"),
                                           QStringLiteral("KB"),
                                           QStringLiteral("MB"),
                                           QStringLiteral("GB"),
                                           QStringLiteral("TB"),
                                           QStringLiteral("PB"),
                                           QStringLiteral("EB"),
                                           QStringLiteral("ZB"),
                                           QStringLiteral("YB")
                                       });

  int count = 0;
  const qreal baseVal = (_unitSystem == 10) ? 1000.0f : 1024.0f;
#if FLT_EVAL_METHOD == 2
  // Avoid that this is treated as long double, as the increased
  // precision breaks the comparison below.
  volatile qreal current = 1.0F;
#else
  qreal current = 1.0f;
#endif
  int units = decimalUnits.size();
  while (count < units) {
      current *= baseVal;
      if (_size < current) {
          break;
      }
      count++;
  }

  if (count >= units) {
      count = (units - 1);
  }

  qreal devider = current/baseVal;
  _size = _size/devider;

  if (!positiveValue) {
      _size *= -1.0;
  }

  ret.first = _size;
  ret.second = (_unitSystem == 10) ? decimalUnits.at(count) : binaryUnits.at(count);

  return ret;
}

Cutelee::SafeString Cutelee::toString(const QVariantList &list)
{
  QString output(QLatin1Char('['));
  auto it = list.constBegin();
  const auto end = list.constEnd();
  while (it != end) {
    const auto item = *it;
    if (isSafeString(item)) {
      output += QStringLiteral("u\'")
                + static_cast<QString>(getSafeString(item).get())
                + QLatin1Char('\'');
    }
    if ((item.userType() == qMetaTypeId<int>())
        || (item.userType() == qMetaTypeId<uint>())
        || (item.userType() == qMetaTypeId<double>())
        || (item.userType() == qMetaTypeId<float>())
        || (item.userType() == qMetaTypeId<long long>())
        || (item.userType() == qMetaTypeId<unsigned long long>())) {
      output += item.toString();
    }
    if (item.userType() == qMetaTypeId<QVariantList>()) {
      output
          += static_cast<QString>(toString(item.value<QVariantList>()).get());
    }
    if ((it + 1) != end)
      output += QStringLiteral(", ");
    ++it;
  }

  return output.append(QLatin1Char(']'));
}
