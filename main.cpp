#include "app.hpp"
#include <memory>
#include <string>

// 【重要】最強のロード関数と設定を読み込む
#include "start/console/config/load.hpp"
// ---------------------------------------------------------
// Windows: WinMain
// ---------------------------------------------------------
#ifdef _WIN32
#include "os/windows/app_impl.hpp"
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow)
{
    // 1. Configをロード！
    auto config = start::config::loadConfig();
    
    // 2. TOMLから設定を抽出（デフォルト値も忘れずに！）
    std::string appTitle = "Start";
    int width = 1200;
    int height = 800;

    if (config) {
        // [app] セクション配下を読み取る
        appTitle = (*config)["app"]["title"].value_or("Start");
        
        // [app.window] セクション配下を読み取る
        width  = (*config)["app"]["window"]["width"].value_or(1200);
        height = (*config)["app"]["window"]["height"].value_or(800);
    }

    // 3. 取得した値を使って、スタック上にアプリを生成！
    // 引数：(インスタンス, タイトル, 幅, 高さ)
    start::windows::WindowsApp myApp(hInst, appTitle, width, height);
    
    return myApp.run();
}

// ---------------------------------------------------------
// Android: JNI (Java Native Interface)
// ---------------------------------------------------------
#elif defined(__ANDROID__)
#include <jni.h>
#include <android/log.h>
#include "os/android/app_impl.hpp" // Android用の実装もここにある想定！

extern "C" JNIEXPORT void JNICALL
Java_framework_start_tspp_MainActivity_nativeOnCreate(JNIEnv *env, jobject thiz, jobject webView)
{
    // AndroidでもConfigをロード
    auto config = start::config::loadConfig();
    
    if (config) {
        auto title = (*config)["app"]["title"].value_or("Start Android");
        __android_log_print(ANDROID_LOG_INFO, "START_CORE", "Launch: %s", title.data());
    }

    // 実際のアプリロジックへ
    // AndroidApp myApp(env, webView); 
    // myApp.run();
}

#else
int main(int argc, char *argv[]) { return 0; }
#endif