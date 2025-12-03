#include "JugadorInfanteria.h"
#include <QGraphicsScene>
#include <QPixmap>
#include <QTransform>
#include <QDebug>

JugadorInfanteria::JugadorInfanteria(QGraphicsItem *parent)
    : Soldado(parent)
    , estadoAnim(AnimIdle)
    , timerAnim(new QTimer(this))
    , frameIndex(0)
    , shotFramesPlayed(0)
    , grenadeFramesPlayed(0)
    , cooldownDisparoMs(500)
    , granadaLanzadaEnEstaAnim(false)
{
    idleRight.clear();
    idleLeft.clear();
    walkRight.clear();
    walkLeft.clear();
    shotRight.clear();
    shotLeft.clear();
    grenadeRight.clear();
    grenadeLeft.clear();

    //
    // 1) Cargar todos los Idle
    //
    QVector<QPixmap> idleTemp;
    for (int i = 1; i <= 9; ++i) {
        QString path = QString(":/images/idle%1.png").arg(i);
        QPixmap p(path);
        if (!p.isNull()) {
            idleTemp.append(p);
        } else {
            qDebug() << "NO CARGÓ Idle:" << path;
        }
    }

    if (idleTemp.isEmpty()) {
        qWarning() << "ERROR: No se pudo cargar ningún Idle. Revisa rutas en el .qrc";
        return;
    }

    // se usa temporalmente Idle1 para medir el tamaño lógico
    setPixmap(idleTemp[0]);
    setScale(1.0); // aseguramos escala 1:1
    QSizeF logicalSizeF = boundingRect().size();
    QSize  logicalSize  = logicalSizeF.toSize();

    // IDLE
    for (const QPixmap &p : idleTemp) {
        QPixmap esc = p.scaled(logicalSize,
                               Qt::IgnoreAspectRatio,
                               Qt::SmoothTransformation);
        idleRight.append(esc);
        idleLeft.append(esc.transformed(QTransform().scale(-1, 1)));
    }

    // WALK 1..7
    for (int i = 1; i <= 7; ++i) {
        QString path = QString(":/images/walk_%1.png").arg(i);
        QPixmap p(path);
        if (!p.isNull()) {
            QPixmap esc = p.scaled(logicalSize,
                                   Qt::IgnoreAspectRatio,
                                   Qt::SmoothTransformation);
            walkRight.append(esc);
            walkLeft.append(esc.transformed(QTransform().scale(-1, 1)));
        } else {
            qDebug() << "NO CARGÓ Walk:" << path;
        }
    }

    // SHOT 1..4
    for (int i = 1; i <= 4; ++i) {
        QString path = QString(":/images/shot_%1.png").arg(i);
        QPixmap p(path);
        if (!p.isNull()) {
            QPixmap esc = p.scaled(logicalSize,
                                   Qt::IgnoreAspectRatio,
                                   Qt::SmoothTransformation);
            shotRight.append(esc);
            shotLeft.append(esc.transformed(QTransform().scale(-1, 1)));
        } else {
            qDebug() << "NO CARGÓ Shot:" << path;
        }
    }

    // GRENADE 1..9
    for (int i = 1; i <= 9; ++i) {
        QString path = QString(":/images/grenade_%1.png").arg(i);
        QPixmap p(path);
        if (!p.isNull()) {
            QPixmap esc = p.scaled(logicalSize,
                                   Qt::IgnoreAspectRatio,
                                   Qt::SmoothTransformation);
            grenadeRight.append(esc);
            grenadeLeft.append(esc.transformed(QTransform().scale(-1, 1)));
        } else {
            qDebug() << "NO CARGÓ Grenade:" << path;
        }
    }

    //
    // 3) Configuración inicial
    //
    dirDisparo = 1;

    if (!idleRight.isEmpty()) {
        setPixmap(idleRight[0]);
    }

    setScale(1.0);

    // Timer de animación
    connect(timerAnim, &QTimer::timeout,
            this, &JugadorInfanteria::actualizarAnimacion);
    timerAnim->start(80); // ~12.5 FPS

    relojDisparo.start();

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();
}

// ---------------------------------------------------------
// Lógica de estado
// ---------------------------------------------------------

bool JugadorInfanteria::estaEnAccion() const
{
    return estadoAnim == AnimShot || estadoAnim == AnimGrenade;
}

void JugadorInfanteria::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;

    switch (event->key()) {
    case Qt::Key_A:
    case Qt::Key_Left:
        moverIzquierda(true);
        dirDisparo = -1;
        if (!estaEnAccion())
            estadoAnim = AnimWalk;
        break;

    case Qt::Key_D:
    case Qt::Key_Right:
        moverDerecha(true);
        dirDisparo = 1;
        if (!estaEnAccion())
            estadoAnim = AnimWalk;
        break;

    case Qt::Key_Space:
        saltar();
        break;

    case Qt::Key_J: {
        // Disparo con cooldown de 0.5s
        if (relojDisparo.elapsed() >= cooldownDisparoMs) {
            Soldado::disparar();
            relojDisparo.restart();
            iniciarAnimacionShot();
        }
        break;
    }

    case Qt::Key_K:
        iniciarAnimacionGranada();
        break;

    default:
        Soldado::keyPressEvent(event);
    }
}

void JugadorInfanteria::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;

    switch (event->key()) {
    case Qt::Key_A:
    case Qt::Key_Left:
        moverIzquierda(false);
        break;
    case Qt::Key_D:
    case Qt::Key_Right:
        moverDerecha(false);
        break;
    default:
        Soldado::keyReleaseEvent(event);
    }

    // Volver a Idle / Walk cuando suelta teclas
    if (!moviendoDerecha && !moviendoIzquierda && !estaEnAccion()) {
        estadoAnim = AnimIdle;
        frameIndex = 0;
    } else if ((moviendoDerecha || moviendoIzquierda) && !estaEnAccion()) {
        estadoAnim = AnimWalk;
    }
}

void JugadorInfanteria::iniciarAnimacionShot()
{
    estadoAnim        = AnimShot;
    frameIndex        = 0;
    shotFramesPlayed  = 0;
}

void JugadorInfanteria::iniciarAnimacionGranada()
{
    estadoAnim               = AnimGrenade;
    frameIndex               = 0;
    grenadeFramesPlayed      = 0;
    granadaLanzadaEnEstaAnim = false;
}

// ---------------------------------------------------------
// Animación
// ---------------------------------------------------------

void JugadorInfanteria::actualizarAnimacion()
{
    const QVector<QPixmap> *frames = nullptr;
    bool mirandoDerecha = (dirDisparo > 0);

    switch (estadoAnim) {
    case AnimIdle:
        frames = mirandoDerecha ? &idleRight : &idleLeft;
        break;
    case AnimWalk:
        frames = mirandoDerecha ? &walkRight : &walkLeft;
        break;
    case AnimShot:
        frames = mirandoDerecha ? &shotRight : &shotLeft;
        break;
    case AnimGrenade:
        frames = mirandoDerecha ? &grenadeRight : &grenadeLeft;
        break;
    }

    if (!frames || frames->isEmpty())
        return;

    switch (estadoAnim) {

    case AnimIdle:
    case AnimWalk:
        frameIndex = (frameIndex + 1) % frames->size();
        setPixmap(frames->at(frameIndex));
        setScale(1.0);
        break;

    case AnimShot:
        if (frameIndex >= frames->size())
            frameIndex = frames->size() - 1;
        setPixmap(frames->at(frameIndex));
        setScale(1.0);
        frameIndex++;
        shotFramesPlayed++;
        if (shotFramesPlayed >= frames->size()) {
            if (moviendoDerecha || moviendoIzquierda)
                estadoAnim = AnimWalk;
            else
                estadoAnim = AnimIdle;
            frameIndex = 0;
        }
        break;

    case AnimGrenade: {
        if (frameIndex >= frames->size())
            frameIndex = frames->size() - 1;

        setPixmap(frames->at(frameIndex));
        setScale(1.0);
        frameIndex++;
        grenadeFramesPlayed++;

        // Cuando terminan los frames de lanzar:
        if (grenadeFramesPlayed >= frames->size()) {

            // Lanzamos la granada SOLO una vez
            if (!granadaLanzadaEnEstaAnim) {
                Soldado::lanzarGranada();
                granadaLanzadaEnEstaAnim = true;
            }

            // Volvemos a Idle o Walk según si se está moviendo
            if (moviendoDerecha || moviendoIzquierda)
                estadoAnim = AnimWalk;
            else
                estadoAnim = AnimIdle;

            frameIndex = 0;
        }
        break;
    }
    }
}




