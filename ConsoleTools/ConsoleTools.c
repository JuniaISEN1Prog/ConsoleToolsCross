#ifdef _WIN32
#include <conio.h>
#include "ConsoleTools.h"
#else
#include "ConsoleToolsLinux.h"
#endif

/***********************************/
//	file ConsoleTools.c
#define VERSION "8"
#define RELEASE "0"
#define PLATFORM "W"
#define DATE "16-09-26"
/***********************************/
// ref : https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences

SYSTEMTIME elapsedTime(bool reset) {
		
	SYSTEMTIME duration;
	FILETIME fcurrentTime, fduration;
	static ULARGE_INTEGER fprevTime64 = { 0,0 };
	ULARGE_INTEGER fcurrTime64, fduration64;
	if (reset) {
		fprevTime64.QuadPart = 0;

	}

	GetSystemTimeAsFileTime(&fcurrentTime);
	fcurrTime64.HighPart = fcurrentTime.dwHighDateTime;
	fcurrTime64.LowPart = fcurrentTime.dwLowDateTime;
	fduration64.QuadPart = fcurrTime64.QuadPart - fprevTime64.QuadPart;
	fduration.dwHighDateTime = fduration64.HighPart;
	fduration.dwLowDateTime = fduration64.LowPart;
	FileTimeToSystemTime(&fduration, &duration);
	fprevTime64 = fcurrTime64;
	return(duration);
}


char readChar(const char* filtre) {

	char car = -1;
	const char* tmp;
	while (car == -1) {
		car = 0;
		while (car == 0) car = (char)_getch();

		if ((tmp = strchr(filtre, car)) != NULL) {
			car = *tmp;
		}
		else car = -1;
	};
	return car;
}


int openConsole() {

	// Seed the random-number generator with the current time so that
	// the numbers will be different every time we run.
	srand((unsigned)time(NULL));


	// Get handles to STDIN and STDOUT. 
	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	if (hStdin == INVALID_HANDLE_VALUE ||
		hStdout == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, TEXT("GetStdHandle"), TEXT("Console Error"),
			MB_OK);
		return 1;
	}
	// Save the current text colors. 

	if (!GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
	{
		MessageBox(NULL, TEXT("GetConsoleScreenBufferInfo"),
			TEXT("Console Error"), MB_OK);
		return 1;
	}

	wOldColorAttrs = csbiInfo.wAttributes;

	// Save the current consoleMode

	if (!GetConsoleMode(hStdout, &Mode))
	{
		MessageBox(NULL, TEXT("GetConsoleMode"),
			TEXT("Console Error"), MB_OK);
		return 1;
	}
	wOldMode = Mode;
	// Set output mode to handle virtual terminal sequences
	Mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if (!SetConsoleMode(hStdout, Mode))
	{
		return 1;
	}
	
	// Display Console Tools Version

	char buffer[128];
	sprintf_s(buffer, _countof(buffer), "ConsoleTools Version %s.%s - %s\n", VERSION, RELEASE, DATE);
	moveCursor(0, 0);
	fputs(buffer, stderr);
	Sleep(2000);
	return(0);
}

int blink(COORD pos, DWORD length, DWORD heigth, DWORD duration) {
	WORD* oldAttrib;
	DWORD nbAttrib;
	WORD newAttrib;
	COORD pix = pos;
	// sauvegarde des attributs de chaque pixel
	oldAttrib = (WORD*)malloc(sizeof(WORD) * length * heigth);
	if (oldAttrib == NULL) return(1);
	for (unsigned int l = 0; l < heigth; l++)
	{
		for (unsigned int c = 0; c < length; c++)
		{
			int offset = (l * length + c);
			pix.X = pos.X + c; pix.Y = pos.Y + l;
			if (!ReadConsoleOutputAttribute(hStdout, oldAttrib + offset, 1, pix, &nbAttrib))
			{
				MessageBox(NULL, TEXT("ReadConsoleOutputAttribute"),
					TEXT("Console Error"), MB_OK);
				return 1;
			}
			else {
				newAttrib = *(oldAttrib + offset);
				newAttrib = newAttrib | COMMON_LVB_REVERSE_VIDEO;
				if (!WriteConsoleOutputAttribute(hStdout, &newAttrib, 1, pix, &nbAttrib))
				{
					MessageBox(NULL, TEXT("WriteConsoleOutputAttribute"),
						TEXT("Console Error"), MB_OK);
					return 1;
				}
			}
		}
	}

	Sleep(duration);
	pix = pos;
	for (unsigned int l = 0; l < heigth; l++)
	{
		for (unsigned int c = 0; c < length; c++)
		{
			int offset = (l * length + c);
			pix.X = pos.X + c; pix.Y = pos.Y + l;
			if (!WriteConsoleOutputAttribute(hStdout, oldAttrib + offset, 1, pix, &nbAttrib))
			{
				MessageBox(NULL, TEXT("WriteConsoleOutputAttribute"),
					TEXT("Console Error"), MB_OK);
				return 1;
			}

		}
	}
	free(oldAttrib);
	return 0;
}

void hideCursor(void) {
	CONSOLE_CURSOR_INFO lpConCurInfo;
	lpConCurInfo.bVisible = false;
	lpConCurInfo.dwSize = 1;
	SetConsoleCursorInfo(hStdout, &lpConCurInfo);
}
void showCursor(void) {
	CONSOLE_CURSOR_INFO lpConCurInfo;
	lpConCurInfo.bVisible = true;
	lpConCurInfo.dwSize = 1;
	SetConsoleCursorInfo(hStdout, &lpConCurInfo);
}
int setWriteColor(WORD col) {

	WORD colAttrib = 0;
	WORD wCurColorAttrs;

	switch (col)
	{
	case	0: // BLACK
		colAttrib = 0;
		break;
	case	1: // BLUE
		colAttrib = FOREGROUND_BLUE;
		break;
	case	2:	// GREEN
		colAttrib = FOREGROUND_GREEN;
		break;
	case	3: // CYAN
		colAttrib = FOREGROUND_BLUE | FOREGROUND_GREEN;
		break;
	case	4: // RED
		colAttrib = FOREGROUND_RED;
		break;
	case	5: // MAGENTA
		colAttrib = FOREGROUND_BLUE | FOREGROUND_RED;
		break;
	case	6: // YELLOW
		colAttrib = FOREGROUND_GREEN | FOREGROUND_RED;
		break;
	case	7: // GREY
		colAttrib = FOREGROUND_INTENSITY;
		break;
	case	8: // DARK GREY
		colAttrib = 0;
		break;
	case	9: // BRIGHT BLUE
		colAttrib = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
		break;
	case	10:	// BRIGHT GREEN
		colAttrib = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		break;
	case	11: // BRIGHT CYAN
		colAttrib = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		break;
	case	12: // BRIGHT RED
		colAttrib = FOREGROUND_RED | FOREGROUND_INTENSITY;
		break;
	case	13: // BRIGHT MAGENTA
		colAttrib = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY;
		break;
	case	14: // BRIGHT YELLOW
		colAttrib = FOREGROUND_YELLOW;
		break;
	case	15: // WHITE
		colAttrib = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
		break;
	default:
		colAttrib = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
		break;
	}


	if (GetConsoleScreenBufferInfo(hStdout, &csbiInfo)) {
		wCurColorAttrs = csbiInfo.wAttributes;

		wCurColorAttrs = wCurColorAttrs & 0xFFF0;
		wCurColorAttrs = wCurColorAttrs | colAttrib;

		if (!SetConsoleTextAttribute(hStdout, wCurColorAttrs))
		{
			MessageBox(NULL, TEXT("SetConsoleTextAttribute"),
				TEXT("Console Error"), MB_OK);
			return 1;
		}
	}



	return 0;
}
int setBackGroundColor(WORD col) {

	WORD colAttrib = 0;
	WORD wCurColorAttrs;

	switch (col)
	{
	case	0: // BLACK
		colAttrib = 0;
		break;
	case	1: // BLUE
		colAttrib = BACKGROUND_BLUE;
		break;
	case	2:	// GREEN
		colAttrib = BACKGROUND_GREEN;
		break;
	case	3: // CYAN
		colAttrib = BACKGROUND_BLUE | BACKGROUND_GREEN;
		break;
	case	4: // RED
		colAttrib = BACKGROUND_RED;
		break;
	case	5: // MAGENTA
		colAttrib = BACKGROUND_BLUE | BACKGROUND_RED;
		break;
	case	6: // YELLOW
		colAttrib = BACKGROUND_GREEN | BACKGROUND_RED;
		break;
	case	7: // GREY
		colAttrib = BACKGROUND_INTENSITY;
		break;
	case	8: // DARK GREY
		colAttrib = 0;
		break;
	case	9: // BRIGHT BLUE
		colAttrib = BACKGROUND_BLUE | BACKGROUND_INTENSITY;
		break;
	case	10:	// BRIGHT GREEN
		colAttrib = BACKGROUND_GREEN | BACKGROUND_INTENSITY;
		break;
	case	11: // BRIGHT CYAN
		colAttrib = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
		break;
	case	12: // BRIGHT RED
		colAttrib = BACKGROUND_RED | BACKGROUND_INTENSITY;
		break;
	case	13: // BRIGHT MAGENTA
		colAttrib = BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_INTENSITY;
		break;
	case	14: // BRIGHT YELLOW
		colAttrib = BACKGROUND_YELLOW;
		break;
	case	15: // WHITE
		colAttrib = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
		break;
	default:
		colAttrib = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
		break;
	}


	if (GetConsoleScreenBufferInfo(hStdout, &csbiInfo)) {
		wCurColorAttrs = csbiInfo.wAttributes;

		wCurColorAttrs = wCurColorAttrs & 0xFF0F;
		wCurColorAttrs = wCurColorAttrs | colAttrib;

		if (!SetConsoleTextAttribute(hStdout, wCurColorAttrs))
		{
			MessageBox(NULL, TEXT("SetConsoleTextAttribute"),
				TEXT("Console Error"), MB_OK);
			return 1;
		}
	}



	return 0;
}
int closeConsole() {
	// Restore the original text colors. 

	if (!SetConsoleTextAttribute(hStdout, wOldColorAttrs)) {
		return 1;
	}

	// Restore the original console mode. 
	if (!SetConsoleMode(hStdout, wOldMode))
	{
		return 1;
	}



	return(0);
}


COORD getConsoleSize() {
	COORD Size;

	//Size.X = csbiInfo.dwSize.X;
	//Size.Y = csbiInfo.dwSize.Y;
	Size.X = csbiInfo.srWindow.Right - csbiInfo.srWindow.Left + 1;
	Size.Y = csbiInfo.srWindow.Bottom - csbiInfo.srWindow.Top + 1;
	return(Size);
}
void printStatusLine(const char* const pszMessage, COORD const Size)
{
	printf(CSI "%d;1H", Size.Y);
	printf(CSI "K"); // clear the line
	printf(pszMessage);
}

void moveCursor(unsigned short int X, unsigned short int Y) {

	COORD pos;
	pos.X = X;
	pos.Y = Y;
	// SetConsoleCursorPosition(hStdout, pos);
	printf(CSI "%d;%dH", Y+ 1, X+1);
	return;
}
int plotChar(char SomeChar) {
	chBuffer[0] = SomeChar;
	chBuffer[1] = 0;
	cRead = 1;
	if (WriteFile(hStdout, chBuffer, cRead, &cWritten, NULL)) {
		return(0);
	}
	else {
		return(GetLastError());
	}
}


int rangedRand(int range_min, int range_max)
{
	int u = (int)((double)rand() / ((double)RAND_MAX + 1) * ((double)range_max - (double)range_min)) + range_min;
	return(u);
}

/// <summary>
/// Generate random numbers in the half-closed interval
/// [range_min, range_max).
/// </summary>
/// <param name="range_min"></param>
/// <param name="range_max"></param>
/// <returns></returns>
float floatRangedRand(float range_min, float range_max)
{
	float u = (float)((double)rand() / ((double)RAND_MAX + 1) * ((double)range_max - (double)range_min)) + range_min;
	return(u);
}

int clearScreen( void ) {
// https://docs.microsoft.com/fr-fr/windows/console/clearing-the-screen

		// Hold original mode to restore on exit to be cooperative with other command-line apps.

		Mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

		// Try to set the mode.
		if (!SetConsoleMode(hStdout, Mode))
		{
			return GetLastError();
		}

		// Write the sequence for clearing the display.
		DWORD written = 0;
		PCWSTR sequence = L"\x1b[2J";
		if (!WriteConsoleW(hStdout, sequence, (DWORD)wcslen(sequence), &written, NULL))
		{
			// If we fail, try to restore the mode on the way out.
			SetConsoleMode(hStdout, wOldMode);
			return GetLastError();
		}

		// To also clear the scroll back, emit L"\x1b[3J" as well.
		// 2J only clears the visible window and 3J only clears the scroll back.
		written = 0;
		sequence = L"\x1b[3J";
		if (!WriteConsoleW(hStdout, sequence, (DWORD)wcslen(sequence), &written, NULL))
		{
			// If we fail, try to restore the mode on the way out.
			SetConsoleMode(hStdout, wOldMode);
			return GetLastError();
		}

		// Restore the mode on the way out to be nice to other command-line applications.
		SetConsoleMode(hStdout, wOldMode);

		return 0;
}



int maxValue(int* array, unsigned int eltsCounts) {
	int max = *array;
	for (size_t i = 0; i < eltsCounts; i++)
	{
		if (array[i] > max) {
			max = array[i];
		}
	}
	return max;
}


int drawArray(int *array, int EltsCount, COORD p1, COORD p2, bool prop, bool reverse, bool paint, int color) {

	int winSizeX;
	int winSizeY;

	// Calculate drawing area size
	winSizeX = p2.X - p1.X + 1;
	winSizeY = p2.Y - p1.Y + 1;

	
	// Quit if too small
	if (winSizeX < 3 || winSizeY < EltsCount + 1) return(-1);
	
	// Adjust size if size X not odd for symetric drawing purpose
	if (winSizeX % 2 == 0) {
		p2.X--;
		winSizeX--;
	}

	// paint option selected : paint background in selected color before drawing array
	if (paint) {
		setBackGroundColor(color);
		for (int line = p1.Y; line <= p2.Y; line++) {
			for (int col = p1.X; col <= p2.X; col++) {
				moveCursor(col, line);
				plotChar(' ');
			}
		}

	}

	int currentColor = 0;
	// set color for stack base
	setWriteColor(RED);
	// draw base
	for (unsigned short col = p1.X; col <= p2.X; col++) {
		moveCursor(col, p2.Y);
		plotChar('\xDB');
	}
	for (unsigned short line = p1.Y; line <= p2.Y; line++) {
		unsigned short col = (p1.X + p2.X) / 2;

		moveCursor( col, line);
		plotChar('\xDB');
	}
	// calculate scale factor to fit drawing area
	float scaleFactor= 1.0;
	int maxVal = maxValue(array, EltsCount);
	scaleFactor = (float) winSizeX / (float) maxVal;
	int midPos = (p1.X + p2.X) / 2;

	// control browsing direction of values in array

	int inc = (reverse) ? -1 : 1;
	
	for (int idx = reverse ? EltsCount - 1 : 0 ; (!reverse && idx < EltsCount)  || (reverse && idx >= 0 ); idx+=inc)
	{

		int val = array[idx];
		// select color index between 7 and 15 for bright colors
		currentColor = val % 7 + 9;
		setWriteColor(currentColor);
		int diskSize; 

		if (prop) {
			diskSize = (int) roundf((val * scaleFactor));
			if (diskSize % 2 == 0 ) {
				diskSize--;
			}
			
			diskSize = diskSize < 3 ? 3 : diskSize;
			diskSize = diskSize > winSizeX ? winSizeX : diskSize;
		}
		else 
		{
			diskSize = winSizeX;
			
		}

		for (int col = midPos - (diskSize / 2); col <= midPos + (diskSize / 2); col++) {
			if (col != midPos) {
				moveCursor(col, p2.Y - idx - 1);
				plotChar('\xDB');
			}
			if (!prop) {
				moveCursor(p1.X + 1, p2.Y - idx - 1);
				setWriteColor(BLACK);
				setBackGroundColor(currentColor);
				printf_s("%3d", val);
				setWriteColor(currentColor);
				setBackGroundColor(BLACK);
			}
		}
	}

	setWriteColor(WHITE);
	moveCursor(0, p2.Y + 1);
	return(0);
 }

void printVerticalBorder()
{
	printf(ESC "(0"); // Enter Line drawing mode
	printf(CSI "104;93m"); // bright yellow on bright blue
	printf("x"); // in line drawing mode, \x78 -> \u2502 "Vertical Bar"
	printf(CSI "0m"); // restore color
	printf(ESC "(B"); // exit line drawing mode
}

void printHorizontalBorder(int linePos, int tabStopCount, int tabStopPostions[], bool displayColumn[], bool fIsTop)
{
	COORD size = getConsoleSize();
	int startColumn = 0;
	int endColumn = size.X -1;
	int tabIdx = 0;
	while (tabIdx < tabStopCount - 1 && !displayColumn[tabIdx]) tabIdx++;
	if (!displayColumn[tabIdx]) tabIdx = 0;
	startColumn = tabStopPostions[tabIdx];

	tabIdx = tabStopCount - 1;
	while (tabIdx > 0 && !displayColumn[tabIdx]) tabIdx--;
	if (!displayColumn[tabIdx]) tabIdx = tabStopCount-1;
	endColumn = tabStopPostions[tabIdx];
	if (startColumn >= endColumn) return;

	printf(CSI "%d;%dH", linePos, startColumn);

	printf(ESC "(0"); // Enter Line drawing mode
	printf(CSI "104;93m"); // Make the border bright yellow on bright blue
	printf(fIsTop ? "l" : "m"); // print left corner 

	int dashCount = endColumn - startColumn - 1;
	for (int i = 0; i < dashCount; i++) {
		printf("q"); // in line drawing mode, \x71 -> \u2500 "HORIZONTAL SCAN LINE-5"
	}
	printf(fIsTop ? "k" : "j"); // print right corner
	printf(CSI "0m");
	printf(ESC "(B"); // exit line drawing mode
}
void enterAlternateBuffer() {
	// Enter the alternate buffer
	printf(CSI "?1049h");
	return;
}
void exitAlternateBuffer() {
	// Exit the alternate buffer
	printf(CSI "?1049l");
}
void setScrollingMargins(int top, int bottom) {
	COORD size;
	size = getConsoleSize();
	printf(CSI "%d;%dr", top+1, size.Y - bottom );
}
void clearAllTabStop() {
	printf(CSI "3g"); // clear all tab stops
}
void defineTabStop(int tabStopCount, int tabStopPostions[]) {
	clearAllTabStop();
	for (int i = 0; i < tabStopCount; i++)
	{
		printf(CSI "1;%dH", tabStopPostions[i]); // Move to column index i+1
		printf(ESC "H"); // set a tab stop
	}
}
void drawColumnedFrame(int tabStopCount, int tabStopPostions[], bool displayColumn[], int topMargin, int bottomMargin) {
	COORD size;
	size = getConsoleSize();
	int linesCount = size.Y - topMargin - bottomMargin - 2;
	int totalTabsCount = linesCount * tabStopCount;
	int printedTabCount = 0;
	
	if (size.Y < (topMargin + bottomMargin + 3)) return;

	setScrollingMargins(topMargin+1, bottomMargin+1);
	defineTabStop(tabStopCount, tabStopPostions);
	printf(CSI "104;93m"); // bright yellow on bright blue
	//printf(CSI "0J"); // clean viewPort
	printf(CSI "%d;1H", topMargin+2); // go to column 1, skip horizontal border
	for (int line = 0; line < linesCount ; line++)
	{
		for (int col = 0; col < tabStopCount; col++)
		{
			if (displayColumn[col]) {
				printVerticalBorder();
				if (printedTabCount < totalTabsCount - 1) // don't advance to next line if this is the last line
					printf("\t"); // advance to next tab stop
			}
			printedTabCount += 1;
		}		
	}
	// Print a top border - Yellow
		
	printHorizontalBorder(topMargin+1, tabStopCount, tabStopPostions, displayColumn, true );

	printHorizontalBorder(size.Y-bottomMargin, tabStopCount, tabStopPostions, displayColumn, false);
	printf(CSI "0m"); // restore color
	return;
}

void redimensionner_console(int largeur, int hauteur) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SMALL_RECT rect;
	COORD coord;

	// Définir les dimensions de la fenêtre
	rect.Left = 0;
	rect.Top = 0;
	rect.Right = largeur - 1;
	rect.Bottom = hauteur - 1;

	// Définir les dimensions du tampon
	coord.X = largeur;
	coord.Y = hauteur;

	// Appliquer les dimensions du tampon
	SetConsoleScreenBufferSize(hConsole, coord);

	// Appliquer les dimensions de la fenêtre
	SetConsoleWindowInfo(hConsole, TRUE, &rect);
}
