/*
    ######################################################
    ##             SHORKTAINMENT - SHORKSAY             ##
    ######################################################
    ## A cute talking shark. A homage to Tony Monroe's  ##
    ## cowsay.                                          ##
    ######################################################
    ## Licence: GNU GENERAL PUBLIC LICENSE Version 3    ##
    ######################################################
    ## Kali (links.sharktastica.co.uk)                  ##
    ######################################################
*/



#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



#define MSG_MAX         1024
#define SHORK_WIDTH     41
#define SHORK_HEIGHT    7



const char SHORK[SHORK_HEIGHT][SHORK_WIDTH] = {
"                 ^`.                 / ",
"  ^_             \\  \\               /  ",
"  \\ \\           /    `~~--__       /   ",
"  {  \\___----~'             `~-_  /    ",
"  / /~~~-, ,_.          \\\\\\ __o,)      ",
"  \\/     \\/   `~~~;   ,--~~-`          ",
"                 '._.'                 "
};
static struct winsize TERM_SIZE;



/**
 * Removes duplicate consecutive spaces and newlines.
 * @param str Input string
 */
void cleanStr(char *str)
{
    int readI = 0;
    int writeI = 0;
    int inWhitespace = 0;

    while (str[readI] != '\0')
    {
        // Begin to operate if space, tab or newline found
        if (str[readI] == ' ' || str[readI] == '\t' || str[readI] == '\n')
        {
            if (!inWhitespace)
            {
                str[writeI++] = ' ';
                inWhitespace = 1;
            }
        }
        // Just copy normally
        else
        {
            str[writeI++] = str[readI];
            inWhitespace = 0;
        }

        readI++;
    }

    // Remove trailing spaces if present
    if (writeI > 0 && str[writeI - 1] == ' ')
        writeI--;

    str[writeI] = '\0';
}

/**
 * Finds and replaces a given search term with a desired replacement term from an
 * input string.
 * @param buffer Input string
 * @param bufferSize Size to use when allocating the result string
 * @param needle Substring to find and replace
 * @param replacement New string to insert
 * @return String after term replacement
 */
char *findReplace(const char *buffer, const size_t bufferSize, const char *needle, const char *replacement)
{
    if (!buffer || !needle || !replacement || bufferSize < 2) return strdup("");

    size_t needleLen = strlen(needle);
    size_t replacementLen = strlen(replacement);
    if (needleLen == 0 || replacementLen == 0) return strdup("");

    char *result = malloc(bufferSize);
    if (!result) return strdup("");

    strncpy(result, buffer, bufferSize);
    result[bufferSize - 1] = '\0';

    char *pos = result;
    while ((pos = strstr(pos, needle)) != NULL)
    {
        size_t tailLen = strlen(pos + needleLen);

        if (replacementLen > needleLen)
        {
            size_t currentLen = strlen(result);
            size_t newLen = currentLen + (replacementLen - needleLen) + 1;
            char *tmp = realloc(result, newLen);
            if (!tmp) break;
            pos = tmp + (pos - result);
            result = tmp;
        }

        memmove(pos + replacementLen, pos + needleLen, tailLen + 1);
        memcpy(pos, replacement, replacementLen);
        pos += replacementLen;
    }

    return result;
}

/**
 * Adds new lines to a given string based on the requested line width.
 * @param buffer Input string
 * @param width Characters per line
 * @param indent Indent to include after newly inserted new line
 * @return Number of lines in the string
 */
int formatNewLines(char *buffer, int width, char *indent)
{
    if (!buffer || width < 1) return 0;

    size_t bufferStrLen = strlen(buffer);
    size_t indentLen = indent ? strlen(indent) : 0;
    int lines = 1;
    int lastSpace = -1;
    int widthCount = 1;

    for (int i = 0; i < bufferStrLen; i++)
    {
        if (buffer[i] == '\033')
        {
            while (i < bufferStrLen && buffer[i] != 'm') i++;
            if (i >= bufferStrLen) break;
            continue; 
        }
        
        if (buffer[i] == ' ') lastSpace = i;
        else if (buffer[i] == '\n')
        {
            lines++;
            widthCount = 0;
            continue;
        }

        if (widthCount == width)
        {
            if (lastSpace != -1)
            {
                buffer[lastSpace] = '\n';
                lines++;

                if (indent && indentLen > 0)
                {
                    memmove(buffer + lastSpace + 1 + indentLen, buffer + lastSpace + 1, bufferStrLen - lastSpace);
                    memcpy(buffer + lastSpace + 1, indent, indentLen);
                    bufferStrLen += indentLen;
                    if (lastSpace <= i) i += indentLen;
                }
            }
            widthCount = i - lastSpace;
        }

        widthCount++;
    }

    return lines;
}

/**
 * @return winsize struct containing the current terminal size in columns and rows
 */
struct winsize getTerminalSize(void)
{
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
    {
        ws.ws_col = 80;
        ws.ws_row = 24;
    }
    return ws;
}

void printMessage(char *msg)
{
    int lines = formatNewLines(msg, SHORK_WIDTH - 4, NULL);
    size_t msgLen = strlen(msg);

    if (lines == 1)
    {
        int padding = SHORK_WIDTH - 5 - msgLen;

        // Print top border
        for (int i = -1; i < padding; i++) printf(" ");
        for (int i = 0; i < msgLen + 2; i++) printf("-");
        printf("\n");

        // Print message
        for (int i = 0; i < padding; i++) printf(" ");
        printf("< %s >\n", msg);

        // Print bottom border
        for (int i = -1; i < padding; i++) printf(" ");
        for (int i = 0; i < msgLen + 2; i++) printf("-");
        printf("\n");
    }
    else
    {
        // Print top border
        printf(" ");
        for (int i = 0; i < SHORK_WIDTH - 3; i++)
            printf("-");
        printf("\n");

        // Print message
        char *currPos = msg;
        for (int i = 0; i < lines; i++)
        {
            char *lineStart = currPos;
            char *nextNL = strchr(currPos, '\n');

            int len;
            // If future newline found, prepare to print until reaching it
            if (nextNL)
            {
                len = nextNL - currPos;
                currPos = nextNL + 1;
            }
            // If no future newline, prepare to print the rest of message
            else
            {
                len = strlen(currPos);
                currPos += len;
            }

            if (i == 0) printf("/ ");
            else if (i == lines - 1) printf("\\ ");
            else printf("| ");

            printf("%.*s", len, lineStart);
            for (int j = len; j < SHORK_WIDTH - 5; j++) printf(" ");

            if (i == 0) printf(" \\\n");
            else if (i == lines - 1) printf(" /\n");
            else printf(" |\n");
        }

        // Print bottom border
        printf(" ");
        for (int i = 0; i < SHORK_WIDTH - 3; i++)
            printf("-");
        printf("\n");
    }
}

void printShork(void)
{
    for(int i = 0; i < SHORK_HEIGHT; i++)
    {
        printf("%s\n", SHORK[i]);
    }
}



int main(int argc, char *argv[])
{
    char *message = malloc(MSG_MAX);
    if (!message)
    {
        printf("ERROR: cannot allocate memory for message\n");
        return 1;
    }
    message[0] = '\0';

    TERM_SIZE = getTerminalSize();
    int likelyPiped = 0;

    // Test for argument-based message
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            size_t msgLen = strlen(message);
            size_t wordLen = strlen(argv[i]);
            if (i < argc - 1) wordLen += 1;

            if (msgLen + wordLen + 1 > MSG_MAX)
            {
                printf("ERROR: message exceeds maximum length (%d characters)\n", MSG_MAX);
                free(message);
                return 1;
            }

            strcat(message, argv[i]);
            if (i < argc - 1) strcat(message, " ");
        }
    }
    // Fallback to stdin-based message (e.g. using a pipe)
    else
    {
        likelyPiped = 1;
        size_t msgLen = 0;
        int c;

        while ((c = getchar()) != EOF && msgLen < MSG_MAX - 1)
        {
            message[msgLen++] = (char)c;
        }
        message[msgLen] = '\0';

        if (msgLen == 0)
        {
            printf("ERROR: No message provided\n");
            free(message);
            return 1;
        }

        if (c != EOF && msgLen >= MSG_MAX - 1)
        {
            printf("ERROR: Input exceeds maximum length\n");
            free(message);
            return 1;
        }

        if (msgLen > 0 && message[msgLen - 1] == '\n')
            message[msgLen - 1] = '\0';
    }

    if (TERM_SIZE.ws_col < SHORK_WIDTH - 1 || TERM_SIZE.ws_row < 11)
    {
        printf("ERROR: terminal size too small (must be %dx%d or more)\n", SHORK_WIDTH + 5, 11);
        free(message);
        return 1;
    }

    cleanStr(message);
    if (likelyPiped && strstr(message, "-- "))
    {
        char *replaced = findReplace(message, MSG_MAX, " -- ", "\n -- ");
        if (replaced != NULL)
        {
            if (replaced != message) free(message);
            message = replaced;
        }
    }
    printMessage(message);
    printShork();

    free(message);
    return 0;
}
