//   MyInteractionMesh.cpp   BoneAnimation    Created by Christian on
//27.11.13.

#include "MyInteractionMeshModel.h"

using namespace MyInteractionMesh;


MyInteractionMeshModel::~MyInteractionMeshModel() {
    VerticeVector::iterator i;
    for (i = _verticesVH1.begin(); i != _verticesVH1.end(); ++i) {
        delete i.base()->first;
    }
    for (i = _verticesVH2.begin(); i != _verticesVH2.end(); ++i) {
        delete i.base()->first;
    }
    _verticesVH1.clear();
    _verticesVH2.clear();
}

Vertice* MyInteractionMeshModel::addVertice(osgAnimation::Bone* fromBone, MyInteractionMeshModelAddOption toVH) {
    Vertice* returnVertice = NULL;
    
    Vertice* vertFromVector = searchBoneInVector(fromBone, toVH);
    
    if (vertFromVector == NULL) {
        Vertice* vertFromBone = new Vertice(fromBone->getWorldMatrices().back().getTrans());
        returnVertice = vertFromBone;
        switch (toVH) {
            case toVH1:
                _verticesVH1.push_back(std::make_pair(returnVertice, fromBone));
                break;
            case toVH2:
                _verticesVH2.push_back(std::make_pair(returnVertice, fromBone));
                break;
            case toOther:
                _verticesOther.push_back(std::make_pair(returnVertice, fromBone));
                break;
            default:
                break;
        }
    } else {
        returnVertice = vertFromVector;
    }
    
    return returnVertice;
}

Vertice* MyInteractionMeshModel::searchBoneInVector(osgAnimation::Bone* target, MyInteractionMeshModelAddOption inVH) {
    VerticeVector::iterator i;
    switch (inVH) {
        case toVH1:
            for(i = _verticesVH1.begin(); i != _verticesVH1.end(); ++i) {
                if (i.base()->second == target) {
                    return i.base()->first;
                }
            }
            return NULL;
            break;
        case toVH2:
            for(i = _verticesVH2.begin(); i != _verticesVH2.end(); ++i) {
                if (i.base()->second == target) {
                    return i.base()->first;
                }
            }
            return NULL;
            break;
        case toOther:
            for(i = _verticesOther.begin(); i != _verticesOther.end(); ++i) {
                if (i.base()->second == target) {
                    return i.base()->first;
                }
            }
            return NULL;
            break;
        default:
            return NULL;
            break;
    }
}

Vertice* MyInteractionMeshModel::getVertice(int n, MyInteractionMeshModelAddOption whichVector) {
    Vertice* returnVertice = NULL;
    switch (whichVector) {
        case toVH1:
            try {
                returnVertice = _verticesVH1.at(n).first;
            } catch (std::out_of_range e) {
                std::cerr << __func__<<" "<<__LINE__<<": "<<" Trying to get node from vector. Out of range Exception"<<std::endl;
            }
            break;
        case toVH2:
            try {
                returnVertice = _verticesVH2.at(n).first;
            } catch (std::out_of_range e) {
                std::cerr << __func__<<" "<<__LINE__<<": "<<" Trying to get node from vector. Out of range Exception"<<std::endl;
            }
            break;
        case toOther:
            try {
                returnVertice = _verticesOther.at(n).first;
            } catch (std::out_of_range e) {
                std::cerr << __func__<<" "<<__LINE__<<": "<<" Trying to get node from vector. Out of range Exception"<<std::endl;
            }
            break;
        default:
            return NULL;
            break;
    }
    return returnVertice;
}

ShapeVector* MyInteractionMeshModel::getConnectionVector() {
    return _connectionVector;
}

int MyInteractionMeshModel::getConnectionVectorSize() {
    return _connectionVector->size();
}

int MyInteractionMeshModel::getVerticeVectorSize(MyInteractionMeshModelAddOption whichVector) {
    switch (whichVector) {
        case toVH1:
            return _verticesVH1.size();
            break;
        case toVH2:
            return _verticesVH2.size();
        case toOther:
            return _verticesOther.size();
        default:
            return 0;
            break;
    }
}

void MyInteractionMeshModel::update(osg::ref_ptr<osg::Group> rootNode) {
    VerticeVector::iterator i;
    // update vertices for Virtual Human 1
    for (i = _verticesVH1.begin(); i != _verticesVH1.end(); ++i) {
        i.base()->first->setValues(i.base()->second->getWorldMatrices(rootNode).back().getTrans());
    }
    // update vertices for Virtual Human 2
    for (i = _verticesVH2.begin(); i != _verticesVH2.end(); ++i) {
        i.base()->first->setValues(i.base()->second->getWorldMatrices(rootNode).back().getTrans());
    }
    // update vertices for other objects ot virtual humans
    for (i = _verticesOther.begin(); i != _verticesOther.end(); ++i) {
        i.base()->first->setValues(i.base()->second->getWorldMatrices(rootNode).back().getTrans());
    }
}