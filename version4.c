#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdbool.h>
#include <time.h>

// Constantes globales
#define TAILLE_SERPENT 10
#define LIMITE_X 80
#define LIMITE_Y 40
#define TEMPORISATION_BASE 100000 
#define STOP 'a'
#define TETE 'O'
#define CORPS 'X'
#define BORDURE '#'
#define PAVE 4
#define TAILLE_PAVE 5
#define POMME '6'
#define ESPACE ' '
#define NOMBRE_PAVES 4
#define DEPART_X 40
#define DEPART_Y 20
#define GAUCHE 'q'
#define DROITE 'd'
#define HAUT 'z'
#define BAS 's'
#define POMMES_VICTOIRE 10

// Plateau de jeu
char plateau[LIMITE_Y][LIMITE_X];

// Déclarations de fonctions
int kbhit();
void gotoXY(int x, int y);
void afficher(int x, int y, char c);
void effacer(int x, int y);
void dessinerSerpent(int lesX[], int lesY[], int taille);
void progresser(int lesX[], int lesY[], int *taille, char direction, bool *collision, bool *pommeMangee);
void disableEcho();
void enableEcho();
void initPlateau();
void dessinerPlateau();
void ajouterPomme();
void ajouterPaves();

int main() {
    int lesX[TAILLE_SERPENT * POMMES_VICTOIRE], lesY[TAILLE_SERPENT * POMMES_VICTOIRE];
    int tailleActuelle = TAILLE_SERPENT;
    char trajectoire = DROITE;
    char touche = ' ';
    bool collision = false, mangee = false;
    int pommesMangees = 0;
    int temporisation = TEMPORISATION_BASE;

    srand(time(NULL));

    initPlateau();
    ajouterPaves();
    ajouterPomme();

    for (int i = 0; i < TAILLE_SERPENT; i++) {
        lesX[i] = DEPART_X - i;
        lesY[i] = DEPART_Y;
    }

    system("clear");
    disableEcho();

    while (touche != STOP && !collision && pommesMangees < POMMES_VICTOIRE) {
        if (kbhit()) {
            touche = getchar();
            if ((touche == DROITE && trajectoire != GAUCHE) || 
                (touche == GAUCHE && trajectoire != DROITE) || 
                (touche == HAUT && trajectoire != BAS) || 
                (touche == BAS && trajectoire != HAUT)) {
                trajectoire = touche;
            }
        }

        progresser(lesX, lesY, &tailleActuelle, trajectoire, &collision, &mangee);

        if (mangee) {
            pommesMangees++;
            ajouterPomme();
            tailleActuelle++;  // Augmente la taille du serpent
            temporisation -= 2000;    // Accélère le jeu
        }

        dessinerPlateau();
        dessinerSerpent(lesX, lesY, tailleActuelle);

        usleep(temporisation);
    }

    enableEcho();
    system("clear");
    if (pommesMangees == POMMES_VICTOIRE) {
        printf("\nFélicitations ! Vous avez gagné !\n");
    } else {
        printf("\nGame Over!\n");
    }
    return 0;
}

// Initialisation du plateau
void initPlateau() {
    for (int y = 0; y < LIMITE_Y; y++) {
        for (int x = 0; x < LIMITE_X; x++) {
            if (x == 0 || x == LIMITE_X - 1 || y == 0 || y == LIMITE_Y - 1) {
                if ((x == LIMITE_X / 2 && (y == 0 || y == LIMITE_Y - 1)) || 
                    (y == LIMITE_Y / 2 && (x == 0 || x == LIMITE_X - 1))) {
                    plateau[y][x] = ESPACE;  // Trou (portail)
                } else {
                    plateau[y][x] = BORDURE;
                }
            } else {
                plateau[y][x] = ESPACE;
            }
        }
    }
}

// Ajoute des pavés sur le plateau
void ajouterPaves() {
    int pavesPlaces = 0;
    while (pavesPlaces < NOMBRE_PAVES) {
        int px = rand() % (LIMITE_X - TAILLE_PAVE - 2) + 1;
        int py = rand() % (LIMITE_Y - TAILLE_PAVE - 2) + 1;
        bool peutPlacer = true;

        for (int dy = 0; dy < TAILLE_PAVE && peutPlacer; dy++) {
            for (int dx = 0; dx < TAILLE_PAVE; dx++) {
                if (plateau[py + dy][px + dx] != ESPACE) {
                    peutPlacer = false;
                    break;
                }
            }
        }

        if (peutPlacer) {
            for (int dy = 0; dy < TAILLE_PAVE; dy++) {
                for (int dx = 0; dx < TAILLE_PAVE; dx++) {
                    plateau[py + dy][px + dx] = BORDURE;
                }
            }
            pavesPlaces++;
        }
    }
}

// Ajoute une pomme sur le plateau 
void ajouterPomme() {
    int x = rand() % (LIMITE_X - 2) + 1;
    int y = rand() % (LIMITE_Y - 2) + 1;

    while (plateau[y][x] != ESPACE) {
        x = rand() % (LIMITE_X - 2) + 1;
        y = rand() % (LIMITE_Y - 2) + 1;
    }

    plateau[y][x] = POMME;
    afficher(x, y, POMME);
}


// Dessine le plateau
void dessinerPlateau() {
    for (int y = 0; y < LIMITE_Y; y++) {
        gotoXY(0, y);  // Positionne le curseur au début de chaque ligne
        for (int x = 0; x < LIMITE_X; x++) {
            printf("%c", plateau[y][x]);
        }
    }
}

// Dessine le serpent
void dessinerSerpent(int lesX[], int lesY[], int taille) {
    for (int i = 0; i < taille; i++) {
        if (i == 0) {
            afficher(lesX[i], lesY[i], TETE);
        } else {
            afficher(lesX[i], lesY[i], CORPS);
        }
    }
}

// Déplacement du serpent
void progresser(int lesX[], int lesY[], int *tailleActuelle, char trajectoire, bool *collision, bool *mangee) {
    int nouveauX = lesX[0];
    int nouveauY = lesY[0];

    // Calcul de la nouvelle position
    switch (trajectoire) {
        case GAUCHE: nouveauX--; break;
        case DROITE: nouveauX++; break;
        case HAUT: nouveauY--; break;
        case BAS: nouveauY++; break;
    }

    // Gérer la réapparition sur le côté opposé
    if (nouveauX < 0) nouveauX = LIMITE_X - 2;
    if (nouveauX >= LIMITE_X) nouveauX = 1;
    if (nouveauY < 0) nouveauY = LIMITE_Y - 2;
    if (nouveauY >= LIMITE_Y) nouveauY = 1;

    *mangee = false;

    // Vérification des collisions
    if (plateau[nouveauY][nouveauX] == BORDURE || plateau[nouveauY][nouveauX] == CORPS) {
        *collision = true;
        return;
    }

    // Si la tête du serpent mange une pomme
    if (plateau[nouveauY][nouveauX] == POMME) {
        *mangee = true;
        plateau[nouveauY][nouveauX] = ESPACE;
    }

    // Déplacement du corps du serpent
    for (int i = *tailleActuelle - 1; i > 0; i--) {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }

    // Mise à jour de la tête
    lesX[0] = nouveauX;
    lesY[0] = nouveauY;

    *collision = false;
}

// Vérifie si une touche a été pressée
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

// Déplace le curseur du terminal
void gotoXY(int x, int y) {
    printf("\033[%d;%dH", y + 1, x + 1);
}

// Affiche un caractère à la position spécifiée
void afficher(int x, int y, char c) {
    gotoXY(x, y);
    printf("%c", c);
    fflush(stdout);
}

// Efface le caractère à la position spécifiée
void effacer(int x, int y) {
    afficher(x, y, ESPACE);
}

// Désactive l'écho du terminal pour la saisie en temps réel
void disableEcho() {
    struct termios tty;

    if (tcgetattr(STDIN_FILENO, &tty) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    tty.c_lflag &= ~ECHO;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

// Réactive l'écho du terminal pour la saisie en temps réel
void enableEcho() {
    struct termios tty;

    if (tcgetattr(STDIN_FILENO, &tty) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    tty.c_lflag |= ECHO;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}
