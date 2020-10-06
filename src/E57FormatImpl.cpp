/*
 * E57FormatImpl.cpp - implementation of private functions of E57 format.
 *
 * Original work Copyright 2009 - 2010 Kevin Ackley (kackley@gwi.net)
 * Modified work Copyright 2018 - 2020 Andy Maloney <asmaloney@gmail.com>
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

#include <cmath>

#include "E57FormatImpl.h"

#include "CheckedFile.h"
#include "ImageFileImpl.h"
#include "SectionHeaders.h"

using namespace e57;

IntegerNodeImpl::IntegerNodeImpl( ImageFileImplWeakPtr destImageFile, int64_t value, int64_t minimum,
                                  int64_t maximum ) :
   NodeImpl( destImageFile ),
   value_( value ), minimum_( minimum ), maximum_( maximum )
{
   // don't checkImageFileOpen, NodeImpl() will do it

   /// Enforce the given bounds
   if ( value < minimum || maximum < value )
   {
      throw E57_EXCEPTION2( E57_ERROR_VALUE_OUT_OF_BOUNDS,
                            "this->pathName=" + this->pathName() + " value=" + toString( value ) +
                               " minimum=" + toString( minimum ) + " maximum=" + toString( maximum ) );
   }
}

bool IntegerNodeImpl::isTypeEquivalent( NodeImplSharedPtr ni )
{
   // don't checkImageFileOpen

   /// Same node type?
   if ( ni->type() != E57_INTEGER )
   {
      return ( false );
   }

   /// Downcast to shared_ptr<IntegerNodeImpl>
   std::shared_ptr<IntegerNodeImpl> ii( std::static_pointer_cast<IntegerNodeImpl>( ni ) );

   /// minimum must match
   if ( minimum_ != ii->minimum_ )
   {
      return ( false );
   }

   /// maximum must match
   if ( maximum_ != ii->maximum_ )
   {
      return ( false );
   }

   /// ignore value_, doesn't have to match

   /// Types match
   return ( true );
}

bool IntegerNodeImpl::isDefined( const ustring &pathName )
{
   // don't checkImageFileOpen

   /// We have no sub-structure, so if path not empty return false
   return pathName.empty();
}

int64_t IntegerNodeImpl::value()
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
   return ( value_ );
}

int64_t IntegerNodeImpl::minimum()
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
   return ( minimum_ );
}

int64_t IntegerNodeImpl::maximum()
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
   return ( maximum_ );
}

void IntegerNodeImpl::checkLeavesInSet( const StringSet &pathNames, NodeImplSharedPtr origin )
{
   // don't checkImageFileOpen

   /// We are a leaf node, so verify that we are listed in set.
   if ( pathNames.find( relativePathName( origin ) ) == pathNames.end() )
   {
      throw E57_EXCEPTION2( E57_ERROR_NO_BUFFER_FOR_ELEMENT, "this->pathName=" + this->pathName() );
   }
}

void IntegerNodeImpl::writeXml( ImageFileImplSharedPtr /*imf???*/, CheckedFile &cf, int indent,
                                const char *forcedFieldName )
{
   // don't checkImageFileOpen

   ustring fieldName;
   if ( forcedFieldName != nullptr )
   {
      fieldName = forcedFieldName;
   }
   else
   {
      fieldName = elementName_;
   }

   cf << space( indent ) << "<" << fieldName << " type=\"Integer\"";

   /// Don't need to write if are default values
   if ( minimum_ != E57_INT64_MIN )
   {
      cf << " minimum=\"" << minimum_ << "\"";
   }
   if ( maximum_ != E57_INT64_MAX )
   {
      cf << " maximum=\"" << maximum_ << "\"";
   }

   /// Write value as child text, unless it is the default value
   if ( value_ != 0 )
   {
      cf << ">" << value_ << "</" << fieldName << ">\n";
   }
   else
   {
      cf << "/>\n";
   }
}

#ifdef E57_DEBUG
void IntegerNodeImpl::dump( int indent, std::ostream &os ) const
{
   // don't checkImageFileOpen
   os << space( indent ) << "type:        Integer"
      << " (" << type() << ")" << std::endl;
   NodeImpl::dump( indent, os );
   os << space( indent ) << "value:       " << value_ << std::endl;
   os << space( indent ) << "minimum:     " << minimum_ << std::endl;
   os << space( indent ) << "maximum:     " << maximum_ << std::endl;
}
#endif

//=============================================================================
ScaledIntegerNodeImpl::ScaledIntegerNodeImpl( ImageFileImplWeakPtr destImageFile, int64_t rawValue, int64_t minimum,
                                              int64_t maximum, double scale, double offset ) :
   NodeImpl( destImageFile ),
   value_( rawValue ), minimum_( minimum ), maximum_( maximum ), scale_( scale ), offset_( offset )
{
   // don't checkImageFileOpen, NodeImpl() will do it

   /// Enforce the given bounds on raw value
   if ( rawValue < minimum || maximum < rawValue )
   {
      throw E57_EXCEPTION2( E57_ERROR_VALUE_OUT_OF_BOUNDS,
                            "this->pathName=" + this->pathName() + " rawValue=" + toString( rawValue ) +
                               " minimum=" + toString( minimum ) + " maximum=" + toString( maximum ) );
   }
}
//=============================================================================
ScaledIntegerNodeImpl::ScaledIntegerNodeImpl( ImageFileImplWeakPtr destImageFile, double scaledValue,
                                              double scaledMinimum, double scaledMaximum, double scale,
                                              double offset ) :
   NodeImpl( destImageFile ),
   value_( static_cast<int64_t>( floor( ( scaledValue - offset ) / scale + .5 ) ) ),
   minimum_( static_cast<int64_t>( floor( ( scaledMinimum - offset ) / scale + .5 ) ) ),
   maximum_( static_cast<int64_t>( floor( ( scaledMaximum - offset ) / scale + .5 ) ) ), scale_( scale ),
   offset_( offset )
{
   // don't checkImageFileOpen, NodeImpl() will do it

   /// Enforce the given bounds on raw value
   if ( scaledValue < scaledMinimum || scaledMaximum < scaledValue )
   {
      throw E57_EXCEPTION2( E57_ERROR_VALUE_OUT_OF_BOUNDS, "this->pathName=" + this->pathName() +
                                                              " scaledValue=" + toString( scaledValue ) +
                                                              " scaledMinimum=" + toString( scaledMinimum ) +
                                                              " scaledMaximum=" + toString( scaledMaximum ) );
   }
}

bool ScaledIntegerNodeImpl::isTypeEquivalent( NodeImplSharedPtr ni )
{
   // don't checkImageFileOpen

   /// Same node type?
   if ( ni->type() != E57_SCALED_INTEGER )
   {
      return ( false );
   }

   /// Downcast to shared_ptr<ScaledIntegerNodeImpl>
   std::shared_ptr<ScaledIntegerNodeImpl> ii( std::static_pointer_cast<ScaledIntegerNodeImpl>( ni ) );

   /// minimum must match
   if ( minimum_ != ii->minimum_ )
   {
      return ( false );
   }

   /// maximum must match
   if ( maximum_ != ii->maximum_ )
   {
      return ( false );
   }

   /// scale must match
   if ( scale_ != ii->scale_ )
   {
      return ( false );
   }

   /// offset must match
   if ( offset_ != ii->offset_ )
   {
      return ( false );
   }

   /// ignore value_, doesn't have to match

   /// Types match
   return ( true );
}

bool ScaledIntegerNodeImpl::isDefined( const ustring &pathName )
{
   // don't checkImageFileOpen

   /// We have no sub-structure, so if path not empty return false
   return pathName.empty();
}

int64_t ScaledIntegerNodeImpl::rawValue()
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
   return ( value_ );
}

double ScaledIntegerNodeImpl::scaledValue()
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
   return ( value_ * scale_ + offset_ );
}

int64_t ScaledIntegerNodeImpl::minimum()
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
   return ( minimum_ );
}
double ScaledIntegerNodeImpl::scaledMinimum()
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
   return ( minimum_ * scale_ + offset_ );
}

int64_t ScaledIntegerNodeImpl::maximum()
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
   return ( maximum_ );
}
double ScaledIntegerNodeImpl::scaledMaximum()
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
   return ( maximum_ * scale_ + offset_ );
}

double ScaledIntegerNodeImpl::scale()
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
   return ( scale_ );
}

double ScaledIntegerNodeImpl::offset()
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
   return ( offset_ );
}

void ScaledIntegerNodeImpl::checkLeavesInSet( const StringSet &pathNames, NodeImplSharedPtr origin )
{
   // don't checkImageFileOpen

   /// We are a leaf node, so verify that we are listed in set.
   if ( pathNames.find( relativePathName( origin ) ) == pathNames.end() )
   {
      throw E57_EXCEPTION2( E57_ERROR_NO_BUFFER_FOR_ELEMENT, "this->pathName=" + this->pathName() );
   }
}

void ScaledIntegerNodeImpl::writeXml( ImageFileImplSharedPtr /*imf*/, CheckedFile &cf, int indent,
                                      const char *forcedFieldName )
{
   // don't checkImageFileOpen

   ustring fieldName;
   if ( forcedFieldName != nullptr )
   {
      fieldName = forcedFieldName;
   }
   else
   {
      fieldName = elementName_;
   }

   cf << space( indent ) << "<" << fieldName << " type=\"ScaledInteger\"";

   /// Don't need to write if are default values
   if ( minimum_ != E57_INT64_MIN )
   {
      cf << " minimum=\"" << minimum_ << "\"";
   }
   if ( maximum_ != E57_INT64_MAX )
   {
      cf << " maximum=\"" << maximum_ << "\"";
   }
   if ( scale_ != 1.0 )
   {
      cf << " scale=\"" << scale_ << "\"";
   }
   if ( offset_ != 0.0 )
   {
      cf << " offset=\"" << offset_ << "\"";
   }

   /// Write value as child text, unless it is the default value
   if ( value_ != 0 )
   {
      cf << ">" << value_ << "</" << fieldName << ">\n";
   }
   else
   {
      cf << "/>\n";
   }
}

#ifdef E57_DEBUG
void ScaledIntegerNodeImpl::dump( int indent, std::ostream &os ) const
{
   // don't checkImageFileOpen
   os << space( indent ) << "type:        ScaledInteger"
      << " (" << type() << ")" << std::endl;
   NodeImpl::dump( indent, os );
   os << space( indent ) << "rawValue:    " << value_ << std::endl;
   os << space( indent ) << "minimum:     " << minimum_ << std::endl;
   os << space( indent ) << "maximum:     " << maximum_ << std::endl;
   os << space( indent ) << "scale:       " << scale_ << std::endl;
   os << space( indent ) << "offset:      " << offset_ << std::endl;
}
#endif

//=============================================================================

FloatNodeImpl::FloatNodeImpl( ImageFileImplWeakPtr destImageFile, double value, FloatPrecision precision,
                              double minimum, double maximum ) :
   NodeImpl( destImageFile ),
   value_( value ), precision_( precision ), minimum_( minimum ), maximum_( maximum )
{
   // don't checkImageFileOpen, NodeImpl() will do it

   /// Since this ctor also used to construct single precision, and defaults for
   /// minimum/maximum are for double precision, adjust bounds smaller if
   /// single.
   if ( precision_ == E57_SINGLE )
   {
      if ( minimum_ < E57_FLOAT_MIN )
      {
         minimum_ = E57_FLOAT_MIN;
      }
      if ( maximum_ > E57_FLOAT_MAX )
      {
         maximum_ = E57_FLOAT_MAX;
      }
   }

   /// Enforce the given bounds on raw value
   if ( value < minimum || maximum < value )
   {
      throw E57_EXCEPTION2( E57_ERROR_VALUE_OUT_OF_BOUNDS,
                            "this->pathName=" + this->pathName() + " value=" + toString( value ) +
                               " minimum=" + toString( minimum ) + " maximum=" + toString( maximum ) );
   }
}

bool FloatNodeImpl::isTypeEquivalent( NodeImplSharedPtr ni )
{
   // don't checkImageFileOpen

   /// Same node type?
   if ( ni->type() != E57_FLOAT )
   {
      return ( false );
   }

   /// Downcast to shared_ptr<FloatNodeImpl>
   std::shared_ptr<FloatNodeImpl> fi( std::static_pointer_cast<FloatNodeImpl>( ni ) );

   /// precision must match
   if ( precision_ != fi->precision_ )
   {
      return ( false );
   }

   /// minimum must match
   if ( minimum_ != fi->minimum_ )
   {
      return ( false );
   }

   /// maximum must match
   if ( maximum_ != fi->maximum_ )
   {
      return ( false );
   }

   /// ignore value_, doesn't have to match

   /// Types match
   return ( true );
}

bool FloatNodeImpl::isDefined( const ustring &pathName )
{
   // don't checkImageFileOpen

   /// We have no sub-structure, so if path not empty return false
   return pathName.empty();
}

double FloatNodeImpl::value() const
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
   return value_;
}

FloatPrecision FloatNodeImpl::precision() const
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
   return precision_;
}

double FloatNodeImpl::minimum() const
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
   return minimum_;
}

double FloatNodeImpl::maximum() const
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
   return maximum_;
}

void FloatNodeImpl::checkLeavesInSet( const StringSet &pathNames, NodeImplSharedPtr origin )
{
   // don't checkImageFileOpen

   /// We are a leaf node, so verify that we are listed in set (either relative
   /// or absolute form)
   if ( pathNames.find( relativePathName( origin ) ) == pathNames.end() &&
        pathNames.find( pathName() ) == pathNames.end() )
   {
      throw E57_EXCEPTION2( E57_ERROR_NO_BUFFER_FOR_ELEMENT, "this->pathName=" + this->pathName() );
   }
}

void FloatNodeImpl::writeXml( ImageFileImplSharedPtr /*imf*/, CheckedFile &cf, int indent, const char *forcedFieldName )
{
   // don't checkImageFileOpen

   ustring fieldName;
   if ( forcedFieldName != nullptr )
   {
      fieldName = forcedFieldName;
   }
   else
   {
      fieldName = elementName_;
   }

   cf << space( indent ) << "<" << fieldName << " type=\"Float\"";
   if ( precision_ == E57_SINGLE )
   {
      cf << " precision=\"single\"";

      /// Don't need to write if are default values
      if ( minimum_ > E57_FLOAT_MIN )
      {
         cf << " minimum=\"" << static_cast<float>( minimum_ ) << "\"";
      }
      if ( maximum_ < E57_FLOAT_MAX )
      {
         cf << " maximum=\"" << static_cast<float>( maximum_ ) << "\"";
      }

      /// Write value as child text, unless it is the default value
      if ( value_ != 0.0 )
      {
         cf << ">" << static_cast<float>( value_ ) << "</" << fieldName << ">\n";
      }
      else
      {
         cf << "/>\n";
      }
   }
   else
   {
      /// Don't need to write precision="double", because that's the default

      /// Don't need to write if are default values
      if ( minimum_ > E57_DOUBLE_MIN )
      {
         cf << " minimum=\"" << minimum_ << "\"";
      }
      if ( maximum_ < E57_DOUBLE_MAX )
      {
         cf << " maximum=\"" << maximum_ << "\"";
      }

      /// Write value as child text, unless it is the default value
      if ( value_ != 0.0 )
      {
         cf << ">" << value_ << "</" << fieldName << ">\n";
      }
      else
      {
         cf << "/>\n";
      }
   }
}

#ifdef E57_DEBUG
void FloatNodeImpl::dump( int indent, std::ostream &os ) const
{
   // don't checkImageFileOpen
   os << space( indent ) << "type:        Float"
      << " (" << type() << ")" << std::endl;
   NodeImpl::dump( indent, os );
   os << space( indent ) << "precision:   ";
   if ( precision() == E57_SINGLE )
   {
      os << "single" << std::endl;
   }
   else
   {
      os << "double" << std::endl;
   }

   /// Save old stream config
   const std::streamsize oldPrecision = os.precision();
   const std::ios_base::fmtflags oldFlags = os.flags();

   os << space( indent ) << std::scientific << std::setprecision( 17 ) << "value:       " << value_ << std::endl;
   os << space( indent ) << "minimum:     " << minimum_ << std::endl;
   os << space( indent ) << "maximum:     " << maximum_ << std::endl;

   /// Restore old stream config
   os.precision( oldPrecision );
   os.flags( oldFlags );
}
#endif

//=============================================================================

StringNodeImpl::StringNodeImpl( ImageFileImplWeakPtr destImageFile, const ustring &value ) :
   NodeImpl( destImageFile ), value_( value )
{
   // don't checkImageFileOpen, NodeImpl() will do it
}

bool StringNodeImpl::isTypeEquivalent( NodeImplSharedPtr ni )
{
   // don't checkImageFileOpen

   /// Same node type?
   if ( ni->type() != E57_STRING )
   {
      return ( false );
   }

   /// ignore value_, doesn't have to match

   /// Types match
   return ( true );
}

bool StringNodeImpl::isDefined( const ustring &pathName )
{
   // don't checkImageFileOpen

   /// We have no sub-structure, so if path not empty return false
   return pathName.empty();
}

ustring StringNodeImpl::value()
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
   return ( value_ );
}

void StringNodeImpl::checkLeavesInSet( const StringSet &pathNames, NodeImplSharedPtr origin )
{
   // don't checkImageFileOpen

   /// We are a leaf node, so verify that we are listed in set.
   if ( pathNames.find( relativePathName( origin ) ) == pathNames.end() )
   {
      throw E57_EXCEPTION2( E57_ERROR_NO_BUFFER_FOR_ELEMENT, "this->pathName=" + this->pathName() );
   }
}

void StringNodeImpl::writeXml( ImageFileImplSharedPtr /*imf*/, CheckedFile &cf, int indent,
                               const char *forcedFieldName )
{
   // don't checkImageFileOpen

   ustring fieldName;
   if ( forcedFieldName != nullptr )
   {
      fieldName = forcedFieldName;
   }
   else
   {
      fieldName = elementName_;
   }

   cf << space( indent ) << "<" << fieldName << " type=\"String\"";

   /// Write value as child text, unless it is the default value
   if ( value_.empty() )
   {
      cf << "/>\n";
   }
   else
   {
      cf << "><![CDATA[";

      size_t currentPosition = 0;
      size_t len = value_.length();

      /// Loop, searching for occurences of "]]>", which will be split across
      /// two CDATA directives
      while ( currentPosition < len )
      {
         size_t found = value_.find( "]]>", currentPosition );

         if ( found == std::string::npos )
         {
            /// Didn't find any more "]]>", so can send the rest.
            cf << value_.substr( currentPosition );
            break;
         }

         /// Must output in two pieces, first send upto end of "]]"  (don't send
         /// the following ">").
         cf << value_.substr( currentPosition, found - currentPosition + 2 );

         /// Then start a new CDATA
         cf << "]]><![CDATA[";

         /// Keep looping to send the ">" plus the remaining part of the string
         currentPosition = found + 2;
      }
      cf << "]]></" << fieldName << ">\n";
   }
}

#ifdef E57_DEBUG
void StringNodeImpl::dump( int indent, std::ostream &os ) const
{
   os << space( indent ) << "type:        String"
      << " (" << type() << ")" << std::endl;
   NodeImpl::dump( indent, os );
   os << space( indent ) << "value:       '" << value_ << "'" << std::endl;
}
#endif

//=============================================================================

BlobNodeImpl::BlobNodeImpl( ImageFileImplWeakPtr destImageFile, int64_t byteCount ) : NodeImpl( destImageFile )
{
   // don't checkImageFileOpen, NodeImpl() will do it

   ImageFileImplSharedPtr imf( destImageFile );

   /// This what caller thinks blob length is
   blobLogicalLength_ = byteCount;

   /// Round segment length up to multiple of 4 bytes
   binarySectionLogicalLength_ = sizeof( BlobSectionHeader ) + blobLogicalLength_;
   unsigned remainder = binarySectionLogicalLength_ % 4;
   if ( remainder > 0 )
   {
      binarySectionLogicalLength_ += 4 - remainder;
   }

   /// Reserve space for blob in file, extend with zeros since writes will
   /// happen at later time by caller
   binarySectionLogicalStart_ = imf->allocateSpace( binarySectionLogicalLength_, true );

   /// Prepare BlobSectionHeader
   BlobSectionHeader header;
   header.sectionLogicalLength = binarySectionLogicalLength_;
#ifdef E57_MAX_VERBOSE
   header.dump(); //???
#endif

   /// Write header at beginning of section
   imf->file_->seek( binarySectionLogicalStart_ );
   imf->file_->write( reinterpret_cast<char *>( &header ), sizeof( header ) );
}

BlobNodeImpl::BlobNodeImpl( ImageFileImplWeakPtr destImageFile, int64_t fileOffset, int64_t length ) :
   NodeImpl( destImageFile )
{
   /// Init blob object that already exists in E57 file currently reading.

   // don't checkImageFileOpen, NodeImpl() will do it

   ImageFileImplSharedPtr imf( destImageFile );

   /// Init state from values read from XML
   blobLogicalLength_ = length;
   binarySectionLogicalStart_ = imf->file_->physicalToLogical( fileOffset );
   binarySectionLogicalLength_ = sizeof( BlobSectionHeader ) + blobLogicalLength_;
}

bool BlobNodeImpl::isTypeEquivalent( NodeImplSharedPtr ni )
{
   // don't checkImageFileOpen, NodeImpl() will do it

   /// Same node type?
   if ( ni->type() != E57_BLOB )
   {
      return ( false );
   }

   /// Downcast to shared_ptr<BlobNodeImpl>
   std::shared_ptr<BlobNodeImpl> bi( std::static_pointer_cast<BlobNodeImpl>( ni ) );

   /// blob lengths must match
   if ( blobLogicalLength_ != bi->blobLogicalLength_ )
   {
      return ( false );
   }

   /// ignore blob contents, doesn't have to match

   /// Types match
   return ( true );
}

bool BlobNodeImpl::isDefined( const ustring &pathName )
{
   // don't checkImageFileOpen, NodeImpl() will do it

   /// We have no sub-structure, so if path not empty return false
   return pathName.empty();
}

int64_t BlobNodeImpl::byteCount()
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
   return ( blobLogicalLength_ );
}

void BlobNodeImpl::read( uint8_t *buf, int64_t start, size_t count )
{
   //??? check start not negative

   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
   if ( static_cast<uint64_t>( start ) + count > blobLogicalLength_ )
   {
      throw E57_EXCEPTION2( E57_ERROR_BAD_API_ARGUMENT,
                            "this->pathName=" + this->pathName() + " start=" + toString( start ) +
                               " count=" + toString( count ) + " length=" + toString( blobLogicalLength_ ) );
   }

   ImageFileImplSharedPtr imf( destImageFile_ );
   imf->file_->seek( binarySectionLogicalStart_ + sizeof( BlobSectionHeader ) + start );
   imf->file_->read( reinterpret_cast<char *>( buf ),
                     static_cast<size_t>( count ) ); //??? arg1 void* ?
}

void BlobNodeImpl::write( uint8_t *buf, int64_t start, size_t count )
{
   //??? check start not negative
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );

   ImageFileImplSharedPtr destImageFile( destImageFile_ );

   if ( !destImageFile->isWriter() )
   {
      throw E57_EXCEPTION2( E57_ERROR_FILE_IS_READ_ONLY, "fileName=" + destImageFile->fileName() );
   }
   if ( !isAttached() )
   {
      throw E57_EXCEPTION2( E57_ERROR_NODE_UNATTACHED, "fileName=" + destImageFile->fileName() );
   }

   if ( static_cast<uint64_t>( start ) + count > blobLogicalLength_ )
   {
      throw E57_EXCEPTION2( E57_ERROR_BAD_API_ARGUMENT,
                            "this->pathName=" + this->pathName() + " start=" + toString( start ) +
                               " count=" + toString( count ) + " length=" + toString( blobLogicalLength_ ) );
   }

   ImageFileImplSharedPtr imf( destImageFile_ );
   imf->file_->seek( binarySectionLogicalStart_ + sizeof( BlobSectionHeader ) + start );
   imf->file_->write( reinterpret_cast<char *>( buf ),
                      static_cast<size_t>( count ) ); //??? arg1 void* ?
}

void BlobNodeImpl::checkLeavesInSet( const StringSet &pathNames, NodeImplSharedPtr origin )
{
   // don't checkImageFileOpen

   /// We are a leaf node, so verify that we are listed in set. ???true for
   /// blobs? what exception get if try blob in compressedvector?
   if ( pathNames.find( relativePathName( origin ) ) == pathNames.end() )
   {
      throw E57_EXCEPTION2( E57_ERROR_NO_BUFFER_FOR_ELEMENT, "this->pathName=" + this->pathName() );
   }
}

void BlobNodeImpl::writeXml( ImageFileImplSharedPtr /*imf*/, CheckedFile &cf, int indent, const char *forcedFieldName )
{
   // don't checkImageFileOpen

   ustring fieldName;
   if ( forcedFieldName != nullptr )
   {
      fieldName = forcedFieldName;
   }
   else
   {
      fieldName = elementName_;
   }

   //??? need to implement
   //??? Type --> type
   //??? need to have length?, check same as in section header?
   uint64_t physicalOffset = cf.logicalToPhysical( binarySectionLogicalStart_ );
   cf << space( indent ) << "<" << fieldName << " type=\"Blob\" fileOffset=\"" << physicalOffset << "\" length=\""
      << blobLogicalLength_ << "\"/>\n";
}

#ifdef E57_DEBUG
void BlobNodeImpl::dump( int indent, std::ostream &os ) const
{
   // don't checkImageFileOpen
   os << space( indent ) << "type:        Blob"
      << " (" << type() << ")" << std::endl;
   NodeImpl::dump( indent, os );
   os << space( indent ) << "blobLogicalLength_:           " << blobLogicalLength_ << std::endl;
   os << space( indent ) << "binarySectionLogicalStart:    " << binarySectionLogicalStart_ << std::endl;
   os << space( indent ) << "binarySectionLogicalLength:   " << binarySectionLogicalLength_ << std::endl;
   //    size_t i;
   //    for (i = 0; i < blobLogicalLength_ && i < 10; i++) {
   //        uint8_t b;
   //        read(&b, i, 1);
   //        os << space(indent) << "data[" << i << "]: "<< static_cast<int>(b)
   //        << std::endl;
   //    }
   //    if (i < blobLogicalLength_)
   //        os << space(indent) << "more data unprinted..." << std::endl;
}
#endif
