//
// Created by Zero on 2021/1/29.
//

#include <iostream>
#include "core/context.h"
#include "parser/json_parser.h"
#include "parser/assimp_parser.h"
#include <memory>
#include "view/application.h"

using std::cout;
using std::endl;
using namespace luminous;

int execute(int argc, char *argv[]) {
    logging::set_log_level(spdlog::level::info);
    Context context{argc, argv};
    context.try_print_help_and_exit();
    if (argc == 1) {
        context.print_help();
        return 0;
    }
    std::unique_ptr<Parser> parser{nullptr};
    App app;
    try {
        set_thread_num(context.thread_num());
        if (context.has_scene()) {
            auto scene_file = context.scene_file();
            if (scene_file.extension() == ".json" || scene_file.extension() == ".bson") {
                parser = std::make_unique<JsonParser>(&context);
            } else {
                parser = std::make_unique<AssimpParser>(&context);
            }
            parser->load(scene_file);
        }
        app.init("luminous", luminous::make_int2(1280,720), &context, *parser);
    } catch (std::exception &e1) {
        cout << e1.what();
        return 0;
    }
    return app.run();
}

int main(int argc, char *argv[]) {
    auto ret = execute(argc, argv);
    return ret;
}