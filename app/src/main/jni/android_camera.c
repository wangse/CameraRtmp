#include "android_camera.h"
#include <android/log.h>

#define TAG "AndroidDemuxer"
#define LOG(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)

#define ANDROID_DEMUXER_CLASS_NAME "org/ffmpeg/device/AndroidDemuxer"

JniGetStaticMethodInfo_t jniGetStaticMethodInfo = NULL;

static AndroidDemuxerCB_t _demuxerCB = NULL;

typedef struct imageFormatName{
    int imageFormat;
    const char * imageFormatName;
} imageFormatName_t;
#define IMAGEFORMAT_NAME(a) {a,#a}
static imageFormatName_t _ifns[] = {
        IMAGEFORMAT_NAME(FLEX_RGBA_8888),
        IMAGEFORMAT_NAME(FLEX_RGB_888),
        IMAGEFORMAT_NAME(JPEG),
        IMAGEFORMAT_NAME(NV16),
        IMAGEFORMAT_NAME(NV21),
        IMAGEFORMAT_NAME(PRIVATE),
        IMAGEFORMAT_NAME(RAW10),
        IMAGEFORMAT_NAME(RAW12),
        IMAGEFORMAT_NAME(RAW_PRIVATE),
        IMAGEFORMAT_NAME(RAW_SENSOR),
        IMAGEFORMAT_NAME(RGB_565),
        IMAGEFORMAT_NAME(YUV_420_888),
        IMAGEFORMAT_NAME(YUV_422_888),
        IMAGEFORMAT_NAME(YUV_444_888),
        IMAGEFORMAT_NAME(YUY2),
        IMAGEFORMAT_NAME(DEPTH_POINT_CLOUD),
        IMAGEFORMAT_NAME(YV12),
        IMAGEFORMAT_NAME(DEPTH16),
};

/*
 * 输入android图像格式标识，返回格式名称
 */
const char * android_ImageFormatName( int ifn )
{
    for(int i=0;i<sizeof(_ifns)/sizeof(imageFormatName_t);i++){
        if(_ifns[i].imageFormat==ifn)
            return _ifns[i].imageFormatName;
    }
    return NULL;
}

/*
 * 输入格式名称返回格式标识
 */
int android_ImageFormat(const char * fn)
{
    for(int i=0;i<sizeof(_ifns)/sizeof(imageFormatName_t);i++){
        if(strcmp(_ifns[i].imageFormatName,fn)==0)
            return _ifns[i].imageFormat;
    }
    return -1;
}

/*
 * 取得Android摄像机数量
 */
int android_getNumberOfCameras()
{
    struct JniMethodInfo jmi;
    int ret = -1;
    if(jniGetStaticMethodInfo(&jmi,ANDROID_DEMUXER_CLASS_NAME,"getNumberOfCameras","()I")) {
        ret = (*jmi.env)->CallStaticIntMethod(jmi.env,jmi.classID,jmi.methodID);
        (*jmi.env)->DeleteLocalRef(jmi.env,jmi.classID);
    }
    return ret;
}

/*
 * 取得Android摄像机信息
 */
int android_getCameraCapabilityInteger(int n, int *pinfo,int len)
{
    struct JniMethodInfo jmi;
    int ret = -1;
    if(jniGetStaticMethodInfo(&jmi,ANDROID_DEMUXER_CLASS_NAME,"getCameraCapabilityInteger","(I[I)I")) {
        jintArray infoObj = (*jmi.env)->NewIntArray(jmi.env,1024*64);
        if(infoObj) {
            ret = (*jmi.env)->CallStaticIntMethod(jmi.env,jmi.classID, jmi.methodID, n, infoObj);
            if (ret > 0 && ret <= len) {
                //   jboolean isCopy = JNI_TRUE;
                jint *info = (*jmi.env)->GetIntArrayElements(jmi.env,infoObj, 0);
                memcpy(pinfo, info, len);
                (*jmi.env)->ReleaseIntArrayElements(jmi.env,infoObj, info, JNI_ABORT);
            }
            (*jmi.env)->DeleteLocalRef(jmi.env,infoObj);
        }
        (*jmi.env)->DeleteLocalRef(jmi.env,jmi.classID);
    }
    return ret;
}

int android_openDemuxer(int tex,int nDevice, int w, int h, int fmt, int fps,
                        int nChannel, int sampleFmt, int sampleRate)
{
    struct JniMethodInfo jmi;
    int ret = -1;
    if(jniGetStaticMethodInfo(&jmi,ANDROID_DEMUXER_CLASS_NAME,"openDemuxer","(IIIIIIIII)I")) {
        ret = (*jmi.env)->CallStaticIntMethod(jmi.env,jmi.classID,jmi.methodID,tex,nDevice,w,h,fmt,
                                           fps,nChannel,sampleFmt,sampleRate);
        (*jmi.env)->DeleteLocalRef(jmi.env,jmi.classID);
    }
    return ret;
}

void android_updatePreivewTexture(float * pTextureMatrix)
{
    if(!pTextureMatrix)return;

    struct JniMethodInfo jmi;
    if(jniGetStaticMethodInfo(&jmi,ANDROID_DEMUXER_CLASS_NAME,"update","([F)Z")) {
        jfloatArray jfa = (*jmi.env)->NewFloatArray(jmi.env,16);
        (*jmi.env)->CallStaticVoidMethod(jmi.env,jmi.classID,jmi.methodID,jfa);
        jfloat * jbuf = (*jmi.env)->GetFloatArrayElements(jmi.env,jfa,0);
        memcpy(pTextureMatrix,jbuf,sizeof(float)*16);
        (*jmi.env)->ReleaseFloatArrayElements(jmi.env,jfa,jbuf,JNI_ABORT);
        (*jmi.env)->DeleteLocalRef(jmi.env,jmi.classID);
    }
}

int64_t android_getPreivewFrameCount()
{
    struct JniMethodInfo jmi;
    if(jniGetStaticMethodInfo(&jmi,ANDROID_DEMUXER_CLASS_NAME,"getGrabFrameCount","(Z)I")) {
        int64_t ret = (int64_t)(*jmi.env)->CallStaticLongMethod(jmi.env,jmi.classID,jmi.methodID);
        (*jmi.env)->DeleteLocalRef(jmi.env,jmi.classID);
        return ret;
    }
    return -1;
}

void android_closeDemuxer()
{
    struct JniMethodInfo jmi;
    if(jniGetStaticMethodInfo(&jmi,ANDROID_DEMUXER_CLASS_NAME,"closeDemuxer","()V")) {
        (*jmi.env)->CallStaticVoidMethod(jmi.env,jmi.classID,jmi.methodID);
        (*jmi.env)->DeleteLocalRef(jmi.env,jmi.classID);
    }
    _demuxerCB = NULL;
}

int android_autoFoucs(int bAutofocus)
{
    struct JniMethodInfo jmi;
    if(jniGetStaticMethodInfo(&jmi,ANDROID_DEMUXER_CLASS_NAME,"autoFoucs","(Z)Z")) {
        int ret = (*jmi.env)->CallBooleanMethod(jmi.env,jmi.classID,jmi.methodID,bAutofocus)?1:0;
        (*jmi.env)->DeleteLocalRef(jmi.env,jmi.classID);
        return ret;
    }
    return 0;
}

void android_releaseBuffer(void * bufObj, unsigned char * buf)
{
    struct JniMethodInfo jmi;
    if(!bufObj || !buf)return;

    if(jniGetStaticMethodInfo(&jmi,ANDROID_DEMUXER_CLASS_NAME,"releaseBuffer","([B)V")) {
        jbyteArray bobj = (jbyteArray)bufObj;
        (*jmi.env)->ReleaseByteArrayElements(jmi.env,bobj,(jbyte*)buf,JNI_ABORT);
        (*jmi.env)->CallStaticVoidMethod(jmi.env,jmi.classID,jmi.methodID,bobj);
        (*jmi.env)->DeleteGlobalRef(jmi.env,bobj);
        (*jmi.env)->DeleteLocalRef(jmi.env,jmi.classID);
    }
}

int android_setDemuxerCallback( AndroidDemuxerCB_t cb )
{
    _demuxerCB = cb;
}

/*
 * 图像数据和音频数据传入
 */
JNIEXPORT void JNICALL
Java_org_ffmpeg_device_AndroidDemuxer_ratainBuffer(JNIEnv * env , jclass cls,
int type, jbyteArray bobj,int len,int fmt,int p0,int p1,jlong timestramp)
{
    if(_demuxerCB){
        if(type==VIDEO_DATA) {
            jbyteArray gobj = (jbyteArray)(*env)->NewGlobalRef(env,bobj);
            jbyte *buf = (*env)->GetByteArrayElements(env,gobj, 0);
            if(_demuxerCB(type, gobj, len, (unsigned char * ) buf,fmt,p0,p1,timestramp )) {
                android_closeDemuxer();
            }
        }else if(type==AUDIO_DATA){
            jbyte *buf = (*env)->GetByteArrayElements(env,bobj, 0);
            if ( _demuxerCB(type, bobj, len, (unsigned char * ) buf,fmt,p0,p1,timestramp )) {
                android_closeDemuxer();
            }
            (*env)->ReleaseByteArrayElements(env,bobj,(jbyte*)buf,JNI_ABORT);
        }else{
            if ( _demuxerCB(type, bobj, 0, NULL,fmt,p0,p1,timestramp )) {
                android_closeDemuxer();
            }
        }
    }
    (*env)->DeleteLocalRef(env,bobj);
}