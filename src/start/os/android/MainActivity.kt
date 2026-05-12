package framework.start.tspp

import android.app.Activity
import android.os.Bundle
import android.webkit.WebView

class MainActivity : Activity() {

    // C++ライブラリのロード！
    companion object {
        init {
            System.loadLibrary("start_core")
        }
    }

    // ネイティブメソッドの宣言
    external fun nativeOnCreate(webView: WebView)

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        
        val webView = WebView(this).apply {
            settings.javaScriptEnabled = true
            settings.domStorageEnabled = true // これも有効にしておくと今時のHTML5が動くよ！
        }
        setContentView(webView)

        // webView.post を使うと、WebViewの準備が整った後に実行されるんだ！
        webView.post {
            nativeOnCreate(webView)
        }
    }
}