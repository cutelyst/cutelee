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

#include "outputstream.h"

#include "safestring.h"

using namespace Cutelee;

OutputStream::OutputStream() : m_stream(0) {}

OutputStream::OutputStream(QTextStream *stream) : m_stream(stream) {}

OutputStream::~OutputStream() {}

QString OutputStream::escape(const QString &input) const
{
    // This could be replaced by QString::toHtmlEscaped()
    // but atm it does not escape single quotes
    QString rich;
    const int len = input.length();
    rich.reserve(int(len * 1.1));
    for (int i = 0; i < len; ++i) {
      const QChar ch = input.at(i);
      if (ch == QLatin1Char('<'))
        rich += QLatin1String("&lt;");
      else if (ch == QLatin1Char('>'))
        rich += QLatin1String("&gt;");
      else if (ch == QLatin1Char('&'))
        rich += QLatin1String("&amp;");
      else if (ch == QLatin1Char('"'))
        rich += QLatin1String("&quot;");
      else if (ch == QLatin1Char('\''))
        rich += QLatin1String("&#39;");
      else
        rich += ch;
    }
    rich.squeeze();
    return rich;
}

QString OutputStream::escape(const Cutelee::SafeString &input) const
{
  return escape(input.get());
}

QString OutputStream::conditionalEscape(const Cutelee::SafeString &input) const
{
  if (!input.isSafe())
    return escape(input.get());
  return input;
}

std::shared_ptr<OutputStream> OutputStream::clone(QTextStream *stream) const
{
  return std::shared_ptr<OutputStream>(new OutputStream(stream));
}

OutputStream &OutputStream::operator<<(const QString &input)
{
  if (m_stream)
    (*m_stream) << input;
  return *this;
}

OutputStream &OutputStream::operator<<(const Cutelee::SafeString &input)
{
  if (m_stream) {
    if (input.needsEscape())
      (*m_stream) << escape(input.get());
    else
      (*m_stream) << input.get();
  }
  return *this;
}
/*
OutputStream& OutputStream::operator<<(const Cutelee::OutputStream::Escape& e)
{
  ( *m_stream ) << escape( e.m_content );
  return *this;
}*/

OutputStream &OutputStream::operator<<(QTextStream *stream)
{
  if (m_stream)
    (*m_stream) << stream->readAll();
  return *this;
}
/*
Cutelee::OutputStream::MarkSafe::MarkSafe(const QString& input)
  : m_safe( false ), m_content( input )
{

}

Cutelee::OutputStream::MarkSafe::MarkSafe(const Cutelee::SafeString& input)
  : m_safe( input.isSafe() ), m_content( input.get() )
{

}
*/
