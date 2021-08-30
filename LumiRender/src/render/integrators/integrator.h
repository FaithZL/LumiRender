//
// Created by Zero on 2021/3/24.
//


#pragma once

#include "core/concepts.h"
#include "render/include/scene_graph.h"
#include "render/sensors/sensor.h"
#include "core/backend/managed.h"
#include "render/samplers/sampler.h"

// todo compile error
//#include "render/include/scene_data.h"


namespace luminous {
    inline namespace render {

        class Scene;

        class Integrator : public Noncopyable {
        protected:
            uint _max_depth{};
            float _rr_threshold{};
            SP<Device> _device{};
            SP<Scene> _scene{};
            Context *_context{};
            Managed<Sampler, Sampler> _sampler;
            Managed<Sensor, Sensor> _camera;
        public:
            Integrator(const SP<Device> &device, Context *context)
                    : _device(device),
                      _context(context) {}

            virtual ~Integrator() = default;

            virtual void init(const std::shared_ptr<SceneGraph> &scene_graph) = 0;

            virtual void init_with_config(const IntegratorConfig &ic) {
                _max_depth = ic.max_depth;
                _rr_threshold = ic.rr_threshold;
            }

            NDSC virtual uint frame_index() const = 0;

            NDSC virtual Sensor *camera() = 0;

            virtual void update() = 0;

            virtual void render() = 0;
        };
    }
}