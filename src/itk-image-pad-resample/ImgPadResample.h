
#ifndef _ImgPadResample_
#define _ImgPadResample_

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <emscripten.h>
#include <bind.h>
#include <val.h>

#include <itkImage.h>
#include <itkVectorImage.h>
#include <itkIdentityTransform.h>
#include <itkInterpolateImageFunction.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkResampleImageFilter.h>
#include <itkImageRegionIteratorWithIndex.h>


using namespace std;
using namespace emscripten;

class ImgPadResample {
public:

  ImgPadResample();
  ~ImgPadResample();

  template<typename ImageType>
  typename ImageType::Pointer GetITKImage(val const & image);

  void SetImage(val const & image){
    m_Image = make_shared<val>(image);
  }

  val GetImage(){
    return *(m_Image.get()); 
  }

  template<typename ImageType>
  void SetOutputBuffer(typename ImageType::Pointer image);

  val GetOutputBuffer(){
    return *(m_OutputBuffer.get()); 
  }

  void SetOutputSize(val const & size){
    m_OutputSize.clear();
    for(unsigned i = 0; i < size["length"].as<unsigned>(); ++i) {
      m_OutputSize.push_back(size[i].as<int>());
    }
  }

  vector<int> GetOutputSize(){
    return m_OutputSize;
  }

  void SetOutputSpacing(val const & spacing){
    m_OutputSpacing.clear();
    for(unsigned i = 0; i < spacing["length"].as<unsigned>(); ++i) {
      m_OutputSpacing.push_back(spacing[i].as<double>());
    }
  }

  vector<double> GetOutputSpacing(){
    return m_OutputSpacing;
  }

  void SetOutputPad(val const & pad){
    m_OutputPad.clear();
    for(unsigned i = 0; i < pad["length"].as<unsigned>(); ++i) {
      m_OutputPad.push_back(pad[i].as<int>());
    }
  }

  vector<int> GetOutputPad(){
    return m_OutputPad;
  }

  int GetInterpolationType(){
    return m_InterpolationType;
  }

  void SetInterpolationTypeToNearest(){
    m_InterpolationType = 0;
  }

  void SetInterpolationTypeToLinear(){
    m_InterpolationType = 1;
  }

  template<typename ImageType>
  typename ImageType::Pointer ResampleImage(typename ImageType::Pointer image, typename ImageType::PixelType zero);

  void Update();
  template<typename PixelType>
  void UpdateTyped();


private:
  shared_ptr<val> m_Image;
  shared_ptr<val> m_OutputBuffer;
  
  vector<int> m_OutputSize;
  vector<double> m_OutputSpacing;
  vector<int> m_OutputPad;
  int m_InterpolationType; 
};

// Binding code
EMSCRIPTEN_BINDINGS(itk_image_j_s) {

  class_<ImgPadResample>("ImgPadResample")
    .constructor<>()
    .function("SetImage", &ImgPadResample::SetImage)
    .function("SetInterpolationTypeToNearest", &ImgPadResample::SetInterpolationTypeToNearest)
    .function("SetInterpolationTypeToLinear", &ImgPadResample::SetInterpolationTypeToLinear)
    .function("SetOutputSize", &ImgPadResample::SetOutputSize)
    .function("SetOutputSpacing", &ImgPadResample::SetOutputSpacing)
    .function("SetOutputPad", &ImgPadResample::SetOutputPad)
    .function("GetOutputBuffer", &ImgPadResample::GetOutputBuffer)
    .function("Update", &ImgPadResample::Update)
    ;
    
}

#endif