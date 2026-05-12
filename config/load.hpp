#pragma once

#include <memory>
#include <toml++/toml.hpp>
#include <print>
#include <string>
// TMPリソースをインクルード！
#include "start/assets.hpp"

using namespace std;

namespace start::config
{
  auto loadConfig() -> shared_ptr<toml::table>
  {
    // 1. TMPアセットから config.toml を取得
    // assets::all に入っているデータ（string_view）を直接使うよ！✨
    string_view toml_data;
    try
    {
      toml_data = start::assets::all.at("config.toml");
    }
    catch (const out_of_range &)
    {
      println("TMPリソースに config.toml が見つからないよ！🚨");
      return nullptr;
    }

    try
    {
      // 2. メモリ上のデータから直接パース！🚀
      auto result = toml::parse(toml_data);

      if (result.empty())
      {
        println("パースしたTOMLが空っぽみたいだよ！🚨");
        return nullptr;
      }

      auto config = make_shared<toml::table>(std::move(result));
      println("TMPから設定データのロードに成功したよ！🚀✨");
      return config;
    }
    catch (const toml::parse_error &err)
    {
      println("TOMLパースエラー！🚨: {}", err.description());
      return nullptr;
    }
  }
}