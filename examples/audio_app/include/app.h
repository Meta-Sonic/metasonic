#pragma once
#include "mts/ui/application.h"
#include "mts/print.h"
#include "audio_device.h"
#include "main_window.h"

class asset_holder {
public:
  asset_holder()
      : _assets(mts::assets::retain_with_initializer(&assets_initializer)) {}

private:
  std::shared_ptr<mts::asset_manager> _assets;

  static inline void assets_initializer() {

    std::shared_ptr<mts::asset_manager> assets = mts::assets::get();

    assets->images.add("logo", RESOURCES_DIRECTORY "/images/Meta-Sonic-256x256.png");

    assets->fonts.add("password", RESOURCES_DIRECTORY "/fonts/password.ttf", 12.0f);

    // clang-format off
    assets->fonts.add(RESOURCES_DIRECTORY "/fonts/roboto-regular.ttf", {
      { "roboto-regular-11", 11.0f },
      { "roboto-regular-12", 12.0f },
      { "roboto-regular-14", 14.0f }
    });
    
    assets->fonts.add(RESOURCES_DIRECTORY "/fonts/roboto-medium.ttf", {
      { "roboto-medium-12", 12.0f },
      { "roboto-medium-14", 14.0f }
    });
    // clang-format on
    //
    auto s = std::make_shared<mts::style>();
    s->set<mts::style::button::background_color>(0x297DFFFF);
    s->set<mts::style::button::hover_background_color>(mts::color(0x297DFFFF).brighter(0.2f));
    s->set<mts::style::button::pressed_background_color>(mts::color(0x297DFFFF).darker(0.2f));
    s->set<mts::style::button::text_color>(0xFFFFFFFF);
    s->set<mts::style::button::hover_text_color>(0xFFFFFFFF);
    s->set<mts::style::button::pressed_text_color>(0xFFFFFFFF);
    s->set<mts::style::button::border_color>(0);
    s->set<mts::style::button::font_name>("roboto-regular-12");
    assets->styles.add("button", s);
  }
};

class app : public mts::application, private asset_holder {
public:
  app();

  virtual ~app() override;
  virtual void init() override;

  inline audio_device& get_audio_device() { return _device; }

private:
  audio_device _device;
  mts::managed_ptr<main_window> _window;
};
