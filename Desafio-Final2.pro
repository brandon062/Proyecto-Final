QT       += core gui \
            multimedia


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CajaReparacion.cpp \
    EnemigoFinalInfanteria.cpp \
    EnemigoInfanteria.cpp \
    Entidad.cpp \
    EstructuraMapa.cpp \
    Granada.cpp \
    JugadorInfanteria.cpp \
    Mina.cpp \
    NivelBatallaAerea.cpp \
    NivelBatallaTanque.cpp \
    NivelHaciaBunkerPrueba.cpp \
    ProyectilInfanteria.cpp \
    ProyectilTanque.cpp \
    Score.cpp \
    Soldado.cpp \
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
    EnemigoFinalInfanteria.h \
    EnemigoInfanteria.h \
    Entidad.h \
    EntidadMovil.h \
    EstructuraMapa.h \
    Granada.h \
    JugadorInfanteria.h \
    Mina.h \
    NivelBatallaAerea.h \
    NivelBatallaTanque.h \
    NivelHaciaBunkerPrueba.h \
    ProyectilInfanteria.h \
    ProyectilTanque.h \
    Score.h \
    Soldado.h \
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
