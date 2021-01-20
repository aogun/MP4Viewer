/*****************************************************************
|
|    AP4 - Expandable base class
|
|    Copyright 2002-2008 Axiomatic Systems, LLC
|
|
|    This file is part of Bento4/AP4 (MP4 Atom Processing Library).
|
|    Unless you have obtained Bento4 under a difference license,
|    this version of Bento4 is Bento4|GPL.
|    Bento4|GPL is free software; you can redistribute it and/or modify
|    it under the terms of the GNU General Public License as published by
|    the Free Software Foundation; either version 2, or (at your option)
|    any later version.
|
|    Bento4|GPL is distributed in the hope that it will be useful,
|    but WITHOUT ANY WARRANTY; without even the implied warranty of
|    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
|    GNU General Public License for more details.
|
|    You should have received a copy of the GNU General Public License
|    along with Bento4|GPL; see the file COPYING.  If not, write to the
|    Free Software Foundation, 59 Temple Place - Suite 330, Boston, MA
|    02111-1307, USA.
|
 ****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "Ap4Debug.h"
#include "Ap4Descriptor.h"
#include "Ap4Utils.h"
#include "Ap4Atom.h"

/*----------------------------------------------------------------------
|   AP4_Expandable::MinHeaderSize
+---------------------------------------------------------------------*/
AP4_Size
AP4_Expandable::MinHeaderSize(AP4_Size payload_size)
{
    // compute how many bytes are needed to encode the payload size
    // plus tag
    return 2+(payload_size/128);
}

/*----------------------------------------------------------------------
|   AP4_Expandable::AP4_Expandable
+---------------------------------------------------------------------*/
AP4_Expandable::AP4_Expandable(AP4_UI32    class_id,
                               ClassIdSize class_id_size, 
                               AP4_Size    header_size, 
                               AP4_Size    payload_size) :
    m_ClassId(class_id),
    m_ClassIdSize(class_id_size),
    m_HeaderSize(header_size),
    m_PayloadSize(payload_size)
{
    AP4_ASSERT(header_size >= 1+1);
    AP4_ASSERT(header_size <= 1+4);
}
/*----------------------------------------------------------------------
|   AP4_Expandable::AP4_Expandable
+---------------------------------------------------------------------*/
AP4_Expandable::AP4_Expandable(AP4_UI32    class_id,
                               ClassIdSize class_id_size,
                               AP4_Size    header_size,
                               AP4_Size    payload_size,
                               AP4_UI64     offset) :
    m_ClassId(class_id),
    m_ClassIdSize(class_id_size),
    m_HeaderSize(header_size),
    m_PayloadSize(payload_size),
    m_offset(offset)
{
    AP4_ASSERT(header_size >= 1+1);
    AP4_ASSERT(header_size <= 1+4);
}

/*----------------------------------------------------------------------
|   AP4_Expandable::Write
+---------------------------------------------------------------------*/
AP4_Result
AP4_Expandable::Write(AP4_ByteStream& stream)
{
    AP4_Result result;

    // write the class id
    switch (m_ClassIdSize) {
        case CLASS_ID_SIZE_08:
            result = stream.WriteUI08((AP4_UI08)m_ClassId);
            if (AP4_FAILED(result)) return result;
            break;
        
        default:
            return AP4_ERROR_INTERNAL;
    }
    
    // write the size
    AP4_ASSERT(m_HeaderSize-1 <= 8);
    AP4_ASSERT(m_HeaderSize >= 2);
    unsigned int size = m_PayloadSize;
    unsigned char bytes[8];

    // last bytes of the encoded size
    bytes[m_HeaderSize-2] = size&0x7F;

    // leading bytes of the encoded size
    for (int i=m_HeaderSize-3; i>=0; i--) {
        // move to the next 7 bits
        size >>= 7;

        // output a byte with a top bit marker
        bytes[i] = (size&0x7F) | 0x80;
    }

    result = stream.Write(bytes, m_HeaderSize-1);
    if (AP4_FAILED(result)) return result;

    // write the fields
    WriteFields(stream);

    return result;
}

/*----------------------------------------------------------------------
|   AP4_Expandable::Inspect
+---------------------------------------------------------------------*/
AP4_Result
AP4_Expandable::Inspect(AP4_AtomInspector& inspector)
{
    char name[64];
    AP4_FormatString(name, sizeof(name), "#:%02x", m_ClassId);
    inspector.StartDescriptor(name, GetHeaderSize(), GetSize(), m_offset);
    inspector.EndDescriptor();

    return AP4_SUCCESS;
}
