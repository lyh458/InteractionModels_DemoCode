#ifndef BONEDEFINITIONS_H
#define BONEDEFINITIONS_H

#include <vector>
#include <string>

//Consider member variable in VHuman
//Virtual human bones
const std::string translationBoneNames[] = { "HumanoidRoot"};
static const std::vector<std::string> translationBones(translationBoneNames, translationBoneNames + sizeof(translationBoneNames) / sizeof(translationBoneNames[0]));

const std::string _boneNames[] = {"HumanoidRoot", "vt10", "vt6", "vt1", "vc7", "skullbase", "l_sternoclavicular", "l_shoulder",
                    "l_elbow", "l_wrist", "r_sternoclavicular", "r_shoulder", "r_elbow", "r_wrist", "l_hip", "l_knee",
                    "l_ankle", "l_metatarsal", "l_forefoot_tip", "r_hip", "r_knee", "r_ankle", "r_metatarsal",
                    "r_forefoot_tip"};

 // const std::string _boneNames[] = {"sacroiliac", "vt10", "vt6", "skullbase", "l_sternoclavicular", "l_shoulder",
 //  "l_elbow", "l_wrist", "r_sternoclavicular", "r_shoulder", "r_elbow", "r_wrist", "l_hip", "l_knee",
 //  "l_ankle", "l_forefoot_tip", "r_hip", "r_knee", "r_ankle", "r_forefoot_tip"};

static const std::vector<std::string> rotationBones(_boneNames, _boneNames + sizeof(_boneNames) / sizeof(_boneNames[0]));

//Fully Body Motions
const std::string _boneNamesPhys[] = {"sacroiliac", "vt10", "vt6", "skullbase", "head","l_sternoclavicular", "l_shoulder","l_elbow", "hand.L", "hand target.L", "r_sternoclavicular", "r_shoulder", "r_elbow", "hand.R", "hand target.R", "l_hip", "l_knee","l_ankle", "l_forefoot_tip", "r_hip", "r_knee", "r_ankle", "r_forefoot_tip"};
// const std::string _boneNamesPhys[] = {"sacroiliac", "vt10", "vt6", "skullbase", "head", "l_sternoclavicular", "l_shoulder","l_elbow", "finger4-1.L", "r_sternoclavicular", "r_shoulder", "r_elbow", "finger4-1.R", "l_hip", "l_knee","l_ankle", "l_forefoot_tip", "r_hip", "r_knee", "r_ankle", "r_forefoot_tip"};
static const std::vector<std::string> rotationBonesPhys(_boneNamesPhys, _boneNamesPhys + sizeof(_boneNamesPhys) / sizeof(_boneNamesPhys[0]));

//Bone Names without hand and head rotations
const std::string _boneNamesPhysNoHands[] = {"sacroiliac", "vt10", "vt6", "l_sternoclavicular", "l_shoulder","l_elbow", "r_sternoclavicular", "r_shoulder", "r_elbow", "l_hip", "l_knee","l_ankle", "l_forefoot_tip", "r_hip", "r_knee", "r_ankle", "r_forefoot_tip"};
static const std::vector<std::string> rotationBonesPhysNoHands(_boneNamesPhysNoHands, _boneNamesPhysNoHands + sizeof(_boneNamesPhysNoHands) / sizeof(_boneNamesPhysNoHands[0]));


//UR5 Bones
const std::string _boneNamesUR5[] = {"Bone", "Bone.001", "Bone.002","Bone.003", "Bone.004", "Bone.005", "Bone.006", "Bone.007", "Wrist", "Thumb", "IndexFinger", "MiddleFinger"};
static const std::vector<std::string> rotationBonesUR5(_boneNamesUR5, _boneNamesUR5 + sizeof(_boneNamesUR5) / sizeof(_boneNamesUR5[0]));

//Object bones
const std::string translationObjectBoneNames[] = {"HumanoidRoot"};
const std::vector<std::string> translationObjectBones(translationObjectBoneNames, translationObjectBoneNames + sizeof(translationObjectBoneNames) / sizeof(translationObjectBoneNames[0]));

const std::string _objectBoneNames[] = {"HumanoidRoot"};
static const std::vector<std::string> rotationObjectBones(_objectBoneNames, _objectBoneNames + sizeof(_objectBoneNames) / sizeof(_objectBoneNames[0]));

//Nao Bones
const std::string _naoTranslationBoneNames[] = {"Torso"};
std::vector<std::string> naoTranslationBones(_naoTranslationBoneNames, _naoTranslationBoneNames + sizeof(_naoTranslationBoneNames) / sizeof(_naoTranslationBoneNames[0]));

const std::string _naoRotationBoneNames[] = {        //"Torso",
                                                     "neckYaw",
                                                     "neckPitch",
                                                     "r_shoulder",
                                                     "r_upperarm",
                                                     "r_forearm",
                                                     "r_hand",
                                                     "r_forearm.002",
                                                     "l_shoulder",
                                                     "l_upperarm",
                                                     "l_forearm",
                                                     "l_hand",
                                                     "l_forearm.001",
                                                     "r_pelvis",
                                                     "r_hip",
                                                     "r_thigh",
                                                     "r_calf",
                                                     "r_ankle",
                                                     "r_hindfoot",
                                                     "l_pelvis",
                                                     "l_hip",
                                                     "l_thigh",
                                                     "l_calf",
                                                     "l_ankle",
                                                     "l_hindfoot"
                                            };

static const std::vector<std::string> naoRotationBones(_naoRotationBoneNames, _naoRotationBoneNames + sizeof(_naoRotationBoneNames) / sizeof(_naoRotationBoneNames[0]));

// Bone names for interaction mesh
static const std::vector<std::string> boneNamesInteractionMesh = {"sacroiliac", "head", "l_elbow", "r_elbow", "hand.L", "hand.R", "l_forefoot_tip", "r_forefoot_tip"};
static const std::vector<std::string> boneNamesInteractionMeshObject = { "HumanoidRoot" };

#endif // BONEDEFINITIONS_H
