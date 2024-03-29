#pragma once
#include "windows.h"

#include <string>
#include <memory>
#include <map>

#include <glad/glad.h> 
#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Camera.h"
#include "Model.h"
#include "Light.h"
#include "Shader.h"
#include "CubeMap.h"
#include "App.h"
#include "Buffers.h"
#include "FileHandler.h"
#include "rtAreaLight.h"
#include "Random.h"
#include "imgui_internal.h"
#include "Timer.h"


struct SystemInfo {
	const unsigned char* driver_name;
	const char* glsl_version;
};

struct FpsInfo {
	float deltatime = 0;
	float lastFrame = 0;
	float lastTime = 0.0f;
	float fps = 0.0f;
	int nbFrames = 0;

	float update() {
		float currentFrame = glfwGetTime();
		deltatime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		nbFrames++;
		if (currentFrame - lastTime >= 1.0f) {
			lastTime += 1.0f;
			fps = 1000.0 / double(nbFrames);
			nbFrames = 0;
		}
		return fps;
	}
};

struct MouseInfo {
	bool firstMouse = true;
	//window coordinates
	float lastX, lastY;
	//imGui scene viewport coordinates
	float ImlastX, ImlastY;
};

enum ImageMode {
	Normal = 0,
	rtImage
};

class Application
{
public:

	Application();
	~Application();

	bool loadScene(const std::string& filepath);
	void startApplication();
	bool loadModel(const std::string& filepath);
	void loadNewScene();
	void addModel();
	void deleteModel(int index);

	//callbacks
	void mouseCallback(GLFWwindow* window, double xpos, double ypos);
	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void framebufferSizeCallback(GLFWwindow* window, int width, int height);
	void processInput(GLFWwindow* window);

private:
	//initialization
	void initApp();
	void initGLFW();
	void initImGui();
	void initHelmirt();
	void setupCallbacks();

	//UIstuff
	void ImGuiMouseCallback(const ImVec2& mousepos);
	void ImGuiContentResizeCallback(const ImVec2& size);

	//main funcs
	void render();
	void update();
	void bloomBlur(Shader& shader, int iterations);
	void updateShadowMaps();
	void renderScene(const glm::mat4& projection, const glm::mat4& view);
	void renderScenePBR(const glm::mat4& projection, const glm::mat4& view);
	void renderToScreen();
	void renderSkybox(const glm::mat4& projection, const glm::mat4& view);
	

	//utils
	void reloadShaders();
	auto getNativeWindow();

	//rtUtils
	void drawrtLights(const glm::mat4& projection, const glm::mat4& view);


	int m_width = 800, m_height = 600;
	Camera m_glcamera;
	GLFWwindow* m_window;
	ImGuiIO imgui_io;

	//helmirt
	helmirt::App app;
	helmirt::rtAreaLight m_rtArealight;
	bool m_drawrtprops = false;
	//system information
	FpsInfo m_fpsinfo;
	SystemInfo m_systeminfo;
	MouseInfo m_mouseinfo;
	Random m_rng;

	//scene information
	std::vector<Shader> m_shaders;
	std::vector<std::unique_ptr<Light>> m_lights;
	std::vector<Model> m_models;
	Animation m_animation;
	std::unique_ptr<Animator> m_animator = nullptr;

	CubeMap m_skybox;
	HDRCubeMap m_HDRenvmap;
	FrameBuffer m_fbo;
	HDRFrameBuffer m_hdrFBO;
	PingPongFrameBuffer m_pingpongBuffer;
	float exposure = 1.0f;
	//bloom options
	bool m_bloomOn = false;
	int m_numBloomIterations = 10;
	float m_bloomThreshold = 1.0f;

	//who has responsibility for shadowmaps (ligths???)
	ShadowMapBuffer m_shadowmap, m_shadowmap2;
	float m_scale = 1.0f;

	//temporary solution
	unsigned int qVAO, qVBO;
	unsigned int threeDqVAO, threeDqVBO;

	//imgui
	bool my_tool_active;

	bool show_rt = false;
	bool m_show_shadowmap = false;
	bool m_renderPBR = false;
	unsigned int getTextureId();

	//static wrapper class to handle glfw callbacks
	// (https://stackoverflow.com/questions/7676971/pointing-to-a-function-that-is-a-class-member-glfw-setkeycallback)
	class GLFWCallbackWrapper {
	public:
		GLFWCallbackWrapper() = delete;
		GLFWCallbackWrapper(const GLFWCallbackWrapper&) = delete;
		GLFWCallbackWrapper(GLFWCallbackWrapper&&) = delete;
		~GLFWCallbackWrapper() = delete;

		static void MouseCallback(GLFWwindow* window, double positionX, double positionY);
		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
		static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

		static void SetApplication(Application *application);

	private:
		static Application* s_application;
	};

};

