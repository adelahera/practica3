# include "AIPlayer.h"
# include "Parchis.h"

const double masinf = 9999999999.0, menosinf = -9999999999.0;
const double gana = masinf - 1, pierde = menosinf + 1;
const int num_pieces = 4;
const int PROFUNDIDAD_MINIMAX = 4;  // Umbral maximo de profundidad para el metodo MiniMax
const int PROFUNDIDAD_ALFABETA = 6; // Umbral maximo de profundidad para la poda Alfa_Beta
double value;
double alpha = menosinf, beta = masinf;

bool AIPlayer::move(){
    cout << "Realizo un movimiento automatico" << endl;
    
    color c_piece;
    int id_piece;
    int dice;
    think(c_piece, id_piece, dice);

    cout << "Movimiento elegido: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    actual->movePiece(c_piece, id_piece, dice);
    return true;
}

void AIPlayer::think(color & c_piece, int & id_piece, int & dice) const{
    
    // El siguiente código se proporciona como sugerencia para iniciar la implementación del agente.

    // double valor; // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda.
    // double alpha = menosinf, beta = masinf; // Cotas iniciales de la poda AlfaBeta
    // // Llamada a la función para la poda (los parámetros son solo una sugerencia, se pueden modificar).
    // valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
    // cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    // ----------------------------------------------------------------- //

    // Si quiero poder manejar varias heurísticas, puedo usar la variable id del agente para usar una u otra.
    switch(id){
        case 0:
            value = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, miHeuristica);
            cout << "Valor minimax: " << value << " Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;            
            break;
        case 1:
            value = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
            cout << "Valor minimax: " << value << " Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;
            
            break;
        case 2:
            thinkAleatorio(c_piece, id_piece, dice);
            break;
        case 3:
            thinkAleatorioMasInteligente(c_piece, id_piece, dice);
            break;
        case 4:
            thinkFichaMasAdelantada(c_piece, id_piece, dice);
            break;
        case 5:
            thinkMejorOpcion(c_piece, id_piece, dice);
            break;
    }
    // cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    
}

void AIPlayer::thinkAleatorio(color & c_piece, int & id_piece, int & dice) const{

    // El color de ficha que se va a mover
    c_piece = actual->getCurrentColor();

    // Vector que almacenará los dados que se pueden usar para el movimiento
    vector<int> current_dices;
    // Vector que almacenará los ids de las fichas que se pueden mover para el dado elegido.
    vector<int> current_pieces;

    // Se obtiene el vector de dados que se pueden usar para el movimiento
    current_dices = actual->getAvailableDices(c_piece);
    // Elijo un dado de forma aleatoria.
    dice = current_dices[rand() % current_dices.size()];

    // Se obtiene el vector de fichas que se pueden mover para el dado elegido
    current_pieces = actual->getAvailablePieces(c_piece, dice);

    // Si tengo fichas para el dado elegido muevo una al azar.
    if(current_pieces.size() > 0){
        id_piece = current_pieces[rand() % current_pieces.size()];
    }
    else{
        // Si no tengo fichas para el dado elegido, pasa turno (la macro SKIP_TURN me permite no mover).
        id_piece = SKIP_TURN;
    }
}

void AIPlayer::thinkAleatorioMasInteligente(color & c_piece, int & id_piece, int & dice) const{

    c_piece = actual->getCurrentColor();

    vector<int> current_dices;
    vector<int> current_pieces;

    current_dices = actual->getAvailableDices(c_piece);

    vector<int> current_dices_que_pueden_mover_ficha;

    for(int i = 0; i < current_dices.size(); i++) {

        current_pieces = actual->getAvailablePieces(c_piece, current_dices[i]);
        if(current_pieces.size() > 0){

            current_dices_que_pueden_mover_ficha.push_back(current_dices[i]);
        }
    }

    if(current_dices_que_pueden_mover_ficha.size() == 0){

        dice = current_dices[rand() % current_dices.size()];
        id_piece = SKIP_TURN;
    }
    else {

        dice = current_dices_que_pueden_mover_ficha[rand() % current_dices_que_pueden_mover_ficha.size()];
        current_pieces = actual->getAvailablePieces(c_piece, dice);
        id_piece = current_pieces[rand() % current_pieces.size()];
    }

}

void AIPlayer::thinkFichaMasAdelantada(color & c_piece, int & id_piece, int & dice) const{

    thinkAleatorioMasInteligente(c_piece, id_piece, dice);

    vector<int> current_pieces = actual->getAvailablePieces(c_piece, dice);

    int id_ficha_mas_adelantada = -1;
    int min_distancia_meta = 9999;

    for(int i = 0; i < current_pieces.size(); i++){

        int distancia_meta = actual->distanceToGoal(c_piece, current_pieces[i]);
        if(distancia_meta < min_distancia_meta){

            min_distancia_meta = distancia_meta;
            id_ficha_mas_adelantada = current_pieces[i];
        }
    }

    if(id_ficha_mas_adelantada == -1){
        id_piece = SKIP_TURN;
    }
    else {
        id_piece = id_ficha_mas_adelantada;
    }

}

void AIPlayer::thinkMejorOpcion(color & c_piece, int & id_piece, int & dice) const{

    color last_c_piece = none;
    int last_id_piece = -1;
    int last_dice = -1;

    Parchis siguiente_hijo = actual->generateNextMove(last_c_piece, last_id_piece, last_dice);
    bool me_quedo_con_esta_accion = false;

    while(!(siguiente_hijo == *actual) && !me_quedo_con_esta_accion) {

        if(siguiente_hijo.isEatingMove() or
            siguiente_hijo.isGoalMove() or
            (siguiente_hijo.gameOver() and siguiente_hijo.getWinner() == this->jugador)) {

             me_quedo_con_esta_accion = true;   

        }
        else {
            siguiente_hijo = actual->generateNextMove(last_c_piece, last_id_piece, last_dice);
        }

    }

    if(me_quedo_con_esta_accion){
        c_piece = last_c_piece;
        id_piece = last_id_piece;
        dice = last_dice;
    }
    else{
        thinkFichaMasAdelantada(c_piece, id_piece, dice);
    }
}

double AIPlayer::Poda_AlfaBeta(const Parchis &actual, int jugador, int profundidad, int profundidad_max, color &c_piece, int &id_piece, int &dice, double alpha, double beta, double (*heuristic)(const Parchis &, int)) const {

    color last_c_piece = none;
    int last_id_piece = -1;
    int last_dice = -1;   

    double value;

    if(actual.gameOver() || profundidad == profundidad_max){

        //devolver el valor de la heuristica del nodo
        return heuristic(actual, jugador);
    }

    Parchis hijo = actual.generateNextMoveDescending(last_c_piece, last_id_piece, last_dice);

    if(actual.getCurrentPlayerId() == jugador){        //Nodo MAX
        //para cada hijo del nodo
        while(!(hijo == actual) && beta > alpha){
            value = max(alpha,Poda_AlfaBeta(hijo, jugador, profundidad+1, profundidad_max, c_piece, id_piece, dice, alpha, beta, heuristic));

            if(value > alpha and profundidad == 0){
                c_piece = last_c_piece;
                id_piece = last_id_piece;
                dice = last_dice;
            }

            alpha = max(alpha, value);
                   
            hijo = actual.generateNextMoveDescending(last_c_piece, last_id_piece, last_dice);
            
        }

        return alpha;
    }
    else{                                   //Nodo MIN
        //para cada hijo del nodo
        while(!(hijo == actual) && beta > alpha){
            value = min(beta,Poda_AlfaBeta(hijo, jugador, profundidad+1, profundidad_max, c_piece, id_piece, dice, alpha, beta, heuristic));

            if(value < beta and profundidad == 0){

                c_piece = last_c_piece;
                id_piece = last_id_piece;
                dice = last_dice;
            }

            beta = min(beta, value);
            
            hijo = actual.generateNextMoveDescending(last_c_piece, last_id_piece, last_dice);
            
        }

        return beta;
    }
}

double AIPlayer::miHeuristica(const Parchis &estado, int jugador) {

    int ganador = estado.getWinner();
    int oponente = (jugador + 1) % 2;

    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    if (ganador == jugador)
    {
        return gana;
    }
    else if (ganador == oponente)
    {
        return pierde;
    }
    else
    {
        // Colores que juega mi jugador y colores del oponente
        vector<color> my_colors = estado.getPlayerColors(jugador);
        vector<color> op_colors = estado.getPlayerColors(oponente);
        double puntos_colores[my_colors.size()];
        double puntos_colores_rival[op_colors.size()];
        color mejor_color;
        color peor_color;
        color mejor_color_rival;
        color peor_color_rival;

        // Recorro todas las fichas de mi jugador
        int puntuacion_jugador = 0;
        int puntuacion_jugador = 0;
        // Recorro colores de mi jugador.
        for (int i = 0; i < my_colors.size(); i++) {
            
            color c = my_colors[i];

            for(int k = 0; k < num_pieces; k++) {

                puntos_colores[i] += estado.distanceToGoal(c, k);
            }
        }

        if(puntos_colores[0] > puntos_colores[1]) {
            mejor_color = my_colors[1];
            peor_color = my_colors[0];
        }
        else {
            mejor_color = my_colors[0];
            peor_color = my_colors[1];
        }

            // Recorro las fichas de ese color.

        for (int j = 0; j < num_pieces; j++) {

            // Valoro positivamente que la ficha esté en casilla segura o meta.
            if (estado.isSafePiece(mejor_color, j)) {

                puntuacion_jugador+=15;
            }

            if (estado.getBoard().getPiece(mejor_color, j).type == goal){

                puntuacion_jugador += 25;
            }
            else if(estado.getBoard().getPiece(mejor_color, j).type == final_queue) {

                puntuacion_jugador += 20;
            }
            else if(estado.getBoard().getPiece(mejor_color, j).type != home) {

                puntuacion_jugador += 5;
            }

            if (estado.isSafePiece(peor_color, j)) {

                puntuacion_jugador+=7;
            }

            if (estado.getBoard().getPiece(peor_color, j).type == goal){

                puntuacion_jugador += 15;
            }
            else if(estado.getBoard().getPiece(peor_color, j).type == final_queue) {

                puntuacion_jugador += 10;
            }
            else if(estado.getBoard().getPiece(peor_color, j).type != home) {

                puntuacion_jugador += 3;
            }

            int num = estado.distanceToGoal(mejor_color, j);
            int res;

            if(num < 64 && num > 0) {
                res = (64 / num) % 20;
            }
            else if(num == 0) {

                res = 20;
            }

            int num2 = estado.distanceToGoal(peor_color, j);
            int res2;

            if(num2 < 64 && num2 > 0) {
                res2 = (64 / num2) % 20;
            }
            else if(num2 == 0) {

                res2 = 20;
            }
        }

        if(estado.eatenPiece().first == mejor_color_rival) {

            puntuacion_jugador += 100;
        }
        else if(estado.eatenPiece().first == peor_color_rival) {

            puntuacion_jugador += 20;
        }

        

        // Recorro todas las fichas del oponente
        int puntuacion_oponente = 0;
        // Recorro colores del oponente.
        for (int i = 0; i < op_colors.size(); i++)
        {
            color c = op_colors[i];

            for(int k = 0; k < num_pieces; k++) {

                puntos_colores_rival[i] += estado.distanceToGoal(c, k);
            }
        }

        if(puntos_colores_rival[0] > puntos_colores_rival[1]) {
            mejor_color_rival = op_colors[1];
            peor_color_rival = op_colors[0];
        }
        else {
            mejor_color_rival = op_colors[0];
            peor_color_rival = op_colors[1];
        }            

            // Recorro las fichas de ese color.
        for (int j = 0; j < num_pieces; j++) {

            // Valoro positivamente que la ficha esté en casilla segura o meta.
            if (estado.isSafePiece(mejor_color_rival, j)) {

                puntuacion_oponente+=15;
            }

            if (estado.getBoard().getPiece(mejor_color_rival, j).type == goal){

                puntuacion_oponente += 25;
            }
            else if(estado.getBoard().getPiece(mejor_color_rival, j).type == final_queue) {

                puntuacion_oponente += 20;
            }
            else if(estado.getBoard().getPiece(mejor_color_rival, j).type != home) {

                puntuacion_oponente += 5;
            }

            if (estado.isSafePiece(peor_color_rival, j)) {

                puntuacion_oponente+=7;
            }

            if (estado.getBoard().getPiece(peor_color_rival, j).type == goal){

                puntuacion_oponente += 15;
            }
            else if(estado.getBoard().getPiece(peor_color_rival, j).type == final_queue) {

                puntuacion_oponente += 10;
            }
            else if(estado.getBoard().getPiece(peor_color_rival, j).type != home) {

                puntuacion_oponente += 3;
            } 

            int num = estado.distanceToGoal(mejor_color_rival, j);
            int res;

            if(num < 64 && num > 0) {
                res = (64 / num) % 20;
            }
            else if(num == 0) {

                res = 20;
            }

            int num2 = estado.distanceToGoal(peor_color_rival, j);
            int res2;

            if(num2 < 64 && num2 > 0) {
                res2 = (64 / num2) % 20;
            }
            else if(num2 == 0) {

                res2 = 20;
            }                      
        }

         

        // for(int i = 0; i < num_pieces; i++) {
        //     for(int j = 0; j < num_pieces; j++) {

        //         if(estado.distanceBoxtoBox(mejor_color_rival, j, mejor_color, i) != -1 && estado.distanceBoxtoBox(mejor_color_rival, j, mejor_color, i) <= 6) {

        //             puntuacion_oponente += 7;
        //         }

        //         if(estado.distanceBoxtoBox(peor_color_rival, j, mejor_color, i) != -1 && estado.distanceBoxtoBox(peor_color_rival, j, mejor_color, i) <= 6) {

        //             puntuacion_oponente += 5;
        //         }

        //         if(estado.distanceBoxtoBox(mejor_color_rival, j, peor_color, i) != -1 && estado.distanceBoxtoBox(mejor_color_rival, j, peor_color, i) <= 6) {

        //             puntuacion_oponente += 5;
        //         }

        //         if(estado.distanceBoxtoBox(peor_color_rival, j, peor_color, i) != -1 && estado.distanceBoxtoBox(peor_color_rival, j, peor_color, i) <= 6) {

        //             puntuacion_oponente += 3;
        //         }


        //         if(estado.distanceBoxtoBox(mejor_color, i, mejor_color_rival, j) != -1 && estado.distanceBoxtoBox(mejor_color, i, mejor_color_rival, j) <= 6) {

        //             puntuacion_jugador += 7;
        //         }

        //         if(estado.distanceBoxtoBox(peor_color, i, mejor_color_rival, j) != -1 && estado.distanceBoxtoBox(peor_color, i, mejor_color_rival, j) <= 6) {

        //             puntuacion_jugador += 5;
        //         }

        //         if(estado.distanceBoxtoBox(mejor_color, i, peor_color_rival, j) != -1 && estado.distanceBoxtoBox(mejor_color, i, peor_color_rival, j) <= 6) {

        //             puntuacion_jugador += 5;
        //         }

        //         if(estado.distanceBoxtoBox(peor_color, i, peor_color_rival, j) != -1 && estado.distanceBoxtoBox(peor_color, i, peor_color_rival, j) <= 6) {

        //             puntuacion_jugador += 3;
        //         }            

        //     }
        // }
            
        if(estado.eatenPiece().first == mejor_color) {

            puntuacion_oponente += 100;
        }
        else if(estado.eatenPiece().first == peor_color) {

            puntuacion_oponente += 20;
        }

        // Devuelvo la puntuación de mi jugador menos la puntuación del oponente.
        return puntuacion_jugador - puntuacion_oponente;
    }

}

double AIPlayer::ValoracionTest(const Parchis &estado, int jugador)
{
    // Heurística de prueba proporcionada para validar el funcionamiento del algoritmo de búsqueda.


    int ganador = estado.getWinner();
    int oponente = (jugador + 1) % 2;

    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    if (ganador == jugador)
    {
        return gana;
    }
    else if (ganador == oponente)
    {
        return pierde;
    }
    else
    {
        // Colores que juega mi jugador y colores del oponente
        vector<color> my_colors = estado.getPlayerColors(jugador);
        vector<color> op_colors = estado.getPlayerColors(oponente);

        // Recorro todas las fichas de mi jugador
        int puntuacion_jugador = 0;
        // Recorro colores de mi jugador.
        for (int i = 0; i < my_colors.size(); i++)
        {
            color c = my_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                // Valoro positivamente que la ficha esté en casilla segura o meta.
                if (estado.isSafePiece(c, j))
                {
                    puntuacion_jugador++;
                }
                else if (estado.getBoard().getPiece(c, j).type == goal)
                {
                    puntuacion_jugador += 5;
                }
            }
        }

        // Recorro todas las fichas del oponente
        int puntuacion_oponente = 0;
        // Recorro colores del oponente.
        for (int i = 0; i < op_colors.size(); i++)
        {
            color c = op_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                if (estado.isSafePiece(c, j))
                {
                    // Valoro negativamente que la ficha esté en casilla segura o meta.
                    puntuacion_oponente++;
                }
                else if (estado.getBoard().getPiece(c, j).type == goal)
                {
                    puntuacion_oponente += 5;
                }
            }
        }

        // Devuelvo la puntuación de mi jugador menos la puntuación del oponente.
        return puntuacion_jugador - puntuacion_oponente;
    }
}

