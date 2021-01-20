/*****************************************************************
|
|    AP4 - sdp Atoms 
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
#include "Ap4SdpAtom.h"
#include "Ap4AtomFactory.h"
#include "Ap4Utils.h"

/*----------------------------------------------------------------------
|   dynamic cast support
+---------------------------------------------------------------------*/
AP4_DEFINE_DYNAMIC_CAST_ANCHOR(AP4_SdpAtom)

/*----------------------------------------------------------------------
|   AP4_SdpAtom::AP4_SdpAtom
+---------------------------------------------------------------------*/
AP4_SdpAtom::AP4_SdpAtom(const char* sdp_text) :
    AP4_Atom(AP4_ATOM_TYPE_SDP_, AP4_ATOM_HEADER_SIZE),
    m_SdpText(sdp_text)
{
    m_Size32 += m_SdpText.GetLength()+1;
}

/*----------------------------------------------------------------------
|   AP4_SdpAtom::AP4_SdpAtom
+---------------------------------------------------------------------*/
AP4_SdpAtom::AP4_SdpAtom(AP4_UI32 size, AP4_ByteStream& stream) :
    AP4_Atom(AP4_ATOM_TYPE_SDP_, size)
{
    // sdptext
    AP4_Size str_size = size-AP4_ATOM_HEADER_SIZE;
    if (str_size > 0) {
        char* str = new char[str_size+1];
        stream.Read(str, str_size);
        str[str_size] = '\0'; // force null-termination
        m_SdpText = str;
        delete[] str;
    }
}

/*----------------------------------------------------------------------
|   AP4_SdpAtom::WriteFields
+---------------------------------------------------------------------*/
AP4_Result
AP4_SdpAtom::WriteFields(AP4_ByteStream& stream)
{
    // sdptext
    AP4_Result result = stream.Write(m_SdpText.GetChars(), m_SdpText.GetLength());
    if (AP4_FAILED(result)) return result;

    // pad with zeros if necessary
    AP4_Size padding = m_Size32-(AP4_ATOM_HEADER_SIZE+m_SdpText.GetLength());
    while (padding--) stream.WriteUI08(0);
    
    return AP4_SUCCESS;
}

/*----------------------------------------------------------------------
|   AP4_SdpAtom::InspectFields
+---------------------------------------------------------------------*/
AP4_Result
AP4_SdpAtom::InspectFields(AP4_AtomInspector& inspector)
{
    inspector.AddField("sdp_text", m_SdpText.GetChars());

    return AP4_SUCCESS;
}

/*----------------------------------------------------------------------
|   AP4_SdpAtom::GetSdpText
+---------------------------------------------------------------------*/
const AP4_String&
AP4_SdpAtom::GetSdpText() const
{
    return m_SdpText;
}
