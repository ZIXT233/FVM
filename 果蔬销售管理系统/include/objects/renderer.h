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
Coord coordAdd(Coord, Coord);
#define ESC "\033"
#define CSI "\033["
#define BACKGROUND_DEFAULT "48;2;253;246;227m"
#define TEXT_COLOR_DEFAULT "38;2;101;123;131m"
Renderer* rendererCreate(int bufferSize);
void rendererDelete(Renderer* renderer);
int renderVPrintf(Renderer* renderer, const char* _Format, va_list aptr);
int renderPrintf(Renderer* renderer, const char* _Format, ...);
void setCursorPos(Renderer* renderer, Coord pos);
void setBackGroundColor(Renderer* renderer, int r, int g, int b);
void setTextColor(Renderer* renderer, int r, int g, int b);
void resetBackgroundColor(Renderer* renderer);
void resetTextColor(Renderer* renderer);
Coord _getCursorPos();
int coordPrintf(Renderer* renderer, Coord pos, const char* const _Format, ...);
void _cls();
void renderClear(Renderer* renderer);
void renderPresent(Renderer* renderer);
#endif