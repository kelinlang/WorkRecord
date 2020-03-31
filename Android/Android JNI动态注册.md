JNI（Java Native Interface）是一套编程接口, 用来实现 Java 代码与本地的 C/C++ 代码进行交互，其有两种注册方式：静态注册和动态注册。

静态注册

理解和使用方式简单, 使用相关工具按流程操作就行, 编码出错率低
JNI 层函数名特别长，且当需要更改类名，包名或者方法时, 需要按照之前方法重新生成头文件, 灵活性不高
初次调用 native 函数时要根据函数名搜索对应的 JNI 层函数来建立联系，效率较低
动态注册

灵活性高, 更改类名、包名或方法时, 只需对更改模块进行少量修改, 效率高
使用用函数映射表来调用相应的函数，效率较高（Android 源码使用的动态注册）
理解和使用方式稍复杂, 容易搞错签名、方法, 导致注册失败
本文主要讲动态注册，静态注册见我另外一篇博客《JNI 传递和返回基本参数》

动态注册
动态注册基本思想是：当在 Java 层调用 System.loadLibrary(libName) 的时候，底层会调用 JNI_OnLoad () 函数，在这个函数中通过 NDK 提供的 RegisterNatives() 方法来将 C/C++ 方法和 Java 方法注册映射起来，后续从 Java 层调用声明为 native 的方法时就会根据映射表找到对应的 C/C++ 方法。注册整体流程如下:

1、编写 Java 层的相关 Native 方法，如

package com.alan.jniexamples.dynamic;

/**
 * Author: AlanWang4523.
 * Date: 19/5/18 11:14.
 * Mail: alanwang4523@gmail.com
 */
public class NativeDynamic {
    static {
        System.loadLibrary("jni_example");
    }

    public native int nativeSetBasicArgs(int iArg, float fArg, long lArg, boolean bArg);

    public native void nativeSetStringArgs(String str);
}

2、编写 C/C++ 代码，将 Java 方法和 C/C++ 方法通过签名信息对应起来，如：

static int JNISetBasicArgs(JNIEnv *env, jobject obj,
                                          jint iArg, jfloat fArg, jlong lArg, jboolean bArg) {
    LOGD("NativeDynamicJNI", "JNISetBasicArgs()-->>iArg = %d, fArg = %f, lArg = %ld, bArg = %d\n",
            iArg, fArg, lArg, bArg);
    return 0;
}

static void JNISetStringArgs(JNIEnv *env, jobject obj,jstring strArg) {
    jboolean iscopy;
    // 这里不能直接使用 strArg，需要将其通过 GetStringUTFChars 接口将其转成 UTF-8 的 字符串的指针
    char *cStr = (char *) env->GetStringUTFChars(strArg, &iscopy);
    LOGD("NativeDynamicJNI", "JNISetStringArgs()--->cStr = %s", cStr);
    // 最后需要释放，否则可能导致内存泄漏
    env->ReleaseStringUTFChars(strArg, cStr);
}

// Java 层声明 native 方法的类的全路径
static const char *className = "com/alan/jniexamples/dynamic/NativeDynamic";

static JNINativeMethod gJni_Methods[] = {
        // 如： nativeSetBasicArgs 是 Java 声明的方法
        // "(IFJZ)I" 是函数签名
        // JNISetBasicArgs 是 C/C++ 声明的方法
        {"nativeSetBasicArgs", "(IFJZ)I", (void*)JNISetBasicArgs},
        {"nativeSetStringArgs", "(Ljava/lang/String;)V", (void*)JNISetStringArgs},
};

3、在 JNI_OnLoad () 函数中通过 NDK 提供的 RegisterNatives() 方法来将 C/C++ 方法和 Java 方法注册映射起来：

jint JNI_OnLoad(JavaVM* jvm, void* reserved){
    JNIEnv* env = NULL;
    jint result = -1;

    if (jvm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        return result;
    }
    
    jniRegisterNativeMethods(env, className, gJni_Methods, NELEM(gJni_Methods));
    
    return JNI_VERSION_1_4;
}

jniRegisterNativeMethods 方法如下：

int jniRegisterNativeMethods(JNIEnv* env, const char* className, const JNINativeMethod* gMethods, int numMethods) {

    LOGD("JNIHelper", "Start registering %s native methods.\n", className);
    jclass clazz = (env)->FindClass(className);
    if (clazz == NULL) {
        LOGE("JNIHelper", "Native registration unable to find class '%s'.\n", className);
        return -1;
    }
    
    int result = 0;
    if ((env)->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        LOGE("JNIHelper", "RegisterNatives failed for '%s'.\n", className);
        result = -1;
    }
    
    (env)->DeleteLocalRef(clazz);
    LOGD("JNIHelper", "Registering %s native methods success.\n", className);
    return result;
}

4、测试调用：

private void testDynamicJNIs() {
        NativeDynamic nativeDynamic = new NativeDynamic();
        nativeDynamic.nativeSetBasicArgs(2, 3.2f, 1000L, true);
        nativeDynamic.nativeSetStringArgs("Hello Alan From Java!");
    }

输出：

05-18 21:13:28.891 8156-8156/com.alan.jniexamples D/NativeDynamicJNI: JNISetBasicArgs()-->>iArg = 2, fArg = 3.200000, lArg = 1000, bArg = 1
05-18 21:13:28.891 8156-8156/com.alan.jniexamples D/NativeDynamicJNI: JNISetStringArgs()--->cStr = Hello Alan From Java!

完整代码见 GitHub
https://github.com/alanwang4523/JNIExamples
————————————————
版权声明：本文为CSDN博主「AlanWang4523」的原创文章，遵循 CC 4.0 BY-SA 版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/u011520181/article/details/90319907