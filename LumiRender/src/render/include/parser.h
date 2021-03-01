//
// Created by Zero on 2021/2/16.
//


#pragma once

#include "core/concepts.h"
#include "graphics/string_util.h"
#include "parameter_set.h"
#include "render/include/model.h"
#include "scene_graph.h"

namespace luminous {

    inline DataWrap create_json_from_file(const std::filesystem::path &fn) {
        std::ifstream fst;
        fst.open(fn.c_str());
        std::stringstream buffer;
        buffer << fst.rdbuf();
        std::string str = buffer.str();
        str = jsonc_to_json(str);
//        LUMINOUS_INFO(str);
        if (fn.extension() == "bson") {
            return DataWrap::from_bson(str);
        } else {
            return DataWrap::parse(str);
        }
    }

    class Parser : public Noncopyable {
    private:
        DataWrap _data;
        Context *_context;
    public:
        explicit Parser(Context *context) : _context(context) {}



        UP<SceneGraph> load_from_json(const std::filesystem::path &fn) {
            _data = create_json_from_file(fn);
            auto shapes = _data["shapes"];
            using namespace std;
            auto scene_graph = make_unique<SceneGraph>(_context);
            scene_graph->shape_configs.reserve(shapes.size());
            for (auto & shape : shapes) {
                auto path = _context->scene_path() / string(shape["params"]["fn"]);
                cout << path << endl;
//                auto m = ModelCache::instance()->get_model(path.string());
//                int a = 0;
            }
            return move(scene_graph);
        }
    };
}