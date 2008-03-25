 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

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
						extension = "";
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

int		 channels		= 0;
int		 rate			= 0;
int		 bits			= 16;

int		 get_more_samples	= 0;
int		 n_samples		= 0;
Buffer<char>	 sampleBuffer;

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
void		 SetVolume(int);
void		 SetPan(int);

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

Bool BoCA::WinampIn::CanOpenStream(const String &streamURI)
{
	return (GetPluginForFile(streamURI) != NIL);
}

Error BoCA::WinampIn::GetStreamInfo(const String &streamURI, Track &format)
{
	InStream	*f_in = new InStream(STREAM_FILE, streamURI, IS_READONLY);

	format.order	= BYTE_INTEL;
	format.fileSize	= f_in->Size();

	delete f_in;

	plugin = GetPluginForFile(streamURI);

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
	plugin->SetVolume		= SetVolume;
	plugin->SetPan			= SetPan;

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

	/* Copy the file and play the temporary copy
	 * if the file name contains Unicode characters.
	 */
	if (String::IsUnicode(streamURI))
	{
		File(streamURI).Copy(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".in"));

		plugin->Play(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".in"));
	}
	else
	{
		plugin->Play(streamURI);
	}

	get_more_samples = 1;

	while (rate == 0) Sleep(10);

	int	 length_ms;
	char	*title = new char [1024];

	plugin->GetFileInfo(NIL, title, &length_ms);

	plugin->Stop();

	delete plugin->outMod;

	/* Remove temporary copy if necessary.
	 */
	if (String::IsUnicode(streamURI))
	{
		File(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".in")).Delete();
	}

	format.rate	= rate;
	format.channels	= channels;
	format.bits	= bits;

	format.length	= (Int) (Float(length_ms) * Float(rate * channels) / 1000.0);

	String	 trackTitle = title;

	delete [] title;

	Int	 artistComplete = 0;

	if (trackTitle != streamURI)
	{
		for (Int m = 0; m < trackTitle.Length(); m++)
		{
			if (trackTitle[  m  ] == ' ' &&
			    trackTitle[m + 1] == '-' &&
			    trackTitle[m + 2] == ' ')
			{
				artistComplete = (m += 3);

				format.title = NIL;
			}

			if (!artistComplete)	format.artist[m] = trackTitle[m];
			else			format.title[m - artistComplete] = trackTitle[m];
		}
	}

	if (artistComplete == 0)
	{
		format.artist = NIL;
		format.title = NIL;
	}

	return Success();
}

BoCA::WinampIn::WinampIn()
{
	configLayer		= NIL;

	plugin			= NIL;

	channels		= 0;
	rate			= 0;
	bits			= 16;

	get_more_samples	= 0;
	n_samples		= 0;

	packageSize		= 0;
}

BoCA::WinampIn::~WinampIn()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::WinampIn::Activate()
{
	plugin = GetPluginForFile(format.origFilename);

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
	plugin->SetVolume		= SetVolume;
	plugin->SetPan			= SetPan;

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

	/* Copy the file and play the temporary copy
	 * if the file name contains Unicode characters.
	 */
	if (String::IsUnicode(format.origFilename))
	{
		File(format.origFilename).Copy(Utilities::GetNonUnicodeTempFileName(format.origFilename).Append(".in"));

		plugin->Play(Utilities::GetNonUnicodeTempFileName(format.origFilename).Append(".in"));
	}
	else
	{
		plugin->Play(format.origFilename);
	}

	return True;
}

Bool BoCA::WinampIn::Deactivate()
{
	plugin->Stop();

	delete plugin->outMod;

	/* Remove temporary copy if necessary.
	 */
	if (String::IsUnicode(format.origFilename))
	{
		File(Utilities::GetNonUnicodeTempFileName(format.origFilename).Append(".in")).Delete();
	}

	return True;
}

Int BoCA::WinampIn::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	get_more_samples	= 32768;
	n_samples		= 0;

	Int	 count = 0;

	while (get_more_samples > 0)
	{
		if (++count == 10)
		{
			if (n_samples == 0)	return -1;
			else			break;
		}

		Sleep(10);
	}

	get_more_samples = 0;

	size = n_samples * (bits / 8) * channels;

	data.Resize(size);

	memcpy(data, sampleBuffer, size);

	return size;
}

ConfigLayer *BoCA::WinampIn::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureWinampIn();

	return configLayer;
}

Void BoCA::WinampIn::FreeConfigurationLayer()
{
	if (configLayer != NIL)
	{
		delete configLayer;

		configLayer = NIL;
	}
}

In_Module *BoCA::WinampIn::GetPluginForFile(const String &file)
{
	In_Module	*plugin = NIL;

	for (Int i = 0; i < winamp_in_plugins.Length(); i++)
	{
		Int		 n = 1;

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
						if (extension.ToLower() == file.Tail(extension.Length()).ToLower())
						{
							plugin = winamp_in_modules.GetNth(i);

							break;
						}

						m++;
						o = m;
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

void SetInfo(int bitrate, int srate, int stereo, int synched)
{
}

void VSASetInfo(int nch, int srate)
{
	/* Some plugins pass the sampling rate as the first   *
	 * parameter, so we just assume that the larger value *
	 * is the sampling rate, the other is the number of   *
	 * channels.					      */

	channels	= Math::Min((Int) nch, srate);
	rate		= Math::Max((Int) nch, srate);
}

void VSAAddPCMData(void *PCMData, int nch, int bps, int timestamp)
{
}

int VSAGetMode(int *specNch, int *waveNch)
{
	return 0;
}

void VSAAdd(void *data, int timestamp)
{
}

void SAVSAInit(int latency, int srate)
{
}

void SAVSADeInit()
{
}

void SAAddPCMData(void *PCMData, int nch, int bps, int timestamp)
{
}

int SAGetMode()
{
	return 0;
}

void SAAdd(void *data, int timestamp, int csa)
{
}

int dsp_isactive()
{
	return get_more_samples;
}

int dsp_dosamples(short int *samples, int numsamples, int bps, int nch, int srate)
{
	if (n_samples == 0) sampleBuffer.Resize(32768 * (bps / 8) * nch);

	memcpy(sampleBuffer + n_samples * (bps / 8) * nch, samples, numsamples * (bps / 8) * nch);

	get_more_samples -= numsamples;
	n_samples	 += numsamples;

	if (get_more_samples <= 8192) get_more_samples = 0;

	return numsamples;
}

void SetVolume(int vol)
{
}

void SetPan(int pan)
{
}

int Out_Open(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms)
{
	bits = bitspersamp;

	return 25;
}

void Out_Close()
{
}

void Out_Flush(int t)
{
}

int Out_Write(char *buf, int len)
{
	return 0;
}

int Out_CanWrite()
{
	return get_more_samples;
}

int Out_IsPlaying()
{
	return 1;
}

int Out_Pause(int pause)
{
	return 0;
}

void Out_SetVolume(int vol)
{
}

void Out_SetPan(int pan)
{
}

int Out_GetOutputTime()
{
	return 0;
}

int Out_GetWrittenTime()
{
	return 0;
}
