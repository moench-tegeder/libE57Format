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

#include <exception>
#include <iostream>
#include <string>

namespace e57
{
   //! @brief Numeric error identifiers used in E57Exception
   enum ErrorCode
   {
      // N.B.  *** When changing error strings here, remember to update the error
      // strings in E57Exception.cpp ****
      E57_SUCCESS = 0,                         //!< operation was successful
      E57_ERROR_BAD_CV_HEADER = 1,             //!< a CompressedVector binary header was bad
      E57_ERROR_BAD_CV_PACKET = 2,             //!< a CompressedVector binary packet was bad
      E57_ERROR_CHILD_INDEX_OUT_OF_BOUNDS = 3, //!< a numerical index identifying a child was out of bounds
      E57_ERROR_SET_TWICE = 4,                 //!< attempted to set an existing child element to a new value
      E57_ERROR_HOMOGENEOUS_VIOLATION = 5,     //!< attempted to add an E57 Element that would have made the children
                                               //!< of a homogenous Vector have different types
      E57_ERROR_VALUE_NOT_REPRESENTABLE = 6,   //!< a value could not be represented in the requested type
      E57_ERROR_SCALED_VALUE_NOT_REPRESENTABLE = 7, //!< after scaling the result could not be represented in the
                                                    //!< requested type
      E57_ERROR_REAL64_TOO_LARGE = 8,            //!< a 64 bit IEEE float was too large to store in a 32 bit IEEE float
      E57_ERROR_EXPECTING_NUMERIC = 9,           //!< Expecting numeric representation in user's buffer, found ustring
      E57_ERROR_EXPECTING_USTRING = 10,          //!< Expecting string representation in user's buffer, found numeric
      E57_ERROR_INTERNAL = 11,                   //!< An unrecoverable inconsistent internal state was detected
      E57_ERROR_BAD_XML_FORMAT = 12,             //!< E57 primitive not encoded in XML correctly
      E57_ERROR_XML_PARSER = 13,                 //!< XML not well formed
      E57_ERROR_BAD_API_ARGUMENT = 14,           //!< bad API function argument provided by user
      E57_ERROR_FILE_IS_READ_ONLY = 15,          //!< can't modify read only file
      E57_ERROR_BAD_CHECKSUM = 16,               //!< checksum mismatch, file is corrupted
      E57_ERROR_OPEN_FAILED = 17,                //!< open() failed
      E57_ERROR_CLOSE_FAILED = 18,               //!< close() failed
      E57_ERROR_READ_FAILED = 19,                //!< read() failed
      E57_ERROR_WRITE_FAILED = 20,               //!< write() failed
      E57_ERROR_LSEEK_FAILED = 21,               //!< lseek() failed
      E57_ERROR_PATH_UNDEFINED = 22,             //!< E57 element path well formed but not defined
      E57_ERROR_BAD_BUFFER = 23,                 //!< bad SourceDestBuffer
      E57_ERROR_NO_BUFFER_FOR_ELEMENT = 24,      //!< no buffer specified for an element in CompressedVectorNode during
                                                 //!< write
      E57_ERROR_BUFFER_SIZE_MISMATCH = 25,       //!< SourceDestBuffers not all same size
      E57_ERROR_BUFFER_DUPLICATE_PATHNAME = 26,  //!< duplicate pathname in CompressedVectorNode read/write
      E57_ERROR_BAD_FILE_SIGNATURE = 27,         //!< file signature not "ASTM-E57"
      E57_ERROR_UNKNOWN_FILE_VERSION = 28,       //!< incompatible file version
      E57_ERROR_BAD_FILE_LENGTH = 29,            //!< size in file header not same as actual
      E57_ERROR_XML_PARSER_INIT = 30,            //!< XML parser failed to initialize
      E57_ERROR_DUPLICATE_NAMESPACE_PREFIX = 31, //!< namespace prefix already defined
      E57_ERROR_DUPLICATE_NAMESPACE_URI = 32,    //!< namespace URI already defined
      E57_ERROR_BAD_PROTOTYPE = 33,              //!< bad prototype in CompressedVectorNode
      E57_ERROR_BAD_CODECS = 34,                 //!< bad codecs in CompressedVectorNode
      E57_ERROR_VALUE_OUT_OF_BOUNDS = 35,        //!< element value out of min/max bounds
      E57_ERROR_CONVERSION_REQUIRED = 36,        //!< conversion required to assign element value, but not requested
      E57_ERROR_BAD_PATH_NAME = 37,              //!< E57 path name is not well formed
      E57_ERROR_NOT_IMPLEMENTED = 38,            //!< functionality not implemented
      E57_ERROR_BAD_NODE_DOWNCAST = 39,          //!< bad downcast from Node to specific node type
      E57_ERROR_WRITER_NOT_OPEN = 40,            //!< CompressedVectorWriter is no longer open
      E57_ERROR_READER_NOT_OPEN = 41,            //!< CompressedVectorReader is no longer open
      E57_ERROR_NODE_UNATTACHED = 42,            //!< node is not yet attached to tree of ImageFile
      E57_ERROR_ALREADY_HAS_PARENT = 43,         //!< node already has a parent
      E57_ERROR_DIFFERENT_DEST_IMAGEFILE = 44,   //!< nodes were constructed with different destImageFiles
      E57_ERROR_IMAGEFILE_NOT_OPEN = 45,         //!< destImageFile is no longer open
      E57_ERROR_BUFFERS_NOT_COMPATIBLE = 46,     //!< SourceDestBuffers not compatible with previously given ones
      E57_ERROR_TOO_MANY_WRITERS = 47,           //!< too many open CompressedVectorWriters of an ImageFile
      E57_ERROR_TOO_MANY_READERS = 48,           //!< too many open CompressedVectorReaders of an ImageFile
      E57_ERROR_BAD_CONFIGURATION = 49,          //!< bad configuration string
      E57_ERROR_INVARIANCE_VIOLATION = 50        //!< class invariance constraint violation in debug mode
   };

   class E57Exception : public std::exception
   {
   public:
      void report( const char *reportingFileName = nullptr, int reportingLineNumber = 0,
                   const char *reportingFunctionName = nullptr, std::ostream &os = std::cout ) const;
      ErrorCode errorCode() const;
      std::string context() const;
      const char *what() const noexcept override;

      // For debugging purposes:
      const char *sourceFileName() const;
      const char *sourceFunctionName() const;
      int sourceLineNumber() const;

      //! \cond documentNonPublic   The following isn't part of the API, and isn't
      //! documented.
      E57Exception() = delete;
      E57Exception( ErrorCode ecode, const std::string &context, const std::string &srcFileName = nullptr,
                    int srcLineNumber = 0, const char *srcFunctionName = nullptr );
      ~E57Exception() noexcept override = default;

   protected:
      ErrorCode errorCode_;
      std::string context_;
      std::string sourceFileName_;
      const char *sourceFunctionName_;
      int sourceLineNumber_;
      //! \endcond
   };

   namespace Utilities
   {
      // Get latest version of ASTM standard supported, and library id string
      void getVersions( int &astmMajor, int &astmMinor, std::string &libraryId );

      std::string errorCodeToString( ErrorCode ecode );
   }
}
