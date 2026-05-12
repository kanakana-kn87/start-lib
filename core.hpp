#pragma once
#include <string>

namespace start
{
  class App
  {
  public:
    virtual ~App() = default;

    // ユーザーが自由に上書きするイベント
    virtual void on_launch() {}
    virtual void on_close() {}

    // OSごとの main.cpp から呼び出される心臓部
    virtual int run() = 0;
  };
}