/*
 * MatlabController.h
 *
 *  Created on: Sep 11, 2012
 *      Author: ddvogt
 */

#ifndef MATLABCONTROLLER_H_
#define MATLABCONTROLLER_H_
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <Eigen/Core>

#include "MatlabInstance.h"

namespace iMod{

  typedef Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic> MatrixType;
  typedef Eigen::Matrix<double,Eigen::Dynamic,1> VectorType; //Column Vector

  typedef Eigen::Map<MatrixType> MatrixMap;
  typedef Eigen::Map<VectorType> VectorMap;

  //typedef Eigen::Map<const Eigen::Matrix> MapTypeConst;

  typedef  std::map<std::string,std::string> Parameters_t;

  class MatlabParameters{
      Parameters_t _parameters;

    public:
      MatlabParameters(){}
      virtual ~MatlabParameters(){}

      void add(std::string name, std::string value){
        _parameters.insert(std::pair<std::string,std::string>(name,value));
      }

      Parameters_t get(){
        return _parameters;
      }

      int getLength(){
        return _parameters.size();
      }
  };

  typedef std::shared_ptr<MatlabParameters> MatlabParametersPtr;

  class MatlabController : public MatlabInstance{

    public:
      /*!
   * Constructor
   * @param mFilePath The path to working directory, e.g. where the learning algorithms are saved
   * */
      MatlabController(std::string mFilePath);
      virtual ~MatlabController();

      /*!
   * Updates the reconstruction vector for B depending on the content of the pose container
   * The prediction is made in joint angle space
   */
      //	virtual bool predict();

      /*!
   * Get a given Matlab matrix variable
   * @param variable A string containing the Matlab variable name
   * */
      MatrixMap getMatrix(std::string variable);


      /*!
   * Set a matrix variable in matlab
   * Each vector in @param A is a row in Matlab
   * */
      //void setMatrix(Poses_t *A, std::string variable);
      void setMatrix(std::string paramName, double* values, int rows, int cols);

      /*!
   * Set a Array of struct Parameter in matlab
   * */
      void setParameterList(std::string paramName, MatlabParametersPtr parameters);

      /*!
   * Get a matlab vector
   * @param variable The name of the Matlab variable
   * The vector is always treated as a column vector!
   * */
      VectorMap getVector(std::string variable);


      /*!
   * Set a collumn vector in Matlab
   * @param variable The matlab variable to be set
   * @param data The actual data that will be put in the vector
   */
      //void setVector(std::string variable, std::vector<double> &data);

      /*!
   * Evaluate a given Matlab string
   */
      void evaluate(std::string message);

    protected:
      /*!
   * Creates the actual Matlab string message that will be send
   * @param latestOnly create the string only with the last added pose
   * @return Returns the formated string message
   */
      //std::string createMatlabDataStringFromPoseContainer(bool latestOnly=true);

      /*!
   * Creates an Matlab string in the form of A = [ ... ]
   * @param matlabVariable The name of the matlab variable which will be used in the string message
   * @param array The data in the message
   * @param length The length of @param array
   * */
      //TODO: This function should set the parameter instead of return the Matlab string
      //std::string createMatlabStringFromDouble(std::string matlabVariable, double* array, int length);
  };
}

#endif /* MATLABCONTROLLER_H_ */
