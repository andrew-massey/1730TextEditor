#include "Buffer.h"

Buffer::Buffer() {}

/**
 * Finds tab characters in the given line and replaces them with spaces.
 * @param line Takes in the string to be modified.
 * @return Returns a string without tabs.
 */
string Buffer::remTabs(string line)
{
    auto tab = line.find("\t");
    if(tab == line.npos)
        return line;
    else
        return remTabs(line.replace(tab, 1, "    "));
}

/**
 * Inserts a new line into the buffer. Takes the line to be inserted,
 * and the number to insert it in.
 * @param line The string to insert
 * @param n The number to insert
 */
void Buffer::insertLine(string line, int n)
{
    line = remTabs(line);                   // Conversion (happens every time)
    lines.insert(lines.begin()+n, line);
}

/**
 * Append a line to the buffer.
 * @param line The string to append to the buffer.
 */
void Buffer::appendLine(string line)
{
    line = remTabs(line);
    lines.push_back(line);
}

/**
 * Remove a line from the buffer.
 * @param n The line to be removed from the buffer.
 */
void Buffer::removeLine(int n)
{
    lines.erase(lines.begin()+n);
}
