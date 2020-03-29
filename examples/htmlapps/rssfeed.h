/*
  This file is part of the Cutelee template system.

  Copyright (c) 2011 Stephen Kelly <steveire@gmail.com>

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

#ifndef RSS_FEED_H
#define RSS_FEED_H

#include <cutelee/filter.h>
#include <cutelee/node.h>

namespace Cutelee
{
class Parser;
class OutputStream;
class Context;
}

class RssFeedNodeFactory : public Cutelee::AbstractNodeFactory
{
  Q_OBJECT
public:
  RssFeedNodeFactory(QObject *parent = 0);

  virtual Cutelee::Node *getNode(const QString &tagContent,
                                  Cutelee::Parser *p) const;
};

class RssFeedNode : public Cutelee::Node
{
  Q_OBJECT
public:
  RssFeedNode(const Cutelee::FilterExpression &url,
              const Cutelee::FilterExpression &query, QObject *parent = 0);

  void setChildNodes(QList<Node *> childNodes);

  virtual void render(Cutelee::OutputStream *stream,
                      Cutelee::Context *c) const;

private:
  Cutelee::FilterExpression m_url;
  Cutelee::FilterExpression m_query;
  QList<Node *> m_childNodes;
};

class XmlRoleNodeFactory : public Cutelee::AbstractNodeFactory
{
  Q_OBJECT
public:
  XmlRoleNodeFactory(QObject *parent = 0);

  virtual Cutelee::Node *getNode(const QString &tagContent,
                                  Cutelee::Parser *p) const;
};

class XmlRoleNode : public Cutelee::Node
{
  Q_OBJECT
public:
  XmlRoleNode(const Cutelee::FilterExpression &query, QObject *parent = 0);

  virtual void render(Cutelee::OutputStream *stream,
                      Cutelee::Context *c) const;

private:
  Cutelee::FilterExpression m_name;
  Cutelee::FilterExpression m_query;
  int m_count;
};

class XmlNamespaceNodeFactory : public Cutelee::AbstractNodeFactory
{
  Q_OBJECT
public:
  XmlNamespaceNodeFactory(QObject *parent = 0);

  virtual Cutelee::Node *getNode(const QString &tagContent,
                                  Cutelee::Parser *p) const;
};

class XmlNamespaceNode : public Cutelee::Node
{
  Q_OBJECT
public:
  XmlNamespaceNode(const Cutelee::FilterExpression &query, const QString &name,
                   QObject *parent = 0);

  virtual void render(Cutelee::OutputStream *stream,
                      Cutelee::Context *c) const;

private:
  QString m_name;
  Cutelee::FilterExpression m_query;
};

class ResizeFilter : public Cutelee::Filter
{
public:
  QVariant doFilter(const QVariant &input,
                    const QVariant &argument = QVariant(),
                    bool autoescape = false) const;
};

#endif
