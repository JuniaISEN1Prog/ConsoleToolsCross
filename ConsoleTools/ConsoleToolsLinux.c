#include "ConsoleToolsLinux.h"
#include <math.h>
#include <wchar.h>

// Variables globales pour la gestion du terminal
static struct termios old_tio, new_tio;
static bool console_initialized = false;

SYSTEMTIME elapsedTime(bool reset) {
    static struct timeval previous = { 0, 0 };
    SYSTEMTIME duration;
    struct timeval current;

    gettimeofday(&current, NULL);
    if (reset || (previous.tv_sec == 0 && previous.tv_usec == 0)) {
        previous = current;
        duration.tv.tv_sec = 0;
        duration.tv.tv_usec = 0;
        return duration;
    }

    duration.tv.tv_sec = current.tv_sec - previous.tv_sec;
    duration.tv.tv_usec = current.tv_usec - previous.tv_usec;
    if (duration.tv.tv_usec < 0) {
        duration.tv.tv_sec--;
        duration.tv.tv_usec += 1000000;
    }
    previous = current;
    return duration;
}

int blink(COORD pos, DWORD length, DWORD height, DWORD duration) {
    if (length == 0 || height == 0) return 0;

    printf(CSI "%d;%d;%d;%d$r", pos.Y + 1, pos.X + 1,
           pos.Y + height, pos.X + length);
    fflush(stdout);
    Sleep(duration);
    printf(CSI "%d;%d;%d;%d$r", pos.Y + 1, pos.X + 1,
           pos.Y + height, pos.X + length);
    fflush(stdout);
    return 0;
}

// Convertir une chaîne en majuscules
void _strupr_s(char* str, size_t size) {
    if (str == NULL) return;
    for (size_t i = 0; i < size && str[i] != '\0'; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

// Initialiser la console
int openConsole(void) {
    if (console_initialized) return 0;

    // Sauvegarder les paramètres actuels du terminal
    tcgetattr(STDIN_FILENO, &old_tio);
    new_tio = old_tio;

    // Activer le support des séquences ANSI
    printf(CSI "?1049h"); // Alternate buffer (optionnel)

    console_initialized = true;
    return 0;
}

// Fermer la console
int closeConsole(void) {
    if (!console_initialized) return 0;

    // Restaurer les paramètres du terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);

    // Réinitialiser les couleurs
    printf(CSI "0m");

    console_initialized = false;
    return 0;
}

// Effacer l'écran
int clearScreen(void) {
    printf(CSI "2J"); // Effacer l'écran
    printf(CSI "3J"); // Effacer le scroll back
    printf(CSI "H");  // Curseur en haut à gauche
    fflush(stdout);
    return ferror(stdout) ? 1 : 0;
}

// Déplacer le curseur
void moveCursor(unsigned short int X, unsigned short int Y) {
    printf(CSI "%d;%dH", Y + 1, X + 1);
    fflush(stdout);
}

// Obtenir la taille de la console
COORD getConsoleSize(void) {
    struct winsize w;
    COORD size;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    size.X = w.ws_col;
    size.Y = w.ws_row;

    return size;
}

// Afficher une ligne de statut
void printStatusLine(const char* const pszMessage, COORD const Size) {
    printf(CSI "%d;1H", Size.Y);
    printf(CSI "K"); // Effacer la ligne
    printf("%s", pszMessage);
    fflush(stdout);
}

// Lire un caractère sans écho (équivalent de _getch())
int _getch(void) {
    int ch;
    struct termios oldt, newt;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
}

wint_t _getwch(void) {
    return (wint_t)_getch();
}

int plotChar(char SomeChar) {
    if (putchar(SomeChar) == EOF) return errno;
    fflush(stdout);
    return 0;
}

int rangedRand(int range_min, int range_max) {
    if (range_max <= range_min) return range_min;
    return (int)((double)rand() / ((double)RAND_MAX + 1.0) *
                 (range_max - range_min)) + range_min;
}

float floatRangedRand(float range_min, float range_max) {
    if (range_max <= range_min) return range_min;
    return (float)((double)rand() / ((double)RAND_MAX + 1.0) *
                   (range_max - range_min)) + range_min;
}

// Lire un caractère avec filtre
char readChar(const char* filtre) {
    char car = -1;
    const char* tmp;

    while (car == -1) {
        car = 0;
        while (car == 0) car = (char)_getch();

        if ((tmp = strchr(filtre, car)) != NULL) {
            car = *tmp;
        } else {
            car = -1;
        }
    }

    return car;
}

// Cacher le curseur
void hideCursor(void) {
    printf(CSI "?25l");
    fflush(stdout);
}

// Afficher le curseur
void showCursor(void) {
    printf(CSI "?25h");
    fflush(stdout);
}

// Définir la couleur de texte (ANSI)
int setWriteColor(int col) {
    int ansi_color;

    switch (col) {
        case BLACK:        ansi_color = 30; break;
        case RED:          ansi_color = 31; break;
        case GREEN:        ansi_color = 32; break;
        case YELLOW:       ansi_color = 33; break;
        case BLUE:         ansi_color = 34; break;
        case MAGENTA:      ansi_color = 35; break;
        case CYAN:         ansi_color = 36; break;
        case GREY:         ansi_color = 37; break;
        case BRIGHT_RED:   ansi_color = 91; break;
        case BRIGHT_GREEN: ansi_color = 92; break;
        case BRIGHT_YELLOW:ansi_color = 93; break;
        case BRIGHT_BLUE:  ansi_color = 94; break;
        case BRIGHT_MAGENTA:ansi_color = 95; break;
        case BRIGHT_CYAN:  ansi_color = 96; break;
        case WHITE:        ansi_color = 97; break;
        default:           ansi_color = 37; break;
    }

    printf(CSI "%dm", ansi_color);
    fflush(stdout);
    return 0;
}

// Définir la couleur de fond (ANSI)
int setBackGroundColor(int col) {
    int ansi_color;

    switch (col) {
        case BLACK:        ansi_color = 40; break;
        case RED:          ansi_color = 41; break;
        case GREEN:        ansi_color = 42; break;
        case YELLOW:       ansi_color = 43; break;
        case BLUE:         ansi_color = 44; break;
        case MAGENTA:      ansi_color = 45; break;
        case CYAN:         ansi_color = 46; break;
        case GREY:         ansi_color = 47; break;
        case BRIGHT_RED:   ansi_color = 101; break;
        case BRIGHT_GREEN: ansi_color = 102; break;
        case BRIGHT_YELLOW:ansi_color = 103; break;
        case BRIGHT_BLUE:  ansi_color = 104; break;
        case BRIGHT_MAGENTA:ansi_color = 105; break;
        case BRIGHT_CYAN:  ansi_color = 106; break;
        case WHITE:        ansi_color = 107; break;
        default:           ansi_color = 40; break;
    }

    printf(CSI "%dm", ansi_color);
    fflush(stdout);
    return 0;
}

int maxValue(int* array, unsigned int eltsCounts) {
    if (array == NULL || eltsCounts == 0) return 0;

    int max = array[0];
    for (unsigned int i = 1; i < eltsCounts; i++) {
        if (array[i] > max) max = array[i];
    }
    return max;
}

int drawArray(int* array, int EltsCount, COORD p1, COORD p2,
              bool prop, bool reverse, bool paint, int color) {
    int winSizeX = p2.X - p1.X + 1;
    int winSizeY = p2.Y - p1.Y + 1;

    if (array == NULL || EltsCount <= 0 || winSizeX < 3 ||
        winSizeY < EltsCount + 1) return -1;

    if (winSizeX % 2 == 0) {
        p2.X--;
        winSizeX--;
    }

    if (paint) {
        setBackGroundColor(color);
        for (int line = p1.Y; line <= p2.Y; line++) {
            for (int col = p1.X; col <= p2.X; col++) {
                moveCursor((unsigned short)col, (unsigned short)line);
                plotChar(' ');
            }
        }
    }

    setWriteColor(RED);
    for (int col = p1.X; col <= p2.X; col++) {
        moveCursor((unsigned short)col, (unsigned short)p2.Y);
        plotChar('#');
    }
    for (int line = p1.Y; line <= p2.Y; line++) {
        moveCursor((unsigned short)((p1.X + p2.X) / 2), (unsigned short)line);
        plotChar('#');
    }

    int max = maxValue(array, (unsigned int)EltsCount);
    float scaleFactor = max > 0 ? (float)winSizeX / (float)max : 0.0f;
    int midPos = (p1.X + p2.X) / 2;
    int increment = reverse ? -1 : 1;

    for (int idx = reverse ? EltsCount - 1 : 0;
         (!reverse && idx < EltsCount) || (reverse && idx >= 0);
         idx += increment) {
        int value = array[idx];
        int currentColor = value % 7 + 9;
        setWriteColor(currentColor);
        int diskSize = prop ? (int)roundf(value * scaleFactor) : winSizeX;
        if (prop) {
            if (diskSize % 2 == 0) diskSize--;
            if (diskSize < 3) diskSize = 3;
            if (diskSize > winSizeX) diskSize = winSizeX;
        }

        for (int col = midPos - diskSize / 2;
             col <= midPos + diskSize / 2; col++) {
            if (col != midPos) {
                moveCursor((unsigned short)col,
                           (unsigned short)(p2.Y - idx - 1));
                plotChar('#');
            }
            if (!prop) {
                moveCursor((unsigned short)(p1.X + 1),
                           (unsigned short)(p2.Y - idx - 1));
                setWriteColor(BLACK);
                setBackGroundColor(currentColor);
                printf("%3d", value);
                setWriteColor(currentColor);
                setBackGroundColor(BLACK);
            }
        }
    }

    setWriteColor(WHITE);
    moveCursor(0, (unsigned short)(p2.Y + 1));
    return 0;
}

// Entrer dans le buffer alternatif
void enterAlternateBuffer(void) {
    printf(CSI "?1049h");
    fflush(stdout);
}

// Sortir du buffer alternatif
void exitAlternateBuffer(void) {
    printf(CSI "?1049l");
    fflush(stdout);
}

void setScrollingMargins(int top, int bottom) {
    COORD size = getConsoleSize();
    int scrollTop = top + 1;
    int scrollBottom = size.Y - bottom;

    if (scrollTop < 1) scrollTop = 1;
    if (scrollBottom > size.Y) scrollBottom = size.Y;
    if (scrollTop >= scrollBottom) return;

    printf(CSI "%d;%dr", scrollTop, scrollBottom);
    fflush(stdout);
}

void clearAllTabStop(void) {
    printf(CSI "3g");
    fflush(stdout);
}

void defineTabStop(int tabStopCount, int tabStopPostions[]) {
    clearAllTabStop();

    for (int i = 0; i < tabStopCount; i++) {
        printf(CSI "1;%dH", tabStopPostions[i]);
        printf(ESC "H");
    }
    fflush(stdout);
}

// Afficher une bordure verticale
void printVerticalBorder(void) {
    printf(ESC "(0"); // Entrer en mode dessin de ligne
    printf(CSI "104;93m"); // Jaune vif sur bleu vif
    printf("x"); // Barre verticale
    printf(CSI "0m"); // Restaurer la couleur
    printf(ESC "(B"); // Sortir du mode dessin de ligne
    fflush(stdout);
}

// Afficher une bordure horizontale
void printHorizontalBorder(int linePos, int tabStopCount, int tabStopPostions[], bool displayColumn[], bool fIsTop) {
    COORD size = getConsoleSize();
    int startColumn = 0;
    int endColumn = size.X - 1;
    int tabIdx = 0;

    while (tabIdx < tabStopCount - 1 && !displayColumn[tabIdx]) tabIdx++;
    if (!displayColumn[tabIdx]) tabIdx = 0;
    startColumn = tabStopPostions[tabIdx];

    tabIdx = tabStopCount - 1;
    while (tabIdx > 0 && !displayColumn[tabIdx]) tabIdx--;
    if (!displayColumn[tabIdx]) tabIdx = tabStopCount - 1;
    endColumn = tabStopPostions[tabIdx];

    if (startColumn >= endColumn) return;

    printf(CSI "%d;%dH", linePos, startColumn);
    printf(ESC "(0"); // Entrer en mode dessin de ligne
    printf(CSI "104;93m"); // Jaune vif sur bleu vif
    printf(fIsTop ? "l" : "m"); // Coin gauche

    int dashCount = endColumn - startColumn - 1;
    for (int i = 0; i < dashCount; i++) {
        printf("q"); // Ligne horizontale
    }

    printf(fIsTop ? "k" : "j"); // Coin droit
    printf(CSI "0m");
    printf(ESC "(B"); // Sortir du mode dessin de ligne
    fflush(stdout);
}

// Dessiner un cadre avec colonnes
void drawColumnedFrame(int tabStopCount, int tabStopPostions[], bool displayColumn[], int topMargin, int bottomMargin) {
    COORD size = getConsoleSize();
    int linesCount = size.Y - topMargin - bottomMargin - 2;
    int totalTabsCount = linesCount * tabStopCount;
    int printedTabCount = 0;

    if (size.Y < (topMargin + bottomMargin + 3)) return;

    setScrollingMargins(topMargin + 1, bottomMargin + 1);
    defineTabStop(tabStopCount, tabStopPostions);

    printf(CSI "104;93m"); // Jaune vif sur bleu vif
    printf(CSI "%d;1H", topMargin + 2); // Aller à la colonne 1

    for (int line = 0; line < linesCount; line++) {
        for (int col = 0; col < tabStopCount; col++) {
            if (displayColumn[col]) {
                printVerticalBorder();
                if (printedTabCount < totalTabsCount - 1) {
                    printf("\t");
                }
            }
            printedTabCount += 1;
        }
    }

    // Bordures horizontales
    printHorizontalBorder(topMargin + 1, tabStopCount, tabStopPostions, displayColumn, true);
    printHorizontalBorder(size.Y - bottomMargin, tabStopCount, tabStopPostions, displayColumn, false);

    printf(CSI "0m"); // Restaurer la couleur
    fflush(stdout);
}

void redimensionner_console(int largeur, int hauteur) {
    struct winsize w;

    if (largeur <= 0 || hauteur <= 0) return;

    w.ws_col = (unsigned short)largeur;
    w.ws_row = (unsigned short)hauteur;
    w.ws_xpixel = 0;
    w.ws_ypixel = 0;

    ioctl(STDOUT_FILENO, TIOCSWINSZ, &w);
    printf(CSI "8;%d;%dt", hauteur, largeur);
    fflush(stdout);
}