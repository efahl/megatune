//------------------------------------------------------------------------------
//--  Copyright (c) 2005,2006 by Eric Fahlgren, All Rights Reserved.          --
//--                                                                          --
//--  This program is free software; you can redistribute it and/or           --
//--  modify it under the terms of the GNU General Public License             --
//--  as published by the Free Software Foundation; either version            --
//--  2 of the License, or (at your option) any later version.                --
//--  See http://www.gnu.org/licenses/gpl.txt                                 --
//------------------------------------------------------------------------------

#ifndef MINIDOM_H
#define MINIDOM_H 1

#if _MSC_VER > 1000
#  pragma once
#endif // _MSC_VER > 1000

#include <assert.h>

//------------------------------------------------------------------------------

#pragma warning(disable:4786)
#include <vector>
#include <map>

#define XML_HEADER "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>"

namespace xml {
   class domElement;

   typedef std::vector<domElement *>  domElementList;
   typedef domElementList::iterator   domElementIterator;
   typedef std::map<CString, CString> attributeMap;
   typedef attributeMap::iterator     attributeMapIterator;

   class domElement {
      public:
         domElement(const CString tag);

         void        addAttribute(CString name, CString value);
         void        addContents (const CString contents);
         domElement *addChild    (const CString tag);

         void cleanContents();
         int  formatContents(int indent, bool forceIndent=false);

         CString        &tag       ();
         attributeMap   &attributes();
         domElementList &children  ();
         CString        &contents  ();

         CString         attribute (CString name, CString defaultValue="");

      private:
         CString        _tag;
         attributeMap   _attributes;
         domElementList _children;
         CString        _contents;
   };

   class DOM {
      public:
         DOM();

         void read (CString fileName, FILE *f=NULL);
         void write(CString fileName);
         void write(FILE *f, bool writeHeader=true);

         domElement *topElement(CString defaultTag="");

      private:
         domElement *_root;
         CString     _fileName;

         FILE *_input;
         int   _iBuf;
         int   _nBuf;
         char  _buffer[1024];
         char  _putback;
         int   _lineNo;

         void error(const char *errorMsg);

         std::vector<domElement *> _elementStack;

         void putBack(char c);
         char getChar();
         CString getToken();
         void parseComment(CString &token);
         void parseElement(CString &token);
         void checkHeader (CString  header);

         int _indentAmount;
         void writeElement(FILE *f, domElement *d, int indent);
   };
};

//------------------------------------------------------------------------------
#endif
