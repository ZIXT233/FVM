#include<Windows.h>
#include<stdio.h>
#include<stdlib.h>
#include"renderer.h"

Renderer* rendererCreate(int bufferSize) {
	Renderer* renderer=(Renderer*) malloc(sizeof(Renderer));
	renderer->buffer = (char*)malloc(bufferSize);
	renderer->offset = renderer->buffer;
	renderer->bufferSize = bufferSize;
	return renderer;
}
void rendererDelete(Renderer* renderer) {
	free(renderer->buffer);
	free(renderer);
}


int renderVPrintf(Renderer* renderer,const char* _Format, va_list aptr) {
	int ret = vsprintf_s(renderer->offset, renderer->buffer + renderer->bufferSize - renderer->offset, _Format, aptr);
	renderer->offset += ret;
	return ret;
}
int renderPrintf(Renderer* renderer,const char* _Format, ...) {
	va_list aptr;
	int ret;
	va_start(aptr, _Format);
	ret = renderVPrintf(renderer,_Format, aptr);
	va_end(aptr);
	return ret;
}


void setCursorPos(Renderer* renderer, Coord pos) {
	renderPrintf(renderer,"\033[%d;%dH", pos.x, pos.y);
}
Coord _getCursorPos() {
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	GetConsoleScreenBufferInfo(hStdout, &csbiInfo);
	Coord pos;
	pos.x = csbiInfo.dwCursorPosition.Y + 1;
	pos.y = csbiInfo.dwCursorPosition.X + 1;
	return pos;
}
int coordPrintf(Renderer* renderer, Coord pos, const char* const _Format, ...) {
	va_list aptr;
	int ret;
	va_start(aptr, _Format);
	setCursorPos(renderer, pos);
	ret = renderVPrintf(renderer,_Format, aptr);
	va_end(aptr);
	return ret;
}
void _cls()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	SMALL_RECT scrollRect;
	COORD scrollTarget;
	CHAR_INFO fill;
	// Get the number of character cells in the current buffer.
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
		return;
	}
	// Scroll the rectangle of the entire buffer.
	scrollRect.Left = 0;
	scrollRect.Top = 0;
	scrollRect.Right = csbi.dwMaximumWindowSize.X;
	scrollRect.Bottom = csbi.dwMaximumWindowSize.Y;
	if (csbi.srWindow.Top != 0) {
		csbi.srWindow.Bottom -= csbi.srWindow.Top;
		csbi.srWindow.Top = 0;
		SetConsoleWindowInfo(hConsole, 1, &csbi.srWindow);
	}

	// Scroll it upwards off the top of the buffer with a magnitude of the entire height.
	scrollTarget.X = 0;
	scrollTarget.Y = (SHORT)(0 - csbi.dwSize.Y);

	// Fill with empty spaces with the buffer's default text attribute.
	fill.Char.UnicodeChar = TEXT(' ');
	fill.Attributes = csbi.wAttributes;
	// Do the scroll
	ScrollConsoleScreenBuffer(hConsole, &scrollRect, NULL, scrollTarget, &fill);

	// Move the cursor to the top left corner too.

}

void renderClear(Renderer* renderer) {
	renderer->offset = renderer->buffer;
}

void renderPresent(Renderer* renderer) {
	_cls();
	fwrite(renderer->buffer, sizeof(char), renderer->offset - renderer->buffer, stdout);
}

