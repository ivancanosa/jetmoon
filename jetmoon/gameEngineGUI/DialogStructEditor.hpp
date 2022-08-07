#pragma once
#include "gameEngineGUI/imguiComponents.hpp"
#include "core/ServiceContext.hpp"
#include "core/World.hpp"

class IDialogStructEditor{
public:
	virtual bool update(World* world, ServiceContext* serviceContext) = 0;
	virtual ~IDialogStructEditor(){};
};

template <class T>
class DialogStructEditor: IDialogStructEditor{
public:
	DialogStructEditor(T& data, bool onlineEditing=false): originalData(data), copyData(data), onlineEditing(onlineEditing){}
	bool update(World* world, ServiceContext* serviceContext) override{
		GLint viewport[4]{};
		glGetIntegerv(GL_VIEWPORT, viewport);
		ImVec2 size{(float)viewport[2]/3, (float)viewport[3]/3};
		ImGui::Begin("Edit Data Structure", &windowOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		if(firstFrame){
			firstFrame = false;
//			ImGui::SetWindowSize({size.x, size.y});
//			ImGui::SetWindowPos({(float)viewport[2]/2 - size.x/2, (float)viewport[3]/2});
		}

		if(onlineEditing){
			onlineEditingFunc(world, serviceContext);
		}else{
			offlineEditingFunc(world, serviceContext);
		}

		ImGui::End();

		return exitDialog;
	}

	void offlineEditingFunc(World* world, ServiceContext* serviceContext){
		imguiDataStruct(world, serviceContext, copyData, 0);
		if(ImGui::Button("Accept")){
			originalData = copyData;
			exitDialog = true;
		}
		ImGui::SameLine();
		if(ImGui::Button("Cancel") || !windowOpen){
			exitDialog = true;
		}
	}

	void onlineEditingFunc(World* world, ServiceContext* serviceContext){
		imguiDataStruct(world, serviceContext, originalData, 0);
		if(ImGui::Button("Accept")){
			exitDialog = true;
		}
		ImGui::SameLine();
		if(ImGui::Button("Cancel") || !windowOpen){
			originalData = copyData;
			exitDialog = true;
		}
	}


	~DialogStructEditor<T>(){};
private:
	bool firstFrame{true}, onlineEditing{false}, exitDialog{false}, windowOpen{true};
	T& originalData;
	T copyData;
};
