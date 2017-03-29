#include <ncurses.h>
#include <string>
#include <cstdlib>

#include "Editor.h"

using namespace std;

/**
 * Initialize the curses display.
 */
void curses_init()
{

    //initscr();                      // Start ncurses mode
    noecho();                       // Don't echo keystrokes
    cbreak();                       // Disable line buffering
    keypad(stdscr, true);           // Enable special keys to be recorded
    start_color();
}

/**
 * Main program entry. Accepts a filename as an argument.
 */
int main(int argc, char* argv[])
{
  WINDOW* s = initscr();
  curses_init();
  string fn = "";
    int count = 0;
  Editor::opener = "";
  if(argc > 1)
    fn = argv[1];
  
    Editor ed(fn);
  do
  {
  //Keep going until we're done
  while(ed.getMode() != 'x')
    {
      try{
      
      ed.updateStatus();
      ed.printStatusLine();
      ed.printLineNumbers();
      ed.drawTopBar();
      ed.printBuff();
      
      refresh();
      
      int input = getch();        // Blocking until input
      ed.handleInput(input);
      }
      catch(exception e){
        ed.errWin(e); 
        continue;
      }

    }
  try{
    fn = Editor::opener;
    ed = Editor(fn);
  }
  catch(exception e){ed.errWin(e);}
  }
  while(count<20);
  refresh();                      // Refresh display
  endwin();                       // End ncurses mode
  
  return EXIT_SUCCESS;
}

