
#include "mainwindow.h"

#include <QHBoxLayout>
#include <QWebView>
#include <QtCore/QDebug>

#include "cutelee_paths.h"
#include <cutelee_templates.h>

#include "localrequestmanager.h"
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
  QHBoxLayout *layout = new QHBoxLayout(this);

  QWebView *wv = new QWebView;

  Cutelee::Engine *engine = new Cutelee::Engine(this);
  engine->addDefaultLibrary("customplugin");

  QSharedPointer<Cutelee::FileSystemTemplateLoader> loader(
      new Cutelee::FileSystemTemplateLoader);
  loader->setTemplateDirs(QStringList() << CUTELEE_TEMPLATE_PATH);
  engine->addTemplateLoader(loader);

  wv->page()->setNetworkAccessManager(new LocalRequestManager(engine, this));

  wv->load(QUrl("template:///home/main.html"));

  layout->addWidget(wv);
}
