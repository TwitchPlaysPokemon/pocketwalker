#include "qt_api.h"

#include <sstream>
#include <iomanip>
#include <QString>
#include <memory>

QtAPI::QtAPI(PocketWalker& emulator, quint16 port, QObject* parent)
    : QObject(parent)
      , emulator(emulator)
      , port(port)
{
}

void QtAPI::start()
{
    server = std::make_unique<QHttpServer>();
    std::vector<QString> endpoints;

    endpoints.push_back("PressButton: (Usage: /PressButton/<ButtonName (left, right, center, none)>) Presses down the specified button, releasing all others. Use 'none' to release all buttons.");
    server->route("/PressButton/<arg>", QHttpServerRequest::Method::Get,
        [this](QString button, const QHttpServerRequest &request) {
            emulator.ReleaseButton(ButtonType::LEFT);
            emulator.ReleaseButton(ButtonType::RIGHT);
            emulator.ReleaseButton(ButtonType::CENTER);
            if (button == "left") {
                emulator.PressButton(ButtonType::LEFT);
            } else if (button == "right") {
                emulator.PressButton(ButtonType::RIGHT);
            } else if (button == "center") {
                emulator.PressButton(ButtonType::CENTER);
            } else if (button == "none") {
                // do nothing
            } else {
                return QHttpServerResponse("Invalid button", QHttpServerResponse::StatusCode::BadRequest);
            }
            return QHttpServerResponse("Ok", QHttpServerResponse::StatusCode::Ok);
        }
    );

    endpoints.push_back("TakeSteps: (Usage: /TakeSteps/<Count:int>) Takes the specified number of steps.");
    server->route("/TakeSteps/<arg>", QHttpServerRequest::Method::Get,
        [this](QString count, const QHttpServerRequest &request) {
            uint8_t steps = static_cast<uint8_t>(std::stoul(count.toStdString()));
            emulator.TakeStep(steps);
            return QHttpServerResponse("Ok", QHttpServerResponse::StatusCode::Ok);
         }
    );

    endpoints.push_back("SetWatts: (Usage: /SetWatts/<Watts:int>) Sets the current Watts.");
    server->route("/SetWatts/<arg>", QHttpServerRequest::Method::Get,
        [this](uint16_t watts, const QHttpServerRequest &request) {
                emulator.SetWatts(watts);
                return QHttpServerResponse("Ok", QHttpServerResponse::StatusCode::Ok);
        }
    );

    endpoints.push_back("SetSessionSteps: (Usage: /SetSessionSteps/<Steps:int>) Sets the current session steps.");
    server->route("/SetSessionSteps/<arg>", QHttpServerRequest::Method::Get,
        [this](uint32_t steps, const QHttpServerRequest &request) {
                emulator.SetSessionSteps(steps);
                return QHttpServerResponse("Ok", QHttpServerResponse::StatusCode::Ok);
        }
    );

    //Memory manipulation endpoints
    endpoints.push_back("Read8: (Usage: /Read8/<Address:int(hex)>) Reads the byte at Address");
    server->route("/Read8/<arg>", QHttpServerRequest::Method::Get,
        [this](QString address, const QHttpServerRequest &request) {
            try {
                uint16_t addr = static_cast<uint16_t>(std::stoul(address.toStdString(), nullptr, 16));
                uint8_t value = emulator.Memory()->Read8(addr);
                std::stringstream ss;
                ss << std::hex << std::uppercase << static_cast<int>(value);
                return QHttpServerResponse(QString::fromStdString(ss.str()), QHttpServerResponse::StatusCode::Ok);
            } catch (const std::exception& e) {
                return QHttpServerResponse(QString::fromUtf8(e.what()), QHttpServerResponse::StatusCode::BadRequest);
            }
        }
    );
    endpoints.push_back("Write8: (Usage: /Write8/<Address:int(hex)>/<Value:int(hex)>) Writes the byte Value at Address");
    server->route("/Write8/<arg>/<arg>", QHttpServerRequest::Method::Get,
        [this](QString address, QString value, const QHttpServerRequest &request) {
            try {
                uint16_t addr = static_cast<uint16_t>(std::stoul(address.toStdString(), nullptr, 16));
                uint8_t val = static_cast<uint8_t>(std::stoul(value.toStdString(), nullptr, 16));
                emulator.Memory()->Write8(addr, val);
                return QHttpServerResponse("Ok", QHttpServerResponse::StatusCode::Ok);
            } catch (const std::exception& e) {
                return QHttpServerResponse(QString::fromUtf8(e.what()), QHttpServerResponse::StatusCode::BadRequest);
            }
        }
    );
    endpoints.push_back("Read16: (Usage: /Read16/<Address:int(hex)>) Reads the 2-byte value at Address");
    server->route("/Read16/<arg>", QHttpServerRequest::Method::Get,
        [this](QString address, const QHttpServerRequest &request) {
            try {
                uint16_t addr = static_cast<uint16_t>(std::stoul(address.toStdString(), nullptr, 16));
                uint16_t value = emulator.Memory()->Read16(addr);
                std::stringstream ss;
                ss << std::hex << std::uppercase << value;
                return QHttpServerResponse(QString::fromStdString(ss.str()), QHttpServerResponse::StatusCode::Ok);
            } catch (const std::exception& e) {
                return QHttpServerResponse(QString::fromUtf8(e.what()), QHttpServerResponse::StatusCode::BadRequest);
            }
        }
    );
    endpoints.push_back("Write16: (Usage: /Write16/<Address:int(hex)>/<Value:int(hex)>) Writes the 2-byte Value at Address");
    server->route("/Write16/<arg>/<arg>", QHttpServerRequest::Method::Get,
        [this](QString address, QString value, const QHttpServerRequest &request) {
            try {
                uint16_t addr = static_cast<uint16_t>(std::stoul(address.toStdString(), nullptr, 16));
                uint16_t val = static_cast<uint16_t>(std::stoul(value.toStdString(), nullptr, 16));
                emulator.Memory()->Write16(addr, val);
                return QHttpServerResponse("Ok", QHttpServerResponse::StatusCode::Ok);
            } catch (const std::exception& e) {
                return QHttpServerResponse(QString::fromUtf8(e.what()), QHttpServerResponse::StatusCode::BadRequest);
            }
        }
    );
    endpoints.push_back("Read32: (Usage: /Read32/<Address:int(hex)>) Reads the 4-byte value at Address");
    server->route("/Read32/<arg>", QHttpServerRequest::Method::Get,
        [this](QString address, const QHttpServerRequest &request) {
            try {
                uint16_t addr = static_cast<uint16_t>(std::stoul(address.toStdString(), nullptr, 16));
                uint32_t value = emulator.Memory()->Read32(addr);
                std::stringstream ss;
                ss << std::hex << std::uppercase << value;
                return QHttpServerResponse(QString::fromStdString(ss.str()), QHttpServerResponse::StatusCode::Ok);
            } catch (const std::exception& e) {
                return QHttpServerResponse(QString::fromUtf8(e.what()), QHttpServerResponse::StatusCode::BadRequest);
            }
        }
    );
    endpoints.push_back("Write32: (Usage: /Write32/<Address:int(hex)>/<Value:int(hex)>) Writes the 4 byte Value at Address");
    server->route("/Write32/<arg>/<arg>", QHttpServerRequest::Method::Get,
        [this](QString address, QString value, const QHttpServerRequest &request) {
            try {
                uint16_t addr = static_cast<uint16_t>(std::stoul(address.toStdString(), nullptr, 16));
                uint32_t val = static_cast<uint32_t>(std::stoul(value.toStdString(), nullptr, 16));
                emulator.Memory()->Write32(addr, val);
                return QHttpServerResponse("Ok", QHttpServerResponse::StatusCode::Ok);
            } catch (const std::exception& e) {
                return QHttpServerResponse(QString::fromUtf8(e.what()), QHttpServerResponse::StatusCode::BadRequest);
            }
        }
    );
    endpoints.push_back("ReadRange: (Usage: /ReadRange/<Address:int(hex)>/<Length:int(hex)>) Reads Length bytes of memory starting at Address");
    server->route("/ReadRange/<arg>/<arg>", QHttpServerRequest::Method::Get,
        [this](QString address, QString length, const QHttpServerRequest &request) {
            try {
                uint16_t addr = static_cast<uint16_t>(std::stoul(address.toStdString(), nullptr, 16));
                uint16_t len = static_cast<uint16_t>(std::stoul(length.toStdString(), nullptr, 16));
                std::vector<uint8_t> buffer(len);
                for (size_t i = 0; i < len; ++i) {
                    buffer[i] = emulator.Memory()->Read8(addr + i);
                }
                std::stringstream ss;
                for (size_t i = 0; i < len; ++i) {
                    ss << std::hex << std::uppercase << static_cast<int>(buffer[i]);
                    if (i < len - 1) ss << " ";
                }
                return QHttpServerResponse(QString::fromStdString(ss.str()), QHttpServerResponse::StatusCode::Ok);
            } catch (const std::exception& e) {
                return QHttpServerResponse(QString::fromUtf8(e.what()), QHttpServerResponse::StatusCode::BadRequest);
            }
        }
    );
    endpoints.push_back("WriteRange: (Usage: /WriteRange/<Address:int(hex)>/<Data:bytes>) Writes Data to memory starting at Address");
    server->route("/WriteRange/<arg>/<arg>", QHttpServerRequest::Method::Get,
        [this](QString address, QString data, const QHttpServerRequest &request) {
            try {
                uint16_t addr = static_cast<uint16_t>(std::stoul(address.toStdString(), nullptr, 16));
                std::istringstream iss(data.toStdString());
                std::vector<uint8_t> buffer;
                std::string byte_str;
                while (iss >> byte_str) {
                    buffer.push_back(static_cast<uint8_t>(std::stoul(byte_str, nullptr, 16)));
                }
                for (size_t i = 0; i < buffer.size(); ++i) {
                    emulator.Memory()->Write8(addr + i, buffer[i]);
                }
                return QHttpServerResponse("Ok", QHttpServerResponse::StatusCode::Ok);
            } catch (const std::exception& e) {
                return QHttpServerResponse(QString::fromUtf8(e.what()), QHttpServerResponse::StatusCode::BadRequest);
            }
        }
    );

    // List all endpoints (this crashes)
    // server->route("/", QHttpServerRequest::Method::Get,
    //     [&endpoints](const QHttpServerRequest &request) {
    //         std::stringstream ss;
    //         ss << "Available endpoints:\n";
    //         for (const auto& route : endpoints) {
    //             ss << route.toStdString() << "\n";
    //         }
    //         return QHttpServerResponse(QString::fromStdString(ss.str()), QHttpServerResponse::StatusCode::Ok);
    //      }
    // );

    auto tcpserver = new QTcpServer();
    if (!tcpserver->listen(QHostAddress::Any, port) || !server->bind(tcpserver)) {
        delete tcpserver;
        return Log::Error("Failed to start API server: {}", tcpserver->errorString().toStdString());
    }
    Log::Info("API listening on port {}", tcpserver->serverPort());

}