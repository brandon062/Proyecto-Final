#include "TanqueEnemigo.h"
#include "Tanque.h"
#include "ProyectilTanque.h"

#include <QGraphicsScene>
#include <QtMath>
#include <QUrl>

TanqueEnemigo::TanqueEnemigo(Tanque *objetivo, QGraphicsItem *parent)
    : QObject()
    , QGraphicsPixmapItem(parent)
    , objetivo(objetivo)
    , anguloCuerpo(0.0)
    , velocidad(1.0)
    , velocidadMaxima(1.0)
    , velocidadGiro(0.7)
    , torreta(nullptr)
    , anguloTorretaRel(0.0)
    , velocidadGiroTorreta(0.7)
    , iaTimer(nullptr)
    , disparoTimer(nullptr)
    , reloadTimer(nullptr)
    , puedeDisparar(true)
    , firePlayer(nullptr)
    , fireAudio(nullptr)
{
    // ===== Chasis Tiger =====
    QPixmap chasis(":/images/tiger_chasis.png");
    QPixmap chasisEscalado = chasis.scaled(100, 100,
                                           Qt::KeepAspectRatio,
                                           Qt::SmoothTransformation);
    setPixmap(chasisEscalado);
    setTransformOriginPoint(boundingRect().center());
    setRotation(anguloCuerpo);

    // ===== Torreta Tiger =====
    QPixmap torretaImg(":/images/tiger_torreta.png");
    QPixmap torretaEscalada = torretaImg.scaled(100, 100,
                                                Qt::KeepAspectRatio,
                                                Qt::SmoothTransformation);
    torreta = new QGraphicsPixmapItem(torretaEscalada, this);
    torreta->setTransformOriginPoint(torreta->boundingRect().center());
    torreta->setPos(0, 0);
    torreta->setRotation(anguloTorretaRel);

    // ===== Timer de IA (movimiento + apuntado) =====
    iaTimer = new QTimer(this);
    connect(iaTimer, &QTimer::timeout, this, &TanqueEnemigo::actualizarIA);
    iaTimer->start(30); // ~33 FPS

    // ===== Timer de disparo =====
    disparoTimer = new QTimer(this);
    connect(disparoTimer, &QTimer::timeout, this, &TanqueEnemigo::disparar);
    disparoTimer->start(2000); // intento de disparar cada 2s

    // ===== Reload (recarga) 8s =====
    reloadTimer = new QTimer(this);
    reloadTimer->setSingleShot(true);
    connect(reloadTimer, &QTimer::timeout, this, &TanqueEnemigo::recargar);

    // ===== Sonido de disparo enemigo =====
    firePlayer = new QMediaPlayer(this);
    fireAudio  = new QAudioOutput(this);
    firePlayer->setAudioOutput(fireAudio);
    fireAudio->setVolume(0.9);
    firePlayer->setSource(QUrl("qrc:/sonidos/tank_enemyfire.mp3"));
}

void TanqueEnemigo::actualizarIA()
{
    if (!scene() || !objetivo)
        return;

    // Centros en escena
    QPointF centroEnemigo = mapToScene(boundingRect().center());
    QPointF centroJugador = objetivo->mapToScene(objetivo->boundingRect().center());

    QPointF vectorObjetivo = centroJugador - centroEnemigo;
    double distancia = qSqrt(vectorObjetivo.x()*vectorObjetivo.x() +
                             vectorObjetivo.y()*vectorObjetivo.y());

    if (distancia < 1.0)
        return;

    // Ángulo desde enemigo hacia jugador
    double rad = qAtan2(vectorObjetivo.y(), vectorObjetivo.x());
    double anguloDeseado = qRadiansToDegrees(rad) + 90.0;

    // === Girar el chasis hacia el jugador ===
    double diff = anguloDeseado - anguloCuerpo;
    while (diff > 180.0) diff -= 360.0;
    while (diff < -180.0) diff += 360.0;

    if (diff > velocidadGiro)
        diff = velocidadGiro;
    else if (diff < -velocidadGiro)
        diff = -velocidadGiro;

    anguloCuerpo += diff;
    setRotation(anguloCuerpo);

    // === Distancia mínima para no acercarse tanto ===
    double distanciaMinima = 300.0;   // antes 150.0 → ahora se queda más lejos

    // Si está más lejos que la distancia mínima, se acerca un poco
    if (distancia > distanciaMinima) {
        double radCuerpo = qDegreesToRadians(anguloCuerpo - 90.0);
        double dx = qCos(radCuerpo) * velocidad;
        double dy = qSin(radCuerpo) * velocidad;
        setPos(x() + dx, y() + dy);
    }
    // Si está más cerca, se queda quieto (no se pega al jugador)

    // === Girar la torreta apuntando al jugador ===
    double anguloTorretaAbs = anguloCuerpo + anguloTorretaRel;
    double anguloTorretaDeseado = anguloDeseado;

    double diffT = anguloTorretaDeseado - anguloTorretaAbs;
    while (diffT > 180.0) diffT -= 360.0;
    while (diffT < -180.0) diffT += 360.0;

    if (diffT > velocidadGiroTorreta)
        diffT = velocidadGiroTorreta;
    else if (diffT < -velocidadGiroTorreta)
        diffT = -velocidadGiroTorreta;

    anguloTorretaRel += diffT;
    torreta->setRotation(anguloTorretaRel);
}

void TanqueEnemigo::disparar()
{
    if (!scene() || !objetivo)
        return;

    // Respeto de recarga: solo dispara si puedeDisparar = true
    if (!puedeDisparar)
        return;

    puedeDisparar = false;
    reloadTimer->start(8000);  // 8 segundos, igual que el jugador

    // Sonido de disparo enemigo
    if (firePlayer->playbackState() == QMediaPlayer::PlayingState) {
        firePlayer->setPosition(0);
    } else {
        firePlayer->play();
    }

    // Centro de la torreta en escena
    QPointF centroTorretaScene = torreta->mapToScene(torreta->boundingRect().center());
    QPointF centroJugador = objetivo->mapToScene(objetivo->boundingRect().center());

    QPointF v = centroJugador - centroTorretaScene;
    double dist = qSqrt(v.x()*v.x() + v.y()*v.y());
    if (dist < 10.0)
        return; // muy cerca

    // Dirección normalizada hacia el jugador
    QPointF dir(v.x() / dist, v.y() / dist);

    // Posición inicial en la punta del cañón
    double barrelLength = torreta->boundingRect().height() / 2.0 + 5.0;
    QPointF muzzlePos = centroTorretaScene + dir * barrelLength;

    // Crear proyectil
    ProyectilTanque *bala = new ProyectilTanque(dir, false);  // false = disparo enemigo
    bala->setPos(muzzlePos);
    scene()->addItem(bala);
}

void TanqueEnemigo::recargar()
{
    puedeDisparar = true;
}

