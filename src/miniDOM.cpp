//------------------------------------------------------------------------------
//--  Copyright (c) 2005,2006 by Eric Fahlgren, All Rights Reserved.          --
//--                                                                          --
//--  This program is free software; you can redistribute it and/or           --
//--  modify it under the terms of the GNU General Public License             --
//--  as published by the Free Software Foundation; either version            --
//--  2 of the License, or (at your option) any later version.                --
//--  See http://www.gnu.org/licenses/gpl.txt                                 --
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "miniDOM.h"

//------------------------------------------------------------------------------

static CString encode(CString s)
{
   s.Replace("&",  "&amp;" );
   s.Replace(">",  "&gt;"  );
   s.Replace("<",  "&lt;"  );
   s.Replace("\"", "&quot;");
   return s;
}

static CString decode(CString s)
{
   s.Replace("&gt;",   ">" );
   s.Replace("&lt;",   "<" );
   s.Replace("&quot;", "\"");
   s.Replace("&amp;",  "&" );
   return s;
}

//------------------------------------------------------------------------------

xml::domElement::domElement(CString tag)
 : _tag(tag)
{
}

void xml::domElement::addAttribute(CString name, CString value)
{
   _attributes[name] = value;
}

void xml::domElement::addContents(const CString contents)
{
   _contents += contents;
}

xml::domElement *xml::domElement::addChild(const CString tag)
{
   domElement *child = new domElement(tag);
   _children.push_back(child);
   return child;
}

void xml::domElement::cleanContents()
{
   _contents.TrimLeft (); 
   _contents.TrimRight(); 
   _contents.Replace  ("\r", "");
   _contents.Replace  ('\t', ' ');
   while (_contents.Replace("  ", " ")) ;
   _contents.Replace  ("\n ", "\n");
   while (_contents.Replace("\n\n", "\n")) ;
}

int xml::domElement::formatContents(int indent, bool forceIndent)
{
   CString pad;
   pad.Format("\n%*s", indent, "");
   int nLines = _contents.Replace("\n", pad);
   if (nLines > 0) {
      pad.Delete(0);
      _contents.Insert (0, pad);
   }
   return nLines;
}

CString             &xml::domElement::tag       () { return _tag;        }
xml::attributeMap   &xml::domElement::attributes() { return _attributes; }
xml::domElementList &xml::domElement::children  () { return _children;   }
CString             &xml::domElement::contents  () { return _contents;   }

CString xml::domElement::attribute(CString name, CString defaultValue)
{
   attributeMapIterator a = _attributes.find(name);
   if (a == _attributes.end())
      return defaultValue;
   else
      return ((*a).second);
}

//==============================================================================

xml::DOM::DOM()
 : _root        (new xml::domElement("root"))
 , _fileName    ("")
 , _lineNo      (0)
 , _indentAmount(3)
{
}

//------------------------------------------------------------------------------

xml::domElement *xml::DOM::topElement(CString defaultTag)
{
   if (_root->children().size() == 0) _root->addChild(defaultTag);
   return _root->children()[0];
}

//------------------------------------------------------------------------------

void xml::DOM::error(const char *errorMsg)
{
   CString msg;
   msg.Format("ERROR: %s:%d: %s", LPCTSTR(_fileName), _lineNo, errorMsg);
   throw msg;
}

//------------------------------------------------------------------------------

void xml::DOM::putBack(char c) { _putback = c; }

char xml::DOM::getChar()
{
   if (_putback) {
      char c = _putback;
      _putback = 0;
      return c;
   }

   if (_iBuf >= _nBuf) {
      _nBuf = fread(_buffer, sizeof(char), sizeof(_buffer), _input);
      _iBuf = 0;
   }
   if (_iBuf >= _nBuf) return 0;
   if (_buffer[_iBuf] == '\n') _lineNo++;
   return _buffer[_iBuf++];
}

//------------------------------------------------------------------------------

CString xml::DOM::getToken()
{
   char    c     = getChar();
   CString token;
   if (c) token = c;
   if (c == '<') {
      do {
         c = getChar();
         if (c == 0) break;
         token += c;
      } while (c != '>');
   }
   else {
      do {
         c = getChar();
         if (c ==  0 ) break;
         if (c != '<') token += c;
      } while (c != '<');
      putBack(c);
   }
   return token;
}

//------------------------------------------------------------------------------

void xml::DOM::parseComment(CString &token)
{
   if (token.Find("<!--") != 0) error("Malformed comment");
   if (token.Find("-->") != token.GetLength()-3) error("Malformed/unclosed comment");
}

//------------------------------------------------------------------------------

void xml::DOM::parseElement(CString &token)
{
   if (token[0] != '<') error("Expected '<' in element");
   token.Delete(0);
   bool close = token[0] == '/';
   if (close) token.Delete(0);
   int endItem = token.FindOneOf(" />");
   CString tag = token.Left(endItem);
   token.Delete(0, endItem);

   if (!close) {
      _elementStack.push_back(_elementStack.back()->addChild(tag));

      do {
         while (token[0] == ' ' || token[0] == '\n') token.Delete(0);
         if (token[0] != '/' && token[0] != '>') {
            endItem = token.Find('=');
            if (endItem == -1) error("Unterminated attribute");
            CString name = token.Left(endItem);
            token.Delete(0, endItem+1);

            if (token[0] != '"') error("Attribute value missing opening quote");
            token.Delete(0);
            endItem = token.Find('"');
            if (endItem == -1) error("Attribute value missing closing quote");
            CString value = token.Left(endItem);
            token.Delete(0, endItem+1);
            _elementStack.back()->addAttribute(name, decode(value));
         }
      } while (token[0] != '/' && token[0] != '>');

      if (token[0] == '/') {
         close = true;
         token.Delete(0);
      }
      if (token[0] != '>') error("Element attributes mangled");
   }

   if (close) {
      if (_elementStack.back()->tag() != tag) error("Mismatched closing element");
      _elementStack.pop_back();
   }
}

//------------------------------------------------------------------------------

void xml::DOM::checkHeader(CString header)
{
#define XML_HEADER "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>"
   CString expectedHeader = XML_HEADER;
   int start = expectedHeader.Find("encoding=\"") + strlen("encoding=\"");
   int end   = expectedHeader.Find('"', start);
   expectedHeader.Delete(start, end-start);
   start = header.Find("encoding=\"") + strlen("encoding=\"");
   end   = header.Find('"', start);
   header.Delete(start, end-start);
   if (header != expectedHeader) error("Malformed XML header");
}

//------------------------------------------------------------------------------

void xml::DOM::read(CString fileName, FILE *f)
{
   _fileName = fileName;
   if (f == NULL) f = fopen(LPCTSTR(_fileName), "r");
   _input = f;
   if (_input == NULL) error("Couldn't open file.");
      _elementStack.push_back(_root);
      _lineNo  = 1;
      _nBuf    = 0;
      _iBuf    = 1;
      _putback = 0;

      CString token = getToken();
      checkHeader(token);
      token = getToken();
      do {
         if (token[0] != '<')
            _elementStack.back()->addContents(decode(token));
         else {
            if (token[1] == '!')
               parseComment(token);
            else
               parseElement(token);
         }
         token = getToken();
      } while (!token.IsEmpty());
   fclose(_input);
}

//------------------------------------------------------------------------------

void xml::DOM::writeElement(FILE *f, xml::domElement *d, int indent)
{
   CString pad;
   pad.Format("%*s", indent, "");

   fprintf(f, "%s<%s", LPCTSTR(pad), LPCTSTR(d->tag()));
   attributeMap   &attributes = d->attributes();
   for (attributeMapIterator i = attributes.begin(); i != attributes.end(); i++) {
      fprintf(f, " %s=\"%s\"", LPCTSTR((*i).first), LPCTSTR(encode((*i).second)));
   }

   d->cleanContents();

   bool hasContents = d->contents().IsEmpty() == 0;
   bool hasChildren = d->children().size()    != 0;

   if (!hasContents && !hasChildren) {
      fprintf(f, " />\n");
   }
   else {
      fprintf(f, ">");
      bool multiLine = false;
      if (hasContents) {
         multiLine = d->formatContents(indent+_indentAmount, hasChildren) > 0;
         if (multiLine)
            fprintf(f, "\n%s\n", LPCTSTR(encode(d->contents())));
         else {
            fprintf(f, "%s", LPCTSTR(encode(d->contents())));
            pad.Empty();
         }
      }

      if (hasChildren) {
         if (!multiLine) fprintf(f, "\n");
         domElementList &children = d->children();
         for (int iChild = 0; iChild < children.size(); iChild++) {
            writeElement(f, children[iChild], indent+_indentAmount);
         }
      }
      fprintf(f, "%s</%s>\n", LPCTSTR(pad), LPCTSTR(d->tag()));
   }
}

//------------------------------------------------------------------------------

void xml::DOM::write(FILE *f, bool writeHeader)
{
   if (writeHeader) fprintf(f, "%s\n", XML_HEADER);
   writeElement(f, topElement(), 0);
}

void xml::DOM::write(CString fileName)
{
   _fileName = fileName;
   FILE *f = fopen(LPCTSTR(_fileName), "w");
   if (f == NULL) error("Couldn't open file.");
   write(f);
   fclose(f);
}

//------------------------------------------------------------------------------
