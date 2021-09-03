//
// Created by Zero on 02/09/2021.
//

#include "integrator.h"
#include "render/include/scene.h"


namespace luminous {
    inline namespace render {
        const SceneData* Integrator::scene_data() const { return _scene->scene_data();}

        void Integrator::init(const std::shared_ptr<SceneGraph> &scene_graph) {
            _scene = _device->create_scene(_context);
            _max_depth = scene_graph->integrator_config.max_depth;
            _rr_threshold = scene_graph->integrator_config.rr_threshold;
            _scene->init(scene_graph);
            _camera.push_back(Sensor::create(scene_graph->sensor_config));
            _sampler.push_back(Sampler::create(scene_graph->sampler_config));
            init_on_device();
        }
    }
}