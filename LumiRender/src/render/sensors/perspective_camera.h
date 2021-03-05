//
// Created by Zero on 2021/3/4.
//


#pragma once

#include "../include/sensor.h"


namespace luminous {

    inline namespace render {
        class PerspectiveCamera : public CameraBase {
        public:
            GEN_CLASS_NAME(PerspectiveCamera)

            NDSC std::string to_string() const;

            static PerspectiveCamera *create(const SensorConfig &config);
        };
    }

} // luminous