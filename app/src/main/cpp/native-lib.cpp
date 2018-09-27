#include <jni.h>
#include <string>
#include <android/log.h>

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#define  LOGE(...) __android_log_print(ANDROID_LOG_ERROR,"test",__VA_ARGS__)

static SLObjectItf  engineSL = NULL;

SLEngineItf CreatSL()
{
    //a 创建引擎对象

    SLresult re;
    SLEngineItf en; //存引擎接口
    re = slCreateEngine(&engineSL,0,0,0,0,0);
    if (re != SL_RESULT_SUCCESS) return  NULL;

    //b 实例化 SL_BOOLEAN_FALSE等待对象创建
    re = (*engineSL)->Realize(engineSL,SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS) return  NULL;

    //c 获取接口
    re = (*engineSL)->GetInterface(engineSL,SL_IID_ENGINE,&en);
    if (re != SL_RESULT_SUCCESS) return  NULL;

    return en;
}

//回调函数
void pcmCallBack(SLAndroidSimpleBufferQueueItf bf, void*contex)
{
    LOGE("pcmCallBack ");
    static FILE *fp = NULL;
    static  char *buf = NULL;
    if (!buf){
        buf = new char[1024*1024];
    }
    if (!fp){
        //没有声音 check这里
        fp = fopen("/sdcard/test.pcm","rb");
    }
    if(!fp){
        LOGE("fopen failed!");
        return;
    }

    if (feof(fp) == 0){

        int len = fread(buf,1,1024,fp);
        if (len > 0){//读取到数据 数据传入队列
            //声音停顿的话，check参数是否传对
            //声音音调不对的话，可能是pcm文件和pcm设置不对
            (*bf)->Enqueue(bf,buf,len);

        }

    }




}


extern "C" JNIEXPORT jstring

JNICALL
Java_aplayer_openslplayer_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";

    //1 创建引擎
    SLEngineItf eng = CreatSL();
    if (eng)
    {
        LOGE("CreatSL success!");

    } else
    {
        LOGE("CreatSL failed!");
    }

    //2 创建混音器

    //a 输出混音器
    SLObjectItf mix = NULL;
    SLresult re = 0;
    re = (*eng)->CreateOutputMix(eng,&mix,0,0,0);
    if (re != SL_RESULT_SUCCESS)
    {
        LOGE("CreateOutputMix failed!");
    };

    //b 实例化
    re = (*mix)->Realize(mix,SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS)
    {
        LOGE("mix Realize  failed!");
    };
    //输出
    SLDataLocator_OutputMix outmix ={SL_DATALOCATOR_OUTPUTMIX,mix};
    SLDataSink audioSink= {&outmix,0};

    //3 配置音频信息

    //创建缓冲队列
    SLDataLocator_AndroidSimpleBufferQueue que = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,10};

    //音频格式配置（要和测试文件一致，实际使用中音频重采样成同样的格式）
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,
            2,//通道数
            SL_SAMPLINGRATE_44_1,//采样率
            SL_PCMSAMPLEFORMAT_FIXED_16, // bitsPerSample
            SL_PCMSAMPLEFORMAT_FIXED_16,// containerSize
            SL_SPEAKER_FRONT_LEFT|SL_SPEAKER_FRONT_RIGHT,//声道
            SL_BYTEORDER_LITTLEENDIAN //字节序,小端
    };
    //播放器使用的结构体
    SLDataSource ds = {&que,&pcm};

    // 4 创建播放器
    //播放器对象
    SLObjectItf  player = NULL;
    SLPlayItf playerInterface = NULL;
    SLAndroidSimpleBufferQueueItf pcmQue = NULL;
    const SLInterfaceID ids[] = {SL_IID_BUFFERQUEUE};//接口id
    const SLboolean  req[] = {SL_BOOLEAN_TRUE};//接口开放
    //sizeof(ids)/sizeof(SLInterfaceID) 参数个数
    re = (*eng)->CreateAudioPlayer(eng,&player,&ds,&audioSink, sizeof(ids)/sizeof(SLInterfaceID),ids,req);
    if (re != SL_RESULT_SUCCESS)
    {
        LOGE("CreateAudioPlayer  failed!");
    } else{
        LOGE("CreateAudioPlayer  success!");
    };
    //实例化
    (*player)->Realize(player,SL_BOOLEAN_FALSE);
    //获取接口
    re = (*player)->GetInterface(player,SL_IID_PLAY,&playerInterface);
    if (re != SL_RESULT_SUCCESS)
    {
        LOGE("player GetInterface  failed!");
    } else{
        LOGE("player GetInterface  success!");
    };
    //获取缓冲队列接口
    re =(*player)->GetInterface(player,SL_IID_BUFFERQUEUE,&pcmQue);
    if (re != SL_RESULT_SUCCESS)
    {
        LOGE("player GetInterface BUFFERQUEUE failed!");
    } else{
        LOGE("player GetInterface BUFFERQUEUE success!");
    };

    //设置回调函数,播放队列为空的时候调用
    /*
     * 第二个参数是回调函数 第三个参数是 给回调函数传的参数
     */
    (*pcmQue)->RegisterCallback(pcmQue,pcmCallBack,0);
    //设置状态 播放
    (*playerInterface)->SetPlayState(playerInterface,SL_PLAYSTATE_PLAYING);

    //启动队列回调
    (*pcmQue)->Enqueue(pcmQue,"",1);








    return env->NewStringUTF(hello.c_str());
}
