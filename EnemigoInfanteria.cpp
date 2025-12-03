#include "EnemigoInfanteria.h"
#include "JugadorInfanteria.h"
#include "ProyectilInfanteria.h"

#include <QGraphicsScene>
#include <QTimer>
#include <QLineF>
#include <QtMath>
#include <QDebug>

EnemigoInfanteria::EnemigoInfanteria(JugadorInfanteria *objetivo, QGraphicsItem *parent)
    : Soldado(parent)
    , jugador(objetivo)
    , estadoActual(Patrulla)
    , timerIA(new QTimer(this))
    , timerAnimEnemy(new QTimer(this))
    , xPatrullaInicio(0)
    , xPatrullaFin(0)
    , tiempoBusqueda(0)
    , estadoAnimEnemigo(AnimIdleEnemy)
    , frameEnemyIndex(0)
    , shotEnemyFramesPlayed(0)
    , cooldownDisparoEnemigoMs(500) // 0.5 segundos
{
    // =====================================================
    // ANIMACIONES DEL ENEMIGO (CARGA Y ESPEJADO)
    // =====================================================

    idleRightEnemy.clear();
    idleLeftEnemy.clear();
    walkRightEnemy.clear();
    walkLeftEnemy.clear();
    shotRightEnemy.clear();
    shotLeftEnemy.clear();
    setEsJugador(false);

    // Tamaño lógico base
    QSize logicalSize(120, 140);

    // IDLE ENEMIGO (idle_1_enemigo.png ... idle_9_enemigo.png)
    for (int i = 1; i <= 9; ++i) {
        QString path = QString(":/images/idle_%1_enemigo.png").arg(i);
        QPixmap p(path);
        if (!p.isNull()) {
            QPixmap esc = p.scaled(logicalSize,
                                   Qt::IgnoreAspectRatio,
                                   Qt::SmoothTransformation);
            idleRightEnemy.append(esc);
            idleLeftEnemy.append(esc.transformed(QTransform().scale(-1, 1)));
        } else {
            qDebug() << "NO CARGÓ ENEMIGO IDLE:" << path;
        }
    }

    // WALK ENEMIGO (walk2_1_enemigo.png ... walk2_8_enemigo.png)
    for (int i = 1; i <= 8; ++i) {
        QString path = QString(":/images/walk2_%1_enemigo.png").arg(i);
        QPixmap p(path);
        if (!p.isNull()) {
            QPixmap esc = p.scaled(logicalSize,
                                   Qt::IgnoreAspectRatio,
                                   Qt::SmoothTransformation);
            walkRightEnemy.append(esc);
            walkLeftEnemy.append(esc.transformed(QTransform().scale(-1, 1)));
        } else {
            qDebug() << "NO CARGÓ ENEMIGO WALK:" << path;
        }
    }

    // SHOT ENEMIGO (shot1_1_enemigo.png ... shot1_4_enemigo.png)
    for (int i = 1; i <= 4; ++i) {
        QString path = QString(":/images/shot1_%1_enemigo.png").arg(i);
        QPixmap p(path);
        if (!p.isNull()) {
            QPixmap esc = p.scaled(logicalSize,
                                   Qt::IgnoreAspectRatio,
                                   Qt::SmoothTransformation);
            shotRightEnemy.append(esc);
            shotLeftEnemy.append(esc.transformed(QTransform().scale(-1, 1)));
        } else {
            qDebug() << "NO CARGÓ ENEMIGO SHOT:" << path;
        }
    }

    // DEAD ENEMIGO: dead2_1_enemigo.png .. dead2_4_enemigo.png
    for (int i = 1; i <= 4; ++i) {
        QString path = QString(":/images/dead2_%1_enemigo.png").arg(i);
        QPixmap p(path);
        if (!p.isNull()) {
            // Usa el mismo logicalSize que usaste para normalizar otras animaciones
            QPixmap esc = p.scaled(logicalSize,
                                   Qt::IgnoreAspectRatio,
                                   Qt::SmoothTransformation);
            deadRightEnemy.append(esc);
            deadLeftEnemy.append(esc.transformed(QTransform().scale(-1, 1)));
        } else {
            qDebug() << "NO CARGÓ DeadEnemy:" << path;
        }
    }

    // Sprite inicial
    if (!idleRightEnemy.isEmpty()) {
        setPixmap(idleRightEnemy[0]);
    }

    dirDisparo = -1; // Por defecto mirando a la izquierda

    // Estado inicial de animación
    estadoAnimEnemigo      = AnimIdleEnemy;
    frameEnemyIndex        = 0;
    shotEnemyFramesPlayed  = 0;

    // ------- SONIDO DE DAÑO ENEMIGO -------
    damagePlayerEnemy = new QMediaPlayer(this);
    damageAudioEnemy  = new QAudioOutput(this);
    damagePlayerEnemy->setAudioOutput(damageAudioEnemy);
    damageAudioEnemy->setVolume(0.9);
    damagePlayerEnemy->setSource(QUrl("qrc:/sonidos/damage.mp3"));

    // ------- SONIDO DE MUERTE ENEMIGO -------
    deathPlayerEnemy = new QMediaPlayer(this);
    deathAudioEnemy  = new QAudioOutput(this);
    deathPlayerEnemy->setAudioOutput(deathAudioEnemy);
    deathAudioEnemy->setVolume(1.0);
    deathPlayerEnemy->setSource(QUrl("qrc:/sonidos/death_enemy.mp3"));

    // Timer de animación (frames)
    connect(timerAnimEnemy, &QTimer::timeout,
            this, &EnemigoInfanteria::actualizarAnimacionEnemigo);
    timerAnimEnemy->start(80);

    // Timer de IA
    connect(timerIA, &QTimer::timeout,
            this, &EnemigoInfanteria::actualizarIA);
    timerIA->start(60);        // cada 60 ms actualiza IA

    // Cooldown de disparo enemigo
    relojDisparoEnemigo.start();
}

// ----------------------------------------
// HELPER: distancia al jugador
// ----------------------------------------

qreal EnemigoInfanteria::distanciaAJugador() const
{
    if (!jugador) return 1e9;
    return QLineF(pos(), jugador->pos()).length();
}

// ----------------------------------------
// HELPER: detección del jugador
// ----------------------------------------

bool EnemigoInfanteria::jugadorVisible() const
{
    if (!scene() || !jugador) return false;

    // Rango máximo de detección
    const qreal rangoDeteccion = 420.0;
    qreal dist = distanciaAJugador();
    if (dist > rangoDeteccion)
        return false;
    return true;
}

// ----------------------------------------
// CAMBIO DE ESTADO DE IA
// ----------------------------------------

void EnemigoInfanteria::cambiarEstado(EstadoIA nuevo)
{
    if (estadoActual == nuevo)
        return;

    estadoActual = nuevo;
}

// ----------------------------------------
// IA: actualizarIA
// ----------------------------------------

void EnemigoInfanteria::actualizarIA()
{
    if (enemigoMuerto)
        return;

    // Si el jugador está muerto, detiene IA por completo
    if (jugador && jugador->estaMuerto()) {

        // detener movimiento
        moverDerecha(false);
        moverIzquierda(false);

        // bloquear animación a Idle
        estadoAnimEnemigo = AnimIdleEnemy;

        return;
    }
    switch (estadoActual) {
    case Patrulla:
        comportamientoPatrulla();
        break;
    case Alerta:
        comportamientoAlerta();
        break;
    case Persecucion:
        comportamientoPersecucion();
        break;
    case Busqueda:
        comportamientoBusqueda();
        break;
    }
}

// ----------------------------------------
// IA: Patrulla
// ----------------------------------------

void EnemigoInfanteria::comportamientoPatrulla()
{
    if (!scene()) return;

    // Inicializar rango de patrulla alrededor de la posición inicial
    if (xPatrullaInicio == 0 && xPatrullaFin == 0) {
        xPatrullaInicio = x() - 100;
        xPatrullaFin    = x() + 100;
    }

    // Movimiento ida y vuelta
    if (x() <= xPatrullaInicio) {
        moverIzquierda(false);
        moverDerecha(true);
        dirDisparo = 1;
    }
    else if (x() + boundingRect().width() >= xPatrullaFin) {
        moverDerecha(false);
        moverIzquierda(true);
        dirDisparo = -1;
    }

    // Si no se está moviendo por algo, empujar a la derecha
    if (!moviendoDerecha && !moviendoIzquierda) {
        moverDerecha(true);
        dirDisparo = 1;
    }

    // Decidir animación (walk/idle) según movimiento
    actualizarEstadoAnimPorMovimiento();

    // Si ve al jugador, pasar a alerta
    if (jugadorVisible()) {
        ultimaPosicionVista = jugador->pos();
        cambiarEstado(Alerta);
    }
}

// ----------------------------------------
// IA: Alerta
// ----------------------------------------

void EnemigoInfanteria::comportamientoAlerta()
{
    if (!jugador) {
        cambiarEstado(Patrulla);
        return;
    }

    if (jugador && jugador->estaMuerto())
        return;

    // Quieto
    moverDerecha(false);
    moverIzquierda(false);

    // Mirar hacia el jugador
    if (jugador->x() < x())
        dirDisparo = -1;
    else
        dirDisparo = 1;

    // Por defecto, idle
    estadoAnimEnemigo = AnimIdleEnemy;

    // Si lo ve y está en rango, disparar con cooldown
    qreal dist = distanciaAJugador();
    if (jugadorVisible() && dist < 380.0) { // rango disparo ligeramente mayor
        if (relojDisparoEnemigo.elapsed() >= cooldownDisparoEnemigoMs) {
            estadoAnimEnemigo      = AnimShotEnemy;
            frameEnemyIndex        = 0;
            shotEnemyFramesPlayed  = 0;

            disparar();
            relojDisparoEnemigo.restart();
        }
    }

    // Si está muy cerca, pasar a persecución
    if (jugadorVisible() && dist < 260.0) {
        cambiarEstado(Persecucion);
    }
    else if (!jugadorVisible()) {
        ultimaPosicionVista = jugador->pos();
        tiempoBusqueda = 180; // 3 segundos
        cambiarEstado(Busqueda);
    }
}

// ----------------------------------------
// IA: Persecución
// ----------------------------------------

void EnemigoInfanteria::comportamientoPersecucion()
{
    if (!jugador) {
        cambiarEstado(Patrulla);
        return;
    }

    if (jugador && jugador->estaMuerto())
        return;

    qreal dist = distanciaAJugador();
    const qreal distMin   = 120.0;
    const qreal distIdeal = 220.0;

    moverDerecha(false);
    moverIzquierda(false);

    // Ajustar distancia: acercarse o alejarse
    if (dist > distIdeal) {
        // Muy lejos -> acercarse
        if (jugador->x() < x()) {
            moverIzquierda(true);
            dirDisparo = -1;
        } else {
            moverDerecha(true);
            dirDisparo = 1;
        }
    }
    else if (dist < distMin) {
        // Muy cerca -> alejarse
        if (jugador->x() < x()) {
            moverDerecha(true);
            dirDisparo = 1;
        } else {
            moverIzquierda(true);
            dirDisparo = -1;
        }
    }
    else {
        // En zona ideal de distancia, puede quedarse quieto
    }

    // Animación según movimiento
    actualizarEstadoAnimPorMovimiento();

    // Disparar si lo ve y está en rango
    if (jugadorVisible() && dist < 400.0) {
        if (relojDisparoEnemigo.elapsed() >= cooldownDisparoEnemigoMs) {
            estadoAnimEnemigo      = AnimShotEnemy;
            frameEnemyIndex        = 0;
            shotEnemyFramesPlayed  = 0;

            disparar();
            relojDisparoEnemigo.restart();
        }
    }

    // Si deja de verlo, pasar a búsqueda
    if (!jugadorVisible()) {
        ultimaPosicionVista = jugador->pos();
        tiempoBusqueda = 180;
        cambiarEstado(Busqueda);
    }
}

// ----------------------------------------
// IA: Búsqueda
// ----------------------------------------

void EnemigoInfanteria::comportamientoBusqueda()
{
    if (!scene()) return;

    moverDerecha(false);
    moverIzquierda(false);

    // Ir hacia la última posición vista del jugador
    if (ultimaPosicionVista.x() < x()) {
        moverIzquierda(true);
        dirDisparo = -1;
    } else {
        moverDerecha(true);
        dirDisparo = 1;
    }

    actualizarEstadoAnimPorMovimiento();

    tiempoBusqueda--;
    if (tiempoBusqueda <= 0) {
        cambiarEstado(Patrulla);
        moverDerecha(false);
        moverIzquierda(false);
        estadoAnimEnemigo = AnimIdleEnemy;
        return;
    }

    // Si lo vuelve a ver, volver a persecución
    if (jugadorVisible()) {
        cambiarEstado(Persecucion);
    }
}

// ----------------------------------------
// ANIMACIÓN: decidir Idle/Walk según movimiento
// ----------------------------------------

void EnemigoInfanteria::actualizarEstadoAnimPorMovimiento()
{
    // Si está en animación de disparo, no sobre-escribir
    if (estadoAnimEnemigo == AnimShotEnemy)
        return;

    if (moviendoDerecha || moviendoIzquierda)
        estadoAnimEnemigo = AnimWalkEnemy;
    else
        estadoAnimEnemigo = AnimIdleEnemy;
}

// ----------------------------------------
// ANIMACIÓN: actualizarAnimacionEnemigo()
// ----------------------------------------

void EnemigoInfanteria::actualizarAnimacionEnemigo()
{
    const QVector<QPixmap> *frames = nullptr;
    bool mirandoDerecha = (dirDisparo > 0);

    switch (estadoAnimEnemigo) {
    case AnimIdleEnemy:
        frames = mirandoDerecha ? &idleRightEnemy : &idleLeftEnemy;
        break;
    case AnimWalkEnemy:
        frames = mirandoDerecha ? &walkRightEnemy : &walkLeftEnemy;
        break;
    case AnimShotEnemy:
        frames = mirandoDerecha ? &shotRightEnemy : &shotLeftEnemy;
        break;
    case AnimDeadEnemy:
        frames = mirandoDerecha ? &deadRightEnemy : &deadLeftEnemy;
        break;
    }


    if (!frames || frames->isEmpty())
        return;

    switch (estadoAnimEnemigo) {

    case AnimIdleEnemy:
    case AnimWalkEnemy:
        frameEnemyIndex = (frameEnemyIndex + 1) % frames->size();
        setPixmap(frames->at(frameEnemyIndex));
        break;

    case AnimShotEnemy:
        if (frameEnemyIndex >= frames->size())
            frameEnemyIndex = frames->size() - 1;

        setPixmap(frames->at(frameEnemyIndex));
        frameEnemyIndex++;
        shotEnemyFramesPlayed++;

        if (shotEnemyFramesPlayed >= frames->size()) {
            // Al terminar el disparo, la IA decidirá si Idle o Walk
            // según movimiento en el próximo tick de IA
            if (moviendoDerecha || moviendoIzquierda)
                estadoAnimEnemigo = AnimWalkEnemy;
            else
                estadoAnimEnemigo = AnimIdleEnemy;

            frameEnemyIndex       = 0;
            shotEnemyFramesPlayed = 0;
        }
        break;
    case AnimDeadEnemy:
        if (frameEnemyIndex >= frames->size())
            frameEnemyIndex = frames->size() - 1;

        setPixmap(frames->at(frameEnemyIndex));

        if (frameEnemyIndex < frames->size() - 1)
            frameEnemyIndex++;

        // no se cambia de estado, se queda congelado
        break;
    }
}

void EnemigoInfanteria::recibirDisparo()
{
    if (enemigoMuerto) return;

    // Sonido de daño
    if (damagePlayerEnemy) {
        if (damagePlayerEnemy->playbackState() == QMediaPlayer::PlayingState)
            damagePlayerEnemy->setPosition(0);
        else
            damagePlayerEnemy->play();
    }

    enemigoMuerto = true;

    // Detener IA y movimiento
    if (timerIA) timerIA->stop();
    moverDerecha(false);
    moverIzquierda(false);

    // Animación de muerte
    iniciarAnimacionMuerteEnemigo();

    // Sonido de muerte
    if (deathPlayerEnemy) {
        deathPlayerEnemy->play();
    }

    QTimer::singleShot(4000, this, [this]() {
        if (scene()) scene()->removeItem(this);
        delete this;
    });
}

void EnemigoInfanteria::iniciarAnimacionMuerteEnemigo()
{
    estadoAnimEnemigo = AnimDeadEnemy;
    frameEnemyIndex = 0;
}

