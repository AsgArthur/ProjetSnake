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

// Declaration des constantes
#define TAILLE_SERPENT 10
#define LIMITE_X 40
#define LIMITE_Y 40

int kbhit()
{
    // la fonction retourne :
    // 1 si un caractere est present
    // 0 si pas de caractere present

    int unCaractere = 0;
    struct termios oldt, newt;
    int ch;
    int oldf;

    // mettre le terminal en mode non bloquant
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    // restaurer le mode du terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        unCaractere = 1;
    }
    return unCaractere;
}

void gotoXY(int x, int y)
{
    printf("\033[%d;%df", y, x);
}

void afficher(int x, int y, char c)
{
    gotoXY(x, y);
    printf("%c", c);
}

void effacer(int x, int y)
{
    afficher(x, y, ' ');
}

void dessinerSerpent(int coordonnees[TAILLE_SERPENT][2])
{
    // Dessiner la tête du serpent ('O') à la position (coordonnees[0][0], coordonnees[0][1])
    afficher(coordonnees[0][0], coordonnees[0][1], 'O');

    // Dessiner le corps du serpent ('X') pour les segments de 1 à TAILLE_SERPENT-1
    for (int i = 1; i < TAILLE_SERPENT; i++)
    {
        afficher(coordonnees[i][0], coordonnees[i][1], 'X');
    }
}

void progresser(int positions[TAILLE_SERPENT][2])
{
    // Effacer le dernier segment du serpent
    effacer(positions[TAILLE_SERPENT - 1][0], positions[TAILLE_SERPENT - 1][1]);

    // Décaler chaque segment vers l'arrière
    for (int i = TAILLE_SERPENT - 1; i > 0; i--)
    {
        positions[i][1] = positions[i - 1][1];
        positions[i][0] = positions[i - 1][0];
    }

    // Avancer la tête du serpent (vers la droite)
    positions[0][0] += 1;

    // Redessiner le serpent
    dessinerSerpent(positions);
}

int main()
{
    int x, y;
    int positions[TAILLE_SERPENT][2];

    // Demande la saisie des coordonnées initiales
    printf("Veuillez entrer les coordonnées initiales <x> <y> : ");
    scanf("%d %d", &x, &y);

    // Validation de la saisie
    while (x <= 0 || x > LIMITE_X || y <= 0 || y > LIMITE_Y)
    {
        printf("Les coordonnées doivent être entre 1 et %d pour x et 1 et %d pour y\n", LIMITE_X, LIMITE_Y);
        printf("Veuillez entrer les coordonnées initiales <x> <y> : ");
        scanf("%d %d", &x, &y);
    }

    // Effacer l'écran avant de commencer
    system("clear");

    // Initialiser les positions du serpent
    for (int i = 0; i < TAILLE_SERPENT; i++)
    {
        positions[i][0] = x - i;
        positions[i][1] = y;
    }

    // Affichage initial du serpent
    dessinerSerpent(positions);

    // Boucle principale
    while (1)
    {
        if (kbhit())
        {
            // Si une touche est appuyée, lire le caractère
            char ch = getchar();

            // Si la touche est 'a', quitter la boucle
            if (ch == 'a')
            {
                break;
            }
        }
        // Faire progresser le serpent vers la droite
        progresser(positions);

        // Pause pour ralentir le mouvement
        usleep(200000); //
    }
    printf("\n");
    return EXIT_SUCCESS;
}
