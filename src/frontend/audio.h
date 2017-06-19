#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED

#include <cstdint>
#include <cstring>

#include "exceptions.h"
#include "lib/al.h"
#include "math.h"
#include "system.h"
#include "utils.h"

namespace Audio
{
    #ifdef E0INTERNAL_AUDIO_H_SPECIAL_ACCESS
    void Initialize();
    void Cleanup();
    void Tick();
    #endif

    namespace Init
    {
        void Frequency(int);
        void MonoSources(int);
        void StereoSources(int);
    }

    namespace Config
    {
        void ReferenceDistance(float);
        void MaxDistance(float);
        void RolloffFactor(float);
    }

    void ForceErrorCheck();

    inline void Volume(float vol)
    {
        alListenerf(AL_GAIN, vol);
    }
    inline void Pitch(float pitch)
    {
        alListenerf(AL_PITCH, pitch);
    }

    inline void ListenerPos(const fvec3 &v)
	{
		alListenerfv(AL_POSITION, v.as_array());
	}
	inline void ListenerVelocity(const fvec3 &v)
	{
		alListenerfv(AL_VELOCITY, v.as_array());
	}
	inline void ListenerRot(const fvec3 &forw, const fvec3 &up)
	{
		fvec3 arr[2] = {forw, up};
		alListenerfv(AL_ORIENTATION, (float *)arr);
	}

	inline void DopplerFactor(float n) {alDopplerFactor(n);}
	inline void SpeedOfSound (float n) {alSpeedOfSound(n);}

    enum class SoundFormat
    {
        mono8    = AL_FORMAT_MONO8,
        mono16   = AL_FORMAT_MONO16,
        stereo8  = AL_FORMAT_STEREO8,
        stereo16 = AL_FORMAT_STEREO16,
    };

    class SoundData
    {
        std::vector<uint8_t> data;
        uint32_t size; // Amount of samples.
        uint32_t freq;
        SoundFormat format;
      public:
        static unsigned int FormatBytesPerSample(SoundFormat format)
        {
            switch (format)
            {
                case SoundFormat::mono8:    return 1;
                case SoundFormat::mono16:   return 2;
                case SoundFormat::stereo8:  return 2;
                case SoundFormat::stereo16: return 4;
            }
            return 0;
        }

        void LoadWAV(Utils::BinaryInput io);
        void SaveWAV(Utils::BinaryOutput io);
        void LoadCompressed(Utils::BinaryInput io);
        void SaveCompressed(Utils::BinaryOutput io);
        void LoadOGG(Utils::BinaryInput io, bool load_as_8bit = 0);
        void LoadWAV_Mono(Utils::BinaryInput io)
        {
            LoadWAV(io.Move());
            if (Stereo())
                Exceptions::IO::BadData(io.Name(), "Mono sound", "Stereo sound");
        }
        void LoadCompressed_Mono(Utils::BinaryInput io)
        {
            LoadCompressed(io.Move());
            if (Stereo())
                Exceptions::IO::BadData(io.Name(), "Mono sound", "Stereo sound");
        }
        void LoadOGG_Mono(Utils::BinaryInput io, bool load_as_8bit = 0)
        {
            LoadOGG(io.Move(), load_as_8bit);
            if (Stereo())
                Exceptions::IO::BadData(io.Name(), "Mono sound", "Stereo sound");
        }
        void LoadWAV_Stereo(Utils::BinaryInput io)
        {
            LoadWAV(io.Move());
            if (Mono())
                Exceptions::IO::BadData(io.Name(), "Stereo sound", "Mono sound");
        }
        void LoadCompressed_Stereo(Utils::BinaryInput io)
        {
            LoadCompressed(io.Move());
            if (Mono())
                Exceptions::IO::BadData(io.Name(), "Stereo sound", "Mono sound");
        }
        void LoadOGG_Stereo(Utils::BinaryInput io, bool load_as_8bit = 0)
        {
            LoadOGG(io.Move(), load_as_8bit);
            if (Mono())
                Exceptions::IO::BadData(io.Name(), "Stereo sound", "Mono sound");
        }

        static SoundData FromWAV              (Utils::BinaryInput io) {SoundData ret; ret.LoadWAV              (io.Move()); return ret;}
        static SoundData FromWAV_Mono         (Utils::BinaryInput io) {SoundData ret; ret.LoadWAV_Mono         (io.Move()); return ret;}
        static SoundData FromWAV_Stereo       (Utils::BinaryInput io) {SoundData ret; ret.LoadWAV_Stereo       (io.Move()); return ret;}
        static SoundData FromCompressed       (Utils::BinaryInput io) {SoundData ret; ret.LoadCompressed       (io.Move()); return ret;}
        static SoundData FromCompressed_Mono  (Utils::BinaryInput io) {SoundData ret; ret.LoadCompressed_Mono  (io.Move()); return ret;}
        static SoundData FromCompressed_Stereo(Utils::BinaryInput io) {SoundData ret; ret.LoadCompressed_Stereo(io.Move()); return ret;}
        static SoundData FromOGG              (Utils::BinaryInput io, bool load_as_8bit = 0) {SoundData ret; ret.LoadOGG       (io.Move(), load_as_8bit); return ret;}
        static SoundData FromOGG_Mono         (Utils::BinaryInput io, bool load_as_8bit = 0) {SoundData ret; ret.LoadOGG_Mono  (io.Move(), load_as_8bit); return ret;}
        static SoundData FromOGG_Stereo       (Utils::BinaryInput io, bool load_as_8bit = 0) {SoundData ret; ret.LoadOGG_Stereo(io.Move(), load_as_8bit); return ret;}


        void Empty(uint32_t new_size, SoundFormat new_format)
        {
            Clear();
            size = new_size;
            format = new_format;
            data.resize(ByteSize());
        }
        void LoadFromMem(uint32_t new_size, SoundFormat new_format, uint8_t *mem)
        {
            Empty(new_size, new_format);
            std::memcpy(data.data(), mem, ByteSize());
        }
        void Clear()
        {
            size = 0;
            data.clear();
        }
        void *Data()
        {
            return data.data();
        }
        const void *Data() const
        {
            return data.data();
        }
        uint32_t Size() const // Amount of samples, not bytes.
        {
            return size;
        }
        uint32_t ByteSize() const
        {
            return size * FormatBytesPerSample(format);
        }
        void SetFrequency(uint32_t new_freq)
        {
            freq = new_freq;
        }
        uint32_t Frequency() const
        {
            return freq;
        }
        SoundFormat Format() const
        {
            return format;
        }
        bool Mono() const
        {
            return format == SoundFormat::mono8 || format == SoundFormat::mono16;
        }
        bool Stereo() const
        {
            return format == SoundFormat::stereo8 || format == SoundFormat::stereo16;
        }
        bool Has8BitsPerSample() const
        {
            return format == SoundFormat::mono8 || format == SoundFormat::stereo8;
        }
        bool Has16BitsPerSample() const
        {
            return format == SoundFormat::mono16 || format == SoundFormat::stereo16;
        }
        uint8_t &AtMono8   (uint32_t pos) {return *(uint8_t *)(&data[pos  ]);}
        int16_t &AtMono16  (uint32_t pos) {return *(int16_t *)(&data[pos*2]);}
        u8vec2  &AtStereo8 (uint32_t pos) {return *(u8vec2  *)(&data[pos*2]);}
        i16vec2 &AtStereo16(uint32_t pos) {return *(i16vec2 *)(&data[pos*4]);}
        uint8_t AtMono8   (uint32_t pos) const {return *(uint8_t *)(&data[pos  ]);}
        int16_t AtMono16  (uint32_t pos) const {return *(int16_t *)(&data[pos*2]);}
        u8vec2  AtStereo8 (uint32_t pos) const {return *(u8vec2  *)(&data[pos*2]);}
        i16vec2 AtStereo16(uint32_t pos) const {return *(i16vec2 *)(&data[pos*4]);}
        SoundData()
        {
            size = 0;
            freq = 0;
            format = SoundFormat::mono8;
        }
        SoundData(const void *from, uint32_t samples, SoundFormat data_format, uint32_t frequency)
        {
            size = samples;
            freq = frequency;
            format = data_format;
            data.resize(ByteSize());
            if (from)
                std::memcpy(data.data(), from, ByteSize());
        }
        ~SoundData()
        {
            Clear();
        }
    };

    class Buffer
    {
        ALuint handle;
        bool stereo;
      public:
        void SetData(const SoundData &data)
        {
            stereo = data.Stereo();
            alBufferData(handle, (ALenum)data.Format(), data.Data(), data.ByteSize(), data.Frequency());
        }

        bool Mono()   const {return !stereo;}
        bool Stereo() const {return  stereo;}

        ALuint Handle() const {return handle;}

        Buffer()
        {
            alGenBuffers(1, &handle);
        }
        Buffer(const SoundData &data) : Buffer()
        {
            SetData(data);
        }

        void Play(const fvec3 &pos, float volume = 1, float pitch = 1) const;
        void Play(float volume = 1, float pitch = 1) const;


        ~Buffer()
        {
            alDeleteBuffers(1, &handle);
        }
    };

    enum class SourceMode
    {
        once,             // Plays sound once.       Sound keeps playing if source object is destroyed.
        once_managed,     // Plays sound once.       Sound STOPS playing if source object is destroyed.
        loop,             // Plays sound infinitely. Sound STOPS playing if source object is destroyed.
    };

    class Source
    {
        int id;
        bool stereo;

        Source(const Source &) = delete;
        Source(Source &&) = delete;
        Source &operator=(const Source &) = delete;
        Source &operator=(Source &&) = delete;
      public:

        Source()                                {id = -1;}
        Source(const Buffer &buffer) : Source() {Open(buffer);}

        void Open(const Buffer &buffer);
        void Close();
        bool Opened() const {return id != -1;}

        bool Mono()   const {return !stereo;}
        bool Stereo() const {return  stereo;}

        void Mode(SourceMode mode);
        void RefDistance(float n);
        void MaxDistance(float n);
        void RolloffPactor(float n);
        void Volume(float n);
        void Pitch(float n);
        void Pos(fvec3 n);
        void Vel(fvec3 n);
        void RelativePosition(bool n);

        bool Playing() const;

        void Play();
        void Stop();

        static int ActiveMono();
        static int ActiveStereo();
        static int MaxMono();
        static int MaxStereo();

        ~Source() {Close();}
    };
}

#endif
