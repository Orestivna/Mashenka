#if ! defined(_SOUND_KUDUSHTEEV_H_)
#define _SOUND_KUDUSHTEEV_H_
#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif
#define SNDDEV_CREATE    1
#define SNDDEV_DEVICE    2
#define SNDDEV_RELEASE   3


#ifdef POWERED_BY_DIRECT_SOUND

#if defined(DIRECTSOUND_VERSION) && (DIRECTSOUND_VERSION < 0x0800)
#define DIRECTSOUND_VERSION  0x0700
#endif

#include <dsound.h>

//класс для проигрывание музыки
class dsound {
private:
	LPDIRECTSOUNDBUFFER snd_buf;
public:
	dsound(void):snd_buf(NULL){}
	~dsound(){ destroy();}

	dsound(const dsound&);
	dsound& operator = (const dsound&);
public:

	bool load(HINSTANCE hinst, LPCSTR res_name, LPCSTR res_type, DWORD flags = DSBCAPS_STATIC){
		input_res res;
		if(!res.load(hinst, res_name, res_type))
			return false;

		sreader rd;
		rd.put(res);

		DWORD riff = 0, dr = 0;

		rd.read(&riff, 4);
		if(riff != MAKEFOURCC('R', 'I', 'F', 'F'))
			return false;
		
		rd.ignore(4);
		rd.read(&riff, 4);
		if(riff != MAKEFOURCC('W', 'A', 'V', 'E'))
			return false;

		rd.read(&riff, 4);
		if(riff != MAKEFOURCC('f', 'm', 't', ' '))
			return false;

		LONG fmt = 0;
		rd.read(&fmt, 4);
		if(fmt == 0)
			return false;

		WAVEFORMATEX wave;
		memset(&wave, 0, sizeof(WAVEFORMATEX));

		rd.read(&wave, sizeof(WAVEFORMATEX));
		rd.ignore(fmt - sizeof(WAVEFORMATEX));
	
		if(wave.wFormatTag != 1)
			return false;

		rd.read(&riff, 4);
		if(riff == MAKEFOURCC('f', 'a', 'c', 't')){
			rd.read(&fmt, 4);
			rd.ignore(fmt);
			rd.read(&riff, 4);
		}

		if(riff != MAKEFOURCC('d', 'a', 't', 'a'))
			return false;

		DWORD size_data = 0;
		rd.read(&size_data, 4);
		if(size_data == 0)
			return false;

		DSBUFFERDESC desc;
		memset(&desc, 0, sizeof(DSBUFFERDESC));
		desc.dwSize         = sizeof(DSBUFFERDESC);
		desc.dwBufferBytes  = size_data;
		desc.dwFlags        = flags;
		desc.lpwfxFormat    = &wave;

		if(FAILED(dsound::getDevice(SNDDEV_DEVICE)->CreateSoundBuffer(&desc, &snd_buf, 0)))
			return false;

		size_data   = 0;
		LPVOID  dst = NULL; // получим адрес буфера для записи в него данных
		if(snd_buf->Lock(0, 0, &dst,  &size_data, 0, 0,  DSBLOCK_ENTIREBUFFER) == DS_OK){
			memcpy(dst, rd.getCurrent(), size_data * sizeof(BYTE));   // копируем внего
			snd_buf->Unlock(dst, size_data, 0, 0);
		}
		res.destroy();
		return true;
	}

	void play(bool loop = false){ 
		if(snd_buf != NULL)
			snd_buf->Play(0, 0, (loop) ? DSBPLAY_LOOPING : 0);
	}

	// Метод останавливает воспроизведение звука
	void stop(void) {
		if(snd_buf != NULL)
			snd_buf->Stop();
	}

	void destroy(void){
		if(snd_buf != NULL){
			snd_buf->Release();
			snd_buf = NULL;
		}
	}

	static IDirectSound* getDevice(int cmd, HWND hwnd = HWND_DESKTOP){
		HRESULT res = S_OK;
		static IDirectSound* dev = NULL;

		switch(cmd){
		case SNDDEV_CREATE:

			if(dev != NULL)
				break;
			::CoInitialize(NULL);

			res = CoCreateInstance(CLSID_DirectSound, NULL, CLSCTX_INPROC_SERVER, IID_IDirectSound, (LPVOID*)&dev);
			if(FAILED(res))
				return NULL;

			dev->Initialize(NULL);
			dev->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
			break;
		case SNDDEV_RELEASE:
			if(dev != NULL){
				dev->Release();
				::CoUninitialize();
			}
			dev = NULL;
			break;
		}
		return dev;
	}

	bool load_test(HINSTANCE hinst, LPCSTR res_name, LPCSTR res_type) { return load(hinst, res_name, res_type); }
};

#else

//класс-звука
class dsound {
private:
	input_res res;
public:
	dsound(void){}
	~dsound(){ destroy();}

	dsound(const dsound&);
	dsound& operator = (const dsound&);
public:

	bool load(HINSTANCE hinst, LPCSTR res_name, LPCSTR res_type, DWORD flags = 0){ 
		destroy();
		return res.load(hinst, res_name, res_type);
	}

	void play(bool loop = false){
		UINT flags = SND_MEMORY | SND_ASYNC;
		if(loop)
			flags |= SND_LOOP;
		sndPlaySoundA(reinterpret_cast<LPCSTR>(res.getData()), flags);
	}

	void stop(void){
		sndPlaySoundA(NULL, 0);
	}

	void destroy(void){
		res.destroy();
	}

	//заглушка
	static LPVOID getDevice(int cmd, HWND hwnd = HWND_DESKTOP){ return NULL; }

	bool load_test(HINSTANCE hinst, LPCSTR res_name, LPCSTR res_type) { return true; }
};

#endif

#endif
