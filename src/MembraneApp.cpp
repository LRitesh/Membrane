#include "MembraneApp.h"

#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

void MembraneApp::prepareSettings( Settings* settings )
{
	float displayWidth = settings->getDisplay()->getWidth();
	float displayHeight = settings->getDisplay()->getHeight();
	settings->setWindowSize( displayWidth, displayHeight );
}

void MembraneApp::loadParams()
{
	mBloomIntensity = 1.0f;
	mParams->addParam( "Bloom Intensity", &mBloomIntensity ).min( 0.0f ).max( 5.0f ).step( 0.1f );

	mParticleScale = 8.0f;
	mParams->addParam( "Particle Scale", &mParticleScale ).min( 0.0f ).max( 10.0f ).step( 0.01f );

	// setup phong lighting
	mLight.Position = vec3( 0.0f, 0.0f, 10.0f );
	mLight.La = vec3( 0.0f, 0.0f, 0.0f );
	mLight.Ld = vec3( 1.0f, 1.0f, 1.0f );
	mLight.Ls = vec3( 1.0f, 1.0f, 1.0f );

	// setup phong material
	mMaterial.Ka = vec3( 1.0f, 1.0f, 1.0f );
	mMaterial.Kd = vec3( 1.0f, 1.0f, 1.0f );
	mMaterial.Ks = vec3( 1.0f, 1.0f, 1.0f );
	mMaterial.Shininess = 40.0f;

	mParams->addParam( "Light.Position", &mLight.Position );
	mParams->addParam( "Light.La", &mLight.La );
	mParams->addParam( "Light.Ld", &mLight.Ld );
	mParams->addParam( "Light.Ls", &mLight.Ls );

	mParams->addParam( "Material.Ka", &mMaterial.Ka );
	mParams->addParam( "Material.Kd", &mMaterial.Kd );
	mParams->addParam( "Material.Ks", &mMaterial.Ks );
	mParams->addParam( "Material.Shininess", &mMaterial.Shininess );
}

void MembraneApp::setup()
{
	mIsFullScreen = false;
	setFullScreen( mIsFullScreen );
	mCam.setPerspective( 60.0f, getWindowAspectRatio(), 1, 1000 );
	mMayaCam.setCurrentCam( mCam );
	mParams = params::InterfaceGl::create( getWindow(), "App parameters", toPixels( ivec2( 200, 400 ) ) );
	loadParams();

	// load shaders
	try {
		mSimpleShader = gl::GlslProg::create( loadAsset( "shaders/simple.vert" ), loadAsset( "shaders/simple.frag" ) );
		mPhongShader = gl::GlslProg::create( loadAsset( "shaders/phong.vert" ), loadAsset( "shaders/phong.frag" ), loadAsset( "shaders/phong.geom" ) );
		mBloomShader = gl::GlslProg::create( loadAsset( "shaders/bloom.vert" ), loadAsset( "shaders/bloom.frag" ) );
	}
	catch( gl::GlslProgCompileExc ex ) {
		console() << "Unable to compile shader:\n" << ex.what() << endl;
	}

	// allocate UBOs
	mLightUbo = gl::Ubo::create( sizeof( mLight ), &mLight, GL_DYNAMIC_DRAW );
	mMaterialUbo = gl::Ubo::create( sizeof( mMaterial ), &mMaterial, GL_DYNAMIC_DRAW );
	mLightUbo->bindBufferBase( 0 );
	mMaterialUbo->bindBufferBase( 1 );
	mPhongShader->uniformBlock( "Light", 0 );
	mPhongShader->uniformBlock( "Material", 1 );

	// create particles
	mParticles = vector<Particle>( 10 );
	mParticlesVbo = gl::Vbo::create( GL_ARRAY_BUFFER, mParticles, GL_STATIC_DRAW );
	geom::BufferLayout particleLayout;
	auto mesh = gl::VboMesh::create( mParticles.size(), GL_POINTS, { { particleLayout, mParticlesVbo } } );
	mParticleBatch = gl::Batch::create( mesh, mPhongShader );

	mLightBatch = gl::Batch::create( geom::Sphere().radius( 0.5f ), mSimpleShader );

	// allocate bloom FBO
	gl::Fbo::Format format;
	format.setSamples( 4 );
	format.setColorTextureFormat( gl::Texture2d::Format().internalFormat( GL_RGBA32F ) );
	mBloomFbo = gl::Fbo::create( getWindowWidth(), getWindowHeight(), format );

	gl::enableDepthWrite();
	gl::enableDepthRead();
	//gl::enableAlphaBlending();
	//gl::enableAdditiveBlending();
}

void MembraneApp::update()
{
	float time = ( float )getElapsedSeconds();
	mLight.Position = glm::vec3( sin( time ) * 15.0f, cos( time ) * 15.0f, cos( time ) * 15.0f );

	// buffer our data to our UBO to reflect any changed parameters
	mLightUbo->bufferSubData( 0, sizeof( mLight ), &mLight );
	mMaterialUbo->bufferSubData( 0, sizeof( mMaterial ), &mMaterial );
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
		gl::setModelMatrix( translate( particle.mPosition ) );
		gl::multModelMatrix( rotate( time, particle.mRotation ) );
		//gl::multModelMatrix( rotate( time / 2.0f, particle.mRotation ) * translate( particle.mPosition ) );
		mPhongShader->bind();
		mPhongShader->uniform( "color", particle.mColor );
		mPhongShader->uniform( "scale", mParticleScale );
		mParticleBatch->draw();
		gl::popMatrices();
	}

	gl::setModelMatrix( translate( mLight.Position ) );
	mLightBatch->getGlslProg()->uniform( "color", ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	mLightBatch->draw();
	mBloomFbo->unbindFramebuffer();

	// back to screen co-ordinates
	gl::setMatricesWindow( toPixels( getWindowSize() ) );
	mBloomShader->bind();
	mBloomFbo->getColorTexture()->bind();
	mBloomShader->uniform( "renderedTexture", 0 );
	mBloomShader->uniform( "bloomIntensity", mBloomIntensity );
	gl::drawString( to_string( getAverageFps() ), vec2( 10.0f, 10.0f ) );
	gl::drawSolidRect( getWindowBounds() );
	mBloomFbo->getColorTexture()->unbind();
	mParams->draw();
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
				mPhongShader = gl::GlslProg::create( loadAsset( "shaders/phong.vert" ), loadAsset( "shaders/phong.frag" ), loadAsset( "shaders/phong.geom" ) );
				mBloomShader = gl::GlslProg::create( loadAsset( "shaders/bloom.vert" ), loadAsset( "shaders/bloom.frag" ) );
			}
			catch( gl::GlslProgCompileExc ex ) {
				console() << "Unable to compile shader:\n" << ex.what() << endl;
			}

			mLightBatch = gl::Batch::create( geom::Sphere().radius( 0.5f ), mSimpleShader );
			mPhongShader->uniformBlock( "Light", 0 );
			mPhongShader->uniformBlock( "Material", 1 );
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

CINDER_APP_NATIVE( MembraneApp, RendererGl( RendererGl::Options().msaa( 4 ) ) )
