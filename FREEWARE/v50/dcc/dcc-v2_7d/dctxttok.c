/* DCTXTTOK.C */

#include "dctxttok.h"

#define VerifSize(x, y)	(NbEltGen(x, (y)0) == __extent(y) + 1)

const Tstring txtAddTok[/*~ IndexType TkAdd */] = {"+", "-", NULL};
/*~ zif ! VerifSize(txtAddTok, TkAdd) "Array 'txtAddTok': bad initialization" */
const Tstring txtAsgnTok[/*~IndexType TkAsgn */] = {"=", "|=", "^=", "&=",
				    "<<=", ">>=", "+=", "-=", "*=", "/=", "%="};
/*~ zif ! VerifSize(txtAsgnTok, TkAsgn)
				     "Array 'txtAsgnTok': bad initialization" */
const Tstring txtCmpTok[/*~ IndexType TkCmp */] = {"==", "!=", ">", ">=", "<=",
									   "<"};
/*~ zif ! VerifSize(txtCmpTok, TkCmp) "Array 'txtCmpTok': bad initialization" */
const Tstring txtIncTok[/*~ IndexType TkInc */] = {"++", "--"};
/*~ zif ! VerifSize(txtIncTok, TkInc) "Array 'txtIncTok': bad initialization" */
const Tstring txtMulTok[/*~ IndexType TkMul */] = {"*", "/", "%"};
/*~ zif ! VerifSize(txtMulTok, TkMul) "Array 'txtMulTok': bad initialization" */
const Tstring txtShiTok[/*~ IndexType TkShi */] = {"<<", ">>"};
/*~ zif ! VerifSize(txtShiTok, TkShi) "Array 'txtShiTok': bad initialization" */

/* End DCTXTTOK.C */
