QT       += core gui \
            multimedia


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CajaReparacion.cpp \
    Entidad.cpp \
    EstructuraMapa.cpp \
    Mina.cpp \
    NivelBatallaAerea.cpp \
    NivelBatallaTanque.cpp \
    ProyectilTanque.cpp \
    Score.cpp \
    Tanque.cpp \
    TanqueEnemigo.cpp \
    Vida.cpp \
    enemigo.cpp \
    explosion.cpp \
    jugador.cpp \
    main.cpp \
    proyectil.cpp \
    proyectilEnemigo.cpp

HEADERS += \
    Avion.h \
    CajaReparacion.h \
    Entidad.h \
    EntidadMovil.h \
    EstructuraMapa.h \
    Mina.h \
    NivelBatallaAerea.h \
    NivelBatallaTanque.h \
    ProyectilTanque.h \
    Score.h \
    Tanque.h \
    TanqueEnemigo.h \
    Vehiculo.h \
    Vida.h \
    enemigo.h \
    explosion.h \
    jugador.h \
    proyectil.h \
    proyectilEnemigo.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    rec.qrc
