#ifndef CONSOLE_TOOLS_LINUX_H
#define CONSOLE_TOOLS_LINUX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <errno.h>

// Définition de types compatibles avec Windows
typedef struct {
    short X;
    short Y;
} COORD;

typedef struct {
    struct timeval tv;
} SYSTEMTIME;

// Macros de compatibilité
#define CSI "\x1b["
#define ESC "\x1b"

// Codes de couleurs
#define BLACK           0
#define BLUE            1
#define GREEN           2
#define CYAN            3
#define RED             4
#define MAGENTA         5
#define YELLOW          6
#define GREY            7
#define DARK_GREY       8
#define BRIGHT_BLUE     9
#define BRIGHT_GREEN    10
#define BRIGHT_CYAN     11
#define BRIGHT_RED      12
#define BRIGHT_MAGENTA  13
#define BRIGHT_YELLOW   14
#define WHITE           15

#define BRIGHTBLUE 9
#define BRIGHTGREEN 10
#define BRIGHTCYAN 11
#define BRIGHTRED 12
#define BRIGHTMAGENTA 13
#define BRIGHTYELLOW 14

// Prototypes des fonctions
int openConsole(void);
int closeConsole(void);
void clearScreen(void);
void moveCursor(unsigned short int X, unsigned short int Y);
COORD getConsoleSize(void);
void redimensionner_console(int largeur, int hauteur);
void printStatusLine(const char* const pszMessage, COORD const Size);
char readChar(const char* filtre);
int _getch(void);
void hideCursor(void);
void showCursor(void);
int setWriteColor(int col);
int setBackGroundColor(int col);
void enterAlternateBuffer(void);
void exitAlternateBuffer(void);
void setScrollingMargins(int top, int bottom);
void clearAllTabStop(void);
void defineTabStop(int tabStopCount, int tabStopPostions[]);
void printVerticalBorder(void);
void printHorizontalBorder(int linePos, int tabStopCount, int tabStopPostions[], bool displayColumn[], bool fIsTop);
void drawColumnedFrame(int tabStopCount, int tabStopPostions[], bool displayColumn[], int topMargin, int bottomMargin);

// Fonctions de compatibilité string
#define _stricmp strcasecmp
#define _strnicmp strncasecmp
#define sprintf_s snprintf
#define printf_s printf
#define strcpy_s(dest, size, src) strncpy(dest, src, size)
#define strncpy_s(dest, destSize, src, count) strncpy(dest, src, (count == _TRUNCATE) ? destSize - 1 : count)
#define strcat_s(dest, size, src) strncat(dest, src, size - strlen(dest) - 1)
#define fopen_s(pFile, filename, mode) ((*pFile = fopen(filename, mode)) == NULL ? -1 : 0)
#define _countof(array) (sizeof(array) / sizeof(array[0]))
#define _TRUNCATE ((size_t)-1)

typedef int errno_t;
#define _set_errno(value) (errno = (value))

// Fonction pour convertir une chaîne en majuscules
void _strupr_s(char* str, size_t size);

// Fonction Sleep compatible
#define Sleep(ms) usleep((ms) * 1000)

#endif // CONSOLE_TOOLS_LINUX_H


