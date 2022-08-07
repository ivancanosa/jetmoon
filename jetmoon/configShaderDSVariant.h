#pragma once

#include <variant>
#include <string>
#include "core/definitions.hpp"

#include <memory>
#include <iosfwd>                        // for string
#include <vector>                        // for vector
#include "utils/visit_struct.hpp"        // for accessor, type_c, VISIT_STRU...

#include "utils/ComponentInspector.hpp"



struct ShaderGraph;

struct TypedShaderGraph{
	bool generated{false};
	std::shared_ptr<ShaderGraph> shaderGraph{nullptr};

	virtual void generate() = 0;
	virtual void overrideFields() = 0;
	ShaderGraph* getShaderGraph(){
		if(!generated){
			this->generate();
			generated = true;
		}
		overrideFields();
		return &(*shaderGraph);
	}
	virtual ~TypedShaderGraph(){}
};


struct PostInvertColor{
	std::string shader{"invertColor"}; 
	int value1;
};
COMPONENT_INSPECTOR(PostInvertColor, value1);

struct GammaCorrection{
	std::string shader{"gammaCorrection"}; 
	float gamma{1.};
};
COMPONENT_INSPECTOR(GammaCorrection, gamma);

struct ToneMapping{
	std::string shader{"toneMapping"}; 
	float exposure{1.};
	float gamma{1.};
};
COMPONENT_INSPECTOR(ToneMapping, exposure, gamma);


struct ColorCorrection{
	std::string shader{"colorCorrection"}; 
	float contrast{1.}, brightness{0.}, saturation{1.}, gamma{1.};
	Vec3 colorFiltering{1., 1., 1.};
};
COMPONENT_INSPECTOR(ColorCorrection, contrast, brightness, saturation, gamma, colorFiltering);

struct ColorMixer{
	std::string shader{"colorMixer"}; 
	float intensity{0.5};
	Vec4 color{0.5, 0.5, 0.5, 1.0};
};
COMPONENT_INSPECTOR(ColorMixer, intensity, color);

struct ChromaticAberration{
	std::string shader{"chromaticAberration"}; 
	float rOffset{0.0005};
	float gOffset{-0.0005};
	float bOffset{};
};
COMPONENT_INSPECTOR(ChromaticAberration, rOffset, gOffset, bOffset);


struct Vignette{
	std::string shader{"vignette"}; 
	float intensity{0.6};
	float innerRadius{0.4};
	float outerRadius{0.65};
};
COMPONENT_INSPECTOR(Vignette, intensity, innerRadius, outerRadius);

//===============================================
//=================== Auxiliary Shaders =========
//===============================================

struct AddTexture{
	std::string shader{"addTexture"}; 
	float intensity{0.6};
};
COMPONENT_INSPECTOR(AddTexture, intensity);

struct ColorGradient{
	std::string shader{"colorGradient"}; 
	Color3 color0{0., 0., 0.};
	Color3 color1{1., 1., 1.};
};
COMPONENT_INSPECTOR(ColorGradient, color0, color1);

struct Step{
	std::string shader{"step"}; 
	float threshold{0.5};
};
COMPONENT_INSPECTOR(Step, threshold);



struct PerlinNoise{
	std::string shader{"perlinNoise"}; 
	float frequency{10};
	float octaves{6};
	float persistence{0.7};
};
COMPONENT_INSPECTOR(PerlinNoise, frequency, octaves, persistence);

struct CellularNoise{
	std::string shader{"cellularNoise"}; 
	float frequency{10};
};
COMPONENT_INSPECTOR(CellularNoise, frequency);


struct Bloom0{
	std::string shader{"bloom0"}; 
	float threshold{1.};
};
COMPONENT_INSPECTOR(Bloom0, threshold);

struct Bloom1{
	std::string shader{"bloom1"}; 
	float threshold{1.};
};
COMPONENT_INSPECTOR(Bloom1, threshold);


struct Bloom: public TypedShaderGraph{
	float threshold{1.};
	float intensity{1.};
	void generate() override;
	void overrideFields() override;
};
COMPONENT_INSPECTOR(Bloom, threshold, intensity);

//===============================================
//================ Shader Variant ===============
//===============================================

using ShaderDSVariant = std::variant<
	PostInvertColor,
	GammaCorrection,
	ToneMapping,
	ColorCorrection,
	ColorMixer,
	ChromaticAberration,
	Vignette,
	Bloom,

	//These should only be used for shader graph
	PerlinNoise,
	CellularNoise,
	ColorGradient,
	Step,
	AddTexture,
	Bloom0,
	Bloom1


>;

//===============================================
//================ Shader Graph =================
//===============================================

struct ShaderConnection{
	int shader0{0}, shader1{0};
	int tex0{0}, tex1{0};
};
COMPONENT_INSPECTOR(ShaderConnection, shader0, shader1, tex0, tex1);

//Assumptions:
//pos -1 is the camera framebuffer
//The framebuffer of the last shader of the vector will be copied to the camera
struct ShaderGraph{
	std::vector<ShaderDSVariant> shaders{};
	std::vector<ShaderConnection> connections{};
};
COMPONENT_INSPECTOR(ShaderGraph, shaders, connections);


