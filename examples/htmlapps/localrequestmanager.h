
#ifndef LOCALREQUESTMANAGER_H
#define LOCALREQUESTMANAGER_H

#include <QtNetwork/QNetworkAccessManager>

#include <cutelee/engine.h>

class LocalRequestManager : public QNetworkAccessManager
{
  Q_OBJECT
public:
  LocalRequestManager(Cutelee::Engine *engine, QObject *parent = 0);

  virtual QNetworkReply *createRequest(Operation op,
                                       const QNetworkRequest &request,
                                       QIODevice *outgoingData = 0);

private:
  Cutelee::Engine *m_engine;
};

#endif
