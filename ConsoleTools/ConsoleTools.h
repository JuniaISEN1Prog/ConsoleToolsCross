#pragma once
#include <windows.h>

// Standard Library C-Style
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <time.h>
#include <conio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

//**************************
// file : ConsoleTools.h
//**************************
//	Author : Pascal RICQ
// 
/***********************************/
#define VERSION "8"
#define RELEASE "0"
#define PLATFORM "W"
#define DATE "16-09-26"
/***********************************/
//	Note : pedagogical purpose

// usage : link with ConsoleTool.obj
//         include ConsoleTools.h
//         call OpenConsole() entering main()
//         call CloseConsole() before exiting main()
// compilation option: x64 nodebug

// See also
// https://docs.microsoft.com/en-us/windows/console/using-the-console
// https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#screen-colors
// https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#samples

HANDLE hStdout, hStdin;
CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
WORD wOldColorAttrs;

CHAR chBuffer[256];
DWORD cRead, cWritten, fdwMode, fdwOldMode;
DWORD Mode;
DWORD wOldMode;

#define BACKGROUND_YELLOW 0xE0
#define FOREGROUND_YELLOW 0x0E
#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define YELLOW 6
#define GREY 7
#define DARKGREY 8
#define BRIGHTBLUE 9
#define BRIGHTGREEN 10
#define BRIGHTCYAN 11
#define BRIGHTRED 12
#define BRIGHTMAGENTA 13
#define BRIGHTYELLOW 14
#define WHITE 15

// console-virtual-terminal-sequences headers
#define ESC "\x1b"
#define CSI "\x1b["


/// <summary>
/// Return Elapsed time since last call of this function
/// if param is true, return current system time
/// SYSTEMTIME tps = elaspedTime(false);
/// Sleep(500);
/// tps = elapsedTime(false);
/// </summary>
/// <param name="reset"> Value : reset elapsed time to 0</param>
/// <returns>return elapsed time as SYSTEMTIME type value</returns>
/// <example>
/// /// <code>
/// SYSTEMTIME tps = elapsedTime(false);
/// Sleep(500);
/// tps = elapsedTime(false);
/// printf("\nElapsed time %d:%d:%d,%03d hh:mm:ss,ms", tps.wHour, tps.wMinute, tps.wSecond, tps.wMilliseconds);
/// </code>
/// </example>
SYSTEMTIME elapsedTime(bool reset);

/// <summary>
/// Waits for keyboard input as a single char
/// </summary>
/// <param name="filtre">: Ref: string containing valid chars</param>
/// <returns>Value: read char</returns>
char readChar(const char* filtre);


// permet d'utiliser les fonctions moveCursor et PlotChar (un seul appel en début de programme)

/// <summary>
/// Initialize console environment before using ConsoleTools functions
/// </summary>
/// <returns></returns>
int openConsole();


/// <summary>
/// Produce reverse video display of a screen zone for specified duration in ms
/// </summary>
/// <param name="pos"></param>
/// <param name="length"></param>
/// <param name="heigth"></param>
/// <param name="duration"></param>
/// <returns></returns>
int blink(COORD pos, DWORD length, DWORD heigth, DWORD duration);
// CloseConsole() doit être appelée après la dernière utilisation de moveCursor et PlotChar

/// <summary>
/// Restore console state before leaving application.
/// </summary>
/// <returns></returns>
int closeConsole();

/// <summary>
/// Returns console size in columns and lines
/// </summary>
/// <returns>Value: struct COORD containing X,Y size for the current console</returns>
COORD getConsoleSize();

/// <summary>
/// helper for printing a message on dedicated message status console line 
/// </summary>
/// <param name="pszMessage"></param>
/// <param name="Size"></param>
void printStatusLine(const char* const pszMessage, COORD const Size);

/// <summary>
/// Defines the cursor position for the next display on current console
/// </summary>
/// <param name="X">: Value : Column index</param>
/// <param name="Y">: Value : Line index</param>
void moveCursor(unsigned short int X, unsigned short int Y);


/// <summary>
/// Display one char
/// </summary>
/// <param name="SomeChar">: Value : char code</param>
/// <returns>Error code or zero</returns>
int plotChar(char SomeChar);


/// <summary>
/// Generate random numbers in the half-closed interval
/// [range_min, range_max).
/// </summary>
/// <param name="range_min"></param>
/// <param name="range_max"></param>
/// <returns></returns>
int rangedRand(int range_min, int range_max);

/// <summary>
/// Generate random float numbers in the half-closed interval
/// [range_min, range_max).
/// </summary>
/// <param name="range_min"></param>
/// <param name="range_max"></param>
/// <returns></returns>
float floatRangedRand(float range_min, float range_max);


/// <summary>
/// Hide console cursor
/// </summary>
/// <param name=""></param>
void hideCursor(void);

/// Display console cursor
void showCursor(void);

/// <summary>
/// Set writing color (Windows API color code)
/// </summary>
/// <param name="col"></param>
/// <returns></returns>
int setWriteColor(WORD col);


/// <summary>
///  Set background color (Windows API color code)
/// </summary>
/// <param name="col"></param>
/// <returns></returns>
int setBackGroundColor(WORD col);

/// <summary>
/// Performs console cleaning
/// </summary>
/// <param name=""></param>
/// <returns>Error code or zero</returns>
int clearScreen(void);

/// <summary>
/// Gets maximum value in an array of interger values
/// </summary>
/// <param name="array"> : Ref: Array of int</param>
/// <param name="eltsCounts">: Value: Count of values to consider starting with index 0</param>
/// <returns></returns>
int maxValue(int* array, unsigned int eltsCounts);


/// <summary>
/// Produce graphical presentation of an integer values array 
/// </summary>
/// <param name="array"> : Ref: Array of int</param>
/// <param name="EltsCount"></param>
/// <param name="p1"></param>
/// <param name="p2"></param>
/// <param name="prop">: Value:true : disk size is proportional to the value</param>
/// <param name="reverse">: Value: true: Array is read from end to beginning</param>
/// <param name="reverse">: Value: true : Area is repainted before drawing</param>
/// <param name="reverse">: Value: Color index used to repaint area</param>
/// <returns></returns>
int drawArray(int* array, int EltsCount, COORD p1, COORD p2, bool prop, bool reverse, bool paint, int color);

void printVerticalBorder();

void printHorizontalBorder(int linePos, int tabStopCount, int tabStopPostions[], bool displayColumn[], bool fIsTop);

void enterAlternateBuffer();

void exitAlternateBuffer();

void setScrollingMargins(int top, int bottom);

void clearAllTabStop();

void defineTabStop(int tabStopCount, int tabStopPostions[]);

void drawColumnedFrame(int tabStopCount, int tabStopPostions[], bool displayColumn[], int topMargin, int bottomMargin);

void redimensionner_console(int largeur, int hauteur);




