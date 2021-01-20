/*****************************************************************
|
|    AP4 - AAC ADTS Parser
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
#include "Ap4BitStream.h"
#include "Ap4AdtsParser.h"

/*----------------------------------------------------------------------
|   constants
+---------------------------------------------------------------------*/
#define AP4_ADTS_HEADER_SIZE 7

#define AP4_ADTS_SYNC_MASK     0xFFF6 /* 12 sync bits plus 2 layer bits */
#define AP4_ADTS_SYNC_PATTERN  0xFFF0 /* 12 sync bits=1 layer=0         */

const AP4_UI32
AP4_AdtsSamplingFrequencyTable[16] =
{
    96000,
    88200,
    64000,
    48000,
    44100,
    32000,
    24000,
    22050,
    16000,
    12000,
    11025,
    8000,
    7350,
    0,      /* Reserved */
    0,      /* Reserved */
    0       /* Escape code */
};

/*----------------------------------------------------------------------+
|    AP4_AdtsHeader::AP4_AdtsHeader
+----------------------------------------------------------------------*/
AP4_AdtsHeader::AP4_AdtsHeader(const AP4_UI08* bytes)
{
    // fixed part
    m_Id                     = ( bytes[1] & 0x08) >> 3;
    m_ProtectionAbsent       =   bytes[1] & 0x01;
    m_ProfileObjectType      = ( bytes[2] & 0xC0) >> 6;
    m_SamplingFrequencyIndex = ( bytes[2] & 0x3C) >> 2;
    m_ChannelConfiguration   = ((bytes[2] & 0x01) << 2) | 
                               ((bytes[3] & 0xC0) >> 6);
    // variable part
    m_FrameLength = ((unsigned int)(bytes[3] & 0x03) << 11) |
                    ((unsigned int)(bytes[4]       ) <<  3) |
                    ((unsigned int)(bytes[5] & 0xE0) >>  5);
    m_RawDataBlocks =               bytes[6] & 0x03;
}

/*----------------------------------------------------------------------+
|    AP4_AdtsHeader::MatchFixed
|
|    Check that two fixed headers are the same
|
+----------------------------------------------------------------------*/
bool
AP4_AdtsHeader::MatchFixed(unsigned char* a, unsigned char* b)
{
    if (a[0]         ==  b[0] &&
        a[1]         ==  b[1] &&
        a[2]         ==  b[2] &&
       (a[3] & 0xF0) == (b[3] & 0xF0)) {
        return true;
    } else {
        return false;
    }
}

/*----------------------------------------------------------------------+
|    AP4_AdtsHeader::Check
+----------------------------------------------------------------------*/
AP4_Result
AP4_AdtsHeader::Check()
{
    // check that the sampling frequency index is valid
    if (m_SamplingFrequencyIndex >= 0xD) {
        return AP4_FAILURE;
    }

    /* MPEG2 does not use all profiles */
    if (m_Id == 1 && m_ProfileObjectType == 3) {
        return AP4_FAILURE;
    }

    return AP4_SUCCESS;
}

/*----------------------------------------------------------------------+
|    AP4_AdtsParser::AP4_AdtsParser
+----------------------------------------------------------------------*/
AP4_AdtsParser::AP4_AdtsParser() :
    m_FrameCount(0)
{
}

/*----------------------------------------------------------------------+
|    AP4_AdtsParser::~AP4_AdtsParser
+----------------------------------------------------------------------*/
AP4_AdtsParser::~AP4_AdtsParser()
{
}

/*----------------------------------------------------------------------+
|    AP4_AdtsParser::Reset
+----------------------------------------------------------------------*/
AP4_Result
AP4_AdtsParser::Reset()
{
    m_FrameCount = 0;

    return AP4_SUCCESS;
}

/*----------------------------------------------------------------------+
|    AP4_AdtsParser::Feed
+----------------------------------------------------------------------*/
AP4_Result
AP4_AdtsParser::Feed(const AP4_UI08* buffer, 
                     AP4_Size*       buffer_size,
                     AP4_Flags       flags)
{
    AP4_Size free_space;

    /* update flags */
    m_Bits.m_Flags = flags;

    /* possible shortcut */
    if (buffer == NULL ||
        buffer_size == NULL || 
        *buffer_size == 0) {
        return AP4_SUCCESS;
    }

    /* see how much data we can write */
    free_space = m_Bits.GetBytesFree();
    if (*buffer_size > free_space) *buffer_size = free_space;
    if (*buffer_size == 0) return AP4_SUCCESS;

    /* write the data */
    return m_Bits.WriteBytes(buffer, *buffer_size); 
}

/*----------------------------------------------------------------------+
|    AP4_AdtsParser::FindHeader
+----------------------------------------------------------------------*/
AP4_Result
AP4_AdtsParser::FindHeader(AP4_UI08* header)
{
    AP4_Size available = m_Bits.GetBytesAvailable();

    /* look for the sync pattern */
    while (available-- >= AP4_ADTS_HEADER_SIZE) {
        m_Bits.PeekBytes(header, 2);

        if ((((header[0] << 8) | header[1]) & AP4_ADTS_SYNC_MASK) == AP4_ADTS_SYNC_PATTERN) {
            /* found a sync pattern, read the entire the header */
            m_Bits.PeekBytes(header, AP4_ADTS_HEADER_SIZE);
            
           return AP4_SUCCESS;
        } else {
            m_Bits.ReadByte(); // skip
        }
    }

    return AP4_ERROR_NOT_ENOUGH_DATA;
}

/*----------------------------------------------------------------------+
|    AP4_AdtsParser::FindFrame
+----------------------------------------------------------------------*/
AP4_Result
AP4_AdtsParser::FindFrame(AP4_AacFrame& frame)
{
    unsigned int   available;
    unsigned char  raw_header[AP4_ADTS_HEADER_SIZE];
    AP4_Result     result;

    /* align to the start of the next byte */
    m_Bits.ByteAlign();
    
    /* find a frame header */
    result = FindHeader(raw_header);
    if (AP4_FAILED(result)) return result;

    /* parse the header */
    AP4_AdtsHeader adts_header(raw_header);

    /* check the header */
    result = adts_header.Check();
    if (AP4_FAILED(result)) goto fail;
    
    /* check if we have enough data to peek at the next header */
    available = m_Bits.GetBytesAvailable();
    if (available >= adts_header.m_FrameLength+AP4_ADTS_HEADER_SIZE) {
        // enough to peek at the header of the next frame
        unsigned char peek_raw_header[AP4_ADTS_HEADER_SIZE];

        m_Bits.SkipBytes(adts_header.m_FrameLength);
        m_Bits.PeekBytes(peek_raw_header, AP4_ADTS_HEADER_SIZE);
        m_Bits.SkipBytes(-((int)adts_header.m_FrameLength));

        /* check the header */
        AP4_AdtsHeader peek_adts_header(peek_raw_header);
        result = peek_adts_header.Check();
        if (AP4_FAILED(result)) goto fail;

        /* check that the fixed part of this header is the same as the */
        /* fixed part of the previous header                           */
        if (!AP4_AdtsHeader::MatchFixed(peek_raw_header, raw_header)) {
            goto fail;
        }
    } else if (available < adts_header.m_FrameLength || (m_Bits.m_Flags & AP4_BITSTREAM_FLAG_EOS) == 0) {
        // not enough for a frame, or not at the end (in which case we'll want to peek at the next header)
        return AP4_ERROR_NOT_ENOUGH_DATA;
    }

    m_Bits.SkipBytes(AP4_ADTS_HEADER_SIZE);

    /* fill in the frame info */
    frame.m_Info.m_Standard = (adts_header.m_Id == 1 ? 
                            AP4_AAC_STANDARD_MPEG2 :
                            AP4_AAC_STANDARD_MPEG4);
    switch (adts_header.m_ProfileObjectType) {
        case 0:
            frame.m_Info.m_Profile = AP4_AAC_PROFILE_MAIN;
            break;

        case 1:
            frame.m_Info.m_Profile = AP4_AAC_PROFILE_LC;
            break;

        case 2: 
            frame.m_Info.m_Profile = AP4_AAC_PROFILE_SSR;
            break;

        case 3:
            frame.m_Info.m_Profile = AP4_AAC_PROFILE_LTP;
    }
    frame.m_Info.m_FrameLength = adts_header.m_FrameLength-AP4_ADTS_HEADER_SIZE;
    frame.m_Info.m_ChannelConfiguration = adts_header.m_ChannelConfiguration;
    frame.m_Info.m_SamplingFrequencyIndex = adts_header.m_SamplingFrequencyIndex;
    frame.m_Info.m_SamplingFrequency = AP4_AdtsSamplingFrequencyTable[adts_header.m_SamplingFrequencyIndex];

    /* skip crc if present */
    if (adts_header.m_ProtectionAbsent == 0) {
        m_Bits.SkipBits(16);
    } 

    /* set the frame source */
    frame.m_Source = &m_Bits;

    return AP4_SUCCESS;

fail:
    /* skip the header and return (only skip the first byte in  */
    /* case this was a false header that hides one just after)  */
    //m_Bits.SkipBytes(-(AP4_ADTS_HEADER_SIZE-1));
    return AP4_ERROR_CORRUPTED_BITSTREAM;
}

/*----------------------------------------------------------------------+
|    AP4_AdtsParser::GetBytesFree
+----------------------------------------------------------------------*/
AP4_Size
AP4_AdtsParser::GetBytesFree()
{
	return (m_Bits.GetBytesFree());
}



/*----------------------------------------------------------------------+
|    AP4_AdtsParser::GetBytesAvailable
+----------------------------------------------------------------------*/
AP4_Size	
AP4_AdtsParser::GetBytesAvailable()
{
	return (m_Bits.GetBytesAvailable());
}



