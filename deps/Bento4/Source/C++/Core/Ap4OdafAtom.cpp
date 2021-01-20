/*****************************************************************
|
|    AP4 - odaf Atoms 
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
#include "Ap4Utils.h"
#include "Ap4OdafAtom.h"

/*----------------------------------------------------------------------
|   dynamic cast support
+---------------------------------------------------------------------*/
AP4_DEFINE_DYNAMIC_CAST_ANCHOR(AP4_OdafAtom)

/*----------------------------------------------------------------------
|   AP4_OdafAtom::Create
+---------------------------------------------------------------------*/
AP4_OdafAtom*
AP4_OdafAtom::Create(AP4_Size size, AP4_ByteStream& stream)
{
    AP4_UI08 version;
    AP4_UI32 flags;
    if (size < AP4_FULL_ATOM_HEADER_SIZE) return NULL;
    if (AP4_FAILED(AP4_Atom::ReadFullHeader(stream, version, flags))) return NULL;
    if (version != 0) return NULL;
    return new AP4_OdafAtom(size, version, flags, stream);
}

/*----------------------------------------------------------------------
|   AP4_OdafAtom::AP4_OdafAtom
+---------------------------------------------------------------------*/
AP4_OdafAtom::AP4_OdafAtom(bool     selective_encryption,
                           AP4_UI08 key_length_indicator,
                           AP4_UI08 iv_length) :
    AP4_Atom(AP4_ATOM_TYPE_ODAF, AP4_FULL_ATOM_HEADER_SIZE+3, 0, 0),
    m_SelectiveEncryption(selective_encryption),
    m_KeyIndicatorLength(key_length_indicator),
    m_IvLength(iv_length)
{
}

/*----------------------------------------------------------------------
|   AP4_OdafAtom::AP4_OdafAtom
+---------------------------------------------------------------------*/
AP4_OdafAtom::AP4_OdafAtom(AP4_UI32        size, 
                           AP4_UI08        version,
                           AP4_UI32        flags,
                           AP4_ByteStream& stream) :
    AP4_Atom(AP4_ATOM_TYPE_ODAF, size, version, flags),
    m_KeyIndicatorLength(0),
    m_IvLength(0)
{
    AP4_UI08 s;
    stream.ReadUI08(s);
    m_SelectiveEncryption = ((s&0x80) != 0);
    stream.ReadUI08(m_KeyIndicatorLength);
    stream.ReadUI08(m_IvLength);
}

/*----------------------------------------------------------------------
|   AP4_OdafAtom::WriteFields
+---------------------------------------------------------------------*/
AP4_Result
AP4_OdafAtom::WriteFields(AP4_ByteStream& stream)
{
    AP4_Result result;

    // selective encryption
    result = stream.WriteUI08(m_SelectiveEncryption ? 0x80 : 0);
    if (AP4_FAILED(result)) return result;

    // key indicator length
    result = stream.WriteUI08(m_KeyIndicatorLength);
    if (AP4_FAILED(result)) return result;

    // IV length
    result = stream.WriteUI08(m_IvLength);
    if (AP4_FAILED(result)) return result;

    return AP4_SUCCESS;
}

/*----------------------------------------------------------------------
|   AP4_OdafAtom::InspectFields
+---------------------------------------------------------------------*/
AP4_Result
AP4_OdafAtom::InspectFields(AP4_AtomInspector& inspector)
{
    inspector.AddField("selective_encryption", m_SelectiveEncryption);
    inspector.AddField("key_indicator_length", m_KeyIndicatorLength);
    inspector.AddField("IV_length", m_IvLength);

    return AP4_SUCCESS;
}
