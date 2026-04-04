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



#include <stdio.h>
#include <string.h>



#define MSG_MAX 1024



int main(int argc, char *argv[])
{
    char message[MSG_MAX];
    message[0] = '\0';

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
                return 1;
            }

            strcat(message, argv[i]);
            if (i < argc - 1) strcat(message, " ");
        }
    }
    // Fallback to stdin-based message (e.g. using a pipe)
    else
    {
        int c;
        size_t msgLen = 0;

        while ((c = getchar()) != EOF)
        {
            if (msgLen + 1 >= MSG_MAX)
            {
                printf("ERROR: message exceeds maximum length (%d characters)\n", MSG_MAX);
                return 1;
            }
            message[msgLen++] = (char)c;
        }

        message[msgLen] = '\0';

        if (msgLen == 0)
        {
            printf("ERROR: No message provided\n");
            return 1;
        }

        // Discard possible trailing newline
        if (message[msgLen - 1] == '\n')
            message[msgLen - 1] = '\0';
    }

    printf("%s\n", message);
    return 0;
}
