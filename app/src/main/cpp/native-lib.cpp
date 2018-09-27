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

extern "C" JNIEXPORT jstring

JNICALL
Java_aplayer_openslplayer_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";

    //1 创建引擎
    SLEngineItf  eng = CreatSL();
    if (eng)
    {
        LOGE("CreatSL success!");

    } else
    {
        LOGE("CreatSL failed!");
    }




















    return env->NewStringUTF(hello.c_str());
}
