#include "DgViewer.h"

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

ImGuiManager::ImGuiManager()
{
	mWindow = nullptr;
}

ImGuiManager::~ImGuiManager()
{
}

bool ImGuiManager::init(int winWidth, int winHeight)
{
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return false;

	// OpenGL, GLSL ���� ����
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // GLFW_OPENGL_COMPAT_PROFILE

	// ������ ����
	mWindow = glfwCreateWindow(winWidth, winHeight, "DgViewer", nullptr, nullptr);
	if (!mWindow)
	{
		glfwTerminate();
		return false;
	}

	//glfwMaximizeWindow(mWindow);	
	glfwMakeContextCurrent(mWindow);
	glfwSwapInterval(1); // ����� �������� �ֱ⿡ ����ȭ

	// ImGui context ����
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigWindowsMoveFromTitleBarOnly = true;			// Title Bar�� ������ ��ġ �̵� ����
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	// Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	// Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;		// Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;		// Enable Multi-Viewport / Platform Windows

	// ImGui style ����
	ImGui::StyleColorsClassic(); // ImGui::StyleColorsDark();  �Ǵ� ImGui::StyleColorsLight(); �� ����
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.GrabRounding = 5.0f;
		style.FrameRounding = 5.0f;
		style.WindowBorderSize = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		style.DockingSeparatorSize = 1.0f;
	}

	// �÷���(GLFW)/������(OpenGL) ����
	ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// ���ϴ� ��Ʈ�� �ε�
	//io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\cour.ttf", 20.0f);
	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 20.0f, NULL, io.Fonts->GetGlyphRangesKorean());
		
	// ���� ��ȭ���ڸ� ���� ���� �Լ�
	ifd::FileDialog::Instance().CreateTexture = [](uint8_t* data, int w, int h, char fmt) -> void *{
		GLuint tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, (fmt == 0) ? GL_BGRA : GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		return (void *)(uintptr_t)tex;
	};

	ifd::FileDialog::Instance().DeleteTexture = [](void* tex) {
		GLuint texID = (GLuint)((uintptr_t)tex);
		glDeleteTextures(1, &texID);
	};

	return true;
}

void ImGuiManager::begin()
{
	// ImGui �������� �����Ѵ�.
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// ������ ���� ����Ʈ�� ��ŷ�� ����Ѵ�.
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
}

void ImGuiManager::end()
{
	// ������
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::Render();
	//int display_w, display_h;
	//glfwGetFramebufferSize(mWindow, &display_w, &display_h);
	//ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	//glViewport(0, 0, display_w, display_h);
	//glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	//glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
	//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
	glfwSwapBuffers(mWindow);
}

void ImGuiManager::cleanUp()
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(mWindow);
	glfwTerminate();
}
