/*
  This file is part of the Cutelee template system.

  Copyright (c) 2010 Michael Jansen <kde@michael-jansen.biz>
  Copyright (c) 2010 Stephen Kelly <steveire@gmail.com>

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

#include "metatype.h"

#include "customtyperegistry_p.h"
#include "metaenumvariable_p.h"

#include <QtCore/QDebug>
#include <QJsonValue>
#include <QAssociativeIterable>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSequentialIterable>

using namespace Cutelee;

Q_GLOBAL_STATIC(CustomTypeRegistry, customTypes)

void Cutelee::MetaType::internalLock() { return customTypes()->mutex.lock(); }

void Cutelee::MetaType::internalUnlock()
{
  return customTypes()->mutex.unlock();
}

void Cutelee::MetaType::registerLookUpOperator(int id, LookupFunction f)
{
  Q_ASSERT(id > 0);
  Q_ASSERT(f);

  customTypes()->registerLookupOperator(id, f);
}

static QVariant doQobjectLookUp(const QObject *const object,
                                const QString &property)
{
  if (!object)
    return QVariant();
  if (property == QStringLiteral("children")) {
    const auto childList = object->children();
    if (childList.isEmpty())
      return QVariant();
    QVariantList children;

    auto it = childList.constBegin();
    const auto end = childList.constEnd();
    for (; it != end; ++it)
      children.append(QVariant::fromValue(*it));
    return children;
  }

  if (property == QStringLiteral("objectName")) {
    return object->objectName();
  }
  // Can't be const because of invokeMethod.
  auto metaObj = object->metaObject();

  QMetaProperty mp;
  for (auto i = 0; i < metaObj->propertyCount(); ++i) {
    // TODO only read-only properties should be allowed here.
    // This might also handle the variant messing I hit before.
    mp = metaObj->property(i);

    if (QString::fromUtf8(mp.name()) != property)
      continue;

    if (mp.isEnumType()) {
      MetaEnumVariable mev(mp.enumerator(), mp.read(object).value<int>());
      return QVariant::fromValue(mev);
    }

    return mp.read(object);
  }
  QMetaEnum me;
  for (auto i = 0; i < metaObj->enumeratorCount(); ++i) {
    me = metaObj->enumerator(i);

    if (QLatin1String(me.name()) == property) {
      MetaEnumVariable mev(me);
      return QVariant::fromValue(mev);
    }

    const auto value = me.keyToValue(property.toLatin1().constData());

    if (value < 0)
      continue;

    const MetaEnumVariable mev(me, value);

    return QVariant::fromValue(mev);
  }
  return object->property(property.toUtf8().constData());
}

static QVariant doJsonArrayLookUp(const QJsonArray &list,
                                  const QString &property)
{
    if (property == QLatin1String("count") || property == QLatin1String("size")) {
        return list.size();
    }

    bool ok = false;
    const int listIndex = property.toInt(&ok);
    if (!ok || listIndex >= list.size()) {
        return QVariant();
    }

    return list.at(listIndex).toVariant();
}

static QVariant doJsonObjectLookUp(const QJsonObject &obj,
                                   const QString &property)
{
    if (property == QLatin1String("count") || property == QLatin1String("size")) {
        return obj.size();
    }

    if (property == QLatin1String("items")) {
        QVariantList list;
        list.reserve(obj.size());
        for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
            list.push_back(QVariantList{it.key(), it.value().toVariant()});
        }
        return list;
    }

    if (property == QLatin1String("keys")) {
        return obj.keys();
    }

    if (property == QLatin1String("values")) {
        QVariantList list;
        list.reserve(obj.size());
        for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
            list.push_back(it.value().toVariant());
        }
        return list;
    }

    return obj.value(property).toVariant();
}

QVariant Cutelee::MetaType::lookup(const QVariant &object,
                                   const QString &property)
{
  if (object.canConvert<QObject *>()) {
    return doQobjectLookUp(object.value<QObject *>(), property);
  }
  if (object.userType() == QMetaType::QJsonDocument) {
      const auto doc = object.toJsonDocument();
      if (doc.isObject()) {
          return doJsonObjectLookUp(doc.object(), property);
      }
      if (doc.isArray()) {
          return doJsonArrayLookUp(doc.array(), property);
      }
      return QVariant();
  }
  if (object.userType() == QMetaType::QJsonValue) {
      const auto val = object.toJsonValue();

      switch (val.type()) {
      case QJsonValue::Bool:
          return val.toBool();
      case QJsonValue::Double:
          return val.toDouble();
      case QJsonValue::String:
          return val.toString();
      case QJsonValue::Array:
          return doJsonArrayLookUp(val.toArray(), property);
      case QJsonValue::Object:
          return doJsonObjectLookUp(val.toObject(), property);
      default:
          return QVariant();
      }
  }
  if (object.userType() == QMetaType::QJsonArray) {
      return doJsonArrayLookUp(object.toJsonArray(), property);
  }
  if (object.userType() == QMetaType::QJsonObject) {
      return doJsonObjectLookUp(object.toJsonObject(), property);
  }
  if (object.canConvert<QVariantList>()) {
    auto iter = object.value<QSequentialIterable>();
    if (property == QStringLiteral("size")
        || property == QStringLiteral("count")) {
      return iter.size();
    }

    auto ok = false;
    const auto listIndex = property.toInt(&ok);

    if (!ok || listIndex >= iter.size()) {
      return QVariant();
    }

    return iter.at(listIndex);
  }
  if (object.canConvert<QVariantHash>() ||
      object.canConvert<QVariantMap>()) {

    auto iter = object.value<QAssociativeIterable>();

    auto mappedValue = iter.value(property);
    if (mappedValue.isValid())
      return mappedValue;

    if (property == QStringLiteral("size")
        || property == QStringLiteral("count")) {
      return iter.size();
    }

    if (property == QStringLiteral("items")) {
      auto it = iter.begin();
      const auto end = iter.end();
      QVariantList list;
      for (; it != end; ++it) {
        list.push_back(QVariantList{it.key(), it.value()});
      }
      return list;
    }

    if (property == QStringLiteral("keys")) {
      auto it = iter.begin();
      const auto end = iter.end();
      QVariantList list;
      for (; it != end; ++it) {
        list.push_back(it.key());
      }
      return list;
    }

    if (property == QStringLiteral("values")) {
      auto it = iter.begin();
      const auto end = iter.end();
      QVariantList list;
      for (; it != end; ++it) {
        list.push_back(it.value());
      }
      return list;
    }

    return QVariant();
  }
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  auto mo = QMetaType::metaObjectForType(object.userType());
#else
  auto mo = QMetaType(object.userType()).metaObject();
#endif
  if (mo) {
    QMetaType mt(object.userType());
    if (mt.flags().testFlag(QMetaType::IsGadget)) {
      const auto idx = mo->indexOfProperty(property.toUtf8().constData());
      if (idx >= 0) {
        const auto mp = mo->property(idx);
        if (mp.isEnumType()) {
          MetaEnumVariable mev(mp.enumerator(), mp.readOnGadget(object.constData()).value<int>());
          return QVariant::fromValue(mev);
        }
        return mp.readOnGadget(object.constData());
      }

      QMetaEnum me;
      for (auto i = 0; i < mo->enumeratorCount(); ++i) {
        me = mo->enumerator(i);

        if (QLatin1String(me.name()) == property) {
          MetaEnumVariable mev(me);
          return QVariant::fromValue(mev);
        }

        const auto value = me.keyToValue(property.toLatin1().constData());

        if (value < 0) {
            continue;
        }

        MetaEnumVariable mev(me, value);
        return QVariant::fromValue(mev);
      }
    }
  }

  return customTypes()->lookup(object, property);
}

bool Cutelee::MetaType::lookupAlreadyRegistered(int id)
{
  return customTypes()->lookupAlreadyRegistered(id);
}
