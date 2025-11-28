#include <QApplication>
#include "NivelBatallaAerea.h"
#include "NivelBatallaTanque.h"
juego *Juego = nullptr;

int main(int argc, char *argv[]){
    QApplication a(argc, argv);

    // === PARA PROBAR NIVEL 2 (TANQUE) ===
    NivelBatallaTanque *nivel2 = new NivelBatallaTanque();
    nivel2->show();

     //Si quisieras volver a probar el nivel 1:
    //Juego = new juego();
    //Juego->show();

    return a.exec();
}

