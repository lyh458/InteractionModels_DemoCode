//
//  MyInteractionMeshController.cpp
//  BoneAnimation
//
//  Created by Christian on 25.11.13.
//
//

#include "MyInteractionMeshController.h"
#include "HelperClassesAndFunctions.h"
#include "../utils/notify.h"

using namespace MyInteractionMesh;

MyInteractionMeshController::MyInteractionMeshController(MyMeshKind kindOfMesh, osg::ref_ptr<osg::Group> root) {
    _kindOfMesh = kindOfMesh;
    _kindOfVerticeVisualisation = Sphere;
    _root = root;
    _model.push_back((MyInteractionMeshModel*) new MyInteractionLineMesh());
    _view.push_back((MyInteractionMeshView*) new MyInteractionLineMeshView(_model[MyMeshKind::LINE]));
    
    _model.push_back((MyInteractionMeshModel*) new MyInteractionTriangleMesh());
    _view.push_back((MyInteractionMeshView*) new MyInteractionTriangleMeshView(_model[MyMeshKind::TRIANGLE]));
    
    _model.push_back((MyInteractionMeshModel*) new MyInteractionTetrahedronMesh());
    _view.push_back((MyInteractionMeshView*) new MyInteractionTetrahedronMeshView(_model[MyMeshKind::TETRAHEDRON]));
    
    _model.push_back((MyInteractionMeshModel*) new MyInteractionLineMesh());
    MyInteractionLineMeshView *lineMeshView = new MyInteractionLineMeshView(_model[MyMeshKind::LINE_1COLOR]);
    lineMeshView->switchLineColoringToSolid(true);
    _view.push_back((MyInteractionMeshView*) lineMeshView);
}
MyInteractionMeshController::~MyInteractionMeshController() {
    for(int i = 0; i < 3; i++) {
        delete _model[i];
        delete _view[i];
    }
    _model.clear();
    _view.clear();
    
//    delete _view;
//    delete _model;
}


void MyInteractionMeshController::addMeshToSceneGraph(osg::ref_ptr<osg::Group> inNode) {
    _view[_kindOfMesh]->compileMeshAndAddToSceneGraph(inNode);
}

void MyInteractionMeshController::addTriangle(BoneAndContext3Tuple bonesAndContext) {
    // store the collected informations into the model
    BoneAndContext bc1 = bonesAndContext.get<0>();
    BoneAndContext bc2 = bonesAndContext.get<1>();
    BoneAndContext bc3 = bonesAndContext.get<2>();
    ((MyInteractionTriangleMesh*) _model[MyMeshKind::TRIANGLE])->addVerticesAndMakeTriangle(bc1, bc2, bc3);
}

void MyInteractionMeshController::addLine(BoneAndContext2Tuple bonesAndContext) {
    BoneAndContext bc1 = bonesAndContext.get<0>();
    BoneAndContext bc2 = bonesAndContext.get<1>();
    ((MyInteractionLineMesh*) _model[MyMeshKind::LINE])->addVerticesAndMakeLine(bc1, bc2);
    ((MyInteractionLineMesh*) _model[MyMeshKind::LINE_1COLOR])->addVerticesAndMakeLine(bc1, bc2);

}

void MyInteractionMeshController::addTetrahedron(BoneAndContext4Tuple bonesAndContext) {
    BoneAndContext bc1 = bonesAndContext.get<0>();
    BoneAndContext bc2 = bonesAndContext.get<1>();
    BoneAndContext bc3 = bonesAndContext.get<2>();
    BoneAndContext bc4 = bonesAndContext.get<3>();
    ((MyInteractionTetrahedronMesh*) _model[MyMeshKind::TETRAHEDRON])->addVerticesAndMakeTetrahedron(bc1, bc2, bc3, bc4);
}


void MyInteractionMeshController::addTriangleWithoutContext(osgAnimation::Bone* b1, osgAnimation::Bone* b2, osgAnimation::Bone* b3) {
    // just uses fake context information (all vertices are stored in one vector)
    BoneAndContext bc1 = std::make_pair(b1, MyInteractionMeshModelAddOption::toVH1);
    BoneAndContext bc2 = std::make_pair(b2, MyInteractionMeshModelAddOption::toVH1);
    BoneAndContext bc3 = std::make_pair(b3, MyInteractionMeshModelAddOption::toVH1);

    ((MyInteractionTriangleMesh*) _model[MyMeshKind::TRIANGLE])->addVerticesAndMakeTriangle(bc1, bc2, bc3);
}

void MyInteractionMeshController::addLineWithoutContext(osgAnimation::Bone* b1, osgAnimation::Bone* b2) {
    // just uses fake context information (all vertices are stored in one vector)
    BoneAndContext bc1 = std::make_pair(b1, MyInteractionMeshModelAddOption::toVH1);
    BoneAndContext bc2 = std::make_pair(b2, MyInteractionMeshModelAddOption::toVH1);
    
    ((MyInteractionLineMesh*) _model[MyMeshKind::LINE])->addVerticesAndMakeLine(bc1, bc2);
}
void MyInteractionMeshController::addTetrahedronWithoutContext(osgAnimation::Bone* b1, osgAnimation::Bone* b2,osgAnimation::Bone* b3,osgAnimation::Bone* b4) {
    // just uses fake context information (all vertices are stored in one vector)
    BoneAndContext bc1 = std::make_pair(b1, MyInteractionMeshModelAddOption::toVH1);
    BoneAndContext bc2 = std::make_pair(b2, MyInteractionMeshModelAddOption::toVH1);
    BoneAndContext bc3 = std::make_pair(b3, MyInteractionMeshModelAddOption::toVH1);
    BoneAndContext bc4 = std::make_pair(b4, MyInteractionMeshModelAddOption::toVH1);

    ((MyInteractionTetrahedronMesh*) _model[MyMeshKind::TETRAHEDRON])->addVerticesAndMakeTetrahedron(bc1, bc2, bc3, bc4);
}

void MyInteractionMeshController::updateMesh() {
    if(_kindOfMesh != OFF) {
        _model[_kindOfMesh]->update(_root);
        _view[_kindOfMesh]->update();
        _view[_kindOfMesh]->visualizeVectors(_kindOfVerticeVisualisation);
    }
}

//void MyInteractionMeshController::readConnectionVectorFromFile(std::string path, osg::Node* vh1Node, osg::Node* vh2Node, std::vector<osg::Node*> otherNodes, std::vector<std::string> otherBoneNames) {
void MyInteractionMeshController::readConnectionVectorFromFile(std::string path, std::vector<osg::Node*> nodeVector, std::vector<std::vector<std::string>> boneNameVector) {
    // nodeVector.size should be equal boneNameVector.size
    // first two elements in each vector should be for VH1, VH2
    // other will be treated as nodes from other objects
    // its important that given number of boneNameVector should be compatible with the connectionVector-file (from Matlab)
    
    std::vector<int> sizesOfBoneVectors;
    std::vector<std::string> boneNameVectorConcat;
    int numberElements = 0;
    
    for (auto i=0; i<boneNameVector.size(); i++) {
        int n = boneNameVector[i].size();
        sizesOfBoneVectors.push_back(n);
        numberElements += n;
    }
    boneNameVectorConcat.reserve(numberElements);
    for (auto i=0; i<boneNameVector.size(); i++) {
        boneNameVectorConcat.insert( boneNameVectorConcat.end(), boneNameVector[i].begin(), boneNameVector[i].end() );
    }
    
    std::string line;
    std::ifstream myfile (path.c_str());
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            std::vector<std::string> parts = explode(line, ',');
            if (parts.size() == 4) {
                int v[4];
                MyInteractionMeshModelAddOption a[4];
                osgAnimation::Bone* b[4];
                BoneAndContext bc[4];
                
                for (int i=0; i<4; ++i) {
                    v[i] = atoi(parts.at(i).c_str()) - 1;

                    if (v[i] < sizesOfBoneVectors[0] ) { // a bone from Virtual Human 1
                        a[i]=toVH1;
                        b[i]= (osgAnimation::Bone*) findNamedNode(boneNameVectorConcat[v[i]], nodeVector[0]);
                    } else if (v[i] < (sizesOfBoneVectors[0] + sizesOfBoneVectors[1])) { // from VH2
                        a[i]=toVH2;
                        b[i]= (osgAnimation::Bone*) findNamedNode(boneNameVectorConcat[v[i]], nodeVector[1]);
                    } else {
                        a[i]=toOther;
                        int correctNodeIndex = 0;
                        int counter = 0;
                        for (auto n=0; n < sizesOfBoneVectors.size(); n++) {
                            counter += sizesOfBoneVectors[n];
                            if(v[i] < counter ) {
                                correctNodeIndex = n;
                                break;
                            }
                        }
                        
                        b[i]= (osgAnimation::Bone*) findNamedNode(boneNameVectorConcat[v[i]], nodeVector[correctNodeIndex]);
                        if (!b[i])
                        {
                            std::cerr << __func__<<" "<<__LINE__<<": "<<"Node in InteractionMesh reference file which is not known in the parameter vectors"<<std::endl;
                        }
                    }
                    bc[i] = std::make_pair(b[i], a[i]);
                }
                
//                switch (_kindOfMesh) {
//                    case LINE: // 6 edges in a tetrahedron
                        addLine(boost::make_tuple(bc[0], bc[1]));
                        addLine(boost::make_tuple(bc[0], bc[2]));
                        addLine(boost::make_tuple(bc[0], bc[3]));
                        addLine(boost::make_tuple(bc[1], bc[2]));
                        addLine(boost::make_tuple(bc[1], bc[3]));
                        addLine(boost::make_tuple(bc[2], bc[3]));
//                        break;
//                    case TRIANGLE: // 4 faces in a tetrahedron
                        addTriangle(boost::make_tuple(bc[0],bc[1],bc[2]));
                        addTriangle(boost::make_tuple(bc[1],bc[2],bc[3]));
                        addTriangle(boost::make_tuple(bc[2],bc[3],bc[0]));
                        addTriangle(boost::make_tuple(bc[3],bc[0],bc[1]));
//                        break;
//                    case TETRAHEDRON: // 1 body in a tetrahedron
                        addTetrahedron(boost::make_tuple(bc[0],bc[1],bc[2],bc[3]));
//                        break;
//                    default:
//                        break;
//                }
            } else {
                UT_NOTIFY(LV_FATAL,"Error while reading vertex connection file.");
                throw new std::exception;
            }
        }
        myfile.close();
    }
    else {
        UT_NOTIFY(LV_FATAL,"Couldn't open vertex connections file.");
        throw new std::exception;
    }
}

void MyInteractionMeshController::switchMode() {
    switch (_kindOfMesh) {
        case LINE: {
            _view[MyMeshKind::LINE]->removeGeodes();
            _view[MyMeshKind::LINE]->removeVisualisationGeodes();
            _kindOfMesh = MyMeshKind::LINE_1COLOR;
            _view[MyMeshKind::LINE_1COLOR]->compileMeshAndAddToSceneGraph(_root);
            break;
        }
        case LINE_1COLOR: {
            _view[MyMeshKind::LINE_1COLOR]->removeGeodes();
            _view[MyMeshKind::LINE_1COLOR]->removeVisualisationGeodes();
            _kindOfMesh = MyMeshKind::TRIANGLE;
            _view[MyMeshKind::TRIANGLE]->compileMeshAndAddToSceneGraph(_root);
            break;
        }
        case TRIANGLE: {
            _view[MyMeshKind::TRIANGLE]->removeGeodes();
            _view[MyMeshKind::TRIANGLE]->removeVisualisationGeodes();
            _kindOfMesh = MyMeshKind::TETRAHEDRON;
            _view[MyMeshKind::TETRAHEDRON]->compileMeshAndAddToSceneGraph(_root);
            break;
        }
        case TETRAHEDRON: {
            _view[MyMeshKind::TETRAHEDRON]->removeGeodes();
            _view[MyMeshKind::TETRAHEDRON]->removeVisualisationGeodes();
            _kindOfMesh = MyMeshKind::OFF;
            break;
        }
        case OFF: {
            _kindOfMesh = MyMeshKind::LINE;
            _view[MyMeshKind::LINE]->compileMeshAndAddToSceneGraph(_root);
            break;
        }
        default:
            break;
    }
}

void MyInteractionMeshController::switchVerticeVisualisationMode() {
    switch (_kindOfVerticeVisualisation) {
        case Off:
            _kindOfVerticeVisualisation = MyVerticeVisualisation::Axis;
            break;
        case Axis:
            _kindOfVerticeVisualisation = MyVerticeVisualisation::Sphere;
            break;
        case Sphere:
            _kindOfVerticeVisualisation = MyVerticeVisualisation::Off;
        default:
            break;
    }
}

