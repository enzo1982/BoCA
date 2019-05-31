 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2019 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>

BoCA_BEGIN_COMPONENT(EncoderMultiEncoderHub)

namespace BoCA
{
	struct ConversionData
	{
		Config		*configuration;

		Array<Track>	 tracksToConvert;
		Array<Track>	 convertedTracks;

		Track		 playlistTrack;
	};

	class EncoderMultiEncoderHub : public CS::EncoderComponent
	{
		private:
			ConfigLayer				*configLayer;

			static Array<ConversionData *>		 conversionData;

			Int					 conversionID;

			Array<IO::OutStream *, Void *>		 streams;
			Array<AS::EncoderComponent *, Void *>	 encoders;

			Array<Threads::Mutex *, Void *>		 mutexes;
			Array<Buffer<UnsignedByte> *, Void *>	 buffers;
			Array<Threads::Thread *, Void *>	 threads;

			Int64					 trackLength;
			Int64					 totalLength;

			Bool					 finished;
			Bool					 cancelled;

			static String				 GetFileNamePattern(const Config *, const Track &);
			static String				 GetPlaylistFileName(const Config *, const Track &, const Array<Track> &);

			Void					 EncodeThread(Int);
		public:
			static const String			&GetComponentSpecs();

								 EncoderMultiEncoderHub();
								~EncoderMultiEncoderHub();

			Bool					 IsThreadSafe() const;

			Bool					 IsLossless() const;

			Bool					 Activate();
			Bool					 Deactivate();

			Int					 WriteData(Buffer<UnsignedByte> &);

			String					 GetOutputFileExtension() const;

			ConfigLayer				*GetConfigurationLayer();
		slots:
			static Void				 OnStartConversion(Int, const Array<Track> &);
			static Void				 OnFinishConversion(Int);
			static Void				 OnCancelConversion(Int);

			Void					 OnFinishTrackConversion(Int, const Track &);
			Void					 OnCancelTrackConversion(Int, const Track &);
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(EncoderMultiEncoderHub)

BoCA_END_COMPONENT(EncoderMultiEncoderHub)
