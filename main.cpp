// $Id: main.cpp,v 1.6 2014-10-09 15:44:18-07 - - $
// Lou George, ligeorge@ucsc.edu  

#include <errno.h>
#include <fstream>
#include <iostream>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <unistd.h>


#include "astree.h"
#include "auxlib.h"
#include "lyutils.h"
#include "stringset.h"

using namespace std;

//From cppstrtok.cpp
string CPP = "/usr/bin/cpp";
FILE* tokenFile;
const size_t LINESIZE = 1024;

void cpplines (FILE* pipe) {
    for(;;){
        char buffer[LINESIZE];
        char* fgets_rc = fgets(buffer, LINESIZE, pipe);
        
        if (fgets_rc == NULL) break;
        if (*buffer == '#') continue;

        char* savepos = NULL;
        char* bufptr = buffer;

        for (int tokenct = 1;; ++tokenct) {
            char* token = strtok_r (bufptr, " \t\n", &savepos);
            bufptr = NULL;
            if (token == NULL) break;
            intern_stringset (token);
        }
    }//for(;;)
}//cpplines

int main (int argc, char** argv) {
    set_execname (argv[0]);
    int setting;
    bool dflag = false, yflag = false;
    string hold = "";

    if (argc == 1){
        fprintf(stderr, "Need a file to handle.\n");
        exit(1);
    }

    while ((setting = getopt(argc, argv, "lyD:@:")) != -1){
        switch (setting){
            case 'l':
                yy_flex_debug = true;
                break;
            case 'y':
                yflag = true;
                break;
            case 'D':
                hold = string(optarg);
                dflag = true;
                break;
            case '@':
                set_debugflags(optarg);
                break;
            default:
                break;
        }//switch
    }//while

    char* base = basename(argv[argc-1]);
    char* period = strchr(base, '.');

    //error handling for file types.
    if (period == NULL){
        fprintf(stderr, "Needs to be a file type.\n");
        set_exitstatus(1);
        return get_exitstatus();
    }

    int len = period - base;
    char* filename = strndup(base, len);

    if (yflag == true){;}    //warning supression

    if (strcmp(period+1, "oc")){
        fprintf(stderr, "Must be type oc.\n");
        set_exitstatus(1);
        return get_exitstatus();
    }

    //make output files and fill command string
    string stringTblFile_name = string(filename) + ".str";
    ofstream stringTableFile;
    stringTableFile.open(stringTblFile_name.c_str());

    string tokOutFile_name = string(filename) + ".tok";
    tokenFile = fopen(tokOutFile_name.c_str(), "w");

    string command = "";

    //check for "-D" flag
    if (dflag){                                 
       command = CPP + " -D " + hold;
    }else{                              //Is this else necessary?
        command = CPP + " " + base;
    }
    //command = CPP + " " + base;

    //correct way of openning the pipe
    yyin = popen(command.c_str(), "r");
    if (yyin == NULL){
        syserrprintf(command.c_str());
        set_exitstatus(1);
        return get_exitstatus();
    }

    //tokens
    while(1){
        unsigned token = yylex();
        if (token == YYEOF) break;
        intern_stringset(get_yytname(token));
    }

    //close pipe
    if (pclose(yyin) != 0){
        set_exitstatus (1);
        return get_exitstatus();
    }

    yylex_destroy();
    dump_stringset(stringTableFile);
    stringTableFile.close();
    fclose(tokenFile);
    return get_exitstatus();
}//main
