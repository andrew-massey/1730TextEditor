#include "Editor.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

Editor::Editor()
{
    x=X_MARGIN;y=0,mode='d';
    filename = "untitled";

    /* Initializes buffer and appends line to
        prevent seg. faults */
    buff = new Buffer();
    buff->appendLine("");
}

Editor::Editor(string fn)
{
    x=X_MARGIN;y=0;
    filename = fn;

    buff = new Buffer();

    ifstream infile(fn.c_str());
    if(infile.is_open())
    {
        while(!infile.eof())
        {
            string temp;
            getline(infile, temp);
            buff->appendLine(temp);
        }
    }
    else
    {
        cerr << "Cannot open file: '" << fn << "'\n";
        buff->appendLine("");
    }
}

void Editor::updateStatus()
{
  status = "\tCOL: " + tos(x - X_MARGIN) + "\tROW: " + tos(y);
}

void Editor::printLineNumbers()
{
  init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
  attron(COLOR_PAIR(1));
  
  for(int i = 0, j = topLineNum; i < LINES - 1; i++, j++)
    {
      mvprintw(i, 0, "      ");
      mvprintw(i, 0, "%d", j);
    }
  attroff(COLOR_PAIR(1));
}

string Editor::tos(int i)
{
    stringstream ss;
    ss << i;
    return ss.str();
}

void Editor::handleInput(int c)
{
    switch(c)
      {
      case KEY_LEFT:
	    moveLeft();
	    return;
      case KEY_RIGHT:
    	moveRight();
    	return;
      case KEY_UP:
    	moveUp();
    	return;
      case KEY_DOWN:
    	moveDown();
    	return;
      case KEY_HOME: //JOE - HOME KEY
    	topLineNum = 0;
    	return;
      case KEY_END: //JOE - END KEY
    	topLineNum = buff->lines.size() - LINES - 2;
    	return;
      case KEY_PPAGE: //JOE - PAGE UP
     	if (topLineNum - (LINES - 1) < 0)
	       topLineNum = 0;
    	else
	      topLineNum -= (LINES - 1);
     	return;
      case KEY_NPAGE: // JOE - PAGE DOWN
    	if (topLineNum + (LINES - 1) > (buff->lines.size() - 1) - LINES - 1)
    	  topLineNum = buff->lines.size() - LINES - 2;
    	else
    	  topLineNum += (LINES - 1);
    	return;
      case KEY_F(1):
    	menu();
    	return;
      case KEY_BTAB:
      case KEY_CTAB:
      case KEY_STAB:
      case KEY_CATAB:
      case 9:
	// The Tab key
	buff->lines[topLineNum + y].insert(x - X_MARGIN, 4, ' ');
	x += 4;
	break;
      case 8:
      case 127:
      case KEY_BACKSPACE: //JOE - LOTS OF CHANGES HERE FOR MAKING THE LINES LINE UP CORRECTLY
	// The Backspace key
	if(x == X_MARGIN && y > 0)
	  {
	    x = buff->lines[topLineNum + y-1].length() + X_MARGIN;
	    backspaceHandler(y-1,y);
	    moveUp();
	  }
	else if (x > X_MARGIN)
	  {
	    // Removes a character
	    buff->lines[topLineNum + y].erase(--x - X_MARGIN,1);
	    //   moveLeft();
	    }
	break;
      case KEY_DC: //JOE - SAME AS BACKSPACE TYPES OF CHANGES
	// The Delete key
	if(x == buff->lines[topLineNum + y].length() + X_MARGIN && y != buff->lines.size() - 1)
	  {
	    backspaceHandler(y, y+1);
	  }
	else
	  {
	    buff->lines[topLineNum + y].erase(x - X_MARGIN, 1);
	  }
	break;
      case KEY_ENTER:
      case 10:
	// The Enter key
	// Bring the rest of the line down
	if(x < buff->lines[topLineNum + y].length() + X_MARGIN)
	  {
	    // Put the rest of the line on a new line
	    buff->insertLine(buff->lines[topLineNum + y].substr(x - X_MARGIN, buff->lines[topLineNum + y].length() - x + X_MARGIN), topLineNum + y + 1);
	    // Remove that part of the line
	    buff->lines[topLineNum + y].erase(x - X_MARGIN, buff->lines[topLineNum + y].length() - x + X_MARGIN);
	  }
	else
	  {
	    buff->insertLine("", topLineNum + y+1);
	  }
	x = X_MARGIN;
	moveDown();
	break;	
  default:
    if((x - X_MARGIN)==100){
   	  if(x < buff->lines[topLineNum + y].length() + X_MARGIN)
	    {
	      // Put the rest of the line on a new line
	      buff->insertLine(buff->lines[topLineNum + y].substr(x - X_MARGIN, buff->lines[topLineNum + y].length() - x + X_MARGIN), topLineNum + y + 1);
	      // Remove that part of the line
	      buff->lines[topLineNum + y].erase(x - X_MARGIN, buff->lines[topLineNum + y].length() - x + X_MARGIN);
	    }
  	  else
	    {
	      buff->insertLine("", topLineNum + y+1);
	    }
	    x = X_MARGIN;
	    moveDown();
    }
	  buff->lines[topLineNum + y].insert(x - X_MARGIN, 1, char(c));
	  x++;
	  break;	
}

    /**save the current file
            saveFile();
            break;
    */
}

//JOE - THIS IS THE BACKSPACE HANDLER - BRINGS THE NEW LINE UP AS NECESSARY
void Editor::backspaceHandler(int firstY, int secondY)
{
  int substringDistanceCalc = (COLS - x - 1);
  buff->lines[topLineNum + firstY] += buff->lines[topLineNum + secondY].substr(0, substringDistanceCalc);
  // Delete the line
  if (buff->lines[topLineNum + secondY].length() < substringDistanceCalc)
    deleteLine(secondY); //deleteLine already has topLine built in
  else
    buff->lines[topLineNum + secondY] = buff->lines[topLineNum + secondY].substr(substringDistanceCalc);
}

void Editor::moveLeft()
{
    if(x > X_MARGIN)
    {
        x--;
        move(y, x);
    }
    /*JOE CHANGE*/
    //If at the beginning of a line, move to end of prev line
    else if (x == X_MARGIN && y > 0)
      {
	x = buff->lines[topLineNum + y-1].length() + X_MARGIN;
	moveUp();
      }
    /*END JOE CHANGE*/
}

void Editor::moveRight()
{
  if(x+1 < (unsigned int)COLS && x+1 <= buff->lines[topLineNum + y].length() + X_MARGIN)
    {
        x++;
        move(y, x);
    }
    /*JOE CHANGE*/
    //If at the end of a line, move to beginning of next line
    else if (x == buff->lines[topLineNum + y].length() + X_MARGIN)
      {
	x = X_MARGIN;
	moveDown();
      }
    /*END OF JOE CHANGE*/
}

void Editor::moveUp()
{
    if(y > 0)
        y--;
    else if (y == 0 && topLineNum > 0)
      topLineNum--;
    if(x >= buff->lines[topLineNum + y].length() + X_MARGIN)
        x = buff->lines[topLineNum + y].length() + X_MARGIN;
    move(y, x);
}

void Editor::moveDown()
{
  if(y+1 < (unsigned int)(LINES-1) && y+1 < buff->lines.size())
        y++;
  else if (y + 1 >= (unsigned int)(LINES - 1) && y < buff->lines.size() - LINES)
      {
	topLineNum++;
      }
    if(x >= buff->lines[topLineNum + y].length() + X_MARGIN)
        x = buff->lines[topLineNum + y].length() + X_MARGIN;
    move(y,x);
}

void Editor::printBuff()
{
  for(int i = 0, j = topLineNum; i < LINES - 1; i++, j++)
    {
      if((unsigned int)i >= buff->lines.size())
        {
            move(i, X_MARGIN);
            clrtoeol();
        }
        else
        {
            mvprintw(i, X_MARGIN, buff->lines[j].c_str());
        }
	clrtoeol();
    }
    move(y, x);
}
void Editor::printStatusLine()
{
    attron(A_REVERSE);
    mvprintw(LINES-1, 0, status.c_str());
    clrtoeol();
    attroff(A_REVERSE);
}

void Editor::deleteLine()
{
    buff->removeLine(topLineNum+y);
}

void Editor::deleteLine(int i)
{
    buff->removeLine(topLineNum + i);
}

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

void Editor::menu(){
  WINDOW *menu = newwin(0,0,0,0);
  keypad(menu, TRUE);
  
  box(menu,0,0);
  wmove(menu,1,1);
  wprintw(menu,"Press the corresponding key for any of the following options:");
  wmove(menu,3,3);
  wprintw(menu,"Open (1)");
  wmove(menu,4,3);
  wprintw(menu,"Save (2)");
  wmove(menu,5,3);
  wprintw(menu,"Save As (3)");
  wmove(menu,6,3);
  wprintw(menu,"Exit (4)");
  wmove(menu,7,3);
  wprintw(menu,"Close menu (5)");
  wrefresh(menu);
  refresh();
  
  char ch = 'a';
  while(ch!='5'){
    ch=getch();
    string s = "";
    switch(ch){
      //open
      case '1':
        break;
      //save
      case '2':{
      saveFile();
      werase(menu);
      refresh();
      break;}
      //save as
      case '3':
      {wmove(menu,15,3);
      wprintw(menu,"What would you like to save the file as?");
      wmove(menu,16,3);
      int w=4;
      wrefresh(menu);
      refresh();
      int key = ch;
      while((ch!=KEY_ENTER)||(ch!=10)){
        key = getch();
        ch = key;
        if(key==KEY_ENTER||key==10){
          break;
        }
        else{
        wprintw(menu,&ch);
          s+=ch;
          wrefresh(menu);
          refresh();
        }
        }
        filename = s;
        saveFile();
        wmove(menu, 20, 3);
        wprintw(menu,"File saved successfully!");
      break;
      }//exit program
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

void Editor::errWin(const std::exception& e){
  WINDOW *warning = newwin(0,0,0,0);
  keypad(warning, TRUE);
  box(warning,0,0);
  
  int px,py;
  getmaxyx(warning,py,px);
  int a = py/2;
  int b = px/2;
  wmove(warning, a, b);
  wprintw(warning, "ERROR:");
  wprintw(warning, e.what());
  wrefresh(warning);
  refresh();
  char c = 'd';
  while(1){
    c = getch();
    if(c == KEY_ENTER){
      werase(warning);
      break;
    }
  } 
}
