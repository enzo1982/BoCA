 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2026 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_MUXERCOMPONENT
#define H_BOCA_MUXERCOMPONENT

#include "component.h"
#include "../common/muxer.h"
#include "../common/metadata/track.h"

namespace BoCA
{
	namespace CS
	{
		abstract class BOCA_DLL_EXPORT MuxerComponent : public Component
		{
			protected:
				IO::Driver		*driver;

				Track			 track;
				Format			 format;

				MuxerCodec		 codec;
				Buffer<UnsignedByte>	 codecConfig;

				Int			 timeScale;

				Bool			 gaplessInfo;

				Int			 gaplessDelay;
				Int			 gaplessPadding;
				Int64			 gaplessLength;
			public:
							 MuxerComponent();
				virtual			~MuxerComponent();

				/* Called to set information about the audio track.
				 */
				virtual Bool		 SetAudioTrackInfo(const Track &);

				/* Set IO driver to be used by this component.
				 */
				Void			 SetDriver(IO::Driver *);

				/* Open/close the container for reading or writing.
				 *
				 * Notes: - The IO driver must be set before calling Open().
				 *	  - When writing, codec information must be set before calling Open().
				 *	  - When reading, codec information is available after calling Open().
				 *	  - When modifying, the IO driver must support reading and writing.
				 */
				virtual Bool		 Open(MuxerMode) = 0;
				virtual Bool		 Close() = 0;

				/* Returns the audio format as declared by the container.
				 *
				 * Fields are zero if the container does not provide them.
				 */
				virtual const Format	&GetFormatInfo() const;

				/* Get/set the codec identifier.
				 */
				virtual MuxerCodec	 GetCodec() const;
				virtual Bool		 SetCodec(MuxerCodec);

				/* Get/set the codec specific configuration record.
				 */
				virtual Bool		 GetCodecConfig(Buffer<UnsignedByte> &) const;
				virtual Bool		 SetCodecConfig(const Buffer<UnsignedByte> &);

				/* Get/set the time scale, i.e. the number of position units per second.
				 */
				virtual Int		 GetTimeScale() const;
				virtual Bool		 SetTimeScale(Int);

				/* Get/set gapless playback information.
				 *
				 * Delay, padding and length are given in time scale units.
				 */
				virtual Bool		 GetGaplessInfo(Int &, Int &, Int64 &) const;
				virtual Bool		 SetGaplessInfo(Int, Int, Int64);

				/* Returns the number of packets or -1 if unknown.
				 */
				virtual Int64		 GetNumberOfPackets();

				/* Returns the duration of the stream in time scale units or -1 if unknown.
				 */
				virtual Int64		 GetDuration();

				/* Seek to the packet containing the specified position.
				 *
				 * Returns the actual position at the start of the next packet
				 * to be read or -1 if the seek operation failed.
				 */
				virtual Int64		 SeekPacket(Int64);

				/* Read the next packet.
				 *
				 * The position is set to the stream position at the end of the
				 * packet in time scale units or to -1 if the position is unknown.
				 *
				 * Returns the size of the packet or -1 if no more packets are available.
				 */
				virtual Int		 ReadPacket(Buffer<UnsignedByte> &, Int64 &, Int &);

				/* Write a packet.
				 *
				 * The position is the stream position at the end of the packet in time scale units.
				 *
				 * Returns the number of bytes written to the output stream or -1 on error.
				 */
				virtual Int		 WritePacket(const UnsignedByte *, Int, Int64, Int);

				/* Update stream information of a stream opened in modify
				 * mode to match the information set with SetAudioTrackInfo().
				 */
				virtual Bool		 UpdateStreamInfo();

				/* Optimize the layout of a file written by this
				 * muxer after its output stream has been closed.
				 */
				virtual Bool		 OptimizeFile(const String &);
		};
	};
};

#endif
