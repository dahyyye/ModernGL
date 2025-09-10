#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <filesystem>

// GLEW/GLFW ���� ��� ����
#include "./include/gl/glew.h"
#include "./include/glfw/glfw3.h"

// GLM ���� ��� ����
#include "./include/glm/glm.hpp"
#include "./include/glm/gtc/matrix_transform.hpp"
#include "./include/glm/gtc/type_ptr.hpp"

// IMGUI ���� ��� ����
#define IMGUI_DEFINE_MATH_OPERATORS
#include "./include/ImGui/imgui.h"
#include "./include/ImGui/imgui_internal.h"
#include "./include/imgui/imgui_impl_opengl3.h"
#include "./include/imgui/imgui_impl_glfw.h"
#include "./include/ImGui/imgui_file_dlg.h"
#include "./include/ImGui/imgui_console.h"

// ��ü ��� ����
#include "ImGuiManager.h"
#include "DgMesh.h"
#include "DgScene.h"
