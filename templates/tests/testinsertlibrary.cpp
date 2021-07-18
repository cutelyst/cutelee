/*
 * This file is part of the Cutelee template system.
 *
 * Copyright (c) 2020 Matthias Fehring <mf@huessenbergnetz.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either version
 * 2.1 of the Licence, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef INSERTLIBRARYTEST_H
#define INSERTLIBRARYTEST_H

#include <QTest>
#include <QDebug>

#include "coverageobject.h"
#include "engine.h"
#include "cutelee_paths.h"
#include "template.h"
#include "node.h"
#include "parser.h"
#include "taglibraryinterface.h"
#include "exception.h"

#define TESTLIBRARYTAG_RENDER_VALUE "Hello World!"

using namespace Cutelee;

class TestInsertLibrary : public CoverageObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testInsertedLibraryTag();
    void testLoadInsertedLibraryTag();

private:
    std::shared_ptr<InMemoryTemplateLoader> m_loader;
    Engine *m_engine = nullptr;
};

class TestLibraryTagFactory : public AbstractNodeFactory
{
    Node *getNode(const QString &tagContent, Parser *p) const override;
};

class TestLibraryTag : public Node
{
    Q_OBJECT
public:
    explicit TestLibraryTag(Parser *parser = nullptr) : Node(parser) {}

    void render(Cutelee::OutputStream *stream, Cutelee::Context *gc) const override;
};

Node *TestLibraryTagFactory::getNode(const QString &tagContent, Parser *p) const
{
    Q_UNUSED(tagContent)
    return new TestLibraryTag(p);
}

void TestLibraryTag::render(Cutelee::OutputStream *stream, Cutelee::Context *gc) const
{
    Q_UNUSED(gc);
    *stream << QStringLiteral(TESTLIBRARYTAG_RENDER_VALUE);
}

class TestLibrary : public QObject, public TagLibraryInterface
{
    Q_OBJECT
    Q_INTERFACES(Cutelee::TagLibraryInterface)
public:
    explicit TestLibrary(QObject *parent = nullptr) : QObject(parent) {}

    virtual QHash<QString, AbstractNodeFactory*> nodeFactories(const QString &name = QString()) override
    {
        Q_UNUSED(name);
        QHash<QString, AbstractNodeFactory*> ret{
            {QStringLiteral("test_library_tag"), new TestLibraryTagFactory()}
        };
        return ret;
    }
};

class TestLoadLibrary : public QObject, public TagLibraryInterface
{
    Q_OBJECT
    Q_INTERFACES(Cutelee::TagLibraryInterface)
public:
    explicit TestLoadLibrary(QObject *parent = nullptr) : QObject(parent) {}

    virtual QHash<QString, AbstractNodeFactory*> nodeFactories(const QString &name = QString()) override
    {
        Q_UNUSED(name);
        QHash<QString, AbstractNodeFactory*> ret{
            {QStringLiteral("test_load_library_tag"), new TestLibraryTagFactory()}
        };
        return ret;
    }
};

void TestInsertLibrary::initTestCase()
{
    m_engine = new Engine(this);
    QVERIFY(m_engine);

    m_loader = std::shared_ptr<InMemoryTemplateLoader>(new InMemoryTemplateLoader());
    m_engine->addTemplateLoader(m_loader);

    m_engine->setPluginPaths({QStringLiteral(CUTELEE_PLUGIN_PATH)});

    m_engine->insertDefaultLibrary(QStringLiteral("test_library"), new TestLibrary(m_engine));

    m_engine->insertLibrary(QStringLiteral("test_load_library"), new TestLoadLibrary(m_engine));
}

void TestInsertLibrary::cleanupTestCase()
{
    delete m_engine;
}

void TestInsertLibrary::testInsertedLibraryTag()
{
    auto t = m_engine->newTemplate(QStringLiteral("{% test_library_tag %}"), QStringLiteral("testInsertedLibraryTag"));

    Context context;

    const QString result = t->render(&context);

    QCOMPARE(result, QStringLiteral(TESTLIBRARYTAG_RENDER_VALUE));
}

void TestInsertLibrary::testLoadInsertedLibraryTag()
{
    Context context;

    auto t1 = m_engine->newTemplate(QStringLiteral("{% test_load_library_tag %}"), QStringLiteral("testLoadInsertedLibraryTag1"));

    const QString result1 = t1->render(&context);

    QVERIFY(result1.isEmpty());

    auto t2 = m_engine->newTemplate(QStringLiteral("{% load test_load_library %}{% test_load_library_tag %}"), QStringLiteral("testLoadInsertedLibraryTag2"));

    const QString result2 = t2->render(&context);

    QCOMPARE(result2, QStringLiteral(TESTLIBRARYTAG_RENDER_VALUE));
}

QTEST_MAIN(TestInsertLibrary)

#include "testinsertlibrary.moc"

#endif // INSERTLIBRARYTEST_H
