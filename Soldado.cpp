#include "Soldado.h"
#include "ProyectilInfanteria.h"
#include "Granada.h"
#include <QGraphicsScene>

Soldado::Soldado(QGraphicsItem *parent)
    : QObject()
    , QGraphicsPixmapItem(parent)
    , velocidadX(4.0)
    , velocidadY(0.0)
    , gravedad(0.8)
    , groundY(400.0)
    , enSuelo(false)
    , moviendoDerecha(false)
    , moviendoIzquierda(false)
    , dirDisparo(1)          // por defecto mira a la derecha
{
    timerFisica = new QTimer(this);
    connect(timerFisica, &QTimer::timeout,
            this, &Soldado::actualizarFisica);
    timerFisica->start(16);

    // -------- SONIDO DE CAMINAR --------
    walkPlayer = new QMediaPlayer(this);
    walkAudio  = new QAudioOutput(this);
    walkPlayer->setAudioOutput(walkAudio);
    walkAudio->setVolume(0.6);
    walkPlayer->setSource(QUrl("qrc:/sonidos/walk.mp3"));
    walkPlayer->setLoops(QMediaPlayer::Infinite);

    // -------- SONIDO DE DISPARO --------
    shotPlayer = new QMediaPlayer(this);
    shotAudio  = new QAudioOutput(this);
    shotPlayer->setAudioOutput(shotAudio);
    shotAudio->setVolume(0.8);
    shotPlayer->setSource(QUrl("qrc:/sonidos/shot.mp3"));
}

void Soldado::moverDerecha(bool activo)
{
    moviendoDerecha = activo;
    actualizarSonidoCaminar();
}

void Soldado::moverIzquierda(bool activo)
{
    moviendoIzquierda = activo;
    actualizarSonidoCaminar();
}

void Soldado::saltar()
{
    if (enSuelo) {
        velocidadY = -15.0;   // impulso hacia arriba
        enSuelo = false;
    }
}

void Soldado::disparar()
{
    if (!scene()) return;

    // Crear la bala indicando si es del jugador o del enemigo
    ProyectilInfanteria *bala = new ProyectilInfanteria(dirDisparo, esJugador);

    // Calcula desde dónde sale la bala (boca del arma):
    qreal px;
    if (dirDisparo > 0) {
        px = x() + boundingRect().width() * 0.8;  // hacia la derecha
    } else {
        px = x() + boundingRect().width() * 0.2;  // hacia la izquierda
    }

    reproducirSonidoDisparo();

    qreal py = y() + boundingRect().height() * 0.55;  // un poco por debajo del centro

    bala->setPos(px, py);
    scene()->addItem(bala);
}

void Soldado::reproducirSonidoDisparo()
{
    if (!shotPlayer) return;

    if (shotPlayer->playbackState() == QMediaPlayer::PlayingState) {
        shotPlayer->setPosition(0);
    } else {
        shotPlayer->play();
    }
}

void Soldado::lanzarGranada()
{
    if (!scene()) return;

    // Crear la granada con la dirección actual y el groundY del soldado
    Granada *g = new Granada(dirDisparo, groundY);

    // Posición X: un poco delante del soldado
    qreal gx;
    if (dirDisparo > 0) {
        gx = x() + boundingRect().width() * 0.5;
    } else {
        gx = x() + boundingRect().width() * 0.1;
    }

    // Pies del soldado (coordenada Y del suelo donde pisa)
    qreal piesY = y() + boundingRect().height();

    const qreal alturaGranada = 175;

    // La granada aparece justo un poquito por encima del suelo
    qreal gy = piesY - alturaGranada - 2;

    g->setPos(gx, gy);
    scene()->addItem(g);
}


void Soldado::actualizarSonidoCaminar()
{
    if (!walkPlayer) return;

    if (moviendoDerecha || moviendoIzquierda) {
        if (walkPlayer->playbackState() != QMediaPlayer::PlayingState) {
            walkPlayer->play();
        }
    } else {
        if (walkPlayer->playbackState() == QMediaPlayer::PlayingState) {
            walkPlayer->stop();
        }
    }
}

void Soldado::actualizarFisica()
{
    qreal vx = 0.0;
    if (moviendoDerecha)  vx += velocidadX;   // usar velocidadX
    if (moviendoIzquierda) vx -= velocidadX;

    velocidadY += gravedad;

    qreal nuevoX = x() + vx;
    qreal nuevoY = y() + velocidadY;

    // Colisión con el suelo
    if (nuevoY + boundingRect().height() >= groundY) {
        nuevoY = groundY - boundingRect().height();
        velocidadY = 0.0;
        enSuelo = true;
    } else {
        enSuelo = false;
    }

    // limitar en los laterales de la escena
    if (scene()) {
        QRectF r = scene()->sceneRect();  // usa el rectángulo de la escena actual
        qreal minX = r.left();
        qreal maxX = r.right() - boundingRect().width();

        if (nuevoX < minX) nuevoX = minX;
        if (nuevoX > maxX) nuevoX = maxX;
    }

    setPos(nuevoX, nuevoY);
}
