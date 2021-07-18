/*
  This file is part of the Cutelee template system.

  Copyright (c) 2009,2010,2011 Stephen Kelly <steveire@gmail.com>

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

#include "lists.h"

#include "metatype.h"
#include "util.h"
#include "variable.h"

#include <QSequentialIterable>
#include <QtCore/QDateTime>

QVariant JoinFilter::doFilter(const QVariant &input, const QVariant &argument,
                              bool autoescape) const
{
  if (!input.canConvert<QVariantList>())
    return QVariant();

  auto iter = input.value<QSequentialIterable>();

  QString ret;
  for (auto it = iter.begin(); it != iter.end(); ++it) {
    const auto var = *it;
    auto s = getSafeString(var);
    if (autoescape)
      s = conditionalEscape(s);

    ret.append(s);
    if ((it + 1) != iter.end()) {
      auto argString = getSafeString(argument);
      ret.append(conditionalEscape(argString));
    }
  }
  return markSafe(ret);
}

QVariant LengthFilter::doFilter(const QVariant &input, const QVariant &argument,
                                bool autoescape) const
{
  Q_UNUSED(autoescape)
  Q_UNUSED(argument)
  if (input.canConvert<QVariantList>())
    return input.value<QSequentialIterable>().size();

  if (input.userType() == qMetaTypeId<SafeString>()
      || input.userType() == qMetaTypeId<QString>())
    return getSafeString(input).get().size();

  return QVariant();
}

QVariant LengthIsFilter::doFilter(const QVariant &input,
                                  const QVariant &argument,
                                  bool autoescape) const
{
  Q_UNUSED(autoescape)
  if (!input.isValid() || (input.userType() == qMetaTypeId<int>())
      || (input.userType() == qMetaTypeId<QDateTime>()))
    return QVariant();

  auto size = 0;
  if (input.canConvert<QVariantList>())
    size = input.value<QSequentialIterable>().size();
  else if (input.userType() == qMetaTypeId<SafeString>()
           || input.userType() == qMetaTypeId<QString>())
    size = getSafeString(input).get().size();

  bool ok;
  auto argInt = getSafeString(argument).get().toInt(&ok);

  if (!ok)
    return QVariant();

  return size == argInt;
}

QVariant FirstFilter::doFilter(const QVariant &input, const QVariant &argument,
                               bool autoescape) const
{
  Q_UNUSED(autoescape)
  Q_UNUSED(argument)

  if (!input.canConvert<QVariantList>())
    return QVariant();

  auto iter = input.value<QSequentialIterable>();

  if (iter.size() == 0)
    return QString();

  return *iter.begin();
}

QVariant LastFilter::doFilter(const QVariant &input, const QVariant &argument,
                              bool autoescape) const
{
  Q_UNUSED(autoescape)
  Q_UNUSED(argument)

  if (!input.canConvert<QVariantList>())
    return QVariant();

  auto iter = input.value<QSequentialIterable>();

  if (iter.size() == 0)
    return QString();

  return *(iter.end() - 1);
}

QVariant RandomFilter::doFilter(const QVariant &input, const QVariant &argument,
                                bool autoescape) const
{
  Q_UNUSED(autoescape)
  Q_UNUSED(argument)

  if (!input.canConvert<QVariantList>())
    return QVariant();

  auto varList = input.value<QVariantList>();

  if (varList.isEmpty())
    return QVariant();

  srand(QDateTime::currentDateTimeUtc().toMSecsSinceEpoch());
  auto rnd = rand() % varList.size();
  return varList.at(rnd);
}

QVariant SliceFilter::doFilter(const QVariant &input, const QVariant &argument,
                               bool autoescape) const
{
  Q_UNUSED(autoescape)
  auto argString = getSafeString(argument);
  auto splitterIndex = argString.get().indexOf(QLatin1Char(':'));
  QString inputString = getSafeString(input);
  if (inputString.isEmpty())
    return QVariant();

  if (splitterIndex >= 0) {
    auto left = argString.get().left(splitterIndex).get().toInt();
    auto right = argString.get().right(splitterIndex).get().toInt();
    if (right < 0) {
      right = inputString.size() + right;
    }
    return inputString.mid(left, right);
  } else {
    return QString(inputString.at(argument.value<int>()));
  }
}

QVariant MakeListFilter::doFilter(const QVariant &_input,
                                  const QVariant &argument,
                                  bool autoescape) const
{
  Q_UNUSED(autoescape)
  Q_UNUSED(argument)
  if (_input.userType() == qMetaTypeId<QVariantList>())
    return _input;
  if (_input.canConvert<QVariantList>())
    return _input.value<QVariantList>();

  auto input = _input;

  if (input.userType() == qMetaTypeId<int>())
    input.convert(QVariant::String);

  if (input.userType() == qMetaTypeId<SafeString>()
      || input.userType() == qMetaTypeId<QString>()) {
    QVariantList list;
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    const auto parts = getSafeString(input).get().split(QString(), QString::SkipEmptyParts);
#else
    const auto parts = getSafeString(input).get().split(QString(), Qt::SkipEmptyParts);
#endif
    for (const auto &var : parts) {
      list << var;
    }
    return list;
  }
  return QVariant();
}

QVariant UnorderedListFilter::doFilter(const QVariant &input,
                                       const QVariant &argument,
                                       bool autoescape) const
{
  Q_UNUSED(argument)

  if (!input.canConvert<QVariantList>())
    return QVariant();

  return markSafe(processList(input.value<QVariantList>(), 1, autoescape));
}

SafeString UnorderedListFilter::processList(const QVariantList &list, int tabs,
                                            bool autoescape) const
{
  QString indent;
  for (auto i = 0; i < tabs; ++i)
    indent.append(QLatin1Char('\t'));
  QStringList output;

  auto i = 0;
  auto listSize = list.size();
  while (i < listSize) {
    auto titleObject = list.at(i);
    auto title = getSafeString(titleObject);
    QString sublist;
    QVariant sublistItem;

    if (titleObject.userType() == qMetaTypeId<QVariantList>()) {
      sublistItem = titleObject;
      title.get().clear();
    } else if (i < listSize - 1) {
      auto nextItem = list.at(i + 1);
      if (nextItem.userType() == qMetaTypeId<QVariantList>()) {
        sublistItem = nextItem;
      }
      ++i;
    }
    if (sublistItem.isValid()) {
      sublist = processList(sublistItem.value<QVariantList>(), tabs + 1,
                            autoescape);
      sublist = QStringLiteral("\n%1<ul>\n%2\n%3</ul>\n%4")
                    .arg(indent, sublist, indent, indent);
    }
    output.append(QStringLiteral("%1<li>%2%3</li>")
                      .arg(indent,
                           autoescape ? conditionalEscape(title) : title,
                           sublist));
    ++i;
  }

  // Should be QLatin1Char() ?
  return output.join(QChar::fromLatin1('\n'));
}

struct DictSortLessThan {
  bool operator()(const std::pair<QVariant, QVariant> &lp,
                  const std::pair<QVariant, QVariant> &rp) const
  {
    const auto l = lp.first;
    const auto r = rp.first;
    switch (l.userType()) {
    case QVariant::Invalid:
      return (r.isValid());
    case QVariant::Int:
      return l.value<int>() < r.value<int>();
    case QVariant::UInt:
      return l.value<uint>() < r.value<uint>();
    case QVariant::LongLong:
      return l.value<long long>() < r.value<long long>();
    case QVariant::ULongLong:
      return l.value<unsigned long long>() < r.value<unsigned long long>();
    case QMetaType::Float:
      return l.value<float>() < r.value<float>();
    case QVariant::Double:
      return l.value<double>() < r.value<double>();
    case QVariant::Char:
      return l.toChar() < r.toChar();
    case QVariant::Date:
      return l.toDate() < r.toDate();
    case QVariant::Time:
      return l.toTime() < r.toTime();
    case QVariant::DateTime:
      return l.toDateTime() < r.toDateTime();
    case QMetaType::QObjectStar:
      return l.value<QObject *>() < r.value<QObject *>();
    }
    if (l.userType() == qMetaTypeId<Cutelee::SafeString>()) {
      if (r.userType() == qMetaTypeId<Cutelee::SafeString>()) {
        return l.value<Cutelee::SafeString>().get()
               < r.value<Cutelee::SafeString>().get();
      } else if (r.userType() == qMetaTypeId<QString>()) {
        return l.value<Cutelee::SafeString>().get() < r.toString();
      }
    } else if (r.userType() == qMetaTypeId<Cutelee::SafeString>()) {
      if (l.userType() == qMetaTypeId<QString>()) {
        return l.toString() < r.value<Cutelee::SafeString>().get();
      }
    } else if (l.userType() == qMetaTypeId<QString>()) {
      if (r.userType() == qMetaTypeId<QString>()) {
        return l.toString() < r.toString();
      }
    }
    return false;
  }
};

QVariant DictSortFilter::doFilter(const QVariant &input,
                                  const QVariant &argument,
                                  bool autoescape) const
{
  Q_UNUSED(autoescape)

  if (!input.canConvert<QVariantList>())
    return QVariant();

  QList<std::pair<QVariant, QVariant>> keyList;
  const auto inList = input.value<QSequentialIterable>();
  for (const QVariant &item : inList) {
    auto var = item;

    const Variable v(getSafeString(argument));

    if (v.literal().isValid()) {
      var = MetaType::lookup(var, v.literal().toString());
    } else {
      const auto lookups = v.lookups();
      Q_FOREACH (const QString &lookup, lookups) {
        var = MetaType::lookup(var, lookup);
      }
    }
    keyList.push_back({var, item});
  }

  DictSortLessThan lt;
  std::stable_sort(keyList.begin(), keyList.end(), lt);

  QVariantList outList;
  auto it = keyList.constBegin();
  const auto end = keyList.constEnd();
  for (; it != end; ++it) {
    outList << it->second;
  }
  return outList;
}
