/*****************************************************************
|
|    AP4 - Commands
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
#include "Ap4Command.h"
#include "Ap4Utils.h"
#include "Ap4Atom.h"

/*----------------------------------------------------------------------
|   dynamic cast support
+---------------------------------------------------------------------*/
AP4_DEFINE_DYNAMIC_CAST_ANCHOR(AP4_Command)

/*----------------------------------------------------------------------
|   AP4_Command::Inspect
+---------------------------------------------------------------------*/
AP4_Result
AP4_Command::Inspect(AP4_AtomInspector& inspector)
{
    char name[64];
    AP4_FormatString(name, sizeof(name), "Command:%02x", m_ClassId);
    inspector.StartDescriptor(name, GetHeaderSize(), GetSize(), AP4_ATOM_INVALID_OFFSET);
    inspector.EndDescriptor();

    return AP4_SUCCESS;
}

/*----------------------------------------------------------------------
|   AP4_UnknownCommand::AP4_UnknownCommand
+---------------------------------------------------------------------*/
AP4_UnknownCommand::AP4_UnknownCommand(AP4_ByteStream& stream,
                                       AP4_UI08        tag,
                                       AP4_Size        header_size,
                                       AP4_Size        payload_size) :
    AP4_Command(tag, header_size, payload_size)
{
    m_Data.SetDataSize(payload_size);
    stream.Read(m_Data.UseData(), payload_size);    
}

/*----------------------------------------------------------------------
|   AP4_UnknownCommand::WriteFields
+---------------------------------------------------------------------*/
AP4_Result
AP4_UnknownCommand::WriteFields(AP4_ByteStream& stream)
{
    // write the payload
    stream.Write(m_Data.GetData(), m_Data.GetDataSize());

    return AP4_SUCCESS;
}
