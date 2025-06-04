// @author: Arthur Cren
// @version: 1.0
// @date: 26-10-2024
// @description: Un serpent qui avance vers la droite et qui s'arrête si on appuie sur 'a'

// Inclusion des bibliothèques
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

// Déclaration des constantes
#define TAILLE_SERPENT 10
#define LIMITE_X 40
#define LIMITE_Y 40
#define TEMPORISATION 200000
#define STOP 'a'
#define TETE 'O'
#define CORPS 'X'

// Déclaration des fonctions
int kbhit();
void gotoXY(int x, int y);
void afficher(int x, int y, char c);
void effacer(int x, int y);
void dessinerSerpent(int coordonnees[TAILLE_SERPENT][2]);
void progresser(int positions[TAILLE_SERPENT][2], char direction);
void disableEcho();
void enableEcho();

int main() {
    int positions[TAILLE_SERPENT][2];
    char direction = 'd'; // Initialement vers la droite

    // Initialisation des positions du serpent
    for (int i = 0; i < TAILLE_SERPENT; i++) {
        positions[i][0] = 20 - i;
        positions[i][1] = 20;
    }

    // Désactiver l'écho pour la saisie de la touche d'arrêt
    disableEcho();

    // Effacer le terminal
    system("clear");

    // Affichage initial du serpent
    dessinerSerpent(positions);

    // Boucle principale
    while (1) {
        if (kbhit()) {
            // Si une touche est appuyée, lire le caractère
            char ch = getchar();

            // Si la touche est 'a', quitter la boucle
            if (ch == STOP) {
                break;
            }

            // Changer la direction du serpent
            if ((ch == 'q' && direction != 'd') || 
                (ch == 'd' && direction != 'q') || 
                (ch == 'z' && direction != 's') || 
                (ch == 's' && direction != 'z')) {
                direction = ch;
            }
        }

        // Effacer la queue du serpent
        effacer(positions[TAILLE_SERPENT - 1][0], positions[TAILLE_SERPENT - 1][1]);

        // Faire progresser le serpent dans la direction actuelle
        progresser(positions, direction);

        // Dessiner le serpent à sa nouvelle position
        dessinerSerpent(positions);

        // Pause pour ralentir le mouvement
        usleep(TEMPORISATION);
    }

    // Réactiver l'écho
    enableEcho();

    printf("\n");
    return EXIT_SUCCESS;
}

// Fonction pour vérifier si une touche a été pressée
int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

// Fonction pour déplacer le curseur à la position (x, y)
void gotoXY(int x, int y) {
    printf("\033[%d;%dH", y, x);
}

// Fonction pour afficher un caractère à la position (x, y)
void afficher(int x, int y, char c) {
    gotoXY(x, y);
    printf("%c", c);
}

// Fonction pour effacer le caractère à la position (x, y)
void effacer(int x, int y) {
    afficher(x, y, ' ');
}

// Fonction pour dessiner le serpent
void dessinerSerpent(int coordonnees[TAILLE_SERPENT][2]) {
    // Dessiner la tête du serpent ('O') uniquement si elle est dans les limites visibles
    if (coordonnees[0][0] >= 1 && coordonnees[0][0] <= LIMITE_X && coordonnees[0][1] >= 1 && coordonnees[0][1] <= LIMITE_Y) {
        afficher(coordonnees[0][0], coordonnees[0][1], TETE);
    }

    // Dessiner le corps du serpent ('X') pour les segments de 1 à TAILLE_SERPENT-1
    for (int i = 1; i < TAILLE_SERPENT; i++) {
        if (coordonnees[i][0] >= 1 && coordonnees[i][0] <= LIMITE_X && coordonnees[i][1] >= 1 && coordonnees[i][1] <= LIMITE_Y) {
            afficher(coordonnees[i][0], coordonnees[i][1], CORPS);
        }
    }
}

// Fonction pour faire progresser le serpent
void progresser(int positions[TAILLE_SERPENT][2], char direction) {
    // Décaler chaque segment vers l'arrière
    for (int i = TAILLE_SERPENT - 1; i > 0; i--) {
        positions[i][1] = positions[i - 1][1];
        positions[i][0] = positions[i - 1][0];
    }

    // Avancer la tête du serpent dans la direction spécifiée
    switch (direction) {
        case 'q': // gauche
            positions[0][0]--;
            break;
        case 'd': // droite
            positions[0][0]++;
            break;
        case 'z': // haut
            positions[0][1]--;
            break;
        case 's': // bas
            positions[0][1]++;
            break;
    }
}

// Fonction pour désactiver l'écho des touches
void disableEcho() {
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Désactiver le flag ECHO
    tty.c_lflag &= ~ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

// Fonction pour réactiver l'écho des touches
void enableEcho() {
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Réactiver le flag ECHO
    tty.c_lflag |= ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}
