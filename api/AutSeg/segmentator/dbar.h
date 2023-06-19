#ifndef DBAR_H
#define DBAR_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <libpq-fe.h>
#include "sys/stat.h"
#include "al.h"

const char *conninfo = NULL;
PGconn *conn;

void DB_LABELDATA();

#endif // DBAR_H
