#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Batch.h"
#include "cinder/MayaCamUI.h"
#include "cinder/Rand.h"

#include "Particle.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class MembraneApp : public AppNative {
	bool mIsFullScreen;

	// camera
	ci::MayaCamUI mMayaCam;
	CameraPersp mCam;

	gl::GlslProgRef	mSimpleShader;
	gl::GlslProgRef	mBloomShader;
	gl::BatchRef mBatch;
	gl::FboRef mBloomFbo;
	mat4 mCubeRotation;

	vector<Particle> mParticles;

  public:
	void prepareSettings( Settings* settings ) override;
	void setup() override;
	void update() override;
	void draw() override;
	void resize() override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void keyDown( KeyEvent event ) override;
};

void MembraneApp::prepareSettings( Settings* settings )
{
	float displayWidth = settings->getDisplay()->getWidth();
	float displayHeight = settings->getDisplay()->getHeight();
	settings->setWindowSize( displayWidth, displayHeight );
}

void MembraneApp::setup()
{
	mIsFullScreen = false;
	setFullScreen( mIsFullScreen );
	mCam.setPerspective( 60.0f, getWindowAspectRatio(), 1, 1000 );
	mMayaCam.setCurrentCam( mCam );

	// load shaders
	try {
		mSimpleShader = gl::GlslProg::create( loadAsset( "shaders/simple.vert" ), loadAsset( "shaders/simple.frag" ) );
	}
	catch( gl::GlslProgCompileExc ex ) {
		console() << "Unable to compile shader:\n" << ex.what() << endl;
	}

	try {
		mBloomShader = gl::GlslProg::create( loadAsset( "shaders/bloom.vert" ), loadAsset( "shaders/bloom.frag" ) );
	}
	catch( gl::GlslProgCompileExc ex ) {
		console() << "Unable to compile shader:\n" << ex.what() << endl;
	}

	mBatch = gl::Batch::create( geom::Cube(), mSimpleShader );
	mParticles = vector<Particle>( 200 );
	// allocate bloom FBO
	gl::Fbo::Format format;
	format.setSamples( 4 );
	format.setColorTextureFormat( gl::Texture2d::Format().internalFormat( GL_RGBA32F ) );
	mBloomFbo = gl::Fbo::create( getWindowWidth(), getWindowHeight(), format );
	gl::enableDepthWrite();
	gl::enableDepthRead();
	gl::enableAlphaBlending();
	//gl::enableAdditiveBlending();
}

void MembraneApp::update()
{
	mCam.setAspectRatio( getWindowAspectRatio() );
}

void MembraneApp::draw()
{
	float time = ( float )getElapsedSeconds();
	gl::clear( Color( 0, 0, 0 ) );
	gl::color( Color( 0.2, 0.7, 1.0 ) );
	// bind to fbo
	mBloomFbo->bindFramebuffer();
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatrices( mMayaCam.getCamera() );

	for( auto particle : mParticles ) {
		gl::pushMatrices();
		//gl::setModelMatrix( translate( particle.mPosition ) );
		//gl::multModelMatrix( rotate( time, particle.mRotation ) );
		gl::multModelMatrix( rotate( time / 2.0f, particle.mRotation ) * translate( particle.mPosition ) );
		mBatch->getGlslProg()->uniform( "color", particle.mColor );
		mBatch->draw();
		gl::popMatrices();
	}

	mBloomFbo->unbindFramebuffer();
	// back to screen co-ordinates
	gl::setMatricesWindow( toPixels( getWindowSize() ) );
	mBloomShader->bind();
	mBloomFbo->getColorTexture()->bind();
	mBloomShader->uniform( "renderedTexture", 0 );
	gl::drawString( to_string( getAverageFps() ), vec2( 10.0f, 10.0f ) );
	gl::drawSolidRect( getWindowBounds() );
	mBloomFbo->getColorTexture()->unbind();
}

void MembraneApp::resize()
{
	mCam.setPerspective( 60.0f, getWindowAspectRatio(), 1, 1000 );
	mMayaCam.setCurrentCam( mCam );
}

void MembraneApp::mouseDown( MouseEvent event )
{
	mMayaCam.mouseDown( event.getPos() );
}

void MembraneApp::mouseDrag( MouseEvent event )
{
	mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
}

void MembraneApp::keyDown( KeyEvent event )
{
	switch( event.getChar() ) {
		case KeyEvent::KEY_y:
			try {
				mSimpleShader = gl::GlslProg::create( loadAsset( "shaders/simple.vert" ), loadAsset( "shaders/simple.frag" ) );
			}
			catch( gl::GlslProgCompileExc ex ) {
				console() << "Unable to compile shader:\n" << ex.what() << endl;
			}

			try {
				mBloomShader = gl::GlslProg::create( loadAsset( "shaders/bloom.vert" ), loadAsset( "shaders/bloom.frag" ) );
			}
			catch( gl::GlslProgCompileExc ex ) {
				console() << "Unable to compile shader:\n" << ex.what() << endl;
			}

			mBatch = gl::Batch::create( geom::Cube(), mSimpleShader );
			break;

		case KeyEvent::KEY_f:
			mIsFullScreen = !mIsFullScreen;
			setFullScreen( mIsFullScreen );
			break;

		case KeyEvent::KEY_ESCAPE:
			quit();
			break;
	}
}

CINDER_APP_NATIVE( MembraneApp, RendererGl )
