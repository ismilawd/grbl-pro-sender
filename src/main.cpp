#include <imgui_internal.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <gps/resources/font/roboto_light.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <TextEditor.h>
#include <gps/GCodeLangDef.h>

GLuint LoadTexture(const char *filename) {
    int x, y, n;
    unsigned char *data = stbi_load(filename, &x, &y, &n, 4);
    GLuint tex_id = 0;
    if (data) {
        glGenTextures(1, &tex_id);
        glBindTexture(GL_TEXTURE_2D, tex_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    return tex_id;
}

void setup_imgui_fonts() {
    ImGuiIO &io = ImGui::GetIO();

    // Clear any defaults if you want only your font
    io.Fonts->Clear();

    float font_pixels = 20.0f;
    ImFont *font = io.Fonts->AddFontFromMemoryTTF(
        __embed_font_Roboto_Light_ttf,
        __embed_font_Roboto_Light_ttf_len,
        font_pixels
    );

    if (!font) {
        fprintf(stderr, "Failed to load embedded font!\n");
    }
}

int main() {
    // Setup GLFW
    if (!glfwInit())
        return 1;

    const char *glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(1280, 720, "ImGui Docking Example", NULL, NULL);
    if (window == NULL)
        return 1;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // IMGUI setup
    IMGUI_CHECKVERSION();
    ImGuiContext *context = ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking

    ImGui::StyleColorsDark();

    float xscale, yscale;
    glfwGetWindowContentScale(window, &xscale, &yscale);
    float dpi_scale = xscale; // On Retina is usually 2.0
    ImGui::GetStyle().ScaleAllSizes(dpi_scale); // Essential for proper UI scaling
    setup_imgui_fonts();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    GLuint texture_start = LoadTexture("/Users/milad/Projects/grbl-pro-sender/embed/texture/start.png");

    TextEditor editor;
    editor.SetLanguageDefinition(GCodeLanguageDefinition());
    editor.SetText(
        "G90 ; Absolute positioning\n"
        "G1 X50 Y25 F1500 ; move\n"
        "M3 S1000 ; spindle on\n"
        "(End of G-code)"
    );

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                ImGui::MenuItem("Open GCode");
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Undo", "CTRL+Z")) {
                }
                if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {
                } // Disabled item
                ImGui::Separator();
                if (ImGui::MenuItem("Cut", "CTRL+X")) {
                }
                if (ImGui::MenuItem("Copy", "CTRL+C")) {
                }
                if (ImGui::MenuItem("Paste", "CTRL+V")) {
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
        float toolbar_height = 40.0f; // must match height above
        float toolbar_top = context->FontSize + context->Style.FramePadding.y * 2.0f;

        ImGui::SetNextWindowPos(ImVec2(0, toolbar_top));
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, toolbar_height));
        // ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
        ImGuiWindowFlags toolbarFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                                        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDocking;

        ImGui::Begin("ToolbarWindow", NULL, toolbarFlags);

        if (ImGui::ImageButton("ButtonStart", texture_start, ImVec2(32, 32))) {
            // Play action
        }
        ImGui::SameLine();

        ImGui::End();
        // ImGui::PopStyleVar();

        ImGui::SetNextWindowPos(ImVec2(0, toolbar_height + toolbar_top + context->Style.FramePadding.y * 2.0f));
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x,
                                        ImGui::GetIO().DisplaySize.y - (
                                            toolbar_height + toolbar_top + context->Style.FramePadding.y * 2.0f)));
        ImGuiWindowFlags dockspaceFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                          ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                                          ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::Begin("DockSpaceWindow", nullptr, dockspaceFlags);

        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);

        ImGui::ShowDemoWindow();

        ImGui::Begin("GCodeEditor");

        editor.Render("GCodeEditor");
        ImGui::End();
        ImGui::End();

        // Rendering
        ImGui::Render();

        glViewport(0, 0, (int) io.DisplaySize.x, (int) io.DisplaySize.y);
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
