#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>

/* wnoutrefresh(win): copies WINDOW pointed by win to logical screen
 * doupdate(): copies logical screen to physical screen
 *
 * wrefresh() = wnoutrefresh() + doupdate(), but do it manually,
 * invoke doupdate only once for drawing many windows!!!
 *
 * (w)getch() will call (w)refresh() on an active window,
 *
 * DON'T USE BOTH windows and stdscr, for simple programs use only stdscr, for
 * more complicated - only windows.
 * */

#define NUMROWS (LINES/2)
#define NUMCOLS (COLS/2)
#define CENTERY ((NUMROWS) -1)
#define CENTERX ((NUMCOLS) -1)

/* This is a bit hacky, we cannot declare it as **arr because it is statically
 * allocated, not malloced, but it's fine since we know dimensions
 * */
static void fillGrid(char *arr);
static void welcomeScreen();
static void drawGrid(int x, int y, char *grid);
static void drawFace(int *x, int *y);

int main(int argc, char** argv)
{
    char grid[NUMROWS][NUMCOLS];

    fillGrid( (char *)grid );    
    welcomeScreen();
    //move the grid to the center
    unsigned int r = CENTERY-NUMROWS/2, c=CENTERX-NUMCOLS/2;
    drawGrid(c,r, (char *)grid);
    move(r,c);
    drawFace(&c, &r);
    drawFace(&c, &r);
    getch();
    return 0;
}

void drawGrid(int x, int y, char* grid)
{
    for(int r=0; r<NUMROWS; r++)
        mvaddstr( y+r, x, ((char *)grid)+NUMCOLS*r);
    mvaddstr(LINES-1, CENTERX, "This is a grid");
    getch();
    clear();
}

void fillGrid(char *arr)
{
    for(int row=0; row<NUMROWS; row++)
    {
        for(int col=0; col<NUMCOLS-1; col++, arr++)
            *arr='.';
        *arr++='\0';
    }
}

void welcomeScreen()
{
    move(CENTERY, CENTERX);
    printw("Type any key:");
    move(CENTERY+1, CENTERX);
    for(int i=0; i<10; i++)
        addch('=');
    getch();
}

void drawFace(int *x, int *y)
{
    int origY = *y;
    addstr(" ^ ^ "); move(++(*y), *x);
    addstr(" o o "); move(++(*y), *x);
    addstr("  ^  "); move(++(*y), *x);
    addstr(" ^ ^ "); move(++(*y), *x);
    addstr("\\___/"); move(++(*y), *x);
    addstr("      "); move(++(*y), *x);
    *y = origY;
    (*x) += 5;
    move(*y, *x);
}

__attribute__((constructor)) static void screenInit()
{
    initscr();
    clear();
    noecho();       //don't print getch() character
    curs_set(0);    //hide the cursor
}

__attribute__((destructor)) static void screenFinish()
{
    endwin();
}

