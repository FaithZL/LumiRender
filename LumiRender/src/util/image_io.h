//
// Created by Zero on 2021/2/20.
//


#pragma once

#include "graphics/optics/rgb.h"
#include <utility>

namespace luminous {
    inline namespace utility {
        using namespace std;

        pair<RGBSpectrum *, int2> load_image(const filesystem::path &fn);

        pair<RGBSpectrum *, int2> load_hdr(const filesystem::path &fn);

        pair<RGBSpectrum *, int2> load_exr(const filesystem::path &fn);

        /**
         * ".bmp" or ".png" or ".tga" or ".jpg" or ".jpeg"
         */
        pair<RGBSpectrum *, int2> load_other(const filesystem::path &fn);

        void save_image(const filesystem::path &fn, RGBSpectrum * rgb, int2 resolution);

        void save_hdr(const filesystem::path &fn, RGBSpectrum * rgb, int2 resolution);

        void save_exr(const filesystem::path &fn, RGBSpectrum * rgb, int2 resolution);

        /**
         * ".bmp" or ".png" or ".tga" or ".jpg" or ".jpeg"
         */
        void save_other(const filesystem::path &fn, RGBSpectrum * rgb, int2 resolution);
    }
}