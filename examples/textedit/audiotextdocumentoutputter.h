/*
  This file is part of the Cutelee template system.

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

#ifndef AUDIOTEXTDOCUMENTDIRECTOR_H
#define AUDIOTEXTDOCUMENTDIRECTOR_H

#include <cutelee/markupdirector.h>
#include <cutelee/plaintextmarkupbuilder.h>
#include <cutelee/texthtmlbuilder.h>

#include "audioobject.h"

class AbstractAudioBuilder : virtual public Cutelee::AbstractMarkupBuilder
{
public:
  virtual void addAudioTag(const QString &source) = 0;
};

class AudioTextHtmlBuilder : public Cutelee::TextHTMLBuilder,
                             public AbstractAudioBuilder
{
public:
  AudioTextHtmlBuilder();

  /* reimp */ void addAudioTag(const QString &source);
};

class AudioPlainTextBuilder : public Cutelee::PlainTextMarkupBuilder,
                              public AbstractAudioBuilder
{
public:
  AudioPlainTextBuilder();

  /* reimp */ void addAudioTag(const QString &source);
};

class AudioTextDocumentDirector : public Cutelee::MarkupDirector
{
public:
  AudioTextDocumentDirector(AbstractAudioBuilder *builder);

  /* reimp */ void processCustomFragment(const QTextFragment &fragment,
                                         const QTextDocument *doc);

private:
  AbstractAudioBuilder *m_builder;
};

#endif
