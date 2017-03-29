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

    initscr();                      // Start ncurses mode
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
  curses_init();
  string fn = "";
  Editor::opener = "";
  if(argc > 1)
    fn = argv[1];
  do
  {
    Editor ed(fn);
  
  //Keep going until we're done
  while(ed.getMode() != 'x')
    {
      try{
      ed.updateStatus();
      ed.printStatusLine();
      ed.printLineNumbers();
      ed.drawTopBar();
      ed.printBuff();
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
}catch(exception e){ed.errWin(e);}}
  while(1);
  refresh();                      // Refresh display
  endwin();                       // End ncurses mode
  
  return EXIT_SUCCESS;
}

