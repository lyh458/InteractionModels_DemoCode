#include <osgShadow/ViewDependentShadowMap>
#include <osgShadow/ShadowedScene>
#include <osg/CullFace>
#include <osg/Geode>
#include <osg/io_utils>
#include <osg/Camera>

#include <sstream>
#include <iostream>

// the cave shadow bug seems to be caused by Viewdependenshadowmap.cpp:2420 (stateset->setTextureMode). See this file on line 266.
// these flags in WarperWrapper.cpp dont change the behaviour:
// glDisable(GL_TEXTURE_GEN_S);
// glDisable(GL_TEXTURE_GEN_T);
// glEnable(GL_TEXTURE_GEN_R);
// glEnable(GL_TEXTURE_GEN_Q);

// same results with
// glDisable(GL_TEXTURE_GEN_S);
// glDisable(GL_TEXTURE_GEN_T);
// glDisable(GL_TEXTURE_GEN_R);
// glEnable(GL_TEXTURE_GEN_Q);
//

//class ownPostDrawCallback : public osg::Camera::DrawCallback
//{
//public:
//    osgShadow::ViewDependentShadowMap::ViewDependentData vdd;

//    void setVDD(osgShadow::ViewDependentShadowMap::ViewDependentData &data){
//        vdd = data;
//    }

//    virtual ~ownPostDrawCallback(){}

//    void operator() (osg::RenderInfo& renderInfo){
//        vdd.getStateSet()->clear();
//    }

//};
//struct MyCameraPostDrawCallback : public osg::Camera::DrawCallback
//{
//    MyCameraPostDrawCallback(osgShadow::ViewDependentShadowMap::ViewDependentData& vdd, osgShadow::ViewDependentShadowMap::ShadowData& sd):
//        _vdd(vdd),
//        _sd(sd)
//    {
//    }

//    virtual void operator () (const osg::Camera& /*camera*/) const
//    {
//        //_vdd.releaseGLObjects();
//        //_vdd.getStateSet()->clear();
//        _vdd.getStateSet()->setTextureMode(_sd._textureUnit,GL_TEXTURE_GEN_S,osg::StateAttribute::OFF);
//        _vdd.getStateSet()->setTextureMode(_sd._textureUnit,GL_TEXTURE_GEN_T,osg::StateAttribute::OFF);
//        _vdd.getStateSet()->setTextureMode(_sd._textureUnit,GL_TEXTURE_GEN_R,osg::StateAttribute::OFF);
//        _vdd.getStateSet()->setTextureMode(_sd._textureUnit,GL_TEXTURE_GEN_Q,osg::StateAttribute::OFF);
//    }

//    osgShadow::ViewDependentShadowMap::ViewDependentData& _vdd;
//    osgShadow::ViewDependentShadowMap::ShadowData& _sd;
//};

using namespace osgShadow;

class VDSMCameraCullCallback : public osg::NodeCallback
{
    public:

        VDSMCameraCullCallback(ViewDependentShadowMap* vdsm, osg::Polytope& polytope);

        virtual void operator()(osg::Node*, osg::NodeVisitor* nv);

        osg::RefMatrix* getProjectionMatrix() { return _projectionMatrix.get(); }
        osgUtil::RenderStage* getRenderStage() { return _renderStage.get(); }

    protected:

        ViewDependentShadowMap*                 _vdsm;
        osg::ref_ptr<osg::RefMatrix>            _projectionMatrix;
        osg::ref_ptr<osgUtil::RenderStage>      _renderStage;
        osg::Polytope                           _polytope;
};

class ComputeLightSpaceBounds : public osg::NodeVisitor, public osg::CullStack
{
public:
    ComputeLightSpaceBounds(osg::Viewport* viewport, const osg::Matrixd& projectionMatrix, osg::Matrixd& viewMatrix):
        osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN)
    {
        setCullingMode(osg::CullSettings::VIEW_FRUSTUM_CULLING);

        pushViewport(viewport);
        pushProjectionMatrix(new osg::RefMatrix(projectionMatrix));
        pushModelViewMatrix(new osg::RefMatrix(viewMatrix),osg::Transform::ABSOLUTE_RF);
    }

    void apply(osg::Node& node)
    {
        if (isCulled(node)) return;

        // push the culling mode.
        pushCurrentMask();

        traverse(node);

        // pop the culling mode.
        popCurrentMask();
    }

    void apply(osg::Geode& node)
    {
        if (isCulled(node)) return;

        // push the culling mode.
        pushCurrentMask();

        for(unsigned int i=0; i<node.getNumDrawables();++i)
        {
            if (node.getDrawable(i))
            {
                updateBound(node.getDrawable(i)->getBound());
            }
        }

        // pop the culling mode.
        popCurrentMask();
    }

    void apply(osg::Billboard&)
    {
        OSG_INFO<<"Warning Billboards not yet supported"<<std::endl;
        return;
    }

    void apply(osg::Projection&)
    {
        // projection nodes won't affect a shadow map so their subgraphs should be ignored
        return;
    }

    void apply(osg::Transform& transform)
    {
        if (isCulled(transform)) return;

        // push the culling mode.
        pushCurrentMask();

        // absolute transforms won't affect a shadow map so their subgraphs should be ignored.
        if (transform.getReferenceFrame()==osg::Transform::RELATIVE_RF)
        {
            osg::ref_ptr<osg::RefMatrix> matrix = new osg::RefMatrix(*getModelViewMatrix());
            transform.computeLocalToWorldMatrix(*matrix,this);
            pushModelViewMatrix(matrix.get(), transform.getReferenceFrame());

            traverse(transform);

            popModelViewMatrix();
        }

        // pop the culling mode.
        popCurrentMask();

    }

    void apply(osg::Camera&)
    {
        // camera nodes won't affect a shadow map so their subgraphs should be ignored
        return;
    }

    void updateBound(const osg::BoundingBox& bb)
    {
        if (!bb.valid()) return;

        const osg::Matrix& matrix = *getModelViewMatrix() * *getProjectionMatrix();

        update(bb.corner(0) * matrix);
        update(bb.corner(1) * matrix);
        update(bb.corner(2) * matrix);
        update(bb.corner(3) * matrix);
        update(bb.corner(4) * matrix);
        update(bb.corner(5) * matrix);
        update(bb.corner(6) * matrix);
        update(bb.corner(7) * matrix);
    }

    void update(const osg::Vec3& v)
    {
        if (v.z()<-1.0f)
        {
            //OSG_NOTICE<<"discarding("<<v<<")"<<std::endl;
            return;
        }
        float x = v.x();
        if (x<-1.0f) x=-1.0f;
        if (x>1.0f) x=1.0f;
        float y = v.y();
        if (y<-1.0f) y=-1.0f;
        if (y>1.0f) y=1.0f;
        _bb.expandBy(osg::Vec3(x,y,v.z()));
    }

    osg::BoundingBox _bb;
};

class ownViewDependentShadowMap : public osgShadow::ViewDependentShadowMap{

public:
        osg::StateSet* _vddstate = NULL;
    /**
     * @brief Reimplementation of ViewDependentShadowMap::createShaders() for custom shader use
     * @details Reimplementation of ViewDependentShadowMap::createShaders() for custom shader use, transparent shadow
     *          0.5 + 0.5*... 50% Color, 50% Shadow 
     *          color *= mix( colorAmbientEmissive, gl_Color, 0.5 + 0.5*shadow2DProj( shadowTexture0, gl_TexCoord[_shadowTextureUnit0_] ).r );
     */
            void createShaders()
            {

                static std::string* own_fragmentShaderSource_withBaseTexture = new std::string(
                    "#version 120                                                            \n"
                    "uniform sampler2D baseTexture;                                          \n"
                    "uniform int baseTextureUnit;                                            \n"
                    "uniform sampler2DShadow shadowTexture0;                                 \n"
                    // "uniform int shadowTextureUnit0;                                         \n"
                    "                                                                        \n"
                    "void main(void)                                                         \n"
                    "{                                                                       \n"
                    "  vec4 colorAmbientEmissive = gl_FrontLightModelProduct.sceneColor;     \n"
                    "  vec4 color = texture2D( baseTexture, gl_TexCoord[0].xy );                                              \n"
                    "  color *= mix( colorAmbientEmissive, gl_Color, 0.5 + 0.5*shadow2DProj( shadowTexture0, gl_TexCoord[_shadowTextureUnit0_] ).r );     \n"
                    "  gl_FragColor = color;                                                                                                \n"
                    "} \n");

                //We don't use this one, but it has to be there
                static std::string* fragmentShaderSource_withBaseTexture_twoShadowMaps = new std::string(
                        "uniform sampler2D baseTexture;                                          \n"
                        "uniform int baseTextureUnit;                                            \n"
                        "uniform sampler2DShadow shadowTexture0;                                 \n"
                        // "uniform int shadowTextureUnit0;                                         \n"
                        "uniform sampler2DShadow shadowTexture1;                                 \n"
                        // "uniform int shadowTextureUnit1;                                         \n"
                        "                                                                        \n"
                        "void main(void)                                                         \n"
                        "{                                                                       \n"
                        "  vec4 colorAmbientEmissive = gl_FrontLightModelProduct.sceneColor;     \n"
                        "  vec4 color = texture2D( baseTexture, gl_TexCoord[0].xy );              \n"
                        "  float shadow0 = shadow2DProj( shadowTexture0, gl_TexCoord[_shadowTextureUnit0_] ).r;   \n"
                        "  float shadow1 = shadow2DProj( shadowTexture1, gl_TexCoord[_shadowTextureUnit1_] ).r;   \n"
                        "  color *= mix( colorAmbientEmissive, gl_Color, shadow0*shadow1 );                     \n"
                        "  gl_FragColor = color;                                                                \n"
                        "} \n");

                unsigned int _baseTextureUnit = 0;

                OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_accessUnfiromsAndProgramMutex);

                _shadowCastingStateSet = new osg::StateSet;

                ShadowSettings* settings = getShadowedScene()->getShadowSettings();

                if (!settings->getDebugDraw())
                {
                    // note soft (attribute only no mode override) setting. When this works ?
                    // 1. for objects prepared for backface culling
                    //    because they usually also set CullFace and CullMode on in their state
                    //    For them we override CullFace but CullMode remains set by them
                    // 2. For one faced, trees, and similar objects which cannot use
                    //    backface nor front face so they usually use CullMode off set here.
                    //    In this case we will draw them in their entirety.

                    _shadowCastingStateSet->setAttribute( new osg::CullFace( osg::CullFace::FRONT ), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );

                    // make sure GL_CULL_FACE is off by default
                    // we assume that if object has cull face attribute set to back
                    // it will also set cull face mode ON so no need for override
                    _shadowCastingStateSet->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );
                }

            #if 1
                float factor = 1.1;
                float units =  4.0;
            #else
                float factor = -1.1;
                float units =  -4.0;
            #endif
                _polygonOffset = new osg::PolygonOffset(factor, units);
                _shadowCastingStateSet->setAttribute(_polygonOffset.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
                _shadowCastingStateSet->setMode(GL_POLYGON_OFFSET_FILL, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);


                _uniforms.clear();
                osg::ref_ptr<osg::Uniform> baseTextureSampler = new osg::Uniform("baseTexture",(int)_baseTextureUnit);
                _uniforms.push_back(baseTextureSampler.get());

                osg::ref_ptr<osg::Uniform> baseTextureUnit = new osg::Uniform("baseTextureUnit",(int)_baseTextureUnit);
                _uniforms.push_back(baseTextureUnit.get());

                for(unsigned int sm_i=0; sm_i<settings->getNumShadowMapsPerLight(); ++sm_i)
                {
                    {
                        std::stringstream sstr;
                        sstr<<"shadowTexture"<<sm_i;
                        osg::ref_ptr<osg::Uniform> shadowTextureSampler = new osg::Uniform(sstr.str().c_str(),(int)(settings->getBaseShadowTextureUnit()+sm_i));
                        _uniforms.push_back(shadowTextureSampler.get());
                    }

                    {
                        // std::stringstream sstr;
                        // sstr<<"shadowTextureUnit"<<sm_i;
                        // osg::ref_ptr<osg::Uniform> shadowTextureUnit = new osg::Uniform(sstr.str().c_str(),(int)(settings->getBaseShadowTextureUnit()+sm_i));
                        // _uniforms.push_back(shadowTextureUnit.get());

                        std::stringstream sstr;
                        sstr.clear();
                        sstr<<"_shadowTextureUnit"<<sm_i<<"_";
                        std::string toreplace = std::string(sstr.str());
                        std::size_t start = own_fragmentShaderSource_withBaseTexture->find(toreplace);
                        std::stringstream ss;
                        ss.clear();
                        ss << (int)(settings->getBaseShadowTextureUnit()+sm_i);
                        if (start != std::string::npos)
                        {
                            own_fragmentShaderSource_withBaseTexture->replace(start, toreplace.size(), ss.str().c_str());
                        }
                    }
                }

                switch(settings->getShaderHint())
                {
                    case(ShadowSettings::NO_SHADERS):
                    {
                        OSG_INFO<<"No shaders provided by, user must supply own shaders"<<std::endl;
                        break;
                    }
                    case(ShadowSettings::PROVIDE_VERTEX_AND_FRAGMENT_SHADER):
                    case(ShadowSettings::PROVIDE_FRAGMENT_SHADER):
                    {
                        _program = new osg::Program;

                        //osg::ref_ptr<osg::Shader> fragment_shader = new osg::Shader(osg::Shader::FRAGMENT, fragmentShaderSource_noBaseTexture);
                        if (settings->getNumShadowMapsPerLight()==2)
                        {
                            auto s = new osg::Shader(osg::Shader::FRAGMENT);
                            s->setShaderSource(fragmentShaderSource_withBaseTexture_twoShadowMaps->c_str());
                            _program->addShader(s);
                        }
                        else
                        {
                            auto s = new osg::Shader(osg::Shader::FRAGMENT);
                            s->setShaderSource(own_fragmentShaderSource_withBaseTexture->c_str());
                            _program->addShader(s);
                        }

                        break;
                    }
                }

                {
                    osg::ref_ptr<osg::Image> image = new osg::Image;
                    image->allocateImage( 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE );
                    *(osg::Vec4ub*)image->data() = osg::Vec4ub( 0xFF, 0xFF, 0xFF, 0xFF );

                    _fallbackBaseTexture = new osg::Texture2D(image.get());
                    _fallbackBaseTexture->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::REPEAT);
                    _fallbackBaseTexture->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::REPEAT);
                    _fallbackBaseTexture->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::NEAREST);
                    _fallbackBaseTexture->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::NEAREST);

                    _fallbackShadowMapTexture = new osg::Texture2D(image.get());
                    _fallbackShadowMapTexture->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::REPEAT);
                    _fallbackShadowMapTexture->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::REPEAT);
                    _fallbackShadowMapTexture->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::NEAREST);
                    _fallbackShadowMapTexture->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::NEAREST);

                }
            }



    osg::StateSet* getVDDState(){

        //std::cout<<"StateNameOld: "<<_vddstate->getName()<< std::endl;

        return _vddstate;
    }

    osg::StateSet* selectStateSetForRenderingShadow(ViewDependentData& vdd) const
    {

        OSG_INFO<<"   selectStateSetForRenderingShadow() "<<vdd.getStateSet()<<std::endl;

        osg::ref_ptr<osg::StateSet> stateset = vdd.getStateSet();

        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_accessUnfiromsAndProgramMutex);

        vdd.getStateSet()->clear();

        stateset->setName("ShadowTextureState");

        vdd.getStateSet()->setTextureAttributeAndModes(0, _fallbackBaseTexture.get(), osg::StateAttribute::ON);

        for(Uniforms::const_iterator itr=_uniforms.begin();
            itr!=_uniforms.end();
            ++itr)
        {
            OSG_INFO<<"addUniform("<<(*itr)->getName()<<")"<<std::endl;
            stateset->addUniform(itr->get());
        }

        if (_program.valid())
        {
            stateset->setAttribute(_program.get());
        }

        LightDataList& pll = vdd.getLightDataList();
        for(LightDataList::iterator itr = pll.begin();
            itr != pll.end();
            ++itr)
        {
            // 3. create per light/per shadow map division of lightspace/frustum
            //    create a list of light/shadow map data structures

            LightData& pl = (**itr);

            // if no texture units have been activated for this light then no shadow state required.
            if (pl.textureUnits.empty()) continue;

            for(LightData::ActiveTextureUnits::iterator atu_itr = pl.textureUnits.begin();
                atu_itr != pl.textureUnits.end();
                ++atu_itr)
            {
                OSG_INFO<<"   Need to assign state for "<<*atu_itr<<std::endl;
            }

        }

        const ShadowSettings* settings = getShadowedScene()->getShadowSettings();
        unsigned int shadowMapModeValue = settings->getUseOverrideForShadowMapTexture() ?
                                                osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE :
                                                osg::StateAttribute::ON;


        ShadowDataList& sdl = vdd.getShadowDataList();
        for(ShadowDataList::iterator itr = sdl.begin();
            itr != sdl.end();
            ++itr)
        {
            // 3. create per light/per shadow map division of lightspace/frustum
            //    create a list of light/shadow map data structures

            ShadowData& sd = (**itr);

            OSG_INFO<<"   ShadowData for "<<sd._textureUnit<<std::endl;

            stateset->setTextureAttributeAndModes(sd._textureUnit, sd._texture.get(), shadowMapModeValue);

            // comment this and you won't have shadows. But neither the cave shadow bug.
            //#ifndef IMOD_CAVE
                stateset->setTextureMode(sd._textureUnit,GL_TEXTURE_GEN_S,osg::StateAttribute::ON);
                stateset->setTextureMode(sd._textureUnit,GL_TEXTURE_GEN_T,osg::StateAttribute::ON);
                stateset->setTextureMode(sd._textureUnit,GL_TEXTURE_GEN_R,osg::StateAttribute::ON);
                stateset->setTextureMode(sd._textureUnit,GL_TEXTURE_GEN_Q,osg::StateAttribute::ON);
                //std::cout<<"_textureUnit: "<<sd._textureUnit<<std::endl;

            //#endif

//                stateset->setRenderingHint(0);


//                //sd._camera->setRenderOrder(osg::Camera::POST_RENDER);
//                //sd._camera->setRenderOrder(osg::Camera::NESTED_RENDER);
//                sd._camera->setPostDrawCallback(new MyCameraPostDrawCallback(vdd, sd));
        }

        //const_cast<ownViewDependentShadowMap*>(this)->_vddstate = stateset.get();

        //std::cout<<"StateNameOld: "<<_vddstate->getName()<< std::endl;

        return vdd.getStateSet();
    }

    void cull(osgUtil::CullVisitor& cv)
    {
        OSG_INFO<<std::endl<<std::endl<<"ViewDependentShadowMap::cull(osg::CullVisitor&"<<&cv<<")"<<std::endl;

        if (!_shadowCastingStateSet)
        {
            OSG_INFO<<"Warning, init() has not yet been called so ShadowCastingStateSet has not been setup yet, unable to create shadows."<<std::endl;
            _shadowedScene->osg::Group::traverse(cv);
            return;
        }

        ViewDependentData* vdd = getViewDependentData(&cv);

        if (!vdd)
        {
            OSG_INFO<<"Warning, now ViewDependentData created, unable to create shadows."<<std::endl;
            _shadowedScene->osg::Group::traverse(cv);
            return;
        }

        ShadowSettings* settings = getShadowedScene()->getShadowSettings();

        OSG_INFO<<"cv->getProjectionMatrix()="<<*cv.getProjectionMatrix()<<std::endl;

        osg::CullSettings::ComputeNearFarMode cachedNearFarMode = cv.getComputeNearFarMode();

        osg::RefMatrix& viewProjectionMatrix = *cv.getProjectionMatrix();

        // check whether this main views projection is perspective or orthographic
        bool orthographicViewFrustum = viewProjectionMatrix(0,3)==0.0 &&
                                       viewProjectionMatrix(1,3)==0.0 &&
                                       viewProjectionMatrix(2,3)==0.0;

        double minZNear = 0.0;
        double maxZFar = DBL_MAX;

        if (cachedNearFarMode==osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR)
        {
            double left, right, top, bottom;
            if (orthographicViewFrustum)
            {
                viewProjectionMatrix.getOrtho(left, right, bottom, top, minZNear, maxZFar);
            }
            else
            {
                viewProjectionMatrix.getFrustum(left, right, bottom, top, minZNear, maxZFar);
            }
            OSG_INFO<<"minZNear="<<minZNear<<", maxZFar="<<maxZFar<<std::endl;
        }

        // set the compute near/far mode to the highest quality setting to ensure we push the near plan out as far as possible
        if (settings->getComputeNearFarModeOverride()!=osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR)
        {
            cv.setComputeNearFarMode(settings->getComputeNearFarModeOverride());
        }

        // 1. Traverse main scene graph
        cv.pushStateSet( _shadowRecievingPlaceholderStateSet.get() );

        osg::ref_ptr<osgUtil::StateGraph> decoratorStateGraph = cv.getCurrentStateGraph();

        cullShadowReceivingScene(&cv);

        cv.popStateSet();

        if (cv.getComputeNearFarMode()!=osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR)
        {
            OSG_INFO<<"Just done main subgraph traversak"<<std::endl;
            // make sure that the near plane is computed correctly so that any projection matrix computations
            // are all done correctly.
            cv.computeNearPlane();
        }

        // clamp the minZNear and maxZFar to those provided by ShadowSettings
        maxZFar = osg::minimum(settings->getMaximumShadowMapDistance(),maxZFar);
        if (minZNear>maxZFar) minZNear = maxZFar*settings->getMinimumShadowMapNearFarRatio();

        //OSG_NOTICE<<"maxZFar "<<maxZFar<<std::endl;

        Frustum frustum(&cv, minZNear, maxZFar);

        // return compute near far mode back to it's original settings
        cv.setComputeNearFarMode(cachedNearFarMode);

        OSG_INFO<<"frustum.eye="<<frustum.eye<<", frustum.centerNearPlane, "<<frustum.centerNearPlane<<" distance = "<<(frustum.eye-frustum.centerNearPlane).length()<<std::endl;


        // 2. select active light sources
        //    create a list of light sources + their matrices to place them
        selectActiveLights(&cv, vdd);

        unsigned int pos_x = 0;
        unsigned int textureUnit = settings->getBaseShadowTextureUnit();
        unsigned int numValidShadows = 0;

        ShadowDataList& sdl = vdd->getShadowDataList();
        ShadowDataList previous_sdl;
        previous_sdl.swap(sdl);

        unsigned int numShadowMapsPerLight = settings->getNumShadowMapsPerLight();
        if (numShadowMapsPerLight>2)
        {
            OSG_NOTICE<<"numShadowMapsPerLight of "<<numShadowMapsPerLight<<" is greater than maximum supported, falling back to 2."<<std::endl;
            numShadowMapsPerLight = 2;
        }

        LightDataList& pll = vdd->getLightDataList();
        for(LightDataList::iterator itr = pll.begin();
            itr != pll.end();
            ++itr)
        {
            // 3. create per light/per shadow map division of lightspace/frustum
            //    create a list of light/shadow map data structures

            LightData& pl = **itr;

            // 3.1 compute light space polytope
            //
            osg::Polytope polytope = computeLightViewFrustumPolytope(frustum, pl);

            // if polytope is empty then no rendering.
            if (polytope.empty())
            {
                OSG_NOTICE<<"Polytope empty no shadow to render"<<std::endl;
                continue;
            }

            // 3.2 compute RTT camera view+projection matrix settings
            //
            osg::Matrixd projectionMatrix;
            osg::Matrixd viewMatrix;
            if (!computeShadowCameraSettings(frustum, pl, projectionMatrix, viewMatrix))
            {
                OSG_NOTICE<<"No valid Camera settings, no shadow to render"<<std::endl;
                continue;
            }

            // if we are using multiple shadow maps and CastShadowTraversalMask is being used
            // traverse the scene to compute the extents of the objects
            if (/*numShadowMapsPerLight>1 &&*/ _shadowedScene->getCastsShadowTraversalMask()!=0xffffffff)
            {
                // osg::ElapsedTime timer;

                osg::ref_ptr<osg::Viewport> viewport = new osg::Viewport(0,0,2048,2048);
                ComputeLightSpaceBounds clsb(viewport.get(), projectionMatrix, viewMatrix);
                clsb.setTraversalMask(_shadowedScene->getCastsShadowTraversalMask());

                osg::Matrixd invertModelView;
                invertModelView.invert(viewMatrix);
                osg::Polytope local_polytope(polytope);
                local_polytope.transformProvidingInverse(invertModelView);

                osg::CullingSet& cs = clsb.getProjectionCullingStack().back();
                cs.setFrustum(local_polytope);
                clsb.pushCullingSet();

                _shadowedScene->accept(clsb);

                // OSG_NOTICE<<"Extents of LightSpace "<<clsb._bb.xMin()<<", "<<clsb._bb.xMax()<<", "<<clsb._bb.yMin()<<", "<<clsb._bb.yMax()<<", "<<clsb._bb.zMin()<<", "<<clsb._bb.zMax()<<std::endl;
                // OSG_NOTICE<<"  time "<<timer.elapsedTime_m()<<"ms, mask = "<<std::hex<<_shadowedScene->getCastsShadowTraversalMask()<<std::endl;

                if (clsb._bb.xMin()>-1.0f || clsb._bb.xMax()<1.0f || clsb._bb.yMin()>-1.0f || clsb._bb.yMax()<1.0f)
                {
                    // OSG_NOTICE<<"Need to clamp projection matrix"<<std::endl;

    #if 1
                    double xMid = (clsb._bb.xMin()+clsb._bb.xMax())*0.5f;
                    double xRange = clsb._bb.xMax()-clsb._bb.xMin();
    #else
                    double xMid = 0.0;
                    double xRange = 2.0;
    #endif
                    double yMid = (clsb._bb.yMin()+clsb._bb.yMax())*0.5f;
                    double yRange = (clsb._bb.yMax()-clsb._bb.yMin());

                    // OSG_NOTICE<<"  xMid="<<xMid<<", yMid="<<yMid<<", xRange="<<xRange<<", yRange="<<yRange<<std::endl;

                    projectionMatrix =
                        projectionMatrix *
                        osg::Matrixd::translate(osg::Vec3d(-xMid,-yMid,0.0)) *
                        osg::Matrixd::scale(osg::Vec3d(2.0/xRange, 2.0/yRange,1.0));

                }

            }

            double splitPoint = 0.0;

            if (numShadowMapsPerLight>1)
            {
                osg::Vec3d eye_v = frustum.eye * viewMatrix;
                osg::Vec3d center_v = frustum.center * viewMatrix;
                osg::Vec3d viewdir_v = center_v-eye_v; viewdir_v.normalize();
                osg::Vec3d lightdir(0.0,0.0,-1.0);

                double dotProduct_v = lightdir * viewdir_v;
                double angle = acosf(dotProduct_v);

                osg::Vec3d eye_ls = eye_v * projectionMatrix;

                OSG_INFO<<"Angle between view vector and eye "<<osg::RadiansToDegrees(angle)<<std::endl;
                OSG_INFO<<"eye_ls="<<eye_ls<<std::endl;

                if (eye_ls.y()>=-1.0 && eye_ls.y()<=1.0)
                {
                    OSG_INFO<<"Eye point inside light space clip region   "<<std::endl;
                    splitPoint = 0.0;
                }
                else
                {
                    double n = -1.0-eye_ls.y();
                    double f = 1.0-eye_ls.y();
                    double sqrt_nf = sqrt(n*f);
                    double mid = eye_ls.y()+sqrt_nf;
                    double ratioOfMidToUseForSplit = 0.8;
                    splitPoint = mid * ratioOfMidToUseForSplit;

                    OSG_INFO<<"  n="<<n<<", f="<<f<<", sqrt_nf="<<sqrt_nf<<" mid="<<mid<<std::endl;
                }
            }

            // 4. For each light/shadow map
            for (unsigned int sm_i=0; sm_i<numShadowMapsPerLight; ++sm_i)
            {
                osg::ref_ptr<ShadowData> sd;

                if (previous_sdl.empty())
                {
                    OSG_INFO<<"Create new ShadowData"<<std::endl;
                    sd = new ShadowData(vdd);
                }
                else
                {
                    OSG_INFO<<"Taking ShadowData from from of previous_sdl"<<std::endl;
                    sd = previous_sdl.front();
                    previous_sdl.erase(previous_sdl.begin());
                }

                osg::ref_ptr<osg::Camera> camera = sd->_camera;

                camera->setProjectionMatrix(projectionMatrix);
                camera->setViewMatrix(viewMatrix);

                if (settings->getDebugDraw())
                {
                    camera->getViewport()->x() = pos_x;
                    pos_x += camera->getViewport()->width() + 40;
                }

                // transform polytope in model coords into light spaces eye coords.
                osg::Matrixd invertModelView;
                invertModelView.invert(camera->getViewMatrix());

                osg::Polytope local_polytope(polytope);
                local_polytope.transformProvidingInverse(invertModelView);


                if (numShadowMapsPerLight>1)
                {
                    // compute the start and end range in non-dimensional coords
    #if 0
                    double r_start = (sm_i==0) ? -1.0 : (double(sm_i)/double(numShadowMapsPerLight)*2.0-1.0);
                    double r_end = (sm_i+1==numShadowMapsPerLight) ? 1.0 : (double(sm_i+1)/double(numShadowMapsPerLight)*2.0-1.0);
    #endif

                    // hardwired for 2 splits
                    double r_start = (sm_i==0) ? -1.0 : splitPoint;
                    double r_end = (sm_i+1==numShadowMapsPerLight) ? 1.0 : splitPoint;

                    // for all by the last shadowmap shift the r_end so that it overlaps slightly with the next shadowmap
                    // to prevent a seam showing through between the shadowmaps
                    if (sm_i+1<numShadowMapsPerLight) r_end+=0.01;


                    if (sm_i>0)
                    {
                        // not the first shadowmap so insert a polytope to clip the scene from before r_start

                        // plane in clip space coords
                        osg::Plane plane(0.0,1.0,0.0,-r_start);

                        // transform into eye coords
                        plane.transformProvidingInverse(projectionMatrix);
                        local_polytope.getPlaneList().push_back(plane);

                        //OSG_NOTICE<<"Adding r_start plane "<<plane<<std::endl;

                    }

                    if (sm_i+1<numShadowMapsPerLight)
                    {
                        // not the last shadowmap so insert a polytope to clip the scene from beyond r_end

                        // plane in clip space coords
                        osg::Plane plane(0.0,-1.0,0.0,r_end);

                        // transform into eye coords
                        plane.transformProvidingInverse(projectionMatrix);
                        local_polytope.getPlaneList().push_back(plane);

                        //OSG_NOTICE<<"Adding r_end plane "<<plane<<std::endl;
                    }

                    local_polytope.setupMask();


                    // OSG_NOTICE<<"Need to adjust RTT camera projection and view matrix here, r_start="<<r_start<<", r_end="<<r_end<<std::endl;
                    // OSG_NOTICE<<"  textureUnit = "<<textureUnit<<std::endl;

                    double mid_r = (r_start+r_end)*0.5;
                    double range_r = (r_end-r_start);

                    // OSG_NOTICE<<"  mid_r = "<<mid_r<<", range_r = "<<range_r<<std::endl;

                    camera->setProjectionMatrix(
                        camera->getProjectionMatrix() *
                        osg::Matrixd::translate(osg::Vec3d(0.0,-mid_r,0.0)) *
                        osg::Matrixd::scale(osg::Vec3d(1.0,2.0/range_r,1.0)));

                }


                osg::ref_ptr<VDSMCameraCullCallback> vdsmCallback = new VDSMCameraCullCallback(this, local_polytope);
                camera->setCullCallback(vdsmCallback.get());

                // 4.3 traverse RTT camera
                //

                cv.pushStateSet(_shadowCastingStateSet.get());

                cullShadowCastingScene(&cv, camera.get());

                cv.popStateSet();

                if (!orthographicViewFrustum && settings->getShadowMapProjectionHint()==ShadowSettings::PERSPECTIVE_SHADOW_MAP)
                {
                    adjustPerspectiveShadowMapCameraSettings(vdsmCallback->getRenderStage(), frustum, pl, camera.get());
                    if (vdsmCallback->getProjectionMatrix())
                    {
                        vdsmCallback->getProjectionMatrix()->set(camera->getProjectionMatrix());
                    }
                }

                // 4.4 compute main scene graph TexGen + uniform settings + setup state
                //
                assignTexGenSettings(&cv, camera.get(), textureUnit, sd->_texgen.get());

                // mark the light as one that has active shadows and requires shaders
                pl.textureUnits.push_back(textureUnit);

                // pass on shadow data to ShadowDataList
                sd->_textureUnit = textureUnit;

                if (textureUnit >= 8)
                {
                    OSG_NOTICE<<"Shadow texture unit is invalid for texgen, will not be used."<<std::endl;
                }
                else
                {
                    sdl.push_back(sd);
                }

                // increment counters.
                ++textureUnit;
                ++numValidShadows ;
            }

        }

        if (numValidShadows>0)
        {
            this->_vddstate = selectStateSetForRenderingShadow(*vdd);
            decoratorStateGraph->setStateSet(_vddstate);
        }


        // OSG_NOTICE<<"End of shadow setup Projection matrix "<<*cv.getProjectionMatrix()<<std::endl;
    }

};
