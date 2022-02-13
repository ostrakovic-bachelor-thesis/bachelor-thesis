#include "GUIObjectDescription.h"

extern const uint8_t brightnessBitmap[100 * 100 * 4];
extern const uint8_t untzLogoBitmap[170 * 170 * 4];
extern const uint8_t playButtonBitmap[75 * 75 * 4];



const GUI::Rectangle::RectangleDescription g_backgroundUpPartGuiRectangleDescription =
{
  .baseDescription =
  {
    .dimension =
    {
      .width  = 390u,
      .height = 140u
    },
    .position =
    {
      .x   = 0u,
      .y   = 0u,
      .tag = GUI::Position::Tag::TOP_LEFT_CORNER
    }
  },
  .color  =
  {
    .red   = 26u,
    .green = 26u,
    .blue  = 29u
  }
};

const GUI::Rectangle::RectangleDescription g_backgroundDownPartGuiRectangleDescription =
{
  .baseDescription =
  {
    .dimension =
    {
      .width  = 390u,
      .height = 250u
    },
    .position =
    {
      .x   = 0,
      .y   = 140,
      .tag = GUI::Position::Tag::TOP_LEFT_CORNER
    }
  },
  .color  =
  {
    .red   = 197u,
    .green = 198u,
    .blue  = 199u
  }
};

const GUI::Image::ImageDescription g_brightnessImageDescription =
{
  .baseDescription =
  {
    .dimension =
    {
      .width  = 75u,
      .height = 75u
    },
    .position =
    {
      .x   = 235,
      .y   = 85,
      .tag = GUI::Position::Tag::CENTER
    }
  },
  .bitmapDescription =
  {
    .colorFormat = GUI::ColorFormat::ARGB8888,
    .dimension =
    {
      .width  = 75u,
      .height = 75u
    },
    .copyPosition =
    {
      .x = 0,
      .y = 0
    },
    .bitmapPtr = brightnessBitmap
  }
};

const GUI::Image::ImageDescription g_untzLogoImageDescription =
{
  .baseDescription =
  {
    .dimension =
    {
      .width  = 170u,
      .height = 170u
    },
    .position =
    {
      .x   = 195,
      .y   = 250,
      .tag = GUI::Position::Tag::CENTER
    }
  },
  .bitmapDescription =
  {
    .colorFormat = GUI::ColorFormat::ARGB8888,
    .dimension =
    {
      .width  = 170u,
      .height = 170u
    },
    .copyPosition =
    {
      .x = 0,
      .y = 0
    },
    .bitmapPtr = untzLogoBitmap
  }
};

const GUI::Image::ImageDescription g_playButtonImageDescription =
{
  .baseDescription =
  {
    .dimension =
    {
      .width  = 75u,
      .height = 75u
    },
    .position =
    {
      .x   = 95,
      .y   = 85,
      .tag = GUI::Position::Tag::CENTER
    }
  },
  .bitmapDescription =
  {
    .colorFormat = GUI::ColorFormat::ARGB8888,
    .dimension =
    {
      .width  = 75u,
      .height = 75u
    },
    .copyPosition =
    {
      .x = 0,
      .y = 0
    },
    .bitmapPtr = playButtonBitmap
  }
};