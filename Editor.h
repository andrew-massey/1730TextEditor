#ifndef EDITOR_H
#define EDITOR_H

#include <ncurses.h>

#include "Buffer.h"

//The left side gap for line numbers
const int X_MARGIN = 7;

//The top gap for the menu bar
const int Y_MARGIN = 2;



class Editor
{
private:
    unsigned int x, y;
    char mode;
    Buffer* buff;
    string status, filename;
    int topLineNum = 0; //The number that controls where we are in the file display

    /* For those of you who do not have -std=c++11 in g++ */
    string tos(int);

    // Cursor movement
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();

    void backspaceHandler(int, int);    // Deletes characters in weird line scenarios
    void deleteLine();                  // Deletes current line
    void deleteLine(int);               // Deletes line <int>

    void saveFile();                    // Saves buffer into the file

public:
    Editor();                           // Normal constructor
    Editor(string);                     // Constructor accepting filename
    //When you open a new file, this holds the filename
    static string opener;
  
    char getMode() {return mode;}
    void handleInput(int);              // Handles keyboard input
    void printBuff();                   // Prints buffer to screen
    void drawTopBar();                  // Prints top bar
    void printStatusLine();             // Prints the status line (like vim!!!)
    void updateStatus();                // Updates the status line (text, not display)
    void menu();                        // Creates the menu with options
    void printLineNumbers();            // Prints line numbers
    void errWin(const std::exception&); //Creates an error window
    void errWin();
};

#endif
