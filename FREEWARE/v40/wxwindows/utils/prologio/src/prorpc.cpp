/*
 * File:	prorpc.cc
 * Purpose:	Prolog-style Remote Procedure Call interface
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "%W% %G%"; */

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx_defs.h>
#include <wx_utils.h>
#endif

#if USE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif

#ifdef wx_msw
#include <strstrea.h>
#else
#include <strstream.h>
#endif
#include <ctype.h>
#include "prorpc.h"
#include <stdarg.h>

extern char *wxDefaultIPCBuffer;
extern int wxDefaultIPCBufferSize;

PrologDatabase rpcPrologDatabase;

IMPLEMENT_CLASS(rpcConnection, wxConnection)
IMPLEMENT_CLASS(rpcServer, wxServer)
IMPLEMENT_CLASS(rpcClient, wxClient)
IMPLEMENT_CLASS(rpcCallTableEntry, wxObject)
IMPLEMENT_CLASS(rpcCallTable, wxList)

rpcServer::rpcServer(void)
{
}

wxConnection *rpcServer::OnAcceptConnection(char *topic)
{
  if (strcmp(topic, "RPC") == 0)
    return new rpcConnection;
  else return NULL;
}


rpcClient::rpcClient(void)
{
}

rpcConnection::rpcConnection(char *buffer, int size):wxConnection(buffer, size)
{
  temp_expr = NULL;
  ReturnValue = NULL;
  call_table = NULL;
}

rpcConnection::rpcConnection(void):wxConnection()
{
  temp_expr = NULL;
  ReturnValue = NULL;
}

rpcConnection::~rpcConnection(void)
{
  if (temp_expr)
    delete temp_expr;
}

void rpcConnection::RegisterCallTable(rpcCallTable *table)
{
  call_table = table;
}

wxConnection *rpcClient::OnMakeConnection(void)
{
  return new rpcConnection;
}

Bool rpcConnection::OnExecute(char *topic, char *data, int size, int format)
{
  if (temp_expr)
  {
    delete temp_expr;
    temp_expr = NULL;
  }

  if (strcmp(topic, "RPC") == 0)
  {
    rpcPrologDatabase.Clear();
    rpcPrologDatabase.ReadPrologFromString(data);
    wxNode *first = rpcPrologDatabase.First();
    if (first)
    {
      PrologExpr *expr = (PrologExpr *)first->Data();
      if (call_table)
        temp_expr = call_table->FindAndExecuteCall(this, expr);
      else temp_expr = OnCall(expr);

      if (temp_expr)
      {
        ostrstream stream(wxDefaultIPCBuffer, wxDefaultIPCBufferSize);
        temp_expr->WritePrologClause(stream);
        stream << (char)0;
        stream.flush();
        ReturnValue = copystring(wxDefaultIPCBuffer);
      }
      else ReturnValue = copystring("error(\"Undefined function\", 1).");
    }
  }
  return TRUE;
}

char *rpcConnection::OnRequest(char *topic, char *item, int *size, int format)
{
  if ((strcmp(topic, "RPC") == 0) && ReturnValue)
  {
    char *s = ReturnValue;
    ReturnValue = NULL;
    return s;
  }
  else return NULL;
}

PrologExpr *rpcConnection::Call(PrologExpr *expr)
{
  ostrstream stream(wxDefaultIPCBuffer, wxDefaultIPCBufferSize);
  expr->WritePrologClause(stream);
  stream << (char)0;
  stream.flush();

  char *ptr = copystring(wxDefaultIPCBuffer);
  Execute(ptr);
  delete[] ptr; // SHOULD THIS BE DELETED?
  int size;
  char *data = Request("RETURN", &size);

  if (data)
  {
    rpcPrologDatabase.Clear();
    rpcPrologDatabase.ReadPrologFromString(data);

    wxNode *first = rpcPrologDatabase.First();
    if (first)
    {
      PrologExpr *expr = (PrologExpr *)first->Data();
      return expr;
    }
  }
  return NULL;
}

PrologExpr *rpcConnection::OnCall(PrologExpr *expr)
{
  return NULL;
}

Bool rpcConnection::OnPoke(char *topic, char *item, char *data, int size, int format)
{
  return FALSE;
}

Bool rpcConnection::OnStartAdvise(char *topic, char *item)
{
  return TRUE;
}

Bool rpcConnection::OnEndAdvise(char *topic, char *item)
{
  return TRUE;
}

Bool rpcConnection::OnAdvise(char *topic, char *item, char *data, int size, int format)
{
  return FALSE;
}


/* Call table implementation
 *
 */

rpcCallTableEntry::rpcCallTableEntry(char *the_functor, rpcCall the_fun, wxList *the_types)
{
  functor = the_functor;
  fun = the_fun;
  types = the_types;
}

rpcCallTableEntry::~rpcCallTableEntry(void)
{
  if (functor)
    delete functor;

  if (types)
    delete types;
}

rpcCallTable::rpcCallTable(void):wxList(wxKEY_STRING)
{
}

rpcCallTable::~rpcCallTable(void)
{
}

void rpcCallTable::AddCall(char *functor, rpcCall the_fun, ...)
{
  wxList *types = new wxList(wxKEY_STRING);
  va_list ap;

  va_start(ap, the_fun);

  for (;;)
  {
    PrologType type = va_arg(ap, PrologType);
    if (((int)type) == 0)
      break;
    else
    {
      int type_int = (int)type;
      types->Append((wxObject *)type_int);
    }
  }
  va_end(ap);

  rpcCallTableEntry *entry = 
    new rpcCallTableEntry(copystring(functor), the_fun, types);

  Append(functor, entry);
}

PrologExpr *rpcCallTable::FindAndExecuteCall(
          wxConnection *connection, PrologExpr *clause)
{
  if (clause && (clause->Type() == PrologList))
  {
      PrologExpr *first_expr = clause->value.first;
      if (first_expr && (first_expr->Type() == PrologWord))
      {
        char *functor = first_expr->WordValue();
        wxNode *found = Find(functor);

        if (!found)
	{
          return wxMakeCall("error", wxMakeString("Unrecognised command"),
                                     wxMakeInteger(rpcERROR_NODEF),
                                     NULL);
	}
        rpcCallTableEntry *entry = (rpcCallTableEntry *)found->Data();
        char *type_error = wxCheckClauseTypes(clause, entry->types);
        if (type_error)
	{
          PrologExpr *ret = wxMakeCall("error", 
                                     wxMakeInteger(rpcERROR_BADARGTYPE),
                                     wxMakeString(type_error), NULL);
          delete type_error;
          return ret;
	}
        return (PrologExpr *)(*(entry->fun))(connection, clause);
      }
      else return wxMakeCall("error", wxMakeString("Ill-formed command"),
                                     wxMakeInteger(rpcERROR_ILLFORMED),
                                     NULL);
  }
  else return wxMakeCall("error", wxMakeString("Ill-formed command"),
                                   wxMakeInteger(rpcERROR_ILLFORMED), NULL);
}

