/*****************************************************************
|
|    AP4 - tims Atoms 
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
#include "Ap4TimsAtom.h"
#include "Ap4AtomFactory.h"
#include "Ap4Utils.h"
#include "Ap4Types.h"

/*----------------------------------------------------------------------
|   dynamic cast support
+---------------------------------------------------------------------*/
AP4_DEFINE_DYNAMIC_CAST_ANCHOR(AP4_TimsAtom)

/*----------------------------------------------------------------------
|   AP4_TimsAtom::AP4_TimsAtom
+---------------------------------------------------------------------*/
AP4_TimsAtom::AP4_TimsAtom(AP4_UI32 timescale) :
    AP4_Atom(AP4_ATOM_TYPE_TIMS, AP4_ATOM_HEADER_SIZE+4),
    m_TimeScale(timescale)
{
}

/*----------------------------------------------------------------------
|   AP4_TimsAtom::AP4_TimsAtom
+---------------------------------------------------------------------*/
AP4_TimsAtom::AP4_TimsAtom(AP4_UI32 size, AP4_ByteStream& stream) :
    AP4_Atom(AP4_ATOM_TYPE_TIMS, size)
{
    stream.ReadUI32(m_TimeScale);
}

/*----------------------------------------------------------------------
|   AP4_TimsAtom::WriteFields
+---------------------------------------------------------------------*/
AP4_Result
AP4_TimsAtom::WriteFields(AP4_ByteStream& stream)
{
    // timescale
    return stream.WriteUI32(m_TimeScale);

}

/*----------------------------------------------------------------------
|   AP4_TimsAtom::InspectFields
+---------------------------------------------------------------------*/
AP4_Result
AP4_TimsAtom::InspectFields(AP4_AtomInspector& inspector)
{
    inspector.AddField("timescale", m_TimeScale);

    return AP4_SUCCESS;
}
