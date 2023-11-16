#include <stdio.h>
#include <postgres.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>

#include "utils/builtins.h"
#include "smallchesslib.h"

PG_MODULE_MAGIC;

/**** Chessgame structure, needs to be array to save data *****/

typedef struct Chessgame {
    char san[8]; // Max Array to store moves
} Chessgame;

/*****************************************************************************/

#define DatumGetChessgameP(X)  ((Chessgame *) DatumGetPointer(X))
#define ChessgamePGetDatum(X)  PointerGetDatum(X)
#define PG_GETARG_CHESSGAME_P(n) DatumGetChessgameP(PG_GETARG_DATUM(n))
#define PG_RETURN_CHESSGAME_P(x) return ChessgamePGetDatum(x)

/*****************************************************************************/

static SCL_Record *
chessgame_make(const char *san)
{
  SCL_Record *record = palloc0(sizeof(Chessgame));
  SCL_recordFromPGN(record, san);
  return record;
}


/*****************************************************************************/

char str[4096];

void putCharStr(char c)
{
    char* s = str;

    while (*s != 0)
        s++;

    *s = c;
    *(s + 1) = 0;
}

/*****************************************************************************/



static char *
chessgame_to_str(const SCL_Record *record)
{
  char* s = str;
  *s = 0;
  SCL_printPGN(record, putCharStr, 0);
  return str;
}

/*****************************************************************************/

PG_FUNCTION_INFO_V1(chessgame_in);
Datum
chessgame_in(PG_FUNCTION_ARGS)
{
  char *str = PG_GETARG_CSTRING(0);
  PG_RETURN_CHESSGAME_P(chessgame_make(&str));
}

PG_FUNCTION_INFO_V1(chessgame_out);
Datum
chessgame_out(PG_FUNCTION_ARGS)
{
  SCL_Record *record = PG_GETARG_CHESSGAME_P(0);
  char *result = chessgame_to_str(record);
  PG_FREE_IF_COPY(record, 0);
  PG_RETURN_CSTRING(result);
}

PG_FUNCTION_INFO_V1(chessgame_recv);
Datum
chessgame_recv(PG_FUNCTION_ARGS)
{
  StringInfo  buf = (StringInfo) PG_GETARG_POINTER(0);
  SCL_Record *record = (SCL_Record *) palloc(sizeof(SCL_Record));
  record = chessgame_make(pq_getmsgstring(buf));
  PG_RETURN_CHESSGAME_P(record);
} 

PG_FUNCTION_INFO_V1(chessgame_send);
Datum
chessgame_send(PG_FUNCTION_ARGS)
{
  SCL_Record *record = PG_GETARG_CHESSGAME_P(0);
  StringInfoData buf;
  pq_begintypsend(&buf);
  pq_sendtext(&buf, record);
  PG_FREE_IF_COPY(record, 0);
  PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

PG_FUNCTION_INFO_V1(chessgame_cast_from_text);
Datum
chessgame_cast_from_text(PG_FUNCTION_ARGS)
{
  text *txt = PG_GETARG_TEXT_P(0);
  char *str = DatumGetCString(DirectFunctionCall1(textout,
               PointerGetDatum(txt)));
  PG_RETURN_CHESSGAME_P(chessgame_make(&str));
}

PG_FUNCTION_INFO_V1(chessgame_cast_to_text);
Datum
chessgame_cast_to_text(PG_FUNCTION_ARGS)
{
  SCL_Record *record  = PG_GETARG_CHESSGAME_P(0);
  text *out = (text *)DirectFunctionCall1(textin,
            PointerGetDatum(chessgame_to_str(record)));
  PG_FREE_IF_COPY(record, 0);
  PG_RETURN_TEXT_P(out);
}
