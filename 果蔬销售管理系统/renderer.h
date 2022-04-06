#ifndef RENDERER_H
#define RENDERER_H
typedef struct tagRenderer {
	char* buffer, * offset;
	int bufferSize;
}Renderer;

typedef struct tagCoord {
	int x;
	int y;
}Coord;

Renderer* rendererCreate(int bufferSize);
void rendererDelete(Renderer* renderer);
int renderVPrintf(Renderer* renderer, const char* _Format, va_list aptr);
int renderPrintf(Renderer* renderer, const char* _Format, ...);
void setCursorPos(Renderer* renderer, Coord pos);
Coord _getCursorPos();
int coordPrintf(Renderer* renderer, Coord pos, const char* const _Format, ...);
void _cls();
void renderClear(Renderer* renderer);
void renderPresent(Renderer* renderer);
#endif