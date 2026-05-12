#include <jni.h>
#include <android/log.h>
#include <string>
#include <string_view>
#include <memory>

// かなかな特製：設定ロードの心臓部！
#include "start/console/config/load.hpp"
// かなかな特製：アセットの宝庫！
#include "start/assets.hpp"

#define LOG_TAG "START_CORE"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using namespace std;

// AndroidのWebViewにHTMLを流し込むヘルパー
void loadHtmlToWebView(JNIEnv* env, jobject webView, const string& html) {
    jclass webViewClass = env->GetObjectClass(webView);
    
    // 魔法のメソッド "loadDataWithBaseURL" を取得
    jmethodID loadMethod = env->GetMethodID(webViewClass, "loadDataWithBaseURL", 
        "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");

    // 引数を準備
    jstring jBaseUrl = env->NewStringUTF("file:///android_asset/"); // 基準パス
    jstring jHtml = env->NewStringUTF(html.c_str());               // かなかなの巨大HTML
    jstring jMimeType = env->NewStringUTF("text/html");            // 形式
    jstring jEncoding = env->NewStringUTF("utf-8");                // 文字コード
    jstring jHistoryUrl = nullptr;                                 // 履歴（なし）

    // 実行！これなら数メガバイトの JS が入っていても、特殊記号があっても完璧に表示される
    env->CallVoidMethod(webView, loadMethod, jBaseUrl, jHtml, jMimeType, jEncoding, jHistoryUrl);

    // メモリリーク防止のお掃除
    env->DeleteLocalRef(jBaseUrl);
    env->DeleteLocalRef(jHtml);
    env->DeleteLocalRef(jMimeType);
    env->DeleteLocalRef(jEncoding);
}

extern "C" {

/**
 * Android起動時にKotlinから呼ばれる
 */
JNIEXPORT void JNICALL
Java_framework_start_tspp_MainActivity_nativeOnCreate(JNIEnv* env, jobject thiz, jobject webView) {
    LOGI("start_core: Initializing with start/consorle/config/load.hpp... 🚀");

    // 1. かなかなが作った loadConfig() を使用！
    auto config = start::config::loadConfig();

    if (config) {
        // 設定が読み込めていれば、中身をチェック
        auto title = (*config)["app"]["title"].value_or("Untitled Start App");
        LOGI("App Title: %s", title);
    } else {
        LOGE("Failed to load config via start/consorle/config/load.hpp 🚨");
    }

    // 2. WebViewに index.html を表示
    try {
        // ここもかなかな指定の assets 経由で！
        auto html = start::assets::all.at("index.html");
        loadHtmlToWebView(env, webView, string(html));
        LOGI("Success: HTML injected into WebView! ✨");
    } catch (const out_of_range&) {
        LOGE("Error: index.html not found in assets.hpp 😱");
    }
}

} // extern "C"