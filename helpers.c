#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include "helpers.h"

/**
 *  Gets a line from a file stream.
 *  Params:
 *      stream: the file stream
 *      readsBeforeEof: a pointer to an int which is set to -1 if no EOF was 
 *      read, or greater than -1 if EOF was read. The number that is set is
 *      the number of times fgets was called in the function before EOF was 
 *      read.
 *  Returns: The String wrapper struct containing information about the line
 *  that was read.
 **/
String get_line(FILE *stream, int *readsBeforeEof) {
    // Each iteration, read this number of chars into line.chars and increase
    // the size of line.chars by this amount.
    int inc = 20;

    *readsBeforeEof = -1; // -1 signifies EOF not reached
    int readsBeforeEofBuffer = 0;

    // initialise String line
    String line;
    line.size = inc + 1;
    line.chars = (char*)malloc(sizeof(char) * line.size);
    line.length = 0;
    
    // start reading
    if (!fgets(line.chars, inc + 1, stream)) {
        line.chars[0] = '\0';
        *readsBeforeEof = readsBeforeEofBuffer;
        return line;
    }
    readsBeforeEofBuffer++;
    int addedLength = strlen(line.chars);
    char last = line.chars[addedLength - 1];
    line.length += addedLength;
    
    char *pos;
    while (last != '\n') {
        line.chars = (char*)realloc(line.chars, sizeof(char) * (line.size += inc));
        pos = line.chars + line.length;
        if (!fgets(pos, inc + 1, stream)) {
            *readsBeforeEof = readsBeforeEofBuffer;
            return line;
        }
        readsBeforeEofBuffer++;
        addedLength = strlen(pos);
        last = pos[addedLength - 1];
        line.length += addedLength;
    }
    line.chars[--line.length] = '\0'; // remove new line at the end
    return line;
}

/** 
 *  Checks if a command string with 2 arguments of the form CMD:ARG1:ARG2 is 
 *  valid, i.e. the first and second arguments are not empty.
 *  e.g. CMD::ARG2 and CMD:ARG1: are invalid.
 *  Params:
 *      cmd: the command string
 *  Returns: true if both arguments are not empty, false otherwise.
 **/
bool is_valid_2_arg_cmd(const char *cmd) {
    char *firstArgIndex = strstr(cmd, ":") + 1;
    char *secondArgIndex = strstr(firstArgIndex, ":") + 1;
    return (secondArgIndex - firstArgIndex > 1) && (strlen(firstArgIndex) > 2);
}

/**
 *  For a command string of the form CMD:ARG1:ARG2, this function returns ARG1.
 *  Params:
 *      cmd: the command
 *  Returns: the string ARG1
 **/
char *get_first_arg(char *cmd) {
    char *firstArgIndex = strstr(cmd, ":") + 1;
    char *secondArgIndex = strstr(firstArgIndex, ":") + 1;
    int firstArgLength = secondArgIndex - firstArgIndex - 1;
    char *firstArg = (char*)malloc(sizeof(char) * (firstArgLength + 1));
    strncpy(firstArg, firstArgIndex, firstArgLength);
    firstArg[firstArgLength] = '\0';
    return firstArg;
}

/**
 *  Creates a new string with unprintable characters (< 32) converted to '?'.
 *  Params:
 *      old: the string to convert
 *  Returns: a new string with '?' replacing unprintable chars.
 **/
char *convert_unprintable(const char *old) {
    int oldLength = strlen(old);
    char *new = (char*)malloc(sizeof(char) * (oldLength + 1));
    for (int i = 0; i < oldLength; i++) {
        if ((int)old[i] < 32) {
            new[i] = '?';
        } else {
            new[i] = old[i];
        }
    }
    new[oldLength] = '\0';
    return new;
}

