#include "NivelHaciaBunkerPrueba.h"
#include "JugadorInfanteria.h"
#include "EnemigoInfanteria.h"
#include "EnemigoFinalInfanteria.h"

#include <QBrush>
#include <QImage>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QDebug>
#include <QGraphicsTextItem>
#include <QKeyEvent>
#include <QFont>

NivelHaciaBunkerPrueba::NivelHaciaBunkerPrueba(QWidget *parent)
    : QGraphicsView(parent)
    , scene(new QGraphicsScene(this))
    , jugador(nullptr)
    , enemigo(nullptr)
    , timerJuego(nullptr)
    , musicaFondo(nullptr)
    , audioFondo(nullptr)
    , reproduciendoFondo3(true)
    , juegoTerminado(false)
    , textoGameOver(nullptr)
    , timerGameOver(nullptr)
{
    // Dimensiones del mundo
    scene->setSceneRect(0, 0, sceneWidth, sceneHeight);
    setScene(scene);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(viewWidth, viewHeight);

    // Fondo estático
    scene->setBackgroundBrush(QBrush(QImage(":/images/bg3.png")));

    // Suelo a lo largo de todo el mundo
    qreal groundY = 400;
    QGraphicsRectItem *suelo = new QGraphicsRectItem(0, groundY, sceneWidth, sceneHeight - groundY);
    suelo->setBrush(Qt::black);
    suelo->setPen(Qt::NoPen);
    scene->addItem(suelo);

    // =============================================
    //        MÚSICA DE FONDO EN BUCLE
    // =============================================
    musicaFondo = new QMediaPlayer;
    audioFondo = new QAudioOutput;

    musicaFondo->setAudioOutput(audioFondo);
    audioFondo->setVolume(0.7);

    // Primera canción del ciclo
    musicaFondo->setSource(QUrl("qrc:/sonidos/fondo3.mp3"));
    musicaFondo->play();

    // Cuando termine una canción, iniciar la otra
    connect(musicaFondo, &QMediaPlayer::mediaStatusChanged,
            this, [this](QMediaPlayer::MediaStatus st)
            {
                if (st == QMediaPlayer::EndOfMedia && !juegoTerminado)
                {
                    // Alternar canción
                    reproduciendoFondo3 = !reproduciendoFondo3;

                    if (reproduciendoFondo3)
                        musicaFondo->setSource(QUrl("qrc:/sonidos/fondo3.mp3"));
                    else
                        musicaFondo->setSource(QUrl("qrc:/sonidos/fondo4.mp3"));

                    musicaFondo->play();
                }
            });

    // ===============================
    //   Sonido de GAME OVER
    // ===============================
    gameOverPlayer = new QMediaPlayer(this);
    gameOverAudio  = new QAudioOutput(this);

    gameOverPlayer->setAudioOutput(gameOverAudio);
    gameOverAudio->setVolume(0.9);
    gameOverPlayer->setSource(QUrl("qrc:/sonidos/game_over.mp3"));

    // ===============================
    //   Sonido de VICTORIA
    // ===============================
    victoriaPlayer = new QMediaPlayer(this);
    victoriaAudio  = new QAudioOutput(this);

    victoriaPlayer->setAudioOutput(victoriaAudio);
    victoriaAudio->setVolume(0.9);
    victoriaPlayer->setSource(QUrl("qrc:/sonidos/mision_completa.mp3"));

    // Jugador
    jugador = new JugadorInfanteria();
    jugador->setPos(100, 300);
    jugador->setGroundY(groundY);
    scene->addItem(jugador);

    // Inicializar posición de cámara en torno al jugador
    qreal jugadorCenterX = jugador->x() + jugador->boundingRect().width() / 2.0;
    qreal halfViewW      = viewWidth / 2.0;
    qreal minCenterX     = halfViewW;
    qreal maxCenterX     = sceneWidth - halfViewW;

    if (jugadorCenterX < minCenterX) jugadorCenterX = minCenterX;
    if (jugadorCenterX > maxCenterX) jugadorCenterX = maxCenterX;

    camXActual = jugadorCenterX;

    // ================================
    //   CREACIÓN DE ENEMIGOS NORMALES
    // ================================
    qreal baseY = groundY;   // la altura del suelo que ya usas

    // Lista de posiciones X donde aparecerán los enemigos
    QList<qreal> posicionesX;
    posicionesX << 600   // enemigo 1
                << 900   // enemigo 2
                << 1200  // enemigo 3
                << 1500  // enemigo 4
                << 1800  // enemigo 5
                << 2100  // enemigo 6
                << 2300  // enemigo 7
                << 2500; // enemigo 8

    for (qreal x : posicionesX) {
        EnemigoInfanteria *e = new EnemigoInfanteria(jugador);
        // Colocar al enemigo con los pies sobre el suelo
        e->setPos(x, baseY - e->boundingRect().height());
        scene->addItem(e);
    }

    // ================================
    //   CREACIÓN DEL JEFE FINAL
    // ================================

    EnemigoFinalInfanteria *boss = new EnemigoFinalInfanteria(jugador);
    qreal bossX = sceneWidth - 200; // rincón derecho
    qreal bossY = groundY - boss->boundingRect().height();
    boss->setPos(bossX, bossY);
    scene->addItem(boss);

    // HUD de vidas (corazones)
    inicializarHUDVidas();
    actualizarHUDPosicion();

    // Cámara inicial centrada cerca del jugador (sin salirse de los límites)
    actualizarCamara();

    // Timer del nivel para actualizar cámara y HUD (y futura lógica general)
    timerJuego = new QTimer(this);
    connect(timerJuego, &QTimer::timeout,
            this, &NivelHaciaBunkerPrueba::actualizarJuego);
    timerJuego->start(40); // ~25 FPS para refrescar HUD y cámara

    // Timer que revisa si el jugador ha muerto (vidas <= 0)
    timerGameOver = new QTimer(this);
    connect(timerGameOver, &QTimer::timeout, this, [this]() {
        if (!juegoTerminado && jugador && jugador->getVidas() <= 0) {
            mostrarGameOver();
        }
    });
    timerGameOver->start(100);
}

void NivelHaciaBunkerPrueba::inicializarHUDVidas()
{
    // Limpiar si hubiera algo previo
    for (auto *c : corazonesHUD) {
        if (scene) {
            scene->removeItem(c);
        }
        delete c;
    }
    corazonesHUD.clear();

    // Cargar corazón y escalarlo para que se vea pequeño
    QPixmap corazonOriginal(":/images/corazon.png");
    if (corazonOriginal.isNull()) {
        qDebug() << "No se pudo cargar :/images/corazon.png";
        return;
    }

    int heartW = 22;
    int heartH = 22;

    QPixmap corazon = corazonOriginal.scaled(
        heartW, heartH,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation);

    // 3 corazones, su posición real se ajusta en actualizarHUDPosicion()
    for (int i = 0; i < 3; ++i) {
        auto *item = new QGraphicsPixmapItem(corazon);
        item->setZValue(100); // por encima de todo
        scene->addItem(item);
        corazonesHUD.append(item);
    }

    if (jugador) {
        actualizarHUDVidas(jugador->getVidas());
    }
}

void NivelHaciaBunkerPrueba::actualizarHUDVidas(int vidas)
{
    // Se asume máximo 3 vidas
    for (int i = 0; i < corazonesHUD.size(); ++i) {
        if (corazonesHUD[i]) {
            corazonesHUD[i]->setVisible(i < vidas);
        }
    }
}

void NivelHaciaBunkerPrueba::actualizarCamara()
{
    if (!jugador) return;

    // Centro deseado según el jugador
    qreal jugadorCenterX = jugador->x() + jugador->boundingRect().width() / 2.0;

    qreal halfViewW  = viewWidth / 2.0;
    qreal minCenterX = halfViewW;
    qreal maxCenterX = sceneWidth - halfViewW;

    // Clampeamos el target
    qreal targetX = jugadorCenterX;
    if (targetX < minCenterX) targetX = minCenterX;
    if (targetX > maxCenterX) targetX = maxCenterX;

    // LERP suave: acercar camXActual hacia targetX
    camXActual = camXActual + (targetX - camXActual) * camLerpFactor;

    // Y fija en Y
    qreal camCenterY = sceneHeight / 2.0;

    centerOn(camXActual, camCenterY);
}


void NivelHaciaBunkerPrueba::actualizarHUDPosicion()
{
    if (corazonesHUD.isEmpty())
        return;

    // Medidas del corazón (todos tienen el mismo tamaño)
    int heartW = corazonesHUD[0]->pixmap().width();
    int heartH = corazonesHUD[0]->pixmap().height();

    int marginLeft   = 10;
    int marginBottom = 10;

    // Reposicionar los corazones en coordenadas de escena
    // pero anclados a la esquina inferior izquierda de la VISTA
    for (int i = 0; i < corazonesHUD.size(); ++i) {

        int viewportX = marginLeft + i * (heartW + 5);
        int viewportY = viewport()->height() - heartH - marginBottom;

        // Se convierte coordenadas de viewport a coordenadas de escena
        QPointF scenePos = mapToScene(QPoint(viewportX, viewportY));
        corazonesHUD[i]->setPos(scenePos);
    }
}

void NivelHaciaBunkerPrueba::actualizarJuego()
{
    if (juegoTerminado || juegoGanado)
        return;

    // Actualizar cámara (side-scroller)
    actualizarCamara();

    // Mantener el HUD pegado a la pantalla aunque la cámara se mueva
    actualizarHUDPosicion();

    // Actualizar HUD de vidas según el estado actual del jugador
    if (jugador) {
        actualizarHUDVidas(jugador->getVidas());
    }

    // ====== COMPROBAR CONDICIÓN DE VICTORIA ======
    bool hayEnemigosVivos = false;

    const auto items = scene->items();
    for (QGraphicsItem *item : items) {

        // Primero se prueba si es jefe final
        if (auto *boss = dynamic_cast<EnemigoFinalInfanteria*>(item)) {
            if (!boss->estaMuerto()) {
                hayEnemigosVivos = true;
                break;
            }
        }
        // Luego enemigos normales
        else if (auto *enemigo = dynamic_cast<EnemigoInfanteria*>(item)) {
            if (!enemigo->estaMuerto()) {
                hayEnemigosVivos = true;
                break;
            }
        }
    }

    if (!hayEnemigosVivos) {
        mostrarVictoria();
    }
}

void NivelHaciaBunkerPrueba::mostrarVictoria()
{
    if (juegoGanado)
        return;

    juegoGanado = true;

    // Detener música de fondo
    if (musicaFondo)
        musicaFondo->stop();

    // Reproducir sonido de victoria
    if (victoriaPlayer) {
        victoriaPlayer->stop();
        victoriaPlayer->play();
    }

    // Crear texto de victoria si aún no existe
    if (!textoVictoria) {
        textoVictoria = new QGraphicsTextItem;
        textoVictoria->setPlainText("MISION COMPLETADA!\nHas logrado despejar el bunker exitosamente");

        QFont font("Arial", 28, QFont::Bold);
        textoVictoria->setFont(font);
        textoVictoria->setDefaultTextColor(Qt::green);
        textoVictoria->setZValue(1000); // por encima de todo

        scene->addItem(textoVictoria);
    }

    // Centrar el texto en el centro de la CÁMARA (no de la escena)
    QRectF br = textoVictoria->boundingRect();
    QPoint viewCenter(viewport()->width() / 2, viewport()->height() / 2);
    QPointF sceneCenter = mapToScene(viewCenter);

    textoVictoria->setPos(sceneCenter.x() - br.width()  / 2.0,
                          sceneCenter.y() - br.height() / 2.0);
}


// =======================
//  GAME OVER / REINICIO
// =======================

void NivelHaciaBunkerPrueba::mostrarGameOver()
{
    juegoTerminado = true;

    // Detener música de fondo
    if (musicaFondo)
        musicaFondo->stop();

    // Reproducir sonido de derrota (se configura más abajo)
    if (gameOverPlayer) {
        gameOverPlayer->stop();
        gameOverPlayer->play();
    }

    // Crear texto si no existe
    if (!textoGameOver) {
        textoGameOver = new QGraphicsTextItem;
        textoGameOver->setPlainText("HAS MUERTO\nPresiona R para volver a jugar");

        QFont font("Arial", 28, QFont::Bold);
        textoGameOver->setFont(font);
        textoGameOver->setDefaultTextColor(Qt::white);
        textoGameOver->setZValue(1000); // bien por encima de todo

        scene->addItem(textoGameOver);
    }

    //  Posicionar el texto EN EL CENTRO DE LA CÁMARA (no del mundo)
    QRectF br = textoGameOver->boundingRect();

    // Centro de la vista (en coordenadas de viewport)
    QPoint viewCenter(viewport()->width() / 2, viewport()->height() / 2);

    // Convertimos el centro de la vista a coordenadas de escena
    QPointF sceneCenter = mapToScene(viewCenter);

    textoGameOver->setPos(sceneCenter.x() - br.width()  / 2.0,
                          sceneCenter.y() - br.height() / 2.0);
}

void NivelHaciaBunkerPrueba::reiniciarNivel()
{
    juegoTerminado = false;
    juegoGanado = false;

    // Eliminar texto de game over
    if (textoGameOver) {
        scene->removeItem(textoGameOver);
        delete textoGameOver;
        textoGameOver = nullptr;
    }

    // Eliminar texto de victoria si existiera
    if (textoVictoria) {
        scene->removeItem(textoVictoria);
        delete textoVictoria;
        textoVictoria = nullptr;
    }

    // Parar y destruir música actual
    if (musicaFondo) {
        musicaFondo->stop();
        delete musicaFondo;
        musicaFondo = nullptr;
    }
    if (audioFondo) {
        delete audioFondo;
        audioFondo = nullptr;
    }

    // Limpiar HUD (corazones)
    for (auto *c : corazonesHUD) {
        if (scene) scene->removeItem(c);
        delete c;
    }
    corazonesHUD.clear();

    // Limpiar todos los items de la escena
    scene->clear();

    // Volver a montar el nivel EXACTAMENTE igual que en el constructor
    // (mismo código, pero sin recrear "scene" ni la configuración del view)

    // Fondo de nuevo
    scene->setBackgroundBrush(QBrush(QImage(":/images/bg3.png")));

    // Suelo
    qreal groundY = 400;
    QGraphicsRectItem *suelo = new QGraphicsRectItem(0, groundY, sceneWidth, sceneHeight - groundY);
    suelo->setBrush(Qt::black);
    suelo->setPen(Qt::NoPen);
    scene->addItem(suelo);

    // Música de fondo nueva
    musicaFondo = new QMediaPlayer;
    audioFondo  = new QAudioOutput;
    musicaFondo->setAudioOutput(audioFondo);
    audioFondo->setVolume(0.7);

    reproduciendoFondo3 = true;
    musicaFondo->setSource(QUrl("qrc:/sonidos/fondo3.mp3"));
    musicaFondo->play();

    connect(musicaFondo, &QMediaPlayer::mediaStatusChanged,
            this, [this](QMediaPlayer::MediaStatus st)
            {
                if (st == QMediaPlayer::EndOfMedia && !juegoTerminado)
                {
                    reproduciendoFondo3 = !reproduciendoFondo3;
                    if (reproduciendoFondo3)
                        musicaFondo->setSource(QUrl("qrc:/sonidos/fondo3.mp3"));
                    else
                        musicaFondo->setSource(QUrl("qrc:/sonidos/fondo4.mp3"));

                    musicaFondo->play();
                }
            });

    // Jugador de nuevo
    jugador = new JugadorInfanteria();
    jugador->setPos(100, 300);
    jugador->setGroundY(groundY);
    scene->addItem(jugador);

    // Recalcular cámara inicial
    qreal jugadorCenterX = jugador->x() + jugador->boundingRect().width() / 2.0;
    qreal halfViewW      = viewWidth / 2.0;
    qreal minCenterX     = halfViewW;
    qreal maxCenterX     = sceneWidth - halfViewW;

    if (jugadorCenterX < minCenterX) jugadorCenterX = minCenterX;
    if (jugadorCenterX > maxCenterX) jugadorCenterX = maxCenterX;
    camXActual = jugadorCenterX;

    // Enemigos normales
    qreal baseY = groundY;
    QList<qreal> posicionesX;
    posicionesX << 600 << 900 << 1200 << 1500
                << 1800 << 2100 << 2400 << 2700;

    for (qreal x : posicionesX) {
        EnemigoInfanteria *e = new EnemigoInfanteria(jugador);
        e->setPos(x, baseY - e->boundingRect().height());
        scene->addItem(e);
    }

    // Jefe final
    EnemigoFinalInfanteria *boss = new EnemigoFinalInfanteria(jugador);
    qreal bossX = sceneWidth - 200;
    qreal bossY = groundY - boss->boundingRect().height();
    boss->setPos(bossX, bossY);
    scene->addItem(boss);

    // HUD de vidas
    inicializarHUDVidas();
    actualizarHUDPosicion();

    // Actualizar cámara
    actualizarCamara();
}

// Captura del teclado: R para reiniciar cuando hay game over
void NivelHaciaBunkerPrueba::keyPressEvent(QKeyEvent *event)
{
    if (juegoTerminado) {
        if (event->key() == Qt::Key_R) {
            reiniciarNivel();
        }
        return;
    }

    // Si se gana, ignorar todas las teclas (todo queda estático)
    if (juegoGanado) {
        return;
    }

    // Comportamiento normal mientras el juego está en curso
    QGraphicsView::keyPressEvent(event);
}




