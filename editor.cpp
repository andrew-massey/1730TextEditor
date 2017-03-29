#include "Editor.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

string Editor::opener = "";


/**
 * Default constructor. Creates an untitled file.
 */
Editor::Editor()
{
    x=X_MARGIN;y=Y_MARGIN,mode='d';
    filename = "untitled";

    /* Initializes buffer and appends line to
        prevent seg. faults */
    buff = new Buffer();
    buff->appendLine("");
}

/**
 * Constructor for the editor that takes a string filename.
 * @param fn The filename to be passed in.
 */
Editor::Editor(string fn)
{
  if (fn.empty()) //Filename string is empty, pass to default constructor
    {
      new (this) Editor(); return;
    }
  else
    {
      x=X_MARGIN;y=Y_MARGIN;
      filename = fn;
      
      buff = new Buffer();
      char * temp = new char[COLS - X_MARGIN]; //JOE - Char array to hold the temp buffer
      
      ifstream infile(fn.c_str());
      if(infile.is_open())
	{
	  while(infile.peek() != EOF)
	    {
	    infile.getline(temp, COLS - X_MARGIN); //JOE - only hold enough of the line to fill the screen
	    string tempConv(temp); //Convert the char array to string
            buff->appendLine(tempConv);
	    infile.clear();
	    }
	}
      else
	{
	  cerr << "Cannot open file: '" << fn << "'\n";
	  buff->appendLine("");
	}
      delete [] temp;
    }
}

/**
 * Draw the bar on the top of the screen.
 */
void Editor::drawTopBar()
{
  WINDOW * topBar;
  topBar = subwin(stdscr, 1, COLS,0,0);
  init_pair(2, COLOR_BLACK, COLOR_WHITE);
  wbkgd(topBar,COLOR_PAIR(2));
  mvwprintw(topBar, 0, 2, "F1: Menu"); //Menu option
  mvwprintw(topBar, 0, (COLS/2)-1, "CS1730 Text Editor"); //Name of the editor
}
/**
* Updates the user of their position
*/
void Editor::updateStatus()
{
  status = " " + filename + "\tCOL: " + tos(x - X_MARGIN) + "\tROW: " + tos(y - Y_MARGIN);
}

/**
 * Prints the line numbers on the left side of the screen.
 */
void Editor::printLineNumbers()
{
  init_pair(1, COLOR_MAGENTA, COLOR_BLACK); //Pretty in pink
  attron(COLOR_PAIR(1));
  
  for(int i = 0, j = topLineNum; i < LINES - 1 - Y_MARGIN; i++, j++)
    {
      mvprintw(i + Y_MARGIN, 0, "      ");
      mvprintw(i + Y_MARGIN, 0, "%d", j);
    }
  attroff(COLOR_PAIR(1));
}

/**
 * Convert numbers to strings.
 * @param i Number to be converted to string
 * @return The converted string
 */
string Editor::tos(int i)
{
    stringstream ss;
    ss << i;
    return ss.str();
}

/**
 * Handle user input from the keyboard.
 * @param c The key int to be interpreted
 */
void Editor::handleInput(int c)
{
    switch(c)
      {
      case KEY_LEFT: //LEFT ARROW
	moveLeft();
	return;
      case KEY_RIGHT: //RIGHT ARROW
    	moveRight();
    	return;
      case KEY_UP: //UP ARROW
    	moveUp();
    	return;
      case KEY_DOWN: //DOWN ARROW
    	moveDown();
    	return;
      case KEY_HOME: //HOME KEY
    	topLineNum = 0;
    	return;
      case KEY_END: //END KEY
    	topLineNum = buff->lines.size() - LINES - 1 + Y_MARGIN;
    	return;
      case KEY_PPAGE: //PAGE UP
	//If we'll exceed the top of the page, just go to the top
     	if (topLineNum - LINES - 1 < 0)
	       topLineNum = 0;
    	else //Else page up
	      topLineNum -= (LINES - 1);
     	return;
      case KEY_NPAGE: // JOE - PAGE DOWN
	//If page down will go beyond the end of the file, set the last page to be exactly the length of the last page
    	if ((unsigned int)(topLineNum + (LINES - 1)) > (buff->lines.size()) - LINES - 1)
    	  topLineNum = buff->lines.size() - LINES - Y_MARGIN;
    	else //Else just go down
    	  topLineNum += (LINES - 1) - Y_MARGIN;
    	return;
      case KEY_F(1): //F1 menu
    	menu();
    	return;
      case KEY_BTAB:
      case KEY_CTAB:
      case KEY_STAB:
      case KEY_CATAB:
      case 9:
	// The Tab key
	buff->lines[topLineNum + y - Y_MARGIN].insert(x - X_MARGIN, 4, ' ');
	x += 4;
	break;
      case 8:
      case 127:
      case KEY_BACKSPACE: //Backspace key
	if(x == X_MARGIN && y > 0 + Y_MARGIN)
	  {
	    x = buff->lines[topLineNum + y-1 - Y_MARGIN].length() + X_MARGIN;
	    backspaceHandler(y-1,y);
	    moveUp();
	  }
	else if (x > X_MARGIN)
	  {
	    // Removes a character
	    buff->lines[topLineNum + y - Y_MARGIN].erase(--x - X_MARGIN,1);
	    //   moveLeft();
	    }
	break;
      case KEY_DC: // The Delete key
	if(x == buff->lines[topLineNum + y - Y_MARGIN].length() + X_MARGIN && topLineNum + y - Y_MARGIN != buff->lines.size() - 1)
	  {
	    backspaceHandler(y, y+1);
	  }
	else
	  {
	    buff->lines[topLineNum + y - Y_MARGIN].erase(x - X_MARGIN, 1);
	  }
	break;
      case KEY_ENTER:
      case 10:
	// The Enter key
	// Bring the rest of the line down
	if(x < buff->lines[topLineNum + y - Y_MARGIN].length() + X_MARGIN)
	  {
	    // Put the rest of the line on a new line
	    buff->insertLine(buff->lines[topLineNum + y - Y_MARGIN].substr(x - X_MARGIN, buff->lines[topLineNum + y - Y_MARGIN].length() - x + X_MARGIN), topLineNum + y + 1 - Y_MARGIN);
	    // Remove that part of the line
	    buff->lines[topLineNum + y - Y_MARGIN].erase(x - X_MARGIN, buff->lines[topLineNum + y - Y_MARGIN].length() - x + X_MARGIN);
	  }
	else
	  {
	    buff->insertLine("", topLineNum + y+1 - Y_MARGIN);
	  }
	x = X_MARGIN;
	moveDown();
	break;	
      default: //Enter all other characters!
	if(x == (unsigned int)(COLS - 1))
	  { //If we're at the end of a line, move to the next line
	    x = X_MARGIN;
	    moveDown();
	    break;
	  }
	else if (buff->lines[topLineNum + y - Y_MARGIN].length() == (unsigned int)(COLS - 1 - X_MARGIN))
	  {
	    break; //JOE - Simplest solution to fixing stuff going to the next line weirdly - just don't do it!
	  }
	buff->lines[topLineNum + y - Y_MARGIN].insert(x - X_MARGIN, 1, char(c));
	x++;
	break;	
      }
}

/**
 * Handles the backspace/delete functionality when it will cause weird line
 * wrapping things to occur.
 * @param firstY The line to be modified.
 * @param secondY The line that will be added to the first, or potentially
 * deleted.
 */
void Editor::backspaceHandler(int firstY, int secondY)
{
  unsigned int substringDistanceCalc = (COLS - x - 1);
  buff->lines[topLineNum + firstY - Y_MARGIN] += buff->lines[topLineNum + secondY - Y_MARGIN].substr(0, substringDistanceCalc);
  // Delete the line
  if (buff->lines[topLineNum + secondY - Y_MARGIN].length() < substringDistanceCalc)
    deleteLine(secondY);
  else
    buff->lines[topLineNum + secondY - Y_MARGIN] = buff->lines[topLineNum + secondY - Y_MARGIN].substr(substringDistanceCalc);
}
/**
 * Moves the cursor left.
 */
void Editor::moveLeft()
{
  //If X isn't on the left edge, move left
  if(x > X_MARGIN)
    {
      x--;
      move(y, x);
    }
  /*JOE CHANGE*/
  //If at the beginning of a line, move to end of prev line
  else if (x == X_MARGIN && y > 0 + Y_MARGIN)
    {
      x = buff->lines[topLineNum + y-1 - Y_MARGIN].length() + X_MARGIN;
      moveUp();
    }
  /*END JOE CHANGE*/
}

/**
 * Move the cursor right.
 */
void Editor::moveRight()
{
  //If X isn't on the right edge, move right
  if(x+1 < (unsigned int)COLS && x+1 <= buff->lines[topLineNum + y - Y_MARGIN].length() + X_MARGIN)
    {
        x++;
        move(y, x);
    }
    /*JOE CHANGE*/
    //If at the end of a line, move to beginning of next line
    else if (x == buff->lines[topLineNum + y - Y_MARGIN].length() + X_MARGIN)
      {
	x = X_MARGIN;
	moveDown();
      }
    /*END OF JOE CHANGE*/
}

/**
 * Move the cursor up.
 */
void Editor::moveUp()
{
  //If not at the top, move up
  if(y > 0 + Y_MARGIN)
    y--;
  else if (y == 0 + Y_MARGIN && topLineNum > 0)
    topLineNum--; //If at the top of the screen but not top of the file, scroll up
  
  //If the line above is shorter, move to the end of that line
  if(x >= buff->lines[topLineNum + y - Y_MARGIN].length() + X_MARGIN)
    x = buff->lines[topLineNum + y - Y_MARGIN].length() + X_MARGIN;
  move(y, x);
}

/**
 * Move the cursor down.
 */
void Editor::moveDown()
{
  //If not at the bottom or the end of the file, move down
  if(y+1 < (unsigned int)(LINES-1) && y+1 + topLineNum - Y_MARGIN < buff->lines.size())
        y++;
  else if (y + 1 >= (unsigned int)(LINES - 1) && y + topLineNum - Y_MARGIN + 1< buff->lines.size())
      {
	topLineNum++; //If at the bottom but not the end of the file, move up
      }
  //If the line below is shorter, move to the end of that line
    if(x >= buff->lines[topLineNum + y - Y_MARGIN].length() + X_MARGIN)
        x = buff->lines[topLineNum + y - Y_MARGIN].length() + X_MARGIN;
    move(y,x);
}

/**
 * Print each line from the buffer to the display.
 */
void Editor::printBuff()
{
  for(int i = 0, j = topLineNum; i < LINES - 1 - Y_MARGIN; i++, j++)
    {
      if((unsigned int)i >= buff->lines.size())
        {
            move(i + Y_MARGIN, X_MARGIN);
            clrtoeol();
        }
        else
        {
            mvprintw(i + Y_MARGIN, X_MARGIN, buff->lines[j].c_str());
        }
	clrtoeol();
    }
    move(y, x);
}

/**
 * Print the status line at the bottom of the screen.
 */
void Editor::printStatusLine()
{
  //Make it a pretty white bar!
  WINDOW * bottomBar;
  bottomBar = subwin(stdscr, 1, COLS, LINES-1,0);
  init_pair(2, COLOR_BLACK, COLOR_WHITE);
  wbkgd(bottomBar,COLOR_PAIR(2));
  mvwprintw(bottomBar, 0, 0, status.c_str());
  wrefresh(bottomBar);
}

/**
 * Delete the current line from the buffer.
 */
void Editor::deleteLine()
{
    buff->removeLine(topLineNum+y - Y_MARGIN);
}

/**
 * Delete the specified line from the buffer.
 * @param i The line to be deleted
 */
void Editor::deleteLine(int i)
{
    buff->removeLine(topLineNum + i - Y_MARGIN);
}

/**
 * Save the file to disk.
 */
void Editor::saveFile()
{
    if(filename == "")
    {
        // Set filename to untitled
        filename = "untitled";
    }

    ofstream f(filename.c_str());
    if(f.is_open())
    {
        for(unsigned int i=0; i<buff->lines.size(); i++)
        {
            f << buff->lines[i] << endl;
        }
        status = "Saved to file!";
    }
    else
    {
        status = "Error: Cannot open file for writing!";
    }
    f.close();
}

/**
 * Print the F1 menu to the screen.
 */
void Editor::menu(){
  int winX, winMaxY, winMaxX;
  WINDOW *menu = newwin((LINES / 2) + Y_MARGIN, COLS / 2, 2, 4);
  keypad(menu, TRUE); //Accept key presses

  getmaxyx(menu, winMaxY, winMaxX);
  winX = (winMaxX - 4)/2;  
  winMaxY = winMaxY; //Shut the compiler up

  //sets up the aesthetics of the menu
  init_pair(2, COLOR_BLACK, COLOR_WHITE);
  wattrset(menu, COLOR_PAIR(2) | WA_BOLD);
  wbkgd(menu, COLOR_PAIR(2));
  box(menu,0,0);
  mvwaddch(menu, 0, winX, ACS_RTEE);
  waddch(menu, ' ');
  waddstr(menu, "Menu");
  waddch(menu, ' ');
  waddch(menu, ACS_LTEE);

  mvwprintw(menu,1,1,"Press the corresponding key for any of the following options:");
  mvwprintw(menu,3,3,"(1) Open");
  mvwprintw(menu,4,3,"(2) Save");
  mvwprintw(menu,5,3,"(3) Save As");
  mvwprintw(menu,6,3,"(4) Exit");
  mvwprintw(menu,7,3,"(5) Close menu");

  wrefresh(menu);
  refresh();
  
  char ch = 'a';

  while(ch!='5')
    {
      ch=getch();
      string s = "";
      
      switch(ch)
	{
	  //open
	case '1':{
   mvwprintw(menu,15,3,"What file would you like to open?");
      wmove(menu,16,3);
      int w=4;
      wrefresh(menu);
      refresh();
      int key = ch;
      while((ch!=KEY_ENTER)||(ch!=10))
        {
    key = getch();
    ch = key;
    if(key==KEY_ENTER||key==10)
      {
        break;
    }
    else
      {
        wprintw(menu,&ch);
        s+=ch;
        wrefresh(menu);
        refresh();
      }
        }

    Editor::opener = s;
    mode ='x';
    werase(menu);
    wrefresh(menu);
    refresh(); 
    return;
	  break;}
	  //save
	case '2':
	  saveFile();
	  werase(menu);
	  refresh();
	  break;
	  //save as
	case '3':
	  {
	    mvwprintw(menu,15,3,"What would you like to save the file as?");
	    wmove(menu,16,3);
	    int w=4;
	    wrefresh(menu);
	    refresh();
	    int key = ch;
	    while((ch!=KEY_ENTER)||(ch!=10))
	      {
		key = getch();
		ch = key;
		if(key==KEY_ENTER||key==10)
		  {
		    break;
		}
    else
		  {
		    mvwprintw(menu,16,w,&ch);
		    s+=ch;
        w++;
		    wrefresh(menu);
		    refresh();
		  }
	      }
	    filename = s;
	    saveFile();
	    wmove(menu, 20, 3);
	    wprintw(menu,"File saved successfully!");
	    break;
	  }
	  //exit program
	case '4': 
	  endwin();
	  exit(EXIT_SUCCESS);
	  break;
	case '5':
	  werase(menu);
	  break;	
	}
    }
}
/**
* errWin(const std::exception& e) - creates an error window
* @param - const std::exception& e: the error
*/
void Editor::errWin(const std::exception& e){
  //creates the window
  WINDOW *warning = newwin(0,0,0,0);
  keypad(warning, TRUE);
  box(warning,0,0);
  
  int px,py;
  getmaxyx(warning,py,px);
  int a = py/2;
  int b = px/2;
  wmove(warning, a, b);
  //prints the error
  wprintw(warning, "ERROR:");
  wprintw(warning, e.what());
  wrefresh(warning);
  refresh();
  char c = 'd';
  while(1){
    c = getch();
    if(c == KEY_ENTER){
      werase(warning);
      refresh();
      break;
    }
  } 
}
