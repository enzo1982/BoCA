 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2011 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include <time.h>

#include "winamp_in.h"
#include "config.h"
#include "dllinterface.h"

using namespace smooth::IO;

const String &BoCA::WinampIn::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (winamp_in_plugins.Length() > 0)
	{
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>Winamp Input Plug-In Adapter</name>	\
		    <version>1.0</version>			\
		    <id>winamp-in</id>				\
		    <type>decoder</type>			\
								\
		";
	}

	for (Int i = 0; i < winamp_in_plugins.Length(); i++)
	{
		Int		 n = 1;
		Array<String>	 extensions;

		for (Int j = 0; true; j++)
		{
			String	 value = winamp_in_modules.GetNth(i)->FileExtensions + j;

			if (n & 1)
			{
				String	 extension;
				Int	 o = 0;

				for (Int m = 0; m < value.Length(); m++)
				{
					extension[m - o] = value[m];

					if (value[m + 1] == ';' || value[m + 1] == 0)
					{
						extensions.Add(extension);

						m++;
						o = m;
						extension = NIL;
					}
				}
			}
			else
			{
				componentSpecs.Append("				\
										\
				    <format>					\
				      <name>").Append(value).Append("</name>	\
										\
				");

				for (Int l = 0; l < extensions.Length(); l++)
				{
					componentSpecs.Append("								\
															\
					      <extension>").Append(extensions.GetNth(l).ToLower()).Append("</extension>	\
															\
					");
				}
										\
				componentSpecs.Append("				\
										\
				    </format>					\
										\
				");

				extensions.RemoveAll();
			}

			j += value.Length();
			n++;

			if (winamp_in_modules.GetNth(i)->FileExtensions[j] == 0 && winamp_in_modules.GetNth(i)->FileExtensions[j + 1] == 0) break;
		}
	}

	if (winamp_in_plugins.Length() > 0)
	{
		componentSpecs.Append("				\
								\
		  </component>					\
								\
		");
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadWinampDLLs();
}

Void smooth::DetachDLL()
{
	FreeWinampDLLs();
}

namespace BoCA
{
	WinampIn	*filter = NIL;

	void		 SetInfo(int, int, int, int);
	void		 VSASetInfo(int, int);
	void		 VSAAddPCMData(void *, int, int, int);
	int		 VSAGetMode(int *, int *);
	void		 VSAAdd(void *, int);
	void		 SAVSAInit(int, int);
	void		 SAVSADeInit();
	void		 SAAddPCMData(void *, int, int, int);
	int		 SAGetMode();
	void		 SAAdd(void *, int, int);
	int		 dsp_isactive();
	int		 dsp_dosamples(short int *, int, int, int, int);

	int		 Out_Open(int, int, int, int, int);
	void		 Out_Close();
	void		 Out_Flush(int);
	int		 Out_Write(char *, int);
	int		 Out_CanWrite();
	int		 Out_IsPlaying();
	int		 Out_Pause(int);
	void		 Out_SetVolume(int);
	void		 Out_SetPan(int);
	int		 Out_GetOutputTime();
	int		 Out_GetWrittenTime();
};

Bool BoCA::WinampIn::CanOpenStream(const String &streamURI)
{
	return (GetPluginForFile(streamURI) != NIL);
}

Error BoCA::WinampIn::GetStreamInfo(const String &streamURI, Track &track)
{
	InStream	*f_in = new InStream(STREAM_FILE, streamURI, IS_READ);

	track.fileSize	= f_in->Size();
	track.length	= -1;

	delete f_in;

	/* Get the correct plugin and set callback functions.
	 */
	In_Module	*plugin = GetPluginForFile(streamURI);

	plugin->SetInfo			= SetInfo;
	plugin->VSASetInfo		= VSASetInfo;
	plugin->VSAAddPCMData		= VSAAddPCMData;
	plugin->VSAGetMode		= VSAGetMode;
	plugin->VSAAdd			= VSAAdd;
	plugin->SAVSAInit		= SAVSAInit;
	plugin->SAVSADeInit		= SAVSADeInit;
	plugin->SAAddPCMData		= SAAddPCMData;
	plugin->SAGetMode		= SAGetMode;
	plugin->SAAdd			= SAAdd;
	plugin->dsp_isactive		= dsp_isactive;
	plugin->dsp_dosamples		= dsp_dosamples;

	plugin->outMod			= new Out_Module();

	plugin->outMod->Open		= Out_Open;
	plugin->outMod->Close		= Out_Close;
	plugin->outMod->Flush		= Out_Flush;
	plugin->outMod->Write		= Out_Write;
	plugin->outMod->CanWrite	= Out_CanWrite;
	plugin->outMod->IsPlaying	= Out_IsPlaying;
	plugin->outMod->Pause		= Out_Pause;
	plugin->outMod->SetVolume	= Out_SetVolume;
	plugin->outMod->SetPan		= Out_SetPan;
	plugin->outMod->GetOutputTime	= Out_GetOutputTime;
	plugin->outMod->GetWrittenTime	= Out_GetWrittenTime;

	/* Create and setup mutex.
	 */
	samplesBufferMutex = new Mutex();

	filter	  = this;
	infoTrack = &track;

	String	 trackTitle;

	/* Copy the file and play the temporary copy
	 * if the file name contains Unicode characters.
	 */
	if (String::IsUnicode(streamURI))
	{
		File(streamURI).Copy(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".in"));

		errorState = plugin->Play(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".in"));
	}
	else
	{
		errorState = plugin->Play(streamURI);
	}

	if (!errorState)
	{
		/* Give the plugin one second to start sending samples.
		 */
		Int	 start = S::System::System::Clock();

		while (S::System::System::Clock() - start < 1000 && samplesBuffer.Size() <= 0) S::System::System::Sleep(0);

		/* Get track info and stop plugin.
		 */
		int	 length_ms = -1;
		char	*title	   = new char [1024];

		plugin->GetFileInfo(NIL, title, &length_ms);

		track.approxLength = (Int) (Float(length_ms) * Float(track.GetFormat().rate) / 1000.0);

		trackTitle = title;

		delete [] title;

		plugin->Stop();
	}

	delete plugin->outMod;
	delete samplesBufferMutex;

	/* Remove temporary copy if necessary.
	 */
	if (String::IsUnicode(streamURI))
	{
		File(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".in")).Delete();
	}

	if (trackTitle.Find(File(				      streamURI ).GetFileName()) == -1 &&
	    trackTitle.Find(File(Utilities::GetNonUnicodeTempFileName(streamURI)).GetFileName()) == -1)
	{
		const Array<String>	&parts = trackTitle.Explode(" - ");

		if (parts.Length() > 1)
		{
			Info	 info = track.GetInfo();

			info.artist = parts.GetFirst();
			info.title  = parts.GetLast();

			track.SetInfo(info);
		}

		String::ExplodeFinish();
	}

	/* Return an error if we didn't get useful format data.
	 */
	if (track.GetFormat().rate     == 0 ||
	    track.GetFormat().channels == 0 ||
	    track.GetFormat().bits     == 0)
	{
		errorState = True;
	}

	if (errorState)	return Error();
	else		return Success();
}

BoCA::WinampIn::WinampIn()
{
	configLayer	   = NIL;

	packageSize	   = 0;

	infoTrack	   = NIL;
	plugin		   = NIL;

	samplesBufferMutex = NIL;

	samplesDone	   = 0;
}

BoCA::WinampIn::~WinampIn()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::WinampIn::Activate()
{
	/* Get the correct plugin and set callback functions.
	 */
	plugin = GetPluginForFile(track.origFilename);

	plugin->SetInfo			= SetInfo;
	plugin->VSASetInfo		= VSASetInfo;
	plugin->VSAAddPCMData		= VSAAddPCMData;
	plugin->VSAGetMode		= VSAGetMode;
	plugin->VSAAdd			= VSAAdd;
	plugin->SAVSAInit		= SAVSAInit;
	plugin->SAVSADeInit		= SAVSADeInit;
	plugin->SAAddPCMData		= SAAddPCMData;
	plugin->SAGetMode		= SAGetMode;
	plugin->SAAdd			= SAAdd;
	plugin->dsp_isactive		= dsp_isactive;
	plugin->dsp_dosamples		= dsp_dosamples;

	plugin->outMod			= new Out_Module();

	plugin->outMod->Open		= Out_Open;
	plugin->outMod->Close		= Out_Close;
	plugin->outMod->Flush		= Out_Flush;
	plugin->outMod->Write		= Out_Write;
	plugin->outMod->CanWrite	= Out_CanWrite;
	plugin->outMod->IsPlaying	= Out_IsPlaying;
	plugin->outMod->Pause		= Out_Pause;
	plugin->outMod->SetVolume	= Out_SetVolume;
	plugin->outMod->SetPan		= Out_SetPan;
	plugin->outMod->GetOutputTime	= Out_GetOutputTime;
	plugin->outMod->GetWrittenTime	= Out_GetWrittenTime;

	/* Create and setup mutex.
	 */
	samplesBufferMutex = new Mutex();
	samplesBufferMutex->Lock();

	filter = this;

	/* Copy the file and play the temporary copy
	 * if the file name contains Unicode characters.
	 */
	if (String::IsUnicode(track.origFilename))
	{
		File(track.origFilename).Copy(Utilities::GetNonUnicodeTempFileName(track.origFilename).Append(".in"));

		plugin->Play(Utilities::GetNonUnicodeTempFileName(track.origFilename).Append(".in"));
	}
	else
	{
		plugin->Play(track.origFilename);
	}

	return True;
}

Bool BoCA::WinampIn::Deactivate()
{
	samplesBufferMutex->Release();

	plugin->Stop();

	delete plugin->outMod;
	delete samplesBufferMutex;

	/* Remove temporary copy if necessary.
	 */
	if (String::IsUnicode(track.origFilename))
	{
		File(Utilities::GetNonUnicodeTempFileName(track.origFilename).Append(".in")).Delete();
	}

	return True;
}

Int BoCA::WinampIn::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	/* Give the plugin one second to send more samples.
	 */
	samplesBufferMutex->Release();

	Int	 start = S::System::System::Clock();

	while (S::System::System::Clock() - start < 1000 && samplesBuffer.Size() <= 0) S::System::System::Sleep(0);

	samplesBufferMutex->Lock();

	/* Copy samples to output buffer.
	 */
	size = samplesBuffer.Size();

	data.Resize(size);

	memcpy(data, samplesBuffer, size);

	samplesBuffer.Resize(0);

	/* Set inBytes to a value that reflects
	 * our approximate position in the file.
	 */
	samplesDone += size / track.GetFormat().channels / (track.GetFormat().bits / 8);
	inBytes = track.fileSize * samplesDone / track.approxLength;

	if (size == 0)	return -1;
	else		return size;
}

ConfigLayer *BoCA::WinampIn::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureWinampIn();

	return configLayer;
}

In_Module *BoCA::WinampIn::GetPluginForFile(const String &file) const
{
	In_Module	*plugin = NIL;

	for (Int i = 0; i < winamp_in_plugins.Length(); i++)
	{
		Int		 n = 1;

		for (Int j = 0; True; j++)
		{
			String	 value = winamp_in_modules.GetNth(i)->FileExtensions + j;

			if (n & 1)
			{
				String	 extension;
				Int	 o = 0;

				for (Int m = 0; m < value.Length(); m++)
				{
					extension[m - o] = value[m];

					if (value[m + 1] == ';' || value[m + 1] == 0)
					{
						if (extension.ToLower() == file.Tail(extension.Length()).ToLower())
						{
							plugin = winamp_in_modules.GetNth(i);

							break;
						}

						m++;
						o = m + 1;
						extension = "";
					}
				}

				if (plugin != NIL) break;
			}

			j += value.Length();
			n++;

			if (winamp_in_modules.GetNth(i)->FileExtensions[j] == 0 && winamp_in_modules.GetNth(i)->FileExtensions[j + 1] == 0) break;
		}
	}

	return plugin;
}

void BoCA::SetInfo(int bitrate, int srate, int stereo, int synched)
{
}

void BoCA::VSASetInfo(int nch, int srate)
{
}

void BoCA::VSAAddPCMData(void *PCMData, int nch, int bps, int timestamp)
{
}

int BoCA::VSAGetMode(int *specNch, int *waveNch)
{
	return 0;
}

void BoCA::VSAAdd(void *data, int timestamp)
{
}

void BoCA::SAVSAInit(int latency, int srate)
{
}

void BoCA::SAVSADeInit()
{
}

void BoCA::SAAddPCMData(void *PCMData, int nch, int bps, int timestamp)
{
}

int BoCA::SAGetMode()
{
	return 0;
}

void BoCA::SAAdd(void *data, int timestamp, int csa)
{
}

int BoCA::dsp_isactive()
{
	return false;
}

int BoCA::dsp_dosamples(short int *samples, int numsamples, int bps, int nch, int srate)
{
	return numsamples;
}

int BoCA::Out_Open(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms)
{
	if (filter->infoTrack == NIL) return 0;

	Format	 format = filter->infoTrack->GetFormat();

	format.order	= BYTE_INTEL;
	format.channels	= numchannels;
	format.rate	= samplerate;
	format.bits	= bitspersamp;

	filter->infoTrack->SetFormat(format);

	return 0;
}

void BoCA::Out_Close()
{
}

void BoCA::Out_Flush(int t)
{
}

int BoCA::Out_Write(char *buf, int len)
{
	filter->samplesBufferMutex->Lock();

	Int	 oSize = filter->samplesBuffer.Size();

	filter->samplesBuffer.Resize(oSize + len);

	memcpy(filter->samplesBuffer + oSize, buf, len);

	filter->samplesBufferMutex->Release();

	return 0;
}

int BoCA::Out_CanWrite()
{
	return Math::Max(0, 32768 - filter->samplesBuffer.Size());
}

int BoCA::Out_IsPlaying()
{
	return true;
}

int BoCA::Out_Pause(int pause)
{
	return 0;
}

void BoCA::Out_SetVolume(int vol)
{
}

void BoCA::Out_SetPan(int pan)
{
}

int BoCA::Out_GetOutputTime()
{
	return 0;
}

int BoCA::Out_GetWrittenTime()
{
	return 0;
}
