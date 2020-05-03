/*
 * Copyright 2009 - 2010 Kevin Ackley (kackley@gwi.net)
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include "NodeImpl.h"

namespace e57
{

   class StructureNodeImpl : public NodeImpl
   {
   public:
      StructureNodeImpl( ImageFileImplWeakPtr destImageFile );
      ~StructureNodeImpl() override = default;

      NodeType type() const override;
      bool isTypeEquivalent( NodeImplSharedPtr ni ) override;
      bool isDefined( const ustring &pathName ) override;
      void setAttachedRecursive() override;

      virtual int64_t childCount() const;

      virtual NodeImplSharedPtr get( int64_t index );
      NodeImplSharedPtr get( const ustring &pathName ) override;

      virtual void set( int64_t index, NodeImplSharedPtr ni );
      void set( const ustring &pathName, NodeImplSharedPtr ni, bool autoPathCreate = false ) override;
      void set( const StringList &fields, unsigned level, NodeImplSharedPtr ni, bool autoPathCreate = false ) override;
      virtual void append( NodeImplSharedPtr ni );

      void checkLeavesInSet( const StringSet &pathNames, NodeImplSharedPtr origin ) override;

      void writeXml( ImageFileImplSharedPtr imf, CheckedFile &cf, int indent,
                     const char *forcedFieldName = nullptr ) override;

#ifdef E57_DEBUG
      void dump( int indent = 0, std::ostream &os = std::cout ) const override;
#endif

   protected:
      friend class CompressedVectorReaderImpl;
      NodeImplSharedPtr lookup( const ustring &pathName ) override;

      std::vector<NodeImplSharedPtr> children_;
   };
}
