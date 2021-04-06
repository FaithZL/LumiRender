//
// Created by Zero on 2021/2/16.
//

#include "parser.h"
#include "parameter_set.h"
#include <iomanip>

namespace luminous {
    inline namespace render {
        DataWrap create_json_from_file(const std::filesystem::path &fn) {
            std::ifstream fst;
            fst.open(fn.c_str());
            std::stringstream buffer;
            buffer << fst.rdbuf();
            std::string str = buffer.str();
            str = jsonc_to_json(str);
            LUMINOUS_DEBUG(str);
            if (fn.extension() == "bson") {
                return DataWrap::from_bson(str);
            } else {
                return DataWrap::parse(str);
            }
        }

        //"transform" : {
        //					"type" : "matrix4x4",
        //					"param" : {
        //						"matrix4x4" : [
        //							1,0,0,0,
        //							0,1,0,0,
        //							0,0,1,0,
        //							0,0,0,1
        //						]
        //					}
        //				}
        TransformConfig parse_transform(const ParameterSet &ps) {
            TransformConfig ret;
            ret.type = ps["type"].as_string("matrix4x4");
            auto param = ps["param"];
            if (ret.type == "matrix4x4") {
                ret.mat4x4 = param["matrix4x4"].as_float4x4();
            } else if (ret.type == "trs") {
                ret.t = param["t"].as_float3();
                ret.r = param["r"].as_float4();
                ret.s = param["s"].as_float3();
            } else {
                // yaw pitch position
                ret.yaw = param["yaw"].as_float();
                ret.pitch = param["pitch"].as_float();
                ret.position = param["position"].as_float3();
            }
            return ret;
        }

        //		{
        //			"name" : "c_box",
        //			"type": "model", or "quad"
        //			"params" : {
        //				"fn": "cornell_box.obj",
        //				"transform" : {
        //					"type" : "trs",
        //					"param": {
        //						"t": [1,1,1],
        //						"r": [1,1,1,60],
        //						"s": [2,2,2]
        //					}
        //				}
        //			}
        //		}
        std::vector<ShapeConfig> parse_shapes(const DataWrap &shapes) {
            std::vector<ShapeConfig> ret;
            ret.reserve(shapes.size());
            for (auto &shape : shapes) {
                ShapeConfig shape_config;
                shape_config.type = string(shape["type"]);
                ParameterSet param(shape["param"]);
                if (shape_config.type == "model"){
                    // todo fixed sub div level bug
                    shape_config.subdiv_level = param["subdiv_level"].as_uint(0u);
                    shape_config.fn = param["fn"].as_string();
                } else if (shape_config.type == "quad") {
                    shape_config.width = param["width"].as_float(1);
                    shape_config.height = param["height"].as_float(1);
                }
                shape_config.name = string(shape["name"]);
                shape_config.o2w = parse_transform(param["transform"]);

                ret.push_back(shape_config);
            }
            return move(ret);
        }

        //"sampler" : {
        //		"type" : "LCGSampler",
        //		"param" : {
        //			"spp" : 16
        //		}
        //	}
        SamplerConfig parse_sampler(const ParameterSet &ps) {
            SamplerConfig ret;
            ret.type = ps["type"].as_string();
            ret.spp = ps["param"]["spp"].as_uint();
            return ret;
        }

        FilmConfig parse_film(const ParameterSet &ps) {
            FilmConfig fc;
            fc.type = "RGBFilm";
            ParameterSet param(ps["param"]);
            fc.resolution = param["resolution"].as_uint2(make_uint2(500, 500));
            fc.file_name = param["file_name"].as_string("luminous.png");
            return fc;
        }

        //	"camera" : {
        //		"type" : "PinholeCamera",
        //		"param" : {
        //			"fov_y" : 20,
        //			"velocity" : 20,
        //			"transform" : {
        //				"type" : "yaw_pitch",
        //				"param" : {
        //					"yaw" : 10,
        //					"pitch": 20,
        //					"position": [1,1,1]
        //				}
        //			}
        //		}
        //	},
        SensorConfig parse_sensor(const ParameterSet &ps) {
            SensorConfig ret;
            ret.type = ps["type"].as_string();
            ParameterSet param(ps["param"]);
            ret.fov_y = param["fov_y"].as_float();
            ret.velocity = param["velocity"].as_float();
            ret.transform_config = parse_transform(param["transform"]);
            ret.film_config = parse_film(param["film"]);
            return ret;
        }

        void Parser::load_from_json(const std::filesystem::path &fn) {
            _data = create_json_from_file(fn);
        }

        SP<SceneGraph> Parser::parse() const {
            auto shapes = _data["shapes"];
            auto scene_graph = make_shared<SceneGraph>(_context);
            scene_graph->shape_configs = parse_shapes(shapes);
            scene_graph->sensor_config = parse_sensor(ParameterSet(_data["camera"]));
            scene_graph->sampler_config = parse_sampler(ParameterSet(_data["sampler"]));
            return scene_graph;
        }
    }
}