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

#include "context.h"

#include "nulllocalizer_p.h"
#include "rendercontext.h"
#include "util.h"

#include <memory>
#include <QtCore/QStringList>

using namespace Cutelee;

namespace Cutelee
{
class ContextPrivate
{
  ContextPrivate(Context *context, const QVariantHash &variantHash)
      : q_ptr(context), m_autoescape(true), m_mutating(false),
        m_urlType(Context::AbsoluteUrls), m_renderContext(new RenderContext),
        m_localizer(new NullLocalizer)
  {
    m_variantHashStack.append(variantHash);
  }

  ~ContextPrivate() { delete m_renderContext; }

  Q_DECLARE_PUBLIC(Context)
  Context *const q_ptr;

  QList<QVariantHash> m_variantHashStack;
  bool m_autoescape;
  bool m_mutating;
  QList<std::pair<QString, QString>> m_externalMedia;
  Context::UrlType m_urlType;
  QString m_relativeMediaPath;
  RenderContext *const m_renderContext;
  std::shared_ptr<AbstractLocalizer> m_localizer;
};
}

Context::Context() : d_ptr(new ContextPrivate(this, QVariantHash())) {}

Context::Context(const QVariantHash &variantHash)
    : d_ptr(new ContextPrivate(this, variantHash))
{
}

Context::Context(const Context &other)
    : d_ptr(new ContextPrivate(this, QVariantHash()))
{
  *this = other;
}

Context &Context::operator=(const Context &other)
{
  if (&other == this)
    return *this;
  d_ptr->m_autoescape = other.d_ptr->m_autoescape;
  d_ptr->m_externalMedia = other.d_ptr->m_externalMedia;
  d_ptr->m_mutating = other.d_ptr->m_mutating;
  d_ptr->m_variantHashStack = other.d_ptr->m_variantHashStack;
  d_ptr->m_urlType = other.d_ptr->m_urlType;
  d_ptr->m_relativeMediaPath = other.d_ptr->m_relativeMediaPath;
  return *this;
}

Context::~Context() { delete d_ptr; }

bool Context::autoEscape() const
{
  Q_D(const Context);
  return d->m_autoescape;
}

void Context::setAutoEscape(bool autoescape)
{
  Q_D(Context);
  d->m_autoescape = autoescape;
}

QVariant Context::lookup(const QString &str) const
{
    Q_D(const Context);
    QVariant ret;

    // return a variant from the stack.
    for (const QVariantHash &h : d->m_variantHashStack) {
        auto it = h.constFind(str);
        if (it != h.constEnd()) {
            ret = it.value();
            // If the user passed a string into the context, turn it into a
            // Cutelee::SafeString.
            if (ret.userType() == qMetaTypeId<QString>()) {
                ret = QVariant::fromValue<Cutelee::SafeString>(
                            getSafeString(ret.toString()));
            }
            return ret;
        }
    }

    return ret;
}

void Context::push()
{
  Q_D(Context);

  const QHash<QString, QVariant> hash;
  d->m_variantHashStack.prepend(hash);
}

void Context::pop()
{
  Q_D(Context);

  d->m_variantHashStack.removeFirst();
}

void Context::insert(const QString &name, const QVariant &variant)
{
  Q_D(Context);

  d->m_variantHashStack[0].insert(name, variant);
}

void Context::insert(const QString &name, QObject *object)
{
  Q_D(Context);

  d->m_variantHashStack[0].insert(name, QVariant::fromValue(object));
}

QHash<QString, QVariant> Context::stackHash(int depth) const
{
  Q_D(const Context);

  return d->m_variantHashStack.value(depth);
}

bool Context::isMutating() const
{
  Q_D(const Context);
  return d->m_mutating;
}

void Context::setMutating(bool mutating)
{
  Q_D(Context);
  d->m_mutating = mutating;
}

void Context::addExternalMedia(const QString &absolutePart,
                               const QString &relativePart)
{
  Q_D(Context);
  d->m_externalMedia.append({absolutePart, relativePart});
}

QList<std::pair<QString, QString> > Context::externalMedia() const
{
  Q_D(const Context);
  return d->m_externalMedia;
}

void Context::clearExternalMedia()
{
  Q_D(Context);
  d->m_externalMedia.clear();
}

void Context::setUrlType(Context::UrlType type)
{
  Q_D(Context);
  d->m_urlType = type;
}

Context::UrlType Context::urlType() const
{
  Q_D(const Context);
  return d->m_urlType;
}

void Context::setRelativeMediaPath(const QString &path)
{
  Q_D(Context);
  d->m_relativeMediaPath = path;
}

QString Context::relativeMediaPath() const
{
  Q_D(const Context);
  return d->m_relativeMediaPath;
}

RenderContext *Context::renderContext() const
{
  Q_D(const Context);
  return d->m_renderContext;
}

void Context::setLocalizer(std::shared_ptr<AbstractLocalizer> localizer)
{
  Q_D(Context);
  if (!localizer) {
    d->m_localizer = std::shared_ptr<AbstractLocalizer>(new NullLocalizer);
    return;
  }
  d->m_localizer = localizer;
}

std::shared_ptr<AbstractLocalizer> Context::localizer() const
{
  Q_D(const Context);
  return d->m_localizer;
}
