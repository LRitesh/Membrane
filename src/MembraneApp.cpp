#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Batch.h"
#include "cinder/MayaCamUI.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class MembraneApp : public AppNative {
	bool mIsFullScreen;

	// camera
	ci::MayaCamUI mMayaCam;
	CameraPersp mCam;

	gl::GlslProgRef	mSimpleShader;
	gl::GlslProgRef	mPostShader;
	gl::BatchRef mBatch;
	gl::FboRef mPostFbo;

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
	settings->setWindowSize( 1000, 700 );
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
		mPostShader = gl::GlslProg::create( loadAsset( "shaders/post.vert" ), loadAsset( "shaders/post.frag" ) );
	}
	catch( gl::GlslProgCompileExc ex ) {
		console() << "Unable to compile shader:\n" << ex.what() << endl;
	}

	mBatch = gl::Batch::create( geom::Cube(), mSimpleShader );
	// allocate post process FBO
	gl::Fbo::Format format;
	format.setSamples( 4 );
	format.setColorTextureFormat( gl::Texture2d::Format().internalFormat( GL_RGBA32F ) );
	mPostFbo = gl::Fbo::create( getWindowWidth(), getWindowHeight(), format );
	gl::enableDepthWrite();
	gl::enableDepthRead();
}

void MembraneApp::update()
{
	mCam.setAspectRatio( getWindowAspectRatio() );
}

void MembraneApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::color( Color( 0.2, 0.7, 1.0 ) );
	// bind to fbo
	mPostFbo->bindFramebuffer();
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatrices( mMayaCam.getCamera() );
	mBatch->draw();
	mPostFbo->unbindFramebuffer();
	// back to screen co-ordinates
	gl::setMatricesWindow( toPixels( getWindowSize() ) );
	mPostShader->bind();
	mPostFbo->getColorTexture()->bind();
	mPostShader->uniform( "renderedTexture", 0 );
	gl::drawSolidRect( getWindowBounds() );
	mPostFbo->getColorTexture()->unbind();
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
				mPostShader = gl::GlslProg::create( loadAsset( "shaders/post.vert" ), loadAsset( "shaders/post.frag" ) );
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
