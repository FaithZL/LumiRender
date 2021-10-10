//
// Created by Zero on 02/09/2021.
//

#include "integrator.h"
#include "render/scene/scene.h"
#include "render/scene/cpu_scene.h"
#include "render/scene/gpu_scene.h"


namespace luminous {
    inline namespace render {
        const SceneData* Integrator::scene_data() const { return _scene->scene_data();}

        void Integrator::init(const std::shared_ptr<SceneGraph> &scene_graph) {
            if (_device->is_cpu()) {
                _scene = std::make_shared<CPUScene>(_device, _context);
            } else {
                _scene = std::make_shared<GPUScene>(_device, _context);
            }
            _max_depth = scene_graph->integrator_config.max_depth;
            _rr_threshold = scene_graph->integrator_config.rr_threshold;
            _scene->init(scene_graph);
            _sampler.push_back(Sampler::create(scene_graph->sampler_config));

            _camera.init(1);
            _camera.add_element(scene_graph->sensor_config);
            _camera->set_film(Film::create(scene_graph->sensor_config.film_config));

            init_on_device();
            LUMINOUS_INFO(get_arena().description());
        }
    }
}