/* $Id$ */
typedef enum { intCon, floatCon, typeOpr, stringCon,cmdId,vId } nodeEnum;
/* constants */
typedef struct {
int value; /* value of constant */
} conNodeType;
/* identifiers */
typedef struct {
char *s; /* subscript to sym array */
} stringNodeType;
typedef struct {
float f; /* subscript to sym array */
} fidNodeType;
/* operators */
typedef struct {
int i; /* subscript to sym array */
} idNodeType;
/* operators */
typedef struct {
int oper; /* operator */
int nops; /* number of operands */
struct nodeTypeTag *op[1]; /* operands (expandable) */
} oprNodeType;
typedef struct nodeTypeTag {
nodeEnum type; /* type of node */
int typeHint; /* WARNING : THIS COULD BE HARMFUL, DUE TO C HACKS.. */
/* union must be last entry in nodeType */
/* because operNodeType may dynamically increase */
union {
conNodeType con; /* constants */
stringNodeType scon; /* string constant */
fidNodeType fid; /* identifiers */
idNodeType id; /* identifiers */
oprNodeType opr; /* operators */
};
} nodeType;
extern int sym[26];

