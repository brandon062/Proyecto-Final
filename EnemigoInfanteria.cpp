#include "EnemigoInfanteria.h"
#include "JugadorInfanteria.h"
#include "ProyectilInfanteria.h"

#include <QGraphicsScene>
#include <QTimer>
#include <QLineF>
#include <QDebug>
#include <QTransform>


EnemigoInfanteria::EnemigoInfanteria(JugadorInfanteria *objetivo,
                                     QGraphicsItem *parent)
    : Soldado(parent)
    , jugador(objetivo)
    , estado(Patrulla)
{
    // Tamaño del sprite del enemigo
    QSize size(120, 140);
    velocidadX = 3.0;

    // --------- CARGA DE SPRITES DE IDLE ----------
    // idle_1_enemigo.png ... idle_9_enemigo.png
    for (int i = 1; i <= 9; ++i) {
        QPixmap p(QString(":/images/idle_%1_enemigo.png").arg(i));
        QPixmap e = p.scaled(size);
        idleRight.append(e);                                  // mirando a la derecha
        idleLeft.append(e.transformed(QTransform().scale(-1,1))); // espejo horizontal = izquierda
    }

    // --------- CARGA DE SPRITES DE CAMINAR ----------
    // walk2_1_enemigo.png ... walk2_8_enemigo.png
    for (int i = 1; i <= 8; ++i) {
        QPixmap p(QString(":/images/walk2_%1_enemigo.png").arg(i));
        QPixmap e = p.scaled(size);
        walkRight.append(e);
        walkLeft.append(e.transformed(QTransform().scale(-1,1)));
    }

    // --------- CARGA DE SPRITES DE DISPARO ----------
    // shot1_1_enemigo.png ... shot1_4_enemigo.png
    for (int i = 1; i <= 4; ++i) {
        QPixmap p(QString(":/images/shot1_%1_enemigo.png").arg(i));
        QPixmap e = p.scaled(size);
        shotRight.append(e);
        shotLeft.append(e.transformed(QTransform().scale(-1,1)));
    }

    // --------- CARGA DE SPRITES DE MUERTE ----------
    // dead2_1_enemigo.png ... dead2_4_enemigo.png
    for (int i = 1; i <= 4; ++i) {
        QPixmap p(QString(":/images/dead2_%1_enemigo.png").arg(i));
        QPixmap e = p.scaled(size);
        deadRight.append(e);
        deadLeft.append(e.transformed(QTransform().scale(-1,1)));
    }

    // Sprite inicial (idle mirando a la derecha)
    setPixmap(idleRight[0]);

    // =====================================================
    //                      SONIDOS
    // =====================================================

    // Sonido cuando el enemigo recibe daño
    damagePlayer = new QMediaPlayer;
    damageAudio  = new QAudioOutput;
    damagePlayer->setAudioOutput(damageAudio);
    damagePlayer->setSource(QUrl("qrc:/sonidos/damage.mp3"));

    // Sonido de muerte
    deathPlayer = new QMediaPlayer;
    deathAudio  = new QAudioOutput;
    deathPlayer->setAudioOutput(deathAudio);
    deathPlayer->setSource(QUrl("qrc:/sonidos/death_enemy.mp3"));

    // =====================================================
    //                      TIMERS
    // =====================================================

    // Timer que actualiza la IA (lógica de comportamiento)
    timerIA = new QTimer;
    connect(timerIA, &QTimer::timeout, this, &EnemigoInfanteria::actualizarIA);
    timerIA->start(100);     // cada 100 ms se evalúa la IA

    // Timer que actualiza la animación (cambio de frame)
    timerAnim = new QTimer;
    connect(timerAnim, &QTimer::timeout, this, &EnemigoInfanteria::actualizarAnimacion);
    timerAnim->start(80);    // cambia frames cada 80 ms

    // controlar el tiempo entre disparos (cooldown)
    relojDisparo.start();
}

// =====================================================
//             FUNCIONES AUXILIARES DE IA
// =====================================================

// Distancia al jugador en píxeles
qreal EnemigoInfanteria::distanciaAJugador() const
{
    if (!jugador) return 1e9;
    return QLineF(pos(), jugador->pos()).length();
}

// Se considera que el jugador "es visible" si está dentro de cierto rango
bool EnemigoInfanteria::jugadorVisible() const
{
    return distanciaAJugador() < 460;
}

// Cambiar de estado de IA (patrulla, alerta, persecución, etc.)
void EnemigoInfanteria::cambiarEstado(EstadoIA nuevo)
{
    estado = nuevo;
}

// =====================================================
//                COMPORTAMIENTO: PATRULLA
// =====================================================

void EnemigoInfanteria::comportamientoPatrulla()
{
    // Se define una zona de patrulla alrededor de la posición inicial
    if (xPatrullaInicio == 0 && xPatrullaFin == 0) {
        xPatrullaInicio = x() - 100;
        xPatrullaFin    = x() + 100;
    }

    // Si llega al inicio, se mueve hacia la derecha
    if (x() <= xPatrullaInicio) {
        moverIzquierda(false);
        moverDerecha(true);
        dirDisparo = 1;
    }
    // Si llega al final, se mueve hacia la izquierda
    else if (x() >= xPatrullaFin) {
        moverDerecha(false);
        moverIzquierda(true);
        dirDisparo = -1;
    }

    // Seleccionar animación Idle/Walk según movimiento
    actualizarEstadoMovimiento();

    // Si el jugador entra en rango, pasamos a Alerta
    if (jugadorVisible())
        cambiarEstado(Alerta);
}

// =====================================================
//                COMPORTAMIENTO: ALERTA
// =====================================================

void EnemigoInfanteria::comportamientoAlerta()
{
    // Se queda quieto
    moverIzquierda(false);
    moverDerecha(false);

    // Mira hacia el lado donde esté el jugador
    if (jugador)
        dirDisparo = (jugador->x() < x() ? -1 : 1);

    estadoAnim = AnimIdle;

    // Si todavía ve al jugador, intenta disparar
    if (jugadorVisible())
    {
        if (relojDisparo.elapsed() >= cooldownMs) {  // respeta el cooldown
            estadoAnim = AnimShot;
            frameIndex = 0;
            shotFramesPlayed = 0;

            disparar();               // función heredada de Soldado (crea ProyectilInfanteria)
            relojDisparo.restart();
        }
    }

    // Si deja de verlo, entra en modo búsqueda
    if (!jugadorVisible()) {
        ultimaPosicionVista = jugador->pos();
        tiempoBusqueda = 180;         // 3 segundos
        cambiarEstado(Busqueda);
    }
    // Si está suficientemente cerca, pasa a persecución (se mueve)
    else if (distanciaAJugador() < 250)
        cambiarEstado(Persecucion);
}

// =====================================================
//             COMPORTAMIENTO: PERSECUCIÓN
// =====================================================

void EnemigoInfanteria::comportamientoPersecucion()
{
    if (!jugador) return;

    qreal dist = distanciaAJugador();

    moverIzquierda(false);
    moverDerecha(false);

    const qreal minDist   = 120;  // distancia mínima (si está más cerca, se aleja)
    const qreal idealDist = 280;  // punto "ideal" para disparar

    // Si está lejos del jugador, se acerca
    if (dist > idealDist) {
        if (jugador->x() < x()) moverIzquierda(true), dirDisparo = -1;
        else                    moverDerecha(true),  dirDisparo =  1;
    }
    // Si está demasiado cerca, se aleja
    else if (dist < minDist) {
        if (jugador->x() < x()) moverDerecha(true),  dirDisparo =  1;
        else                    moverIzquierda(true),dirDisparo = -1;
    }

    // Ajustar animación Idle/Walk según se mueva o no
    actualizarEstadoMovimiento();

    // Si lo ve y está en rango, dispara
    if (jugadorVisible() && dist < 350)
    {
        if (relojDisparo.elapsed() >= cooldownMs) {
            estadoAnim = AnimShot;
            frameIndex = 0;
            shotFramesPlayed = 0;
            disparar();
            relojDisparo.restart();
        }
    }

    // Si lo pierde de vista, entra en búsqueda
    if (!jugadorVisible()) {
        ultimaPosicionVista = jugador->pos();
        tiempoBusqueda = 180;
        cambiarEstado(Busqueda);
    }
}

// =====================================================
//             COMPORTAMIENTO: BÚSQUEDA
// =====================================================

void EnemigoInfanteria::comportamientoBusqueda()
{
    moverIzquierda(false);
    moverDerecha(false);

    // Se dirige hacia la última posición donde vio al jugador
    if (ultimaPosicionVista.x() < x()) {
        moverIzquierda(true);
        dirDisparo = -1;
    } else {
        moverDerecha(true);
        dirDisparo = 1;
    }

    actualizarEstadoMovimiento();

    // Disminuye el tiempo de búsqueda
    tiempoBusqueda--;

    // Si se acaba el tiempo, vuelve a patrulla
    if (tiempoBusqueda <= 0) {
        cambiarEstado(Patrulla);
        return;
    }

    // Si vuelve a ver al jugador, pasa a persecución
    if (jugadorVisible())
        cambiarEstado(Persecucion);
}

// =====================================================
//               LOOP DE IA PRINCIPAL
// =====================================================

void EnemigoInfanteria::actualizarIA()
{
    if (enemigoMuerto) return;

    // Si el jugador ya está muerto, el enemigo deja de moverse y solo queda idle
    if (!jugador || jugador->estaMuerto()) {
        estadoAnim = AnimIdle;
        return;
    }

    switch (estado) {
    case Patrulla:     comportamientoPatrulla();     break;
    case Alerta:       comportamientoAlerta();       break;
    case Persecucion:  comportamientoPersecucion();  break;
    case Busqueda:     comportamientoBusqueda();     break;
    }
}

// =====================================================
//     SELECCIONAR ANIMACIÓN SEGÚN MOVIMIENTO
// =====================================================

void EnemigoInfanteria::actualizarEstadoMovimiento()
{
    // No cambiamos animación si está disparando o muerto
    if (estadoAnim == AnimShot || estadoAnim == AnimDead)
        return;

    // moviendoDerecha / moviendoIzquierda vienen de Soldado
    if (moviendoDerecha || moviendoIzquierda)
        estadoAnim = AnimWalk;
    else
        estadoAnim = AnimIdle;
}

// =====================================================
//           ACTUALIZACIÓN DE ANIMACIÓN (frames)
// =====================================================

void EnemigoInfanteria::actualizarAnimacion()
{
    const QVector<QPixmap> *frames = nullptr;
    bool der = (dirDisparo > 0);

    // Elegir el conjunto de frames según el estado actual
    switch (estadoAnim) {
    case AnimIdle:  frames = der ? &idleRight : &idleLeft; break;
    case AnimWalk:  frames = der ? &walkRight : &walkLeft; break;
    case AnimShot:  frames = der ? &shotRight : &shotLeft; break;
    case AnimDead:  frames = der ? &deadRight : &deadLeft; break;
    }

    // si por alguna razón no hay frames, no se hace nada
    if (!frames || frames->isEmpty())
        return;

    switch (estadoAnim)
    {
    case AnimIdle:
    case AnimWalk:
        // Animación en bucle
        frameIndex = (frameIndex + 1) % frames->size();
        setPixmap(frames->at(frameIndex));
        break;

    case AnimShot:
        // Animación de disparo que se reproduce una sola vez
        if (frameIndex >= frames->size())
            frameIndex = frames->size()-1;

        setPixmap(frames->at(frameIndex));
        frameIndex++;
        shotFramesPlayed++;

        // Cuando termina la animación de disparo, volver a Idle o Walk
        if (shotFramesPlayed >= frames->size()) {
            estadoAnim = (moviendoDerecha || moviendoIzquierda) ? AnimWalk : AnimIdle;
            frameIndex = 0;
            shotFramesPlayed = 0;
        }
        break;

    case AnimDead:
        // Animación de muerte, avanza hasta el último frame y se queda ahí
        if (frameIndex < frames->size())
            setPixmap(frames->at(frameIndex));
        if (frameIndex < frames->size()-1)
            frameIndex++;
        break;
    }
}

// =====================================================
//          CUANDO EL ENEMIGO RECIBE UN DISPARO
// =====================================================

void EnemigoInfanteria::recibirDisparo()
{
    if (enemigoMuerto) return;

    // Reproducir sonido de daño
    if (damagePlayer)
        damagePlayer->play();

    // Este enemigo normal muere con un solo disparo
    enemigoMuerto = true;

    // Detener movimiento
    moverIzquierda(false);
    moverDerecha(false);

    // Iniciar animación de muerte
    iniciarAnimacionMuerte();

    // Sonido de muerte
    if (deathPlayer)
        deathPlayer->play();

    // Borrarlo de la escena después de 4s (cuando termina la animación)
    QTimer::singleShot(4000, this, [this](){
        if (scene()) scene()->removeItem(this);
        delete this;
    });
}

// Inicializa el estado de animación de muerte
void EnemigoInfanteria::iniciarAnimacionMuerte()
{
    estadoAnim = AnimDead;
    frameIndex = 0;
}



