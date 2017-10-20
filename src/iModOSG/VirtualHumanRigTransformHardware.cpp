#include "VirtualHumanRigTransformHardware.h"

/**
 * @brief override ()operator
 * @details will be called for each frame to initialize shaders and to compute new palette matrix
 * 
 * @param geom RigGeometry of active object. Will be called automatically for each frame.
 */
void VirtualHumanRigTransformHardware::operator()(osgAnimation::RigGeometry& geom)
{
    // initialize the shader programs
    if (_needInit && !init(geom))
    {
        return;
    }

    // compute new matrix palette for skinning
    computeMatrixPaletteUniform(geom.getMatrixFromSkeletonToGeometry(), geom.getInvMatrixFromSkeletonToGeometry());
}

/**
 * @brief initializes shader program or set software rendering based on vertice complexity
 * @details this function will create the chosen shaders using the chosen rendering and color modes
 * 
 * @param geom the chosen RigGeometry
 * @return true on success, false on error
 */
bool VirtualHumanRigTransformHardware::init(osgAnimation::RigGeometry& geom)
{
    osg::Geometry& source = *geom.getSourceGeometry();
    osg::Vec3Array* verticesSrc = dynamic_cast<osg::Vec3Array*>(source.getVertexArray());
    if (!verticesSrc)
    {
        OSG_WARN << "VirtualHumanRigTransformHardware: no vertex array in the geometry " << geom.getName() << std::endl;
        return false;
    }

    if (!geom.getSkeleton())
    {
        OSG_WARN << "VirtualHumanRigTransformHardware: no skeleton set in geometry " << geom.getName() << std::endl;
        return false;
    }

    // copy shallow from source geometry to rig
    geom.copyFrom(source);

    // create BoneMap from skeleton
    osgAnimation::BoneMapVisitor mapVisitor;
    geom.getSkeleton()->accept(mapVisitor);
    osgAnimation::BoneMap bm = mapVisitor.getBoneMap();

    // check, if it would be better to use software skinning (gpu-cpu transitions are expensive)
    if (_RenderingMode == HardwareRenderingMode::POSSIBLE)
    {
        auto vert_count = verticesSrc->size();
        if (vert_count < __VHRTH_FORCE_SOFTWARE_VERTICE_COUNT__)
        {           
            geom.setRigTransformImplementation(new osgAnimation::RigTransformSoftware);
            osg::notify(osg::WARN) << "VirtualHumanRigTransformHardware: skipping a geometry of " << vert_count << " vertices.\n";
            return false;
        }
    } else if (_RenderingMode == HardwareRenderingMode::OFF)
    {
        geom.setRigTransformImplementation(new osgAnimation::RigTransformSoftware);
        osg::notify(osg::WARN) << "VirtualHumanRigTransformHardware: set rendering mode of geometry to 'Software'.\n";
        return true; // all worked as planned...
    }
    

    // create Vec4Array of bone transformations
    if (!createPalette(verticesSrc->size(), bm, geom.getVertexInfluenceSet().getVertexToBoneList()))
    {
        std::cout<<"VirtualHumanRigTransformHardware: createPalette() failed.";
        return false;
    }

    int attribIndex = 11; // 11 seems to work with OSG. See http://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/attributes.php
    int nbAttribs = getNumVertexAttrib();

    // create new shader program
    osg::ref_ptr<osg::Program> program = new osg::Program;
    program->setName("HardwareSkinning");

    // check if we need to create a new shader
    {
        std::stringstream vertex_path;
        vertex_path.clear();
        if (!_shader.valid())
        {
            // load shader file
            vertex_path << iMODOSG_DATA_DIR << "/Shaders/vh_skinning.vert"; // iMODOSG_DATA_DIR = cmake defined
            _shader = osg::Shader::readShaderFile(osg::Shader::VERTEX, vertex_path.str());
        }
    
        if (!_shader.valid())
        {
            osg::notify(osg::WARN) << "VirtualHumanRigTransformHardware: can't load VertexShader " << vertex_path.str() << std::endl;
            return false;
        }
    }

    // set matrixPalette size depending on how many boneweights influence a vertex
    {
        std::string str = _shader->getShaderSource();
        std::string toreplace = std::string("MAX_MATRIX");
        std::size_t start = str.find(toreplace);
        std::stringstream ss;
        ss.clear();
        ss << getMatrixPaletteUniform()->getNumElements();
        str.replace(start, toreplace.size(), ss.str());
        _shader->setShaderSource(str);
    }

    for (int i = 0; i < nbAttribs; i++)
    {
        std::stringstream ss;
        ss.clear();
        ss << "boneWeight" << i;
        program->addBindAttribLocation(ss.str(), attribIndex + i);
        geom.setVertexAttribArray(attribIndex + i, getVertexAttrib(i)); // osg::Array::Binding::BIND_PER_VERTEX
        geom.setVertexAttribBinding(attribIndex + i, osg::Geometry::BIND_PER_VERTEX);
        osg::notify(osg::INFO) << "VirtualHumanRigTransformHardware: Bound vertex attribute " << attribIndex + i << " to '" << ss.str() << "'\n";
    }

    program->addShader(_shader.get());

    osg::ref_ptr<osg::StateSet> ss = geom.getOrCreateStateSet();
    ss->addUniform(getMatrixPaletteUniform());
    ss->addUniform(new osg::Uniform("nbBonesPerVertex", getNumBonesPerVertex()));
    ss->setAttributeAndModes(program.get(), osg::StateAttribute::ON);

    // optionally, set some material properties:
    if (_ColorMode == HardwareRenderingColorMode::BRIGHT)
    {
        osg::ref_ptr<osg::Material> mat = new osg::Material;
        // mat->setDiffuse( osg::Material::FRONT, osg::Vec4( .2f, .9f, .9f, 1.f ) );
        mat->setSpecular( osg::Material::FRONT, osg::Vec4( 1.f, 1.f, 1.f, 1.f ) );
        mat->setShininess( osg::Material::FRONT, 1.f );
        ss->setAttribute( mat.get() );
    } else if (_ColorMode == HardwareRenderingColorMode::NINJA)
    {
        osg::ref_ptr<osg::Material> mat = new osg::Material;
        mat->setDiffuse( osg::Material::FRONT, osg::Vec4( .1f, .1f, .1f, 0.1f ) );
        mat->setSpecular( osg::Material::FRONT, osg::Vec4( 0.f, 0.f, 0.f, 0.f ) );
        mat->setShininess( osg::Material::FRONT, 0.2f );
        ss->setAttribute( mat.get() );
    } else if (_ColorMode == HardwareRenderingColorMode::SICK)
    {
        osg::ref_ptr<osg::Material> mat = new osg::Material;
        mat->setDiffuse( osg::Material::FRONT, osg::Vec4( .2f, .9f, .9f, 1.f ) );
        mat->setSpecular( osg::Material::FRONT, osg::Vec4( .5f, 1.f, .5f, 1.f ) );
        mat->setShininess( osg::Material::FRONT, 1.f );
        ss->setAttribute( mat.get() );
    }

    _needInit = false;
    return true;
}

/**
 * @brief create MatrixPalette for each bone
 * @description this will later be passed to the hardware shader and used to determine vertex normals and positions
 */
bool VirtualHumanRigTransformHardware::createPalette(int nbVertexes, osgAnimation::BoneMap boneMap, const osgAnimation::VertexInfluenceSet::VertexIndexToBoneWeightMap& vertexIndexToBoneWeightMap)
{
    typedef std::map<std::string, int> BoneNameCountMap;
    typedef std::map<std::string, int> BoneNamePaletteIndex;
    BoneNamePaletteIndex bname2palette;
    BonePalette palette;
    BoneNameCountMap boneNameCountMap;

    // init vertex attribute data
    VertexIndexWeightList vertexIndexWeight;
    vertexIndexWeight.resize(nbVertexes);

    int maxBonePerVertex = 0;
    for (osgAnimation::VertexInfluenceSet::VertexIndexToBoneWeightMap::const_iterator it = vertexIndexToBoneWeightMap.begin(); it != vertexIndexToBoneWeightMap.end(); ++it)
    {
        int vertexIndex = it->first;
        const osgAnimation::VertexInfluenceSet::BoneWeightList& boneWeightList = it->second;
        int bonesForThisVertex = 0;
        for (osgAnimation::VertexInfluenceSet::BoneWeightList::const_iterator it = boneWeightList.begin(); it != boneWeightList.end(); ++it)
        {
            const osgAnimation::VertexInfluenceSet::BoneWeight& bw = *it;
            if (boneNameCountMap.find(bw.getBoneName()) != boneNameCountMap.end())
            {
                boneNameCountMap[bw.getBoneName()]++;
                bonesForThisVertex++; // count max number of bones per vertexes
                vertexIndexWeight[vertexIndex].push_back(IndexWeightEntry(bname2palette[bw.getBoneName()],bw.getWeight()));
            }
            else if (fabs(bw.getWeight()) > 1e-4) // dont use bone with weight too small
            {
                if (boneMap.find(bw.getBoneName()) == boneMap.end())
                {
                    OSG_INFO << "RigTransformHardware::createPalette can't find bone " << bw.getBoneName() << " skip this influence" << std::endl;
                    continue;
                }
                boneNameCountMap[bw.getBoneName()] = 1; // for stats
                bonesForThisVertex++;
                palette.push_back(boneMap[bw.getBoneName()]);
                bname2palette[bw.getBoneName()] = palette.size()-1;
                vertexIndexWeight[vertexIndex].push_back(IndexWeightEntry(bname2palette[bw.getBoneName()],bw.getWeight()));
            }
            // don't be so verbose
            // else
            // {
            //     OSG_WARN << "RigTransformHardware::createPalette Bone " << bw.getBoneName() << " has a weight " << bw.getWeight() << " for vertex " << vertexIndex << " this bone will not be in the palette" << std::endl;
            // }
        }
        maxBonePerVertex = osg::maximum(maxBonePerVertex, bonesForThisVertex);
    }
    OSG_INFO << "RigTransformHardware::createPalette maximum number of bone per vertex is " << maxBonePerVertex << std::endl;
    OSG_INFO << "RigTransformHardware::createPalette matrix palette has " << boneNameCountMap.size() << " entries" << std::endl;

    for (BoneNameCountMap::iterator it = boneNameCountMap.begin(); it != boneNameCountMap.end(); ++it)
    {
        OSG_INFO << "RigTransformHardware::createPalette Bone " << it->first << " is used " << it->second << " times" << std::endl;
    }

    OSG_INFO << "RigTransformHardware::createPalette will use " << boneNameCountMap.size() * 4 << " uniforms" << std::endl;


    for (int i = 0 ; i < (int)vertexIndexWeight.size(); i++)
        vertexIndexWeight[i].resize(maxBonePerVertex);

    _nbVertexes = nbVertexes;
    _bonesPerVertex = maxBonePerVertex;
    _bonePalette = palette;
    _vertexIndexMatrixWeightList = vertexIndexWeight;
    _uniformMatrixPalette = createVertexUniform();
    _boneWeightAttribArrays = createVertexAttribList();
    return true;
}

