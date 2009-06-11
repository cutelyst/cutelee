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

#ifndef SCRIPTABLE_CONTEXT_H
#define SCRIPTABLE_CONTEXT_H

#include <QObject>
#include <QVariant>

namespace Grantlee
{
class Context;
}

using namespace Grantlee;

class ScriptableContext : public QObject
{
  Q_OBJECT
public:
  ScriptableContext( Context *c, QObject* parent = 0 );

  Context* context() {
    return m_c;
  }

public slots:
  QVariant lookup( const QString &name );
  void insert( const QString &name, const QVariant &variant );
  void push();
  void pop();

  QString render( const QObjectList &list );


private:
  Context *m_c;

};

#endif
