//
// Created by Zero on 2021/2/19.
//


#include "application.h"
#include <iostream>
#include "util/stats.h"
#include "render/include/task.h"
#include "gpu/framework/cuda_impl.h"
#include "cpu/cpu_impl.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <ShlObj.h>

#endif

using std::cout;
using std::endl;

namespace luminous {

    static float4 bg_color = make_float4(0.2);

    template<typename T = App>
    T *get_user_ptr(GLFWwindow *window) {
        return static_cast<T *>(glfwGetWindowUserPointer(window));
    }

    static void on_glfw_error(int error, const char *description) {
        fprintf(stderr, "Glfw Error %d: %s\n", error, description);
    }

    /*! callback for a window resizing event */
    static void glfw_resize(GLFWwindow *window, int width, int height) {
        if (width <= 0 || height <= 0) {
            return;
        }
        get_user_ptr(window)->on_resize(make_uint2(width, height));
    }

    /*! callback for a char key press or release */
    static void glfw_char_event(GLFWwindow *window,
                                unsigned int key) {

    }

    /*! callback for a key press or release*/
    static void glfw_key_event(GLFWwindow *window,
                               int key,
                               int scancode,
                               int action,
                               int mods) {
        get_user_ptr(window)->on_key_event(key, scancode, action, mods);
    }

    /*! callback for _moving_ the mouse to a new position */
    static void glfw_cursor_move(GLFWwindow *window, double x, double y) {
        get_user_ptr(window)->on_cursor_move(make_int2(x, y));
    }

    /*! callback for pressing _or_ releasing a mouse button*/
    static void glfw_mouse_event(GLFWwindow *window,
                                 int button,
                                 int action,
                                 int mods) {
        get_user_ptr(window)->on_mouse_event(button, action, mods);
    }

    static void glfw_scroll_event(GLFWwindow *window, double scroll_x, double scroll_y) {
        get_user_ptr(window)->on_scroll_event(scroll_x, scroll_y);
    }

    App::~App() {
        if(_show_window) {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
            glfwTerminate();
        }
    }

    void App::on_cursor_move(int2 new_pos) {
        int2 delta = new_pos - _last_mouse_pos;
        if (nonzero(_last_mouse_pos) && _left_key_press) {
            _task->update_camera_view(delta.x, -delta.y);
            _need_update = true;
        }
        _last_mouse_pos = new_pos;
    }

    void App::on_scroll_event(double scroll_x, double scroll_y) {
        _need_update = true;
        if (_left_key_press) {
            auto camera = _task->camera();
            float factor = 0.01;
            camera->update_lens_radius(factor * scroll_y);
            LUMINOUS_INFO("current lens radius is ", camera->lens_radius());
            return;
        } else if (_right_key_press) {
            auto camera = _task->camera();
            float factor = 0.1;
            camera->update_focal_distance(factor * scroll_y);
            LUMINOUS_INFO("current focal distance is ", camera->focal_distance());
            return;
        }
        _task->update_camera_fov_y(scroll_y);
    }

    void App::on_key_event(int key, int scancode, int action, int mods) {
        _task->on_key(key, scancode, action, mods);
        _need_update = true;
    }

    void App::on_mouse_event(int button, int action, int mods) {
        // todo
        if (button == 0) {
            _left_key_press = bool(action);
        }
        if (button == 1) {
            _right_key_press = bool(action);
        }
    }

    void App::on_resize(const uint2 &new_size) {
        glViewport(0, 0, new_size.x, new_size.y);
        _task->update_film_resolution(new_size);
        _need_update = true;
    }


    void App::init(const string &title, const int2 &size, Context *context, const Parser &parser) {
        TASK_TAG("launch app")
        _size = size;
        if (context->use_gpu()) {
            _task = std::make_unique<Task>(create_cuda_device(), context);
        } else {
            _task = std::make_unique<Task>(create_cpu_device(), context);
        }
        _task->init(parser);

        _show_window = context->show_window();

        if (_show_window) {
            init_with_gui(title);
        }
    }

    App::App(const std::string &title, const int2 &size, Context *context, const Parser &parser)
            : _size(size) {
        init(title, size, context, parser);
    }

    void App::init_gl_context() {
        auto res = make_int2(_size);
        glGenTextures(1, &_gl_ctx.fb_texture);
        glBindTexture(GL_TEXTURE_2D, _gl_ctx.fb_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, res.x, res.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, test_color);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                        GL_REPEAT);    // set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        _gl_ctx.program = createGLProgram(s_vert_source, s_frag_source);
        _gl_ctx.program_tex = getGLUniformLocation(_gl_ctx.program, "render_tex");
        glUseProgram(_gl_ctx.program);
        glUniform1i(_gl_ctx.program_tex, 0);

        glBindTexture(GL_TEXTURE_2D, 0);

        glGenVertexArrays(1, &_gl_ctx.vao);
        glGenBuffers(1, &_gl_ctx.vbo);

        glBindVertexArray(_gl_ctx.vao);

        glBindBuffer(GL_ARRAY_BUFFER, _gl_ctx.vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void App::imgui_begin() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void App::imgui_end() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void App::check_and_update() {
        if (_need_update) {
            _task->update();
            _need_update = false;
        }
    }

    void App::render(double delta_elapsed) {
        _task->render_gui(delta_elapsed);
    }

    int App::run_with_gui() {

        float t0, t1, t2;
        float elapsed_time;

        _clock.reset();

        while (!glfwWindowShouldClose(_handle)) {
            glfwPollEvents();

            elapsed_time = _clock.get_elapsed_time();
            _clock.tick();

            t1 = t0 = _clock.get_time();
            check_and_update();
            t2 = _clock.get_time();
            _frame_stats.update_time += t2 - t1;

            t1 = t2;
            render(elapsed_time);
            t2 = _clock.get_time();
            _frame_stats.render_time += t2 - t1;

            t1 = t2;
            update_render_texture();
            draw();
            t2 = _clock.get_time();
            _frame_stats.display_time += t2 - t1;

            _frame_stats.last_frame_elapsed = t2 - t0;
            _frame_stats.last_sample_elapsed += _frame_stats.last_frame_elapsed;
            ++_frame_stats.frame_count;

            imgui_begin();
            display_stats();
            imgui_end();

            glfwSwapBuffers(_handle);
        }
        return 0;
    }

    void App::display_stats() {

        constexpr float update_min_time_interval{.5f};

        static char display_text[128] = {};

        if (_frame_stats.last_sample_elapsed > update_min_time_interval) {
            snprintf(display_text, 128,
                     "FPS          : %8.1f\n"
                     "state update : %8.1f ms\n"
                     "render       : %8.1f ms\n"
                     "display      : %8.1f ms",
                     _frame_stats.frame_count / _frame_stats.last_sample_elapsed,
                     _frame_stats.update_time * 1000.f / _frame_stats.frame_count,
                     _frame_stats.render_time * 1000.f / _frame_stats.frame_count,
                     _frame_stats.display_time * 1000.f / _frame_stats.frame_count
            );

            _frame_stats.frame_count = 0;
            _frame_stats.last_sample_elapsed = 0.0f;
            _frame_stats.update_time = 0.0f;
            _frame_stats.render_time = 0.0f;
            _frame_stats.display_time = 0.0f;
        }

        if (display_text[0]) {
            ImGui::SetNextWindowPos(ImVec2{10.f, 10.f});
            ImGui::Begin("Frame Statistics", nullptr,
                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                         ImGuiWindowFlags_NoSavedSettings);
            ImGui::TextColored(ImColor(0.7f, 0.7f, 0.7f, 1.0f), display_text);
            ImGui::End();
        }
    }


    int App::run_with_cli() {
        while (!_task->complete()) {
            check_and_update();
            render(.0);
        }

        return 0;
    }

    int App::run() {
        if (_show_window) {
            return run_with_gui();
        } else {
            return run_with_cli();
        }
    }

    void App::init_event_cb() {
        glfwSetFramebufferSizeCallback(_handle, glfw_resize);
        glfwSetMouseButtonCallback(_handle, glfw_mouse_event);
        glfwSetKeyCallback(_handle, glfw_key_event);
        glfwSetCharCallback(_handle, glfw_char_event);
        glfwSetCursorPosCallback(_handle, glfw_cursor_move);
        glfwSetScrollCallback(_handle, glfw_scroll_event);
    }

    void App::draw() const {
        glClearColor(bg_color.x, bg_color.y, bg_color.z, bg_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        glUniform1i(_gl_ctx.program_tex, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _gl_ctx.fb_texture);
        glUseProgram(_gl_ctx.program);
        glBindVertexArray(_gl_ctx.vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void App::init_window(const std::string &title, const uint2 &size) {
        if (!_show_window) {
            return;
        }

        glfwSetErrorCallback(on_glfw_error);
        if (!glfwInit())
            exit(EXIT_FAILURE);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
        _handle = glfwCreateWindow(size.x, size.y, title.c_str(), NULL, NULL);
        if (!_handle) {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        glfwSetWindowUserPointer(_handle, this);
        glfwMakeContextCurrent(_handle);
        glfwSwapInterval(0);

        if (gladLoadGL() == 0) {
            fprintf(stderr, "Failed to initialize OpenGL loader!\n");
            exit(EXIT_FAILURE);
        }

        init_imgui();
    }

    void App::init_imgui() {
        const char *glsl_version = "#version 130";
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(_handle, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
#ifdef _WIN32
        PWSTR pszFontFolder = nullptr;
        SHGetKnownFolderPath(FOLDERID_Fonts, KF_FLAG_DEFAULT, NULL, &pszFontFolder);
        if (pszFontFolder) {
            std::size_t cch = 0;
            cch = WideCharToMultiByte(CP_ACP, 0, pszFontFolder, -1, NULL, 0, NULL, NULL);
            if (cch > 0) {
                std::string strFontFolder(cch, '\0');
                WideCharToMultiByte(CP_ACP, 0, pszFontFolder, -1, &strFontFolder[0], strFontFolder.size(), NULL, NULL);
                strFontFolder.insert(cch - 1, "\\LUCON.ttf");
                auto pArialFont = io.Fonts->AddFontFromFileTTF(strFontFolder.data(), 14);
            }
            CoTaskMemFree(pszFontFolder);
        }
#else
        auto pFont = io.Fonts->AddFontDefault();
        pFont->FontSize = 14;
#endif
    }

    void App::set_title(const std::string &s) {
        glfwSetWindowTitle(_handle, s.c_str());
    }

    void App::update_render_texture() {
        auto res = _task->resolution();
        test_color = _task->get_frame_buffer();

        glBindTexture(GL_TEXTURE_2D, _gl_ctx.fb_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, res.x, res.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, test_color);
    }


}