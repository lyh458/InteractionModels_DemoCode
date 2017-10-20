#ifndef REFL_H
#define REFL_H

#include <osg/TexGen>
#include <osg/Texture2D>
#include <osg/PolygonMode>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/ClipNode>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>


static const char* waterVert = {
    "uniform float osg_FrameTime;\n"
    "varying vec4 projCoords;\n"
    "varying vec3 lightDir, eyeDir;\n"

    "void main()\n"
    "{\n"
    " vec3 T = vec3(0.0, 1.0, 0.0);\n"
    " vec3 N = vec3(0.0, 0.0, 1.0);\n"
    " vec3 B = vec3(1.0, 0.0, 0.0);\n"
    " T = normalize(gl_NormalMatrix * T);\n"
    " B = normalize(gl_NormalMatrix * B);\n"
    " N = normalize(gl_NormalMatrix * N);\n"

    " mat3 TBNmat;\n"
    " TBNmat[0][0] = T[0]; TBNmat[1][0] = T[1]; TBNmat[2][0] = T[2];\n"
    " TBNmat[0][1] = B[0]; TBNmat[1][1] = B[1]; TBNmat[2][1] = B[2];\n"
    " TBNmat[0][2] = N[0]; TBNmat[1][2] = N[1]; TBNmat[2][2] = N[2];\n"

    " vec3 vertexInEye = vec3(gl_ModelViewMatrix * gl_Vertex);\n"
    " lightDir = gl_LightSource[0].position.xyz - vertexInEye;\n"
    " lightDir = normalize(TBNmat * lightDir);\n"
    " eyeDir = normalize(TBNmat * (-vertexInEye));\n"

    " gl_TexCoord[0] = gl_MultiTexCoord0;\n"
    " gl_TexCoord[4] = gl_MultiTexCoord4;\n"
    " gl_Position = ftransform();\n"
    " projCoords = gl_Position;\n"
    "}\n"
};

static const char* waterFrag = {
    "uniform sampler2D reflection;\n"
    "uniform sampler2D refraction;\n"
    "uniform sampler2D checkerboard;\n"
    "varying vec4 projCoords;\n"
    "varying vec3 lightDir, eyeDir;\n"

    "void main()\n"
    "{\n"
    " vec4 N = texture2D(checkerboard, gl_TexCoord[0]);\n"
    " N = N * 2.0 - vec4(1.0);\n"
    " N.a = 1.0; N = normalize(N);\n"

    " vec3 refVec = normalize(reflect(-lightDir, vec3(N) * 0.6));\n"
    " float refAngle = clamp(dot(eyeDir, refVec), 0.0, 1.0);\n"
    " vec4 specular = vec4(pow(refAngle, 40.0));\n"

    " vec2 dist = texture2D(refraction, 100*gl_TexCoord[0]).xy;\n"
    " dist = (dist * 2.0 - vec2(1.0)) * 0.1;\n"
    " vec2 uv = projCoords.xy / projCoords.w;\n"
    " uv = clamp((uv + 1.0) * 0.5 + dist, 0.0, 1.0);\n"

    " vec4 checker = texture2D(checkerboard, 5*gl_TexCoord[0].st);\n"
    " vec4 refl = texture2D(reflection, uv);\n"
    " vec4 color = 0.7*refl + specular;\n"
    " gl_FragColor = mix(0.5*color, checker, 0.5);\n"
    "}\n"
};

class osgReflectionHelper {

    osg::Geode* createScreenQuad( float width, float height, float scale )
    {
        osg::Geometry* geom = osg::createTexturedQuadGeometry(
            osg::Vec3(), osg::Vec3(width,0.0f,0.0f), osg::Vec3(0.0f,height,0.0f),
            0.0f, 0.0f, width*scale, height*scale );
        osg::ref_ptr<osg::Geode> quad = new osg::Geode;
        quad->addDrawable( geom );
        
        int values = osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED;
        quad->getOrCreateStateSet()->setAttribute(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL), values );
        quad->getOrCreateStateSet()->setMode( GL_LIGHTING, values );
        return quad.release();
    }

    osg::Camera* createRTTCamera( osg::Camera::BufferComponent buffer, osg::Texture* tex, bool isAbsolute )
    {
        osg::ref_ptr<osg::Camera> camera = new osg::Camera;
        camera->setClearColor( osg::Vec4() );
        camera->setClearMask( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );
        camera->setRenderTargetImplementation( osg::Camera::FRAME_BUFFER_OBJECT );
        camera->setRenderOrder( osg::Camera::PRE_RENDER, 1 );

        if ( tex )
        {
            tex->setFilter( osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR );
            tex->setFilter( osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR );
            camera->setViewport( 0, 0, tex->getTextureWidth(), tex->getTextureHeight() );
            camera->attach( buffer, tex );
        }
        
        if ( isAbsolute )
        {
            camera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
            camera->setProjectionMatrix( osg::Matrix::ortho2D(0.0, 1.0, 0.0, 1.0) );
            camera->setViewMatrix( osg::Matrix::identity() );
            camera->addChild( createScreenQuad(1.0f, 1.0f, 1.0f) );
        }
        return camera.release();
    }

osg::Texture2D* createTexture( const std::string& filename )
{
    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    texture->setImage( osgDB::readImageFile(filename) );
    texture->setWrap( osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT );
    texture->setWrap( osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT );
    texture->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR );
    texture->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
    return texture.release();
}

public:

std::vector<osg::ref_ptr<osg::Node>> _nodeList;

void addNodetoScene(osg::ref_ptr<osg::Node> node){
        _nodeList.push_back(node);
    }

osg::ref_ptr<osg::Node> getReflScene()
{
    osg::ref_ptr<osg::Group> sceneGroup = new osg::Group();

    for(int i = 0; i<_nodeList.size(); i++)
    {
        sceneGroup->addChild(_nodeList.at(i));
    }

   // The reversed node
    float z = -1.25f;
    osg::ref_ptr<osg::MatrixTransform> reverse = new osg::MatrixTransform;
    reverse->preMult(osg::Matrix::translate(0.0f, 0.0f, -z) *
                     osg::Matrix::scale(1.0f, 1.0f, -1.0f) *
                     osg::Matrix::translate(0.0f, 0.0f, z) );
    reverse->addChild( sceneGroup.get() );
    
    osg::ref_ptr<osg::ClipPlane> clipPlane = new osg::ClipPlane;
    clipPlane->setClipPlane( 0.0, 0.0, z, z );
    clipPlane->setClipPlaneNum( 0 );
    
    osg::ref_ptr<osg::ClipNode> clipNode = new osg::ClipNode;
    clipNode->addClipPlane( clipPlane.get() );
    clipNode->addChild( reverse.get() );
    
    // The RTT camera
    osg::ref_ptr<osg::Texture2D> tex2D = new osg::Texture2D;
    tex2D->setTextureSize( 512, 512 );
    tex2D->setInternalFormat( GL_RGBA );
    
    osg::ref_ptr<osg::Camera> rttCamera = createRTTCamera(osg::Camera::COLOR_BUFFER, tex2D.get(), false);
    rttCamera->addChild( clipNode.get() );
    
    // The water plane
    const osg::Vec3& center = osg::Vec3(0,0,0);
    float planeSize = 50.0f;
    osg::Vec3 planeCorner( center.x()-0.5f*planeSize, center.y()-0.5f*planeSize, z );
    osg::ref_ptr<osg::Geometry> quad = osg::createTexturedQuadGeometry(
        planeCorner, osg::Vec3(planeSize, 0.0f, 0.0f), osg::Vec3(0.0f, planeSize, 0.0f) );
    
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable( quad.get() );

    osg::StateSet* ss = geode->getOrCreateStateSet();
    ss->setTextureAttributeAndModes( 2, tex2D.get() );
    ss->setTextureAttributeAndModes( 4, createTexture("D:/VRSeminar/InteractionModels/data/objects/tex/water_DUDV.png") );
    ss->setTextureAttributeAndModes( 6, createTexture("D:/VRSeminar/InteractionModels/data/objects/tex/checkerboard_metric.png"));
    ss->setMode(GL_CULL_FACE,osg::StateAttribute::OFF);


    osg::ref_ptr<osg::Program> program = new osg::Program;
    program->addShader( new osg::Shader(osg::Shader::VERTEX, waterVert) );
    program->addShader( new osg::Shader(osg::Shader::FRAGMENT, waterFrag) );
    geode->getOrCreateStateSet()->setAttributeAndModes( program.get() );
    geode->getOrCreateStateSet()->addUniform( new osg::Uniform("reflection", 2) );
    geode->getOrCreateStateSet()->addUniform( new osg::Uniform("refraction", 4) );
    geode->getOrCreateStateSet()->addUniform( new osg::Uniform("checkerboard", 6));

    // Build the scene graph
    osg::ref_ptr<osg::Group> root = new osg::Group;
    root->addChild( rttCamera.get() );
    root->addChild( geode.get() );
    root->addChild( sceneGroup.get() );

    return root;
}

};
#endif REFL_H
