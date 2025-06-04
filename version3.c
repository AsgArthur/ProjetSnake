#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdbool.h>
#include <time.h>

// Constantes globales pour la configuration du jeu
#define TAILLE_SERPENT 10
#define LIMITE_X 80
#define LIMITE_Y 40
#define TEMPORISATION 100000
#define STOP 'a'
#define TETE 'O'
#define CORPS 'X'
#define BORDURE '#'
#define PAVE '#'
#define ESPACE ' '
#define NOMBRE_PAVES 4
#define TAILLE_PAVE 5
#define DEPART_X 40
#define DEPART_Y 20
#define GAUCHE 'q'
#define DROITE 'd'
#define HAUT 'z'
#define BAS 's'

// Déclaration du plateau de jeu
char plateau[LIMITE_Y][LIMITE_X];

// Déclaration des fonctions
int kbhit();
void gotoXY(int x, int y);
void afficher(int x, int y, char c);
void effacer(int x, int y);
void dessinerSerpent(int coordonnees[TAILLE_SERPENT][2]);
void progresser(int positions[TAILLE_SERPENT][2], char direction, bool *collision);
void disableEcho();
void enableEcho();
void initPlateau(int serpent[TAILLE_SERPENT][2]);
void dessinerPlateau();

int main() {
    // Positions du serpent
    int positions[TAILLE_SERPENT][2];
    char direction = DROITE; // Direction initiale vers la droite
    bool collision = false;

    // Positionnement initial du serpent au centre
    for (int i = 0; i < TAILLE_SERPENT; i++) {
        positions[i][0] = DEPART_X - i;
        positions[i][1] = DEPART_Y;
    }

    // Initialisation du plateau avec bordures et pavés
    initPlateau(positions);

    // Désactiver l'écho du terminal pour un contrôle en temps réel
    disableEcho();
    system("clear");
    dessinerPlateau();
    dessinerSerpent(positions);

    // Boucle principale du jeu
    while (!collision) {
        if (kbhit()) {
            // Si une touche est pressée, on lit le caractère
            char ch = getchar();
            if (ch == STOP) { // Si la touche est 'a', le jeu s'arrête
                break;
            }
            // Changement de direction avec vérification pour éviter un demi-tour
            if ((ch == GAUCHE && direction != DROITE) || 
                (ch == DROITE && direction != GAUCHE) || 
                (ch == HAUT && direction != BAS) || 
                (ch == BAS && direction != HAUT)) {
                direction = ch;
            }
        }

        // Efface la queue du serpent de l'affichage
        effacer(positions[TAILLE_SERPENT - 1][0], positions[TAILLE_SERPENT - 1][1]);

        // Mise à jour de la position du serpent et détection des collisions
        progresser(positions, direction, &collision);

        // Affichage du serpent avec ses nouvelles coordonnées
        dessinerSerpent(positions);

        // Temporisation pour ralentir le mouvement
        usleep(TEMPORISATION);
    }

    // Réactivation de l'écho du terminal
    enableEcho();
    system("clear");
    printf("\nTu as perdu!\n");
    return EXIT_SUCCESS;
}

// Initialisation du plateau de jeu avec bordures et pavés
void initPlateau(int serpent[TAILLE_SERPENT][2]) {
    srand(time(NULL));

    // Placement des bordures
    for (int y = 0; y < LIMITE_Y; y++) {
        for (int x = 0; x < LIMITE_X; x++) {
            if (y == 0 || y == LIMITE_Y - 1 || x == 0 || x == LIMITE_X - 1) {
                plateau[y][x] = BORDURE;  // Bordures en haut, bas, gauche, droite
            } else {
                plateau[y][x] = ESPACE;  // Espace vide à l'intérieur
            }
        }
    }

    // Génération des pavés en évitant les bordures et la position initiale du serpent
    for (int i = 0; i < NOMBRE_PAVES; i++) {
        bool positionValide = false;
        int px, py;

        // Continue à chercher jusqu'à ce qu'une position valide soit trouvée
        while (!positionValide) {
            // Génération des positions des pavés, gardant une distance d'au moins 2 cases des bordures
            px = rand() % (LIMITE_X - 4) + 2; // Évite les bordures (2 cases d'écart)
            py = rand() % (LIMITE_Y - 4) + 2; // Évite les bordures (2 cases d'écart)

            positionValide = true;

            // Vérifie que le pavé ne soit pas sur ou adjacent au serpent
            for (int j = 0; j < TAILLE_SERPENT; j++) {
                int serpentX = serpent[j][0];
                int serpentY = serpent[j][1];

                // Vérifie que le pavé n'est pas sur le serpent ou dans une case adjacente (distance de 2 cases)
                if (abs(px - serpentX) <= 1 && abs(py - serpentY) <= 1) {
                    positionValide = false;
                    break;
                }
            }
        }

        // Place le pavé s'il est à une position valide
        for (int dy = 0; dy < TAILLE_PAVE; dy++) {
            for (int dx = 0; dx < TAILLE_PAVE; dx++) {
                if ((py + dy < LIMITE_Y) && (px + dx < LIMITE_X)) {
                    plateau[py + dy][px + dx] = PAVE;
                }
            }
        }
    }
}

// Affiche le plateau de jeu sur le terminal
void dessinerPlateau() {
    system("clear");
    for (int y = 0; y < LIMITE_Y; y++) {
        for (int x = 0; x < LIMITE_X; x++) {
            printf("%c", plateau[y][x]);
        }
        printf("\n");
    }
}

// Déplacement du serpent et détection des collisions
void progresser(int positions[TAILLE_SERPENT][2], char direction, bool *collision) {
    // Décalage des segments du serpent
    for (int i = TAILLE_SERPENT - 1; i > 0; i--) {
        positions[i][0] = positions[i - 1][0];
        positions[i][1] = positions[i - 1][1];
    }

    // Mise à jour de la tête en fonction de la direction
    switch (direction) {
        case GAUCHE: positions[0][0]--; break;
        case DROITE: positions[0][0]++; break;
        case HAUT: positions[0][1]--; break;
        case BAS: positions[0][1]++; break;
    }

    // Détection de collision
    int teteX = positions[0][0];
    int teteY = positions[0][1];

    // Vérifie si la tête du serpent sort des limites
    if (teteX < 0 || teteX >= LIMITE_X || teteY < 0 || teteY >= LIMITE_Y) {
        *collision = true;
        return;
    }

    // Vérifie si la tête du serpent entre en collision avec un pavé
    if (plateau[teteY][teteX] == PAVE) {
        *collision = true;
        return;
    }

    // Vérifie si la tête du serpent entre en collision avec son propre corps
    for (int i = 1; i < TAILLE_SERPENT; i++) {
        if (teteX == positions[i][0] && teteY == positions[i][1]) {
            *collision = true;
            return;
        }
    }

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
    printf("\033[%d;%dH", y, x);
}

// Affiche un caractère à la position spécifiée
void afficher(int x, int y, char c) {
    gotoXY(x, y);
    printf("%c", c);
}

// Efface le caractère à la position spécifiée
void effacer(int x, int y) {
    afficher(x, y, ESPACE);
}

// Dessine le serpent sur le plateau
void dessinerSerpent(int coordonnees[TAILLE_SERPENT][2]) {
    if (coordonnees[0][0] >= 1 && coordonnees[0][0] <= LIMITE_X && coordonnees[0][1] >= 1 && coordonnees[0][1] <= LIMITE_Y) {
        afficher(coordonnees[0][0], coordonnees[0][1], TETE);
    }

    for (int i = 1; i < TAILLE_SERPENT; i++) {
        if (coordonnees[i][0] >= 1 && coordonnees[i][0] <= LIMITE_X && coordonnees[i][1] >= 1 && coordonnees[i][1] <= LIMITE_Y) {
            afficher(coordonnees[i][0], coordonnees[i][1], CORPS);
        }
    }
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
