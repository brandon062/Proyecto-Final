#include "Tanque.h"
#include "ProyectilTanque.h"

#include <QTimer>
#include <QKeyEvent>
#include <QtMath>
#include <QUrl>
#include <QGraphicsScene>
#include "explosion.h"
#include "EstructuraMapa.h"
#include "Mina.h"
#include "TanqueEnemigo.h"
#include "CajaReparacion.h"

Tanque::Tanque(QGraphicsItem *parent)
    : QObject()
    , QGraphicsPixmapItem(parent)
    , anguloCuerpo(0.0)   // 0 grados = tanque mirando hacia arriba
    , velocidad(0.0)
    // movimiento LENTO
    , aceleracion(0.15)
    , friccion(0.92)
    , velocidadMaxima(1.5)
    , velocidadReversaMax(1.2)
    , velocidadGiro(0.9)
    , torreta(nullptr)
    , anguloTorretaRel(0.0)
    , girarIzquierda(false)
    , girarDerecha(false)
    , avanzar(false)
    , retroceder(false)
    , girarTorretaIzq(false)
    , girarTorretaDer(false)
    , motorLoopPlayer(nullptr)
    , motorLoopAudio(nullptr)
    , drivingPlayer(nullptr)
    , drivingAudio(nullptr)
    , turretPlayer(nullptr)
    , turretAudio(nullptr)
    , firePlayer(nullptr)
    , fireAudio(nullptr)
    , puedeDisparar(true)
    , reloadTimer(nullptr)
    , vidas(3)
    , destruido(false)
    , congelado(false)
{
    // ===== Chasis del tanque =====
    QPixmap chasis(":/images/sherman_chasis.png");
    QPixmap chasisEscalado = chasis.scaled(120, 120,
                                           Qt::KeepAspectRatio,
                                           Qt::SmoothTransformation);
    setPixmap(chasisEscalado);

    setTransformOriginPoint(boundingRect().center());
    setRotation(anguloCuerpo);     // 0 = mirando hacia arriba

    // ===== Torreta =====
    QPixmap torretaImg(":/images/sherman_torreta.png");
    QPixmap torretaEscalada = torretaImg.scaled(120, 120,
                                                Qt::KeepAspectRatio,
                                                Qt::SmoothTransformation);

    torreta = new QGraphicsPixmapItem(torretaEscalada, this);
    torreta->setTransformOriginPoint(torreta->boundingRect().center());
    torreta->setPos(0, 0);
    torreta->setRotation(anguloTorretaRel);

    // ===== Sonido continuo del motor (tank_engine.mp3) =====
    motorLoopPlayer = new QMediaPlayer(this);
    motorLoopAudio  = new QAudioOutput(this);
    motorLoopPlayer->setAudioOutput(motorLoopAudio);
    motorLoopAudio->setVolume(0.4);
    motorLoopPlayer->setSource(QUrl("qrc:/sonidos/tank_engine.mp3"));
    motorLoopPlayer->setLoops(QMediaPlayer::Infinite);
    motorLoopPlayer->play();

    // ===== Sonido de movimiento =====
    drivingPlayer = new QMediaPlayer(this);
    drivingAudio  = new QAudioOutput(this);
    drivingPlayer->setAudioOutput(drivingAudio);
    drivingAudio->setVolume(0.7);
    drivingPlayer->setSource(QUrl("qrc:/sonidos/tank_driving.mp3"));
    drivingPlayer->setLoops(QMediaPlayer::Infinite);

    // ===== Sonido de torreta (tank_turret.mp3) =====
    turretPlayer = new QMediaPlayer(this);
    turretAudio  = new QAudioOutput(this);
    turretPlayer->setAudioOutput(turretAudio);
    turretAudio->setVolume(0.7);
    turretPlayer->setSource(QUrl("qrc:/sonidos/tank_turret.mp3"));
    turretPlayer->setLoops(QMediaPlayer::Infinite);

    // ===== Sonido de disparo (tank_fire.mp3) =====
    firePlayer = new QMediaPlayer(this);
    fireAudio  = new QAudioOutput(this);
    firePlayer->setAudioOutput(fireAudio);
    fireAudio->setVolume(0.9);
    firePlayer->setSource(QUrl("qrc:/sonidos/tank_fire.mp3"));

    // ===== sonido de reparación =====
    repairPlayer = new QMediaPlayer(this);
    repairAudio  = new QAudioOutput(this);
    repairPlayer->setAudioOutput(repairAudio);
    repairAudio->setVolume(0.9);
    repairPlayer->setSource(QUrl("qrc:/sonidos/reparacion.mp3"));


    // ===== Timer de recarga (8 segundos) =====
    reloadTimer = new QTimer(this);
    reloadTimer->setSingleShot(true);
    connect(reloadTimer, &QTimer::timeout, this, &Tanque::recargar);

    // ===== Timer de física =====
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Tanque::actualizarFisica);
    timer->start(16); // ~60 FPS
}

void Tanque::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_A:
        girarIzquierda = true;
        break;
    case Qt::Key_D:
        girarDerecha = true;
        break;
    case Qt::Key_W:
    case Qt::Key_Up:
        avanzar = true;
        break;
    case Qt::Key_S:
    case Qt::Key_Down:
        retroceder = true;
        break;
    case Qt::Key_Left:
        girarTorretaIzq = true;
        if (turretPlayer->playbackState() != QMediaPlayer::PlayingState) {
            turretPlayer->play();
        }
        break;

    case Qt::Key_Right:
        girarTorretaDer = true;
        if (turretPlayer->playbackState() != QMediaPlayer::PlayingState) {
            turretPlayer->play();
        }
        break;
    case Qt::Key_Space:
        disparar();
        break;
    default:
        break;
    }
}

void Tanque::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_A:
        girarIzquierda = false;
        break;
    case Qt::Key_D:
        girarDerecha = false;
        break;
    case Qt::Key_W:
    case Qt::Key_Up:
        avanzar = false;
        break;
    case Qt::Key_S:
    case Qt::Key_Down:
        retroceder = false;
        break;
    case Qt::Key_Left:
        girarTorretaIzq = false;
        if (!girarTorretaDer) {              // si no sigues girando a la derecha
            turretPlayer->stop();
            turretPlayer->setPosition(0);    // para que empiece desde el inicio la próxima vez
        }
        break;

    case Qt::Key_Right:
        girarTorretaDer = false;
        if (!girarTorretaIzq) {              // si no sigue girando a la izquierda
            turretPlayer->stop();
            turretPlayer->setPosition(0);
        }
        break;
    }
}

void Tanque::actualizarFisica()
{
    if (destruido || congelado)
        return;

    // === Girar chasis ===
    if (girarIzquierda) {
        anguloCuerpo -= velocidadGiro;
        setRotation(anguloCuerpo);
    }
    if (girarDerecha) {
        anguloCuerpo += velocidadGiro;
        setRotation(anguloCuerpo);
    }

    // === Acelerar / retroceder ===
    if (avanzar) {
        velocidad += aceleracion;
        if (velocidad > velocidadMaxima)
            velocidad = velocidadMaxima;
    }
    if (retroceder) {
        velocidad -= aceleracion;
        if (velocidad < -velocidadReversaMax)
            velocidad = -velocidadReversaMax;
    }

    // Fricción
    velocidad *= friccion;
    if (qAbs(velocidad) < 0.01)
        velocidad = 0.0;

    // === Movimiento según ángulo del chasis ===
    double rad = qDegreesToRadians(anguloCuerpo - 90.0);
    double dx = qCos(rad) * velocidad;
    double dy = qSin(rad) * velocidad;

    QPointF posAnterior = pos();
    setPos(x() + dx, y() + dy);

    // === Colisión con estructuras ===
    QList<QGraphicsItem*> colisiones = collidingItems();
    for (QGraphicsItem *item : colisiones) {
        if (dynamic_cast<EstructuraMapa*>(item)) {
            // si chocó con una estructura, revertir movimiento
            setPos(posAnterior);
            velocidad = 0;
            break;
        }
    }

    // === Colisión con tanques enemigos ===
    colisiones = collidingItems();
    for (QGraphicsItem *item : colisiones) {
        if (dynamic_cast<TanqueEnemigo*>(item)) {
            // Si toca un tanque enemigo, revertimos el movimiento
            setPos(posAnterior);
            velocidad = 0;
            break;
        }
    }

    // === Colisión con minas (solo afecta al jugador) ===
    colisiones = collidingItems();  // recalculamos después de ajustar posición
    for (QGraphicsItem *item : colisiones) {
        Mina *mina = dynamic_cast<Mina*>(item);
        if (mina) {
            if (scene()) {
                // Centro del jugador para la explosión
                QPointF centro = mapToScene(boundingRect().center());

                Explosion *exp = new Explosion();
                scene()->addItem(exp);
                exp->setPos(
                    centro.x() - exp->boundingRect().width()  / 2.0,
                    centro.y() - exp->boundingRect().height() / 2.0
                    );
            }

            // Aplicar daño al jugador
            recibirImpacto();

            // Eliminar la mina del mapa
            if (scene()) {
                scene()->removeItem(mina);
            }
            delete mina;

            // Si el tanque murió dentro de recibirImpacto(),
            // 'destruido' estará en true, así que salimos de la función
            if (destruido)
                return;

            // Si no murió, no queremos procesar más minas este frame
            break;
        }
    }

    // === Colisión con caja de reparación (solo jugador) ===
    colisiones = collidingItems();
    for (QGraphicsItem *item : colisiones) {
        CajaReparacion *caja = dynamic_cast<CajaReparacion*>(item);
        if (caja) {
            // Solo repara si tenemos menos de 3 vidas
            if (vidas < 3) {
                vidas++;
                emit vidasCambiaron(vidas);

                // reproducir sonido de reparación
                if (repairPlayer->playbackState() == QMediaPlayer::PlayingState) {
                    repairPlayer->setPosition(0);
                } else {
                    repairPlayer->play();
                }

                // eliminar la caja del mapa
                if (scene()) {
                    scene()->removeItem(caja);
                }
                delete caja;
            }
            // Si ya tiene 3 vidas, no pasa nada y la caja permanece.
            break;
        }
    }

    // === Rotación de la torreta ===
    if (girarTorretaIzq) {
        anguloTorretaRel -= 1.0;
    }
    if (girarTorretaDer) {
        anguloTorretaRel += 1.0;
    }
    torreta->setRotation(anguloTorretaRel);

    // === Sonido de movimiento ===
    bool enMovimiento = qAbs(velocidad) > 0.1;
    if (enMovimiento) {
        if (drivingPlayer->playbackState() != QMediaPlayer::PlayingState) {
            drivingPlayer->setPosition(0);
            drivingPlayer->play();
        }
    } else {
        if (drivingPlayer->playbackState() == QMediaPlayer::PlayingState) {
            drivingPlayer->stop();
        }
    }

    // === LÍMITES DE PANTALLA ===
    qreal minX = 0;
    qreal maxX = 800 - boundingRect().width();
    qreal maxY = 600 - boundingRect().height();

    if (x() < minX) setX(minX);
    if (x() > maxX) setX(maxX);
    if (y() > maxY) setY(maxY);

    // Solo permitimos pasar por arriba si NO hay tanques enemigos
    bool hayEnemigos = false;
    if (scene()) {
        const auto itemsEscena = scene()->items();
        for (QGraphicsItem *it : itemsEscena) {
            if (dynamic_cast<TanqueEnemigo*>(it)) {
                hayEnemigos = true;
                break;
            }
        }
    }

    if (hayEnemigos && y() < 0) {
        // Si aún hay enemigos, no dejar que salga por arriba
        setY(0);
    }
}

void Tanque::setVidas(int v)
{
    //limite entre 0 y 3
    if (v < 0) v = 0;
    if (v > 3) v = 3;

    vidas = v;

    // Aquí no emitimos vidasCambiaron ni hacemos lógica de muerte,
    // eso se maneja con recibirImpacto() y en el nivel.
}

void Tanque::setCongelado(bool c)
{
    congelado = c;

    if (congelado) {
        // Apagar todos los sonidos del tanque
        if (motorLoopPlayer)  motorLoopPlayer->stop();
        if (drivingPlayer)    drivingPlayer->stop();
        if (turretPlayer)     turretPlayer->stop();
        if (firePlayer)       firePlayer->stop();
        if (repairPlayer)     repairPlayer->stop();
    }
}

void Tanque::disparar()
{
    if (!puedeDisparar)
        return;   // todavía en recarga

    puedeDisparar = false;
    reloadTimer->start(8000);   // 8 segundos de recarga

    // Sonido de disparo
    if (firePlayer->playbackState() == QMediaPlayer::PlayingState) {
        firePlayer->setPosition(0);
    } else {
        firePlayer->play();
    }

    if (!scene()) return;

    // Centro de la torreta en escena
    QPointF centroTorretaScene = torreta->mapToScene(torreta->boundingRect().center());

    // Ángulo absoluto del cañón
    double anguloAbsoluto = anguloCuerpo + anguloTorretaRel;
    double rad = qDegreesToRadians(anguloAbsoluto - 90.0);
    QPointF dir(qCos(rad), qSin(rad));

    // Punto de salida en la punta del cañón
    double barrelLength = torreta->boundingRect().height() / 3.5 + 5.0;
    QPointF muzzlePos = centroTorretaScene + dir * barrelLength;

    // Crear proyectil
    ProyectilTanque *bala = new ProyectilTanque(dir, true);  // true = es del jugador
    bala->setPos(muzzlePos);
    scene()->addItem(bala);
}

void Tanque::recargar()
{
    puedeDisparar = true;
}

void Tanque::recibirImpacto()
{
    if (destruido)
        return;

    vidas--;

    emit vidasCambiaron(vidas);

    if (vidas <= 0) {
        destruido = true;

        if (motorLoopPlayer)  motorLoopPlayer->stop();
        if (drivingPlayer)    drivingPlayer->stop();
        if (turretPlayer)     turretPlayer->stop();
        if (firePlayer)       firePlayer->stop();

        emit jugadorMurio();

        if (scene()) {
            scene()->removeItem(this);
        }
    }
}






