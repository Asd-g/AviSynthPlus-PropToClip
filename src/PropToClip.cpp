#include <cstdlib>
#include <cstring>
#include <string>
#include <map>

#include "avisynth.h"

static const std::map<int, std::string> pixel_format_table =
{ // names for lookup by pixel_type or name
  {VideoInfo::CS_BGR24, "RGB24"},
  {VideoInfo::CS_BGR32, "RGB32"},
  {VideoInfo::CS_YUY2 , "YUY2"},
  {VideoInfo::CS_YV24 , "YV24"},
  {VideoInfo::CS_YV16 , "YV16"},
  {VideoInfo::CS_YV12 , "YV12"},
  {VideoInfo::CS_I420 , "YV12"},
  {VideoInfo::CS_YUV9 , "YUV9"},
  {VideoInfo::CS_YV411, "YV411"},
  {VideoInfo::CS_Y8   , "Y8"},

  {VideoInfo::CS_YUV420P10, "YUV420P10"},
  {VideoInfo::CS_YUV422P10, "YUV422P10"},
  {VideoInfo::CS_YUV444P10, "YUV444P10"},
  {VideoInfo::CS_Y10      , "Y10"},
  {VideoInfo::CS_YUV420P12, "YUV420P12"},
  {VideoInfo::CS_YUV422P12, "YUV422P12"},
  {VideoInfo::CS_YUV444P12, "YUV444P12"},
  {VideoInfo::CS_Y12      , "Y12"},
  {VideoInfo::CS_YUV420P14, "YUV420P14"},
  {VideoInfo::CS_YUV422P14, "YUV422P14"},
  {VideoInfo::CS_YUV444P14, "YUV444P14"},
  {VideoInfo::CS_Y14      , "Y14"},
  {VideoInfo::CS_YUV420P16, "YUV420P16"},
  {VideoInfo::CS_YUV422P16, "YUV422P16"},
  {VideoInfo::CS_YUV444P16, "YUV444P16"},
  {VideoInfo::CS_Y16      , "Y16"},
  {VideoInfo::CS_YUV420PS , "YUV420PS"},
  {VideoInfo::CS_YUV422PS , "YUV422PS"},
  {VideoInfo::CS_YUV444PS , "YUV444PS"},
  {VideoInfo::CS_Y32      , "Y32"},

  {VideoInfo::CS_BGR48    , "RGB48"},
  {VideoInfo::CS_BGR64    , "RGB64"},

  {VideoInfo::CS_RGBP     , "RGBP"},
  {VideoInfo::CS_RGBP10   , "RGBP10"},
  {VideoInfo::CS_RGBP12   , "RGBP12"},
  {VideoInfo::CS_RGBP14   , "RGBP14"},
  {VideoInfo::CS_RGBP16   , "RGBP16"},
  {VideoInfo::CS_RGBPS    , "RGBPS"},

  {VideoInfo::CS_YUVA420, "YUVA420"},
  {VideoInfo::CS_YUVA422, "YUVA422"},
  {VideoInfo::CS_YUVA444, "YUVA444"},
  {VideoInfo::CS_YUVA420P10, "YUVA420P10"},
  {VideoInfo::CS_YUVA422P10, "YUVA422P10"},
  {VideoInfo::CS_YUVA444P10, "YUVA444P10"},
  {VideoInfo::CS_YUVA420P12, "YUVA420P12"},
  {VideoInfo::CS_YUVA422P12, "YUVA422P12"},
  {VideoInfo::CS_YUVA444P12, "YUVA444P12"},
  {VideoInfo::CS_YUVA420P14, "YUVA420P14"},
  {VideoInfo::CS_YUVA422P14, "YUVA422P14"},
  {VideoInfo::CS_YUVA444P14, "YUVA444P14"},
  {VideoInfo::CS_YUVA420P16, "YUVA420P16"},
  {VideoInfo::CS_YUVA422P16, "YUVA422P16"},
  {VideoInfo::CS_YUVA444P16, "YUVA444P16"},
  {VideoInfo::CS_YUVA420PS , "YUVA420PS"},
  {VideoInfo::CS_YUVA422PS , "YUVA422PS"},
  {VideoInfo::CS_YUVA444PS , "YUVA444PS"},

  {VideoInfo::CS_RGBAP     , "RGBAP"},
  {VideoInfo::CS_RGBAP10   , "RGBAP10"},
  {VideoInfo::CS_RGBAP12   , "RGBAP12"},
  {VideoInfo::CS_RGBAP14   , "RGBAP14"},
  {VideoInfo::CS_RGBAP16   , "RGBAP16"},
  {VideoInfo::CS_RGBAPS    , "RGBAPS"},
};

static const std::multimap<int, std::string> pixel_format_table_ex =
{ // alternative names for lookup by name (multimap!)
  {VideoInfo::CS_YV24 , "YUV444"},
  {VideoInfo::CS_YV16 , "YUV422"},
  {VideoInfo::CS_YV12 , "YUV420"},
  {VideoInfo::CS_YV411, "YUV411"},
  {VideoInfo::CS_RGBP , "RGBP8"},
  {VideoInfo::CS_RGBAP, "RGBAP8"},
  {VideoInfo::CS_YV24 , "YUV444P8"},
  {VideoInfo::CS_YV16 , "YUV422P8"},
  {VideoInfo::CS_YV12 , "YUV420P8"},
  {VideoInfo::CS_YV411, "YUV411P8"},
  {VideoInfo::CS_YUVA420, "YUVA420P8"},
  {VideoInfo::CS_YUVA422, "YUVA422P8"},
  {VideoInfo::CS_YUVA444, "YUVA444P8"},
};

class PropToClip : public GenericVideoFilter
{
    char* prop;

public:
    PropToClip(PClip _child, const char* prop_, const char* pixel_type, IScriptEnvironment* env)
        : GenericVideoFilter(_child)
    {
        PVideoFrame src = child->GetFrame(0, env);

        int err;
        PVideoFrame msrc = env->propGetFrame(env->getFramePropsRO(src), prop_, 0, &err);
        if (err)
            env->ThrowError("PropToClip: no frame stored in property");

        prop = reinterpret_cast<char*>(malloc(strlen(prop_) + 1));
        strcpy(prop, prop_);

        vi.pixel_type = VideoInfo::CS_UNKNOWN;
        std::string name_to_find = pixel_type;
        for (auto& c : name_to_find)
            c = toupper(c); // uppercase input string
        for (auto it = pixel_format_table.begin(); it != pixel_format_table.end(); ++it)
        {
            if ((it->second).compare(name_to_find) == 0)
                vi.pixel_type = it->first;
        }
        // find by alternative names e.g. YUV420 or YUV420P8 instead of YV12
        for (auto it = pixel_format_table_ex.begin(); it != pixel_format_table_ex.end(); ++it)
        {
            if ((it->second).compare(name_to_find) == 0)
                vi.pixel_type = it->first;
        }

        if (vi.pixel_type == VideoInfo::CS_UNKNOWN)
            env->ThrowError("PropToClip: unknown pixel_type %s", pixel_type);
        
        vi.width = msrc->GetRowSize();
        vi.height = msrc->GetHeight();
    }

    PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env) override
    {
        return env->propGetFrame(env->getFramePropsRO(child->GetFrame(n, env)), prop, 0, nullptr);
    }

    int __stdcall SetCacheHints(int cachehints, int frame_range) override
    {
        return cachehints == CACHE_GET_MTMODE ? MT_NICE_FILTER : 0;
    }

    ~PropToClip()
    {
        free(prop);
    }
};

AVSValue __cdecl Create_PropToClip(AVSValue args, void* user_data, IScriptEnvironment* env)
{
    return new PropToClip(
        args[0].AsClip(),
        args[1].AsString("_Alpha"),
        args[2].AsString("Y8"),
        env);
}

const AVS_Linkage* AVS_linkage;

extern "C" __declspec(dllexport)
const char* __stdcall AvisynthPluginInit3(IScriptEnvironment * env, const AVS_Linkage* const vectors)
{
    AVS_linkage = vectors;

    env->AddFunction("PropToClip", "c[prop]s[pixel_type]s", Create_PropToClip, 0);
    return "PropToClip";
}
