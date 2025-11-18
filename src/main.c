#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
    printf("Usage: %s -n -f <database file>\n", argv[0]);
    printf("\t-n - create new database file\n");
    printf("\t-f - (required) path to database file\n");
}

int main(int argc, char *argv[]) {

    int c;
    bool newfile = false;
    char *filepath = NULL;

    int dbfd = -1;
    struct dbheader_t *dbhd;

    while ((c = getopt(argc, argv, "nf:")) != -1) {
        switch (c) {
        case 'n':
            newfile = true;
            break;
        case 'f':
            filepath = optarg;
            break;
        case '?':
            printf("Unkown argument -%c\n", c);
        default:
            return -1;
        }
    }

    if (filepath == NULL) {
        printf("filepath is a required arguemnt\n");
        print_usage(argv);
        return 0;
    }

    if (newfile) {
        dbfd = create_db_file(filepath);
        if (dbfd == STATUS_ERROR) {
            printf("Unable to create database file \n");
            return -1;
        }
        if (create_db_header(&dbhd) == STATUS_ERROR) {
            printf("Error: Could not create new database header\n");
            return -1;
        }
    } else {
        dbfd = open_db_file(filepath);
        if (dbfd == STATUS_ERROR) {
            printf("Unable to open database file\n");
            return -1;
        }
        if (validate_db_header(dbfd, &dbhd)) {
            printf("Error: Could not read new database header\n");
            return -1;
        }
    }


    return 0;
}
