#pragma once
#include <unordered_map>
#include <string>

const std::string imagePayload					= "imagePayload";
const std::string animationControllerPayload	= "animationControllerPayload";
const std::string prefabPayload					= "prefabPayload";
const std::string scenePayload					= "scenePayload";
const std::string jsonPayload					= "jsonPayload";
const std::string audioPayload					= "audioPayload";
const std::string entityPayload					= "entityPayload";

const std::unordered_map<std::string, std::string> extensionToPayload{
	{".png", imagePayload}, {".jpg", imagePayload}, {".jpeg", imagePayload},
	{".animationController", animationControllerPayload}, {".prefab", prefabPayload},
	{".scene", scenePayload},	{".json", jsonPayload}, {".mp3", audioPayload}, {".wav", audioPayload}
};
