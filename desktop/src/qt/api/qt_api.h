#pragma once

#include <vector>
#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QTimer>
#include <QThread>
#include <QHostAddress>
#include <QHttpServer>

#include "core/pokewalker/pocketwalker.h"
#include "core/utils/logger.h"

class QtAPI: public QObject
{
    Q_OBJECT

public:
    explicit QtAPI(PocketWalker& emulator,  quint16 port, QObject* parent = nullptr);

public slots:
    void start();

private:
    PocketWalker& emulator;
    quint16 port;
    std::unique_ptr<QHttpServer> server;
    std::vector<QString> endpoints;
};
