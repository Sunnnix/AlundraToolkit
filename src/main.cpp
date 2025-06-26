#include <stdio.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_opengl3.h>
#include <format>
#include <algorithm>
#include "util.h"
#include "Loader/engine.h"

ImGuiIO* io;

int initImGui(SDL_Window* window, SDL_GLContext* context);
void imgUIEventPolling(SDL_Event* event);
void RenderImGui(RenderState rs, Engine* level);

int main(int argc, char* argv[]) {
	Engine engine("test/DATAS.BIN");
	if (engine.Init()) {
		printf("Initialization failed!");
		return -1;
	}

	if (initImGui(engine.GetWindow(), engine.GetContext())) {
		printf("ImGui initialization failed!");
		if (io)
			delete io;
		return -1;
	}

	engine.Run(imgUIEventPolling, RenderImGui);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	return 0;
}

int initImGui(SDL_Window* window, SDL_GLContext* context) {
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO(); (void)io;
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(window, *context);
	ImGui_ImplOpenGL3_Init("#version 330");

	return 0;
}

void imgUIEventPolling(SDL_Event* event) {
	ImGui_ImplSDL2_ProcessEvent(event);
}

void imGuiMinimap(Camera* cam) {

	// Preparation
	Rect camRect = cam->GetViewRect();
	ImVec2 worldSize(1248, 960);
	int scale = 5;

	ImVec2 mapStartPos = ImGui::GetCursorScreenPos();
	mapStartPos.x += 15;
	mapStartPos.y += 15;
	ImVec2 mapSize(worldSize.x / scale, worldSize.y / scale);
	ImVec2 mapEndPos(mapStartPos.x + mapSize.x, mapStartPos.y + mapSize.y);

	ImVec2 camStartPos(mapStartPos.x + camRect.X / scale, mapStartPos.y + camRect.Y / scale);
	ImVec2 camSize(camRect.Width / scale, camRect.Height / scale);
	ImVec2 camEndPos(camStartPos.x + camSize.x, camStartPos.y + camSize.y);

	// Drawing
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	
	drawList->AddRectFilled(mapStartPos, mapEndPos, IM_COL32(40, 40, 40, 255));
	drawList->AddRect(mapStartPos, mapEndPos, IM_COL32(255, 255, 255, 255));

	drawList->AddRectFilled(camStartPos, camEndPos, IM_COL32(200, 200, 0, 180));
	drawList->AddRect(camStartPos, camEndPos, IM_COL32(255, 255, 0, 255));

	// Interaction
	ImVec2 invButtonSpace(mapSize.x + 30, mapSize.y + 30);
	ImGui::InvisibleButton("##MinimapButton", invButtonSpace);
	ImVec2 mouse = io->MousePos;
	if (ImGui::IsMouseDown(0) && ImGui::IsItemHovered()) {
		ImVec2 dMouse(mouse.x - mapStartPos.x - camSize.x / 2, mouse.y - mapStartPos.y - camSize.y / 2);
		dMouse.x = std::max(0.0f, std::min(mapSize.x - camSize.x, dMouse.x));
		dMouse.y = std::max(0.0f, std::min(mapSize.y - camSize.y, dMouse.y));
		cam->SetPosition(Point(dMouse.x * scale, dMouse.y * scale));
	}
}

void imGuiDebugWindow(Engine* engine) {
	ImGui::Begin("Debug Window");

	// Level
	int levelI = engine->GetLevelIndex();
	if (ImGui::SliderInt("##Level", &levelI, 0, Level::LEVEL_MAX_INDEX))
		engine->SetLevelIndex(levelI);
	ImGui::SameLine();
	if(ImGui::Button("-##Level"))
		engine->SetLevelIndex(levelI - 1);
	ImGui::SameLine();
	if(ImGui::Button("+##Level"))
		engine->SetLevelIndex(levelI + 1);
	ImGui::SameLine();
	ImGui::Text("Level");

	imGuiMinimap(engine->GetCamera());

	ImGui::End();
}

void RenderImGui(RenderState rs, Engine* engine) {
	if (rs != POST_RENDER)
		return;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	imGuiDebugWindow(engine);

	// Rendering
	glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Docking
	if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}
}