TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        web_server.cpp

LIBS += -L/usr/lib -lssl -lcrypto -lboost_filesystem \
        -lboost_thread -lboost_system -pthread -lsqlite3

HEADERS += \
    dispatcher.hpp \
    libs/asio_compatibility.hpp \
    libs/crypto.hpp \
    libs/json.hpp \
    libs/mutex.hpp \
    libs/server_http.hpp \
    libs/server_https.hpp \
    libs/sqlite_orm.h \
    libs/status_code.hpp \
    libs/utility.hpp \
    token.hpp

DISTFILES += \
    certs/server.crt \
    certs/server.key
