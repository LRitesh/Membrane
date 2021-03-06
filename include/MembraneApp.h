#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/Ubo.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Vao.h"
#include "cinder/CameraUi.h"
#include "cinder/params/Params.h"

#include "Particle.h"

// This is dependent on the C++ compiler structuring these vars in RAM the same way that GL's std140 does
// so always align data as 4N or NNNN
// more info: http://www.opentk.com/node/2926
struct LightInfo {
	ci::vec3 Position;
	float PositionPadding;	// to align a vec3 to 4N
	ci::vec3 La;	// ambient light intensity
	float LaPadding;
	ci::vec3 Ld;	// diffuse light intensity
	float LdPadding;
	ci::vec3 Ls;	// specular light intensity
	float LsPadding;
};

struct MaterialInfo {
	ci::vec3 Ka;	// ambient reflectivity
	float KaPadding;
	ci::vec3 Kd;	// diffuse reflectivity
	float KdPadding;
	ci::vec3 Ks;	// specular reflectivity
	float Shininess;	// specular shininess
};

class MembraneApp : public ci::app::App {
	bool mIsFullScreen;
	bool mDrawLight;

	// particles
	ci::gl::BatchRef mParticleBatch;
	std::vector<Particle> mParticles;
	float mParticleScale;
	ci::gl::VaoRef mParticleAttributes[2];
	// two VBOs for transform feedback
	ci::gl::VboRef mParticlesVbo[2];
	std::uint32_t mSourceIndex = 0;
	std::uint32_t mDestinationIndex = 1;

	ci::params::InterfaceGlRef mParams;

	// camera
	ci::CameraPersp mCam;
	ci::CameraUi mCamUi;

	// shaders
	ci::gl::GlslProgRef mUpdateShader;
	ci::gl::GlslProgRef	mSimpleShader;
	ci::gl::GlslProgRef	mPhongShader;
	ci::gl::GlslProgRef	mBloomShader;

	// bloom
	ci::gl::FboRef mBloomFbo;
	float mBloomIntensity;

	// phong light
	ci::gl::BatchRef mLightBatch;
	LightInfo mLight;
	ci::gl::UboRef mLightUbo;

	// phong material
	MaterialInfo mMaterial;
	ci::gl::UboRef mMaterialUbo;

  public:
	void prepareSettings( Settings* settings );
	void loadParams();
	void setup() override;
	void update() override;
	void draw() override;
	void resize() override;
	void keyDown( ci::app::KeyEvent event ) override;
};
