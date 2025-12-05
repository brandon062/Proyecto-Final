#include "NivelBatallaTanque.h"
#include "CajaReparacion.h"

#include <QBrush>
#include <QImage>
#include <QFont>

NivelBatallaTanque::NivelBatallaTanque(QWidget *parent)
    : QGraphicsView(parent)
    , scene(new QGraphicsScene(this))
    , tanqueJugador(nullptr)
    , tanqueEnemigo1(nullptr)
    , tanqueEnemigo2(nullptr)
    , tanqueEnemigo3(nullptr)
    , estructura1(nullptr)
    , estructura2(nullptr)
    , estructura3(nullptr)
    , mina1(nullptr)
    , mina2(nullptr)
    , mina3(nullptr)
    , mina4(nullptr)
    , vidaIcon1(nullptr)
    , vidaIcon2(nullptr)
    , vidaIcon3(nullptr)
    , timerProgreso(new QTimer(this))
    , nivelActual(1)
    , enTransicion(false)
    , mensajeAvanzar(nullptr)
    , mensajeMostrado(false)
    , enGameOver(false)
    , gameOverPlayer(new QMediaPlayer(this))
    , gameOverAudio(new QAudioOutput(this))
    , vidasGlobal(3)
    , bgPlayer(new QMediaPlayer(this))
    , bgAudio(new QAudioOutput(this))
    , bgTrackIndex(1)
    , missionPlayer(new QMediaPlayer(this))
    , missionAudio(new QAudioOutput(this))
    , misionCompletada(false)
{
    scene->setSceneRect(0, 0, 800, 600);
    setScene(scene);

    setBackgroundBrush(QBrush(QImage(":/images/bg2.png")));
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(800, 600);

    // Audio de Game Over
    gameOverPlayer->setAudioOutput(gameOverAudio);
    gameOverAudio->setVolume(0.9);
    gameOverPlayer->setSource(QUrl("qrc:/sonidos/game_over.mp3"));

    // Música de fondo
    bgPlayer->setAudioOutput(bgAudio);
    bgAudio->setVolume(0.6);
    connect(bgPlayer, &QMediaPlayer::mediaStatusChanged,
            this, &NivelBatallaTanque::onBgStatusChanged);

    bgTrackIndex = 1;
    bgPlayer->setSource(QUrl("qrc:/sonidos/fondo1.mp3"));
    bgPlayer->play();

    // Música de misión completada
    missionPlayer->setAudioOutput(missionAudio);
    missionAudio->setVolume(0.9);
    missionPlayer->setSource(QUrl("qrc:/sonidos/mision_completa.mp3"));

    // primer nivel
    configurarNivel1();

    connect(timerProgreso, &QTimer::timeout,
            this, &NivelBatallaTanque::verificarProgreso);
    timerProgreso->start(50);

    show();
}

// --------- UI VIDAS ---------

void NivelBatallaTanque::crearVidasUI()
{
    // Siempre se llama después de scene->clear(), así que los iconos viejos
    // ya están destruidos por la escena. Dejamos simplemente punteros limpios:
    vidaIcon1 = nullptr;
    vidaIcon2 = nullptr;
    vidaIcon3 = nullptr;

    if (!scene)
        return;

    QPixmap icon(":/images/vida_tanque.png");   // ajusta el path
    if (icon.isNull())
        return;

    QPixmap esc = icon.scaled(32, 32,
                              Qt::KeepAspectRatio,
                              Qt::SmoothTransformation);

    int baseY = 600 - esc.height() - 8;
    int baseX = 10;
    int sep   = 5;

    vidaIcon1 = scene->addPixmap(esc);
    vidaIcon1->setPos(baseX, baseY);

    vidaIcon2 = scene->addPixmap(esc);
    vidaIcon2->setPos(baseX + esc.width() + sep, baseY);

    vidaIcon3 = scene->addPixmap(esc);
    vidaIcon3->setPos(baseX + 2 * (esc.width() + sep), baseY);
}

void NivelBatallaTanque::actualizarVidasUI(int vidasRestantes)
{
    vidasGlobal = vidasRestantes;
    if (vidaIcon1) vidaIcon1->setVisible(vidasRestantes >= 1);
    if (vidaIcon2) vidaIcon2->setVisible(vidasRestantes >= 2);
    if (vidaIcon3) vidaIcon3->setVisible(vidasRestantes >= 3);
}

// --------- CONFIGURACIÓN DE NIVELES ---------

void NivelBatallaTanque::configurarNivel1()
{
    scene->clear();
    scene->setBackgroundBrush(QBrush(QImage(":/images/bg2.png")));

    nivelActual = 1;
    enTransicion = false;
    mensajeMostrado = false;
    mensajeAvanzar = nullptr;
    enGameOver = false;
    misionCompletada = false;

    crearVidasUI();
    actualizarVidasUI(vidasGlobal);

    // Estructuras
    estructura1 = new EstructuraMapa(":/images/edificio1.png");
    estructura1->setPos(80, 80);
    scene->addItem(estructura1);

    estructura2 = new EstructuraMapa(":/images/edificio2.png");
    estructura2->setPos(550, 70);
    scene->addItem(estructura2);

    estructura3 = new EstructuraMapa(":/images/edificio3.png");
    estructura3->setPos(360, 300);
    estructura3->setScale(2);
    scene->addItem(estructura3);

    // Minas
    mina1 = new Mina(); mina1->setPos(250, 260); scene->addItem(mina1);
    mina2 = new Mina(); mina2->setPos(500, 380); scene->addItem(mina2);
    mina3 = new Mina(); mina3->setPos(180, 320); scene->addItem(mina3);
    mina4 = new Mina(); mina4->setPos(600, 300); scene->addItem(mina4);

    // Tanque jugador
    tanqueJugador = new Tanque();
    tanqueJugador->setFlag(QGraphicsItem::ItemIsFocusable);
    tanqueJugador->setFocus();
    tanqueJugador->setPos(120, 450);
    scene->addItem(tanqueJugador);

    // aplicar vidas globales al tanque recién creado
    tanqueJugador->setVidas(vidasGlobal);

    connect(tanqueJugador, &Tanque::vidasCambiaron,
            this, &NivelBatallaTanque::actualizarVidasUI);
    connect(tanqueJugador, &Tanque::jugadorMurio,
            this, &NivelBatallaTanque::manejarGameOver);

    // Enemigo
    tanqueEnemigo1 = new TanqueEnemigo(tanqueJugador);
    tanqueEnemigo1->setPos(620, 120);
    scene->addItem(tanqueEnemigo1);

    tanqueEnemigo2 = nullptr;
    tanqueEnemigo3 = nullptr;
}

void NivelBatallaTanque::configurarNivel2()
{
    scene->clear();
    scene->setBackgroundBrush(QBrush(QImage(":/images/bg2.png")));

    nivelActual = 2;
    enTransicion = false;
    mensajeMostrado = false;
    mensajeAvanzar = nullptr;
    enGameOver = false;
    misionCompletada = false;

    crearVidasUI();
    actualizarVidasUI(vidasGlobal);

    // Estructuras
    estructura1 = new EstructuraMapa(":/images/edificio4.png");
    estructura1->setPos(100, 120);
    scene->addItem(estructura1);

    estructura2 = new EstructuraMapa(":/images/edificio5.png");
    estructura2->setPos(520, 150);
    scene->addItem(estructura2);

    estructura3 = new EstructuraMapa(":/images/edificio6.png");
    estructura3->setPos(330, 350);
    scene->addItem(estructura3);

    // Minas
    mina1 = new Mina(); mina1->setPos(220, 300); scene->addItem(mina1);
    mina2 = new Mina(); mina2->setPos(580, 320); scene->addItem(mina2);
    mina3 = new Mina(); mina3->setPos(150, 450); scene->addItem(mina3);
    mina4 = new Mina(); mina4->setPos(450, 250); scene->addItem(mina4);

    // Caja de reparación SOLO en nivel 2
    CajaReparacion *caja = new CajaReparacion();
    caja->setPos(360, 300);
    scene->addItem(caja);

    // Tanque jugador
    tanqueJugador = new Tanque();
    tanqueJugador->setFlag(QGraphicsItem::ItemIsFocusable);
    tanqueJugador->setFocus();
    tanqueJugador->setPos(400, 520);
    scene->addItem(tanqueJugador);

    // aplicar vidas globales al tanque recién creado
    tanqueJugador->setVidas(vidasGlobal);

    connect(tanqueJugador, &Tanque::vidasCambiaron,
            this, &NivelBatallaTanque::actualizarVidasUI);
    connect(tanqueJugador, &Tanque::jugadorMurio,
            this, &NivelBatallaTanque::manejarGameOver);

    // Enemigos (2)
    tanqueEnemigo1 = new TanqueEnemigo(tanqueJugador);
    tanqueEnemigo1->setPos(30, 240);
    scene->addItem(tanqueEnemigo1);

    tanqueEnemigo2 = new TanqueEnemigo(tanqueJugador);
    tanqueEnemigo2->setPos(400, 180);
    scene->addItem(tanqueEnemigo2);

    tanqueEnemigo3 = nullptr;
}

void NivelBatallaTanque::configurarNivel3()
{
    scene->clear();
    scene->setBackgroundBrush(QBrush(QImage(":/images/bg2.png")));

    nivelActual = 3;
    enTransicion = false;
    mensajeMostrado = false;
    mensajeAvanzar = nullptr;
    enGameOver = false;
    misionCompletada = false;

    crearVidasUI();
    actualizarVidasUI(vidasGlobal);

    // Estructuras
    estructura1 = new EstructuraMapa(":/images/edificio4.png");
    estructura1->setPos(90, 200);
    scene->addItem(estructura1);

    estructura2 = new EstructuraMapa(":/images/edificio3.png");
    estructura2->setPos(520, 80);
    estructura2->setScale(2);
    scene->addItem(estructura2);

    estructura3 = new EstructuraMapa(":/images/edificio7.png");
    estructura3->setPos(300, 380);
    estructura3->setScale(1.5);
    scene->addItem(estructura3);

    // Minas
    mina1 = new Mina(); mina1->setPos(200, 260); scene->addItem(mina1);
    mina2 = new Mina(); mina2->setPos(570, 260); scene->addItem(mina2);
    mina3 = new Mina(); mina3->setPos(260, 440); scene->addItem(mina3);
    mina4 = new Mina(); mina4->setPos(480, 380); scene->addItem(mina4);

    // Tanque jugador
    tanqueJugador = new Tanque();
    tanqueJugador->setFlag(QGraphicsItem::ItemIsFocusable);
    tanqueJugador->setFocus();
    tanqueJugador->setPos(380, 520);
    scene->addItem(tanqueJugador);

    // aplicar vidas globales al tanque recién creado
    tanqueJugador->setVidas(vidasGlobal);

    connect(tanqueJugador, &Tanque::vidasCambiaron,
            this, &NivelBatallaTanque::actualizarVidasUI);
    connect(tanqueJugador, &Tanque::jugadorMurio,
            this, &NivelBatallaTanque::manejarGameOver);

    // Enemigos (3)
    tanqueEnemigo1 = new TanqueEnemigo(tanqueJugador);
    tanqueEnemigo1->setPos(140, 120);
    scene->addItem(tanqueEnemigo1);

    tanqueEnemigo2 = new TanqueEnemigo(tanqueJugador);
    tanqueEnemigo2->setPos(400, 150);
    scene->addItem(tanqueEnemigo2);

    tanqueEnemigo3 = new TanqueEnemigo(tanqueJugador);
    tanqueEnemigo3->setPos(350, 180);
    scene->addItem(tanqueEnemigo3);
}

// --------- PANTALLA CARGANDO ---------

void NivelBatallaTanque::mostrarPantallaCargando()
{
    scene->clear();
    scene->setBackgroundBrush(Qt::black);

    QGraphicsTextItem *texto = scene->addText("Cargando...");
    QFont fuente("Arial", 24);
    texto->setFont(fuente);
    texto->setDefaultTextColor(Qt::white);

    QRectF sr = scene->sceneRect();
    QRectF br = texto->boundingRect();
    texto->setPos(sr.center().x() - br.width() / 2.0,
                  sr.center().y() - br.height() / 2.0);
}

// --------- PROGRESO ENTRE NIVELES ---------

void NivelBatallaTanque::verificarProgreso()
{
    // Si estamos en transición, game over o ya se mostró la misión completada,
    // no hacemos nada.
    if (enTransicion || enGameOver || misionCompletada)
        return;

    if (!tanqueJugador)
        return;
    if (!tanqueJugador->scene())
        return;  // jugador fue removido (murió)

    // ¿Quedan tanques enemigos en la escena?
    bool hayEnemigos = false;
    const auto itemsEscena = scene->items();
    for (QGraphicsItem *it : itemsEscena) {
        if (dynamic_cast<TanqueEnemigo*>(it)) {
            hayEnemigos = true;
            break;
        }
    }

    // === NIVEL 3: Misión completada ===
    if (nivelActual == 3) {
        if (!hayEnemigos) {
            mostrarMisionCompletada();
        }
        return;   // en nivel 3 ya no hay más escenas por arriba
    }

    // === NIVELES 1 y 2: lógica de avanzar ===

    if (!hayEnemigos && !mensajeMostrado) {
        mensajeAvanzar = scene->addText("Todos los enemigos destruidos, sigue avanzando!");
        QFont fuente("Arial", 14);
        mensajeAvanzar->setFont(fuente);
        mensajeAvanzar->setDefaultTextColor(Qt::black);
        mensajeAvanzar->setPos(10, 10);
        mensajeMostrado = true;
    }

    if (hayEnemigos)
        return;

    // Si no hay enemigos y el jugador sobrepasa el límite superior, cambiamos de nivel
    qreal parteInferior = tanqueJugador->y() + tanqueJugador->boundingRect().height();
    if (parteInferior < 0) {
        enTransicion = true;
        mostrarPantallaCargando();

        if (nivelActual == 1) {
            QTimer::singleShot(2000, this, &NivelBatallaTanque::cargarNivel2);
        } else if (nivelActual == 2) {
            QTimer::singleShot(2000, this, &NivelBatallaTanque::cargarNivel3);
        }
    }
}

void NivelBatallaTanque::cargarNivel2()
{
    configurarNivel2();
}

void NivelBatallaTanque::cargarNivel3()
{
    configurarNivel3();
}

// --------- GAME OVER ---------

void NivelBatallaTanque::manejarGameOver()
{
    if (enGameOver)
        return;

    enGameOver = true;
    enTransicion = true;

    detenerMusicaFondo();

    QGraphicsTextItem *texto =
        scene->addText("Has sido destruido.\nPresiona R para volver a jugar");
    QFont fuente("Arial", 20);
    texto->setFont(fuente);
    texto->setDefaultTextColor(Qt::red);

    QRectF sr = scene->sceneRect();
    QRectF br = texto->boundingRect();
    texto->setPos(sr.center().x() - br.width() / 2.0,
                  sr.center().y() - br.height() / 2.0);

    if (gameOverPlayer->playbackState() == QMediaPlayer::PlayingState) {
        gameOverPlayer->setPosition(0);
    } else {
        gameOverPlayer->play();
    }
}

// --------- Musica de fondo ---------

void NivelBatallaTanque::onBgStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia) {
        // Cambiar de canción
        if (bgTrackIndex == 1) {
            bgTrackIndex = 2;
            bgPlayer->setSource(QUrl("qrc:/sonidos/fondo2.mp3"));
        } else {
            bgTrackIndex = 1;
            bgPlayer->setSource(QUrl("qrc:/sonidos/fondo1.mp3"));
        }
        bgPlayer->play();
    }
}

void NivelBatallaTanque::detenerMusicaFondo()
{
    if (bgPlayer) {
        bgPlayer->stop();
    }
}

// --------- Mision completa ---------

void NivelBatallaTanque::mostrarMisionCompletada()
{
    if (misionCompletada)
        return;

    misionCompletada = true;
    enTransicion = true;   // pausa cualquier otra lógica de progreso

    // Detener música de fondo
    detenerMusicaFondo();

    //️ Congelar al tanque del jugador y apagarle sus sonidos
    if (tanqueJugador) {
        tanqueJugador->setCongelado(true);
        tanqueJugador->clearFocus();               // no recibe más teclas
        tanqueJugador->setFlag(QGraphicsItem::ItemIsFocusable, false);
    }

    // Texto de victoria
    QGraphicsTextItem *texto =
        scene->addText("MISION COMPLETADA!!\nlograstes eliminar a todos los tanques alemanes");
    QFont fuente("Arial", 25);
    texto->setFont(fuente);
    texto->setDefaultTextColor(Qt::green);

    QRectF sr = scene->sceneRect();
    QRectF br = texto->boundingRect();
    texto->setPos(sr.center().x() - br.width() / 2.0,
                  sr.center().y() - br.height() / 2.0);

    // Música de misión completada
    if (missionPlayer->playbackState() == QMediaPlayer::PlayingState) {
        missionPlayer->setPosition(0);
    } else {
        missionPlayer->play();
    }

    QTimer::singleShot(6000, this, [this]() {
        emit nivelCompletado(true);   // avisa al MainWindow DESPUÉS de los 6 s
    });
}


// --------- TECLAS (reinicio con R) ---------

void NivelBatallaTanque::keyPressEvent(QKeyEvent *event)
{
    if (enGameOver && event->key() == Qt::Key_R) {
        gameOverPlayer->stop();

        // Reset de vidas globales
        vidasGlobal = 3;

        // Reset de estado de misión
        misionCompletada = false;
        enGameOver = false;
        enTransicion = false;

        //  Reiniciar música de fondo desde fondo1
        bgTrackIndex = 1;
        bgPlayer->setSource(QUrl("qrc:/sonidos/fondo1.mp3"));
        bgPlayer->play();

        configurarNivel1();   // empezar desde escena 1
        return;
    }

    QGraphicsView::keyPressEvent(event);
}



