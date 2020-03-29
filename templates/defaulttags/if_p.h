/*
  This file is part of the Cutelee template system.

  Copyright (c) 2016 Stephen Kelly <steveire@gmail.com>

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

#ifndef IF_P_H
#define IF_P_H

#include <QtCore/QPair>
#include <QtCore/QStringList>
#include <QtCore/QVector>

#include "../lib/exception.h"
#include "filterexpression.h"
#include "node.h"
#include "util.h"

namespace Cutelee
{
class Parser;
}

class IfToken;

class IfParser
{
public:
  IfParser(Cutelee::Parser *parser, const QStringList &args);

  QSharedPointer<IfToken> parse();

  QSharedPointer<IfToken> expression(int rbp = {});

private:
  QSharedPointer<IfToken> createNode(const QString &content) const;

  QSharedPointer<IfToken> consumeToken();

private:
  Cutelee::Parser *mParser;
  QVector<QSharedPointer<IfToken>> mParseNodes;
  int mPos = 0;
  QSharedPointer<IfToken> mCurrentToken;
};

static bool contains(const QVariant &needle, const QVariant &var)
{
  if (Cutelee::isSafeString(var)) {
    return getSafeString(var).get().contains(getSafeString(needle));
  } else if (var.canConvert<QVariantList>()) {
    auto container = var.value<QVariantList>();
    if (Cutelee::isSafeString(needle)) {
      return container.contains(Cutelee::getSafeString(needle).get());
    }
    return container.contains(needle);
  }
  if (var.canConvert<QVariantHash>()) {
    auto container = var.value<QVariantHash>();
    if (Cutelee::isSafeString(needle)) {
      return container.contains(Cutelee::getSafeString(needle).get());
    }
    return container.contains(needle.toString());
  }
  return false;
}

class IfToken
{
public:
  enum OpCode {
    Invalid,
    Literal,
    OrCode,
    AndCode,
    NotCode,
    InCode,
    NotInCode,
    EqCode,
    NeqCode,
    GtCode,
    GteCode,
    LtCode,
    LteCode,
    Sentinal
  };

  static QSharedPointer<IfToken> makeSentinal()
  {
    return QSharedPointer<IfToken>::create(0, QString(), Sentinal);
  }

  using ArgsType = std::pair<QSharedPointer<IfToken>, QSharedPointer<IfToken>>;

  IfToken(int lbp, const QString &tokenName, OpCode opCode) : mArgs()
  {
    mLbp = lbp;
    mTokenName = tokenName;
    mOpCode = opCode;
  }

  IfToken(const Cutelee::FilterExpression &fe) : mFe(fe)
  {
    mLbp = 0;
    mTokenName = QStringLiteral("literal");
    mOpCode = Literal;
  }

  void nud(IfParser *parser);
  void led(QSharedPointer<IfToken> left, IfParser *parser);

  QVariant evaluate(Cutelee::Context *c) const;

  int lbp() const { return mLbp; }

  int mLbp;
  QString mTokenName;

  Cutelee::FilterExpression mFe;
  ArgsType mArgs;

  OpCode mOpCode;

  QString tokenName() const { return mTokenName; }
};

void IfToken::nud(IfParser *parser)
{
  switch (mOpCode) {
  default:
    break;
  case IfToken::Literal:
    return;
  case IfToken::NotCode:
    mArgs.first = parser->expression(mLbp);
    mArgs.second.clear();
    return;
  }

  throw Cutelee::Exception(
      Cutelee::TagSyntaxError,
      QStringLiteral("Not expecting '%1' in this position in if tag.")
          .arg(mTokenName));
}

void IfToken::led(QSharedPointer<IfToken> left, IfParser *parser)
{
  switch (mOpCode) {
  default:
    break;
  case IfToken::OrCode:
  case IfToken::AndCode:
  case IfToken::InCode:
  case IfToken::NotInCode:
  case IfToken::EqCode:
  case IfToken::NeqCode:
  case IfToken::GtCode:
  case IfToken::GteCode:
  case IfToken::LtCode:
  case IfToken::LteCode:
    mArgs.first = left;
    mArgs.second = parser->expression(mLbp);
    return;
  }

  throw Cutelee::Exception(
      Cutelee::TagSyntaxError,
      QStringLiteral("Not expecting '%1' as infix operator in if tag.")
          .arg(mTokenName));
}

IfParser::IfParser(Cutelee::Parser *parser, const QStringList &args)
    : mParser(parser)
{
  mParseNodes.reserve(args.size());
  if (args.size() > 1) {
    auto it = args.begin() + 1, end = args.end() - 1;
    for (; it != end; ++it) {
      if (*it == QLatin1String("not")
          && *std::next(it) == QLatin1String("in")) {
        mParseNodes.push_back(createNode(QStringLiteral("not in")));
        ++it;
        if (it == end) {
          break;
        }
      } else {
        mParseNodes.push_back(createNode(*it));
      }
    }
    mParseNodes.push_back(createNode(*it));
  }
  mPos = 0;
  mCurrentToken = consumeToken();
}

QSharedPointer<IfToken> IfParser::consumeToken()
{
  if (mPos >= mParseNodes.size()) {
    return IfToken::makeSentinal();
  }
  auto t = mParseNodes[mPos];
  ++mPos;
  return t;
}

QSharedPointer<IfToken> IfParser::expression(int rbp)
{
  auto t = mCurrentToken;
  mCurrentToken = consumeToken();
  t->nud(this);
  auto left = t;
  while (rbp < mCurrentToken->lbp()) {
    t = mCurrentToken;
    mCurrentToken = consumeToken();
    t->led(left, this);
    left = t;
  }
  return left;
}

QSharedPointer<IfToken> IfParser::parse()
{
  auto r = expression();

  if (mCurrentToken->mOpCode != IfToken::Sentinal) {
    throw Cutelee::Exception(
        Cutelee::TagSyntaxError,
        QStringLiteral("Unused '%1' at end of if expression.")
            .arg(mCurrentToken->tokenName()));
  }

  return r;
}

QSharedPointer<IfToken> IfParser::createNode(const QString &content) const
{
  if (content == QLatin1String("or")) {
    return QSharedPointer<IfToken>::create(6, content, IfToken::OrCode);
  }
  if (content == QLatin1String("and")) {
    return QSharedPointer<IfToken>::create(7, content, IfToken::AndCode);
  }
  if (content == QLatin1String("in")) {
    return QSharedPointer<IfToken>::create(9, content, IfToken::InCode);
  }
  if (content == QLatin1String("not in")) {
    return QSharedPointer<IfToken>::create(9, content, IfToken::NotInCode);
  }
  if (content == QLatin1String("==")) {
    return QSharedPointer<IfToken>::create(10, content, IfToken::EqCode);
  }
  if (content == QLatin1String("!=")) {
    return QSharedPointer<IfToken>::create(10, content, IfToken::NeqCode);
  }
  if (content == QLatin1String(">")) {
    return QSharedPointer<IfToken>::create(10, content, IfToken::GtCode);
  }
  if (content == QLatin1String(">=")) {
    return QSharedPointer<IfToken>::create(10, content, IfToken::GteCode);
  }
  if (content == QLatin1String("<")) {
    return QSharedPointer<IfToken>::create(10, content, IfToken::LtCode);
  }
  if (content == QLatin1String("<=")) {
    return QSharedPointer<IfToken>::create(10, content, IfToken::LteCode);
  }
  if (content == QStringLiteral("not")) {
    return QSharedPointer<IfToken>::create(8, content, IfToken::NotCode);
  }
  return QSharedPointer<IfToken>::create(
      Cutelee::FilterExpression(content, mParser));
}

QVariant IfToken::evaluate(Context *c) const
{
  try {
    switch (mOpCode) {
    case Literal:
      return mFe.resolve(c);
    case OrCode:
      return Cutelee::variantIsTrue(mArgs.first->evaluate(c))
             || Cutelee::variantIsTrue(mArgs.second->evaluate(c));
    case AndCode:
      return Cutelee::variantIsTrue(mArgs.first->evaluate(c))
             && Cutelee::variantIsTrue(mArgs.second->evaluate(c));
    case NotCode:
      return !Cutelee::variantIsTrue(mArgs.first->evaluate(c));
    case InCode:
      return contains(mArgs.first->evaluate(c), mArgs.second->evaluate(c));
    case NotInCode:
      return !contains(mArgs.first->evaluate(c), mArgs.second->evaluate(c));
    case EqCode:
      return Cutelee::equals(mArgs.first->evaluate(c),
                              mArgs.second->evaluate(c));
    case NeqCode:
      return !Cutelee::equals(mArgs.first->evaluate(c),
                               mArgs.second->evaluate(c));
    case GtCode:
      return mArgs.first->evaluate(c) > mArgs.second->evaluate(c);
    case GteCode:
      return mArgs.first->evaluate(c) >= mArgs.second->evaluate(c);
    case LtCode:
      return mArgs.first->evaluate(c) < mArgs.second->evaluate(c);
    case LteCode:
      return mArgs.first->evaluate(c) <= mArgs.second->evaluate(c);
    default:
      Q_ASSERT(!"Invalid OpCode");
      return QVariant();
    }
  } catch (const Cutelee::Exception &) {
    return false;
  }
}

#endif
