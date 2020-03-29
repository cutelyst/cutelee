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

#include "rssfeed.h"

#include <cutelee_templates.h>

#include <QAbstractXmlNodeModel>
#include <QXmlNodeModelIndex>
#include <QXmlQuery>
#include <QXmlResultItems>
#include <QtCore/QBuffer>
#include <QtCore/QDebug>
#include <QtCore/QEventLoop>
#include <QtCore/QFile>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

Q_DECLARE_METATYPE(QXmlQuery)

RssFeedNodeFactory::RssFeedNodeFactory(QObject *parent)
    : Cutelee::AbstractNodeFactory(parent)
{
}

Cutelee::Node *RssFeedNodeFactory::getNode(const QString &tagContent,
                                            Cutelee::Parser *p) const
{
  QStringList expr = smartSplit(tagContent);
  Cutelee::FilterExpression url(expr.at(1), p);
  Cutelee::FilterExpression query(expr.at(2), p);

  RssFeedNode *n = new RssFeedNode(url, query);

  QList<Cutelee::Node *> nodes = p->parse(n, "endrssfeed");
  p->takeNextToken();

  n->setChildNodes(nodes);

  return n;
}

RssFeedNode::RssFeedNode(const Cutelee::FilterExpression &url,
                         const Cutelee::FilterExpression &query,
                         QObject *parent)
    : Cutelee::Node(parent), m_url(url), m_query(query)
{
}

void RssFeedNode::setChildNodes(QList<Cutelee::Node *> childNodes)
{
  m_childNodes = childNodes;
}

void RssFeedNode::render(Cutelee::OutputStream *stream,
                         Cutelee::Context *c) const
{
  QNetworkAccessManager *mgr
      = new QNetworkAccessManager(const_cast<RssFeedNode *>(this));
  QUrl url(Cutelee::getSafeString(m_url.resolve(c)));
  QNetworkReply *reply = mgr->get(QNetworkRequest(url));
  QEventLoop eLoop;
  connect(mgr, SIGNAL(finished(QNetworkReply *)), &eLoop, SLOT(quit()));
  eLoop.exec(QEventLoop::ExcludeUserInputEvents);

  c->push();
  foreach (Cutelee::Node *n, m_childNodes) {
    if (!n->inherits(XmlNamespaceNode::staticMetaObject.className()))
      continue;
    Cutelee::OutputStream _dummy;
    n->render(&_dummy, c);
  }

  QXmlQuery query;
  QByteArray ba = reply->readAll();

  QBuffer buffer;
  buffer.setData(ba);
  buffer.open(QIODevice::ReadOnly);
  query.bindVariable("inputDocument", &buffer);
  QString ns;
  QHash<QString, QVariant> h = c->lookup("_ns").toHash();
  QHash<QString, QVariant>::const_iterator it = h.constBegin();
  const QHash<QString, QVariant>::const_iterator end = h.constEnd();
  for (; it != end; ++it) {
    if (it.key().isEmpty()) {
      ns += QStringLiteral("declare default element namespace ")
            + QLatin1String(" \"") + it.value().toString()
            + QLatin1String("\";\n");
    } else {
      ns += QStringLiteral("declare namespace ") + it.key()
            + QLatin1String(" = \"") + it.value().toString()
            + QLatin1String("\";\n");
    }
  }
  query.setQuery(ns + "doc($inputDocument)"
                 + Cutelee::getSafeString(m_query.resolve(c)).get());

  QXmlResultItems result;
  query.evaluateTo(&result);

  QXmlItem item(result.next());
  int count = 0;
  while (!item.isNull()) {
    if (count++ > 20)
      break;
    query.setFocus(item);
    c->push();
    foreach (Cutelee::Node *n, m_childNodes) {
      if (n->inherits(XmlNamespaceNode::staticMetaObject.className()))
        continue;
      c->insert("_q", QVariant::fromValue(query));
      n->render(stream, c);
    }
    c->pop();
    item = result.next();
  }
  c->pop();
}

XmlRoleNodeFactory::XmlRoleNodeFactory(QObject *parent) {}

Cutelee::Node *XmlRoleNodeFactory::getNode(const QString &tagContent,
                                            Cutelee::Parser *p) const
{
  QStringList expr = smartSplit(tagContent);
  Cutelee::FilterExpression query(expr.at(1), p);
  return new XmlRoleNode(query);
}

XmlRoleNode::XmlRoleNode(const Cutelee::FilterExpression &query,
                         QObject *parent)
    : m_query(query), m_count(0)
{
}

static QString unescape(const QString &_input)
{
  QString input = _input;
  input.replace("&lt;", "<");
  input.replace("&gt;", ">");
  input.replace("&quot;", "\"");
  input.replace("&amp;", "&");
  return input;
}

void XmlRoleNode::render(Cutelee::OutputStream *stream,
                         Cutelee::Context *c) const
{
  QXmlQuery q = c->lookup("_q").value<QXmlQuery>();
  QHash<QString, QVariant> h = c->lookup("_ns").toHash();
  QString ns;
  QHash<QString, QVariant>::const_iterator it = h.constBegin();
  const QHash<QString, QVariant>::const_iterator end = h.constEnd();
  for (; it != end; ++it) {
    if (it.key().isEmpty()) {
      ns += QStringLiteral("declare default element namespace ")
            + QLatin1String(" \"") + it.value().toString()
            + QLatin1String("\";\n");
    } else {
      ns += QStringLiteral("declare namespace ") + it.key()
            + QLatin1String(" = \"") + it.value().toString()
            + QLatin1String("\";\n");
    }
  }
  q.setQuery(ns + Cutelee::getSafeString(m_query.resolve(c)));
  QString s;
  q.evaluateTo(&s);
  (*stream) << unescape(s);
}

XmlNamespaceNodeFactory::XmlNamespaceNodeFactory(QObject *parent) {}

Cutelee::Node *XmlNamespaceNodeFactory::getNode(const QString &tagContent,
                                                 Cutelee::Parser *p) const
{
  QStringList expr = smartSplit(tagContent);
  Cutelee::FilterExpression query(expr.at(1), p);
  QString name;
  if (expr.size() == 4)
    name = expr.at(3);
  return new XmlNamespaceNode(query, name);
}

XmlNamespaceNode::XmlNamespaceNode(const Cutelee::FilterExpression &query,
                                   const QString &name, QObject *parent)
    : m_query(query), m_name(name)
{
}

void XmlNamespaceNode::render(Cutelee::OutputStream *stream,
                              Cutelee::Context *c) const
{
  QString q = Cutelee::getSafeString(m_query.resolve(c));
  QHash<QString, QVariant> h = c->lookup("_ns").toHash();
  h.insert(m_name, q);
  c->insert("_ns", h);
}

QVariant ResizeFilter::doFilter(const QVariant &input, const QVariant &argument,
                                bool autoescape) const
{
  QString url = Cutelee::getSafeString(input);
  url.replace("_s", "_z");
  return url;
}
