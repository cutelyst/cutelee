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

#include <QtCore/QStringList>

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

  std::shared_ptr<IfToken> parse();

  std::shared_ptr<IfToken> expression(int rbp = {});

private:
  std::shared_ptr<IfToken> createNode(const QString &content) const;

  std::shared_ptr<IfToken> consumeToken();

private:
  Cutelee::Parser *mParser;
  std::vector<std::shared_ptr<IfToken>> mParseNodes;
  int mPos = 0;
  std::shared_ptr<IfToken> mCurrentToken;
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

  static std::shared_ptr<IfToken> makeSentinal()
  {
    return std::shared_ptr<IfToken>(new IfToken{0, QString(), Sentinal});
  }

  using ArgsType = std::pair<std::shared_ptr<IfToken>, std::shared_ptr<IfToken>>;

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
  void led(std::shared_ptr<IfToken> left, IfParser *parser);

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
    mArgs.second = {};
    return;
  }

  throw Cutelee::Exception(
      Cutelee::TagSyntaxError,
      QStringLiteral("Not expecting '%1' in this position in if tag.")
          .arg(mTokenName));
}

void IfToken::led(std::shared_ptr<IfToken> left, IfParser *parser)
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

std::shared_ptr<IfToken> IfParser::consumeToken()
{
  if (size_t(mPos) >= mParseNodes.size()) {
    return IfToken::makeSentinal();
  }
  auto t = mParseNodes[mPos];
  ++mPos;
  return t;
}

std::shared_ptr<IfToken> IfParser::expression(int rbp)
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

std::shared_ptr<IfToken> IfParser::parse()
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

std::shared_ptr<IfToken> IfParser::createNode(const QString &content) const
{
  if (content == QLatin1String("or")) {
    return std::shared_ptr<IfToken>(new IfToken{6, content, IfToken::OrCode});
  }
  if (content == QLatin1String("and")) {
    return std::shared_ptr<IfToken>(new IfToken{7, content, IfToken::AndCode});
  }
  if (content == QLatin1String("in")) {
    return std::shared_ptr<IfToken>(new IfToken{9, content, IfToken::InCode});
  }
  if (content == QLatin1String("not in")) {
    return std::shared_ptr<IfToken>(new IfToken{9, content, IfToken::NotInCode});
  }
  if (content == QLatin1String("==")) {
    return std::shared_ptr<IfToken>(new IfToken{10, content, IfToken::EqCode});
  }
  if (content == QLatin1String("!=")) {
    return std::shared_ptr<IfToken>(new IfToken{10, content, IfToken::NeqCode});
  }
  if (content == QLatin1String(">")) {
    return std::shared_ptr<IfToken>(new IfToken{10, content, IfToken::GtCode});
  }
  if (content == QLatin1String(">=")) {
    return std::shared_ptr<IfToken>(new IfToken{10, content, IfToken::GteCode});
  }
  if (content == QLatin1String("<")) {
    return std::shared_ptr<IfToken>(new IfToken{10, content, IfToken::LtCode});
  }
  if (content == QLatin1String("<=")) {
    return std::shared_ptr<IfToken>(new IfToken{10, content, IfToken::LteCode});
  }
  if (content == QStringLiteral("not")) {
    return std::shared_ptr<IfToken>(new IfToken{8, content, IfToken::NotCode});
  }
  return std::shared_ptr<IfToken>(new IfToken{
      Cutelee::FilterExpression(content, mParser)});
}

QVariant IfToken::evaluate(Cutelee::Context *c) const
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
      return Cutelee::gt(mArgs.first->evaluate(c), mArgs.second->evaluate(c));
    case GteCode:
      return Cutelee::gte(mArgs.first->evaluate(c), mArgs.second->evaluate(c));
    case LtCode:
      return Cutelee::lt(mArgs.first->evaluate(c), mArgs.second->evaluate(c));
    case LteCode:
      return Cutelee::lte(mArgs.first->evaluate(c), mArgs.second->evaluate(c));
    default:
      Q_ASSERT(!"Invalid OpCode");
      return QVariant();
    }
  } catch (const Cutelee::Exception &) {
    return false;
  }
}

#endif
