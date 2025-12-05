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
    setEsJugador(true);
    relojGranada.start();

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
    setScale(1.0);
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

    // DEAD 1..4 (jugador muerto)
    for (int i = 1; i <= 4; ++i) {
        QString path = QString(":/images/dead_%1.png").arg(i);
        QPixmap p(path);
        if (!p.isNull()) {
            QPixmap esc = p.scaled(logicalSize,
                                   Qt::IgnoreAspectRatio,
                                   Qt::SmoothTransformation);
            deadRight.append(esc);
            deadLeft.append(esc.transformed(QTransform().scale(-1, 1)));
        } else {
            qDebug() << "NO CARGÓ Dead:" << path;
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

    // ------- SONIDO DE LANZAR GRANADA -------
    throwPlayer = new QMediaPlayer(this);
    throwAudio  = new QAudioOutput(this);
    throwPlayer->setAudioOutput(throwAudio);
    throwAudio->setVolume(0.8);
    throwPlayer->setSource(QUrl("qrc:/sonidos/lanzar.mp3"));

    // ------- SONIDO DE DAÑO -------
    damagePlayer = new QMediaPlayer(this);
    damageAudio  = new QAudioOutput(this);
    damagePlayer->setAudioOutput(damageAudio);
    damageAudio->setVolume(0.9);
    damagePlayer->setSource(QUrl("qrc:/sonidos/damage.mp3"));

    // ------- SONIDO DE MUERTE -------
    deathPlayer = new QMediaPlayer(this);
    deathAudio  = new QAudioOutput(this);
    deathPlayer->setAudioOutput(deathAudio);
    deathAudio->setVolume(1.0);
    deathPlayer->setSource(QUrl("qrc:/sonidos/death.mp3"));
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
    if (estaMuerto()) {
        // No responder a teclas si está muerto
        event->ignore();
        return;
    }
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

        // --- COOLDOWN DE GRANADA (7 segundos = 7000 ms) ---
        if (relojGranada.elapsed() < cooldownGranadaMs) {
            // AÚN NO PUEDE LANZAR OTRA GRANADA
            return;
        }

        // Reiniciar cooldown
        relojGranada.restart();

        // Iniciar animación de lanzar granada
        iniciarAnimacionGranada();

        // Sonido de lanzar granada
        if (throwPlayer) {
            if (throwPlayer->playbackState() == QMediaPlayer::PlayingState)
                throwPlayer->setPosition(0);
            else
                throwPlayer->play();
        }

        break;

    default:
        Soldado::keyPressEvent(event);
    }
}

void JugadorInfanteria::keyReleaseEvent(QKeyEvent *event)
{
    if (estaMuerto()) {
        // No responder a teclas si esta muerto
        event->ignore();
        return;
    }

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

void JugadorInfanteria::recibirDisparo()
{
    if (vidas <= 0) return;

    // Sonido de daño
    if (damagePlayer) {
        if (damagePlayer->playbackState() == QMediaPlayer::PlayingState)
            damagePlayer->setPosition(0);
        else
            damagePlayer->play();
    }

    vidas--;

    if (vidas <= 0) {

        // Detener TODOS los sonidos del jugador (caminar, disparo, lanzar, daño)
        detenerTodosLosSonidos();

        // Animación de muerte
        iniciarAnimacionMuerte();

        // Sonido de muerte
        if (deathPlayer)
            deathPlayer->play();
    }
}

void JugadorInfanteria::iniciarAnimacionMuerte()
{
    if (muerteProcesada) return;
    muerteProcesada = true;

    estadoAnim = AnimDead;
    frameIndex = 0;

    // El jugador ya no puede moverse ni saltar ni disparar
    moviendoDerecha = false;
    moviendoIzquierda = false;
}

void JugadorInfanteria::detenerTodosLosSonidos()
{
    // STOP caminar
    if (walkPlayer && walkPlayer->playbackState() == QMediaPlayer::PlayingState)
        walkPlayer->stop();

    // STOP disparo
    if (shotPlayer && shotPlayer->playbackState() == QMediaPlayer::PlayingState)
        shotPlayer->stop();

    // STOP lanzar granada
    if (throwPlayer && throwPlayer->playbackState() == QMediaPlayer::PlayingState)
        throwPlayer->stop();

    // STOP recibir daño
    if (damagePlayer && damagePlayer->playbackState() == QMediaPlayer::PlayingState)
        damagePlayer->stop();
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
    case AnimDead:
        frames = mirandoDerecha ? &deadRight : &deadLeft;
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

            // Se lanza la granada SOLO una vez por animación
            if (!granadaLanzadaEnEstaAnim) {
                Soldado::lanzarGranada();
                granadaLanzadaEnEstaAnim = true;
            }

            // Volvemos a Idle o Walk según si se esta moviendo
            if (moviendoDerecha || moviendoIzquierda)
                estadoAnim = AnimWalk;
            else
                estadoAnim = AnimIdle;

            frameIndex = 0;
            grenadeFramesPlayed = 0;
        }
        break;


    case AnimDead:
        // reproducir la animación UNA sola vez, y quedarse en el ultimo frame
        if (frameIndex >= frames->size())
            frameIndex = frames->size() - 1;

        setPixmap(frames->at(frameIndex));

        if (frameIndex < frames->size() - 1)
            frameIndex++;

        // no se cambia mas de estado: se queda tirado
        break;
    }
    }
}




