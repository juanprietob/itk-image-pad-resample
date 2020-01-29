
#ifndef _ImgPadResample_
#define _ImgPadResample_

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <emscripten.h>
#include <bind.h>
#include <val.h>

#include <itkImage.h>
#include <itkImageIOBase.h>
#include <itkVectorImage.h>
#include <itkIdentityTransform.h>
#include <itkInterpolateImageFunction.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkResampleImageFilter.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkDataObject.h>


using namespace std;
using namespace emscripten;

struct ImgPadResample {
public:

  using DataObjectType = typename itk::DataObject;

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

  void SetImageDimension(const int dim){
    m_Dimension = dim;
  }

  int GetImageDimension(){
    return m_Dimension;
  }

  void SetPixelType(const itk::ImageIOBase::IOPixelType pixelType){
    m_PixelType = pixelType;
  }

  itk::ImageIOBase::IOPixelType GetPixelType(){
    return m_PixelType;
  }

  void SetComponentType(const itk::ImageIOBase::IOComponentType componentType){
    m_ComponentType = componentType;
  }

  itk::ImageIOBase::IOComponentType GetComponentType(){
    return m_ComponentType;
  }

  val GetOutput(){
    return *(m_OutputImage.get());
  }

  template <typename ImageType>
  void SetOutputITKImage(typename ImageType::Pointer image, string componentType);

  void SetOutput(val const & image){
    m_OutputImage = make_shared<val>(image);
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

  void SetFitSpacingToOutputSizeOn(){
    m_FitSpacingToOutputSize = true;
  }

  void SetFitSpacingToOutputSizeOff(){
    m_FitSpacingToOutputSize = false;
  }

  void SetIsoSpacingOn(){
    m_IsoSpacing = true;
  }

  void SetIsoSpacingOff(){
    m_IsoSpacing = false;
  }

  void SetCenterImageOn(){
    m_CenterImage = true;
  }

  void SetCenterImageOff(){
    m_CenterImage = false;
  }

  template<typename ImageType>
  typename ImageType::Pointer ResampleImage(typename ImageType::Pointer image, typename ImageType::PixelType zero);

  virtual void Update();
  template<typename PixelType>
  void UpdateTyped(string componentType);


private:
  shared_ptr<val> m_Image;
  shared_ptr<val> m_OutputImage;
  itk::ImageIOBase::IOComponentType m_ComponentType;
  itk::ImageIOBase::IOPixelType m_PixelType;
  int m_Dimension;

  vector<int> m_OutputSize;
  vector<double> m_OutputSpacing;
  vector<int> m_OutputPad;
  int m_InterpolationType;
  bool m_FitSpacingToOutputSize;
  bool m_IsoSpacing;
  bool m_CenterImage;
};

// Binding code
EMSCRIPTEN_BINDINGS(itk_img_pad_resample) {

  class_<ImgPadResample>("ImgPadResample")
    .constructor<>()
    .function("SetImage", &ImgPadResample::SetImage)
    .function("SetInterpolationTypeToNearest", &ImgPadResample::SetInterpolationTypeToNearest)
    .function("SetInterpolationTypeToLinear", &ImgPadResample::SetInterpolationTypeToLinear)
    .function("SetOutputSize", &ImgPadResample::SetOutputSize)
    .function("SetOutputSpacing", &ImgPadResample::SetOutputSpacing)
    .function("SetOutputPad", &ImgPadResample::SetOutputPad)
    .function("GetOutput", &ImgPadResample::GetOutput)
    .function("SetFitSpacingToOutputSizeOn", &ImgPadResample::SetFitSpacingToOutputSizeOn)
    .function("SetFitSpacingToOutputSizeOff", &ImgPadResample::SetFitSpacingToOutputSizeOff)
    .function("SetIsoSpacingOn", &ImgPadResample::SetIsoSpacingOn)
    .function("SetIsoSpacingOff", &ImgPadResample::SetIsoSpacingOff)
    .function("SetCenterImageOn", &ImgPadResample::SetCenterImageOn)
    .function("SetCenterImageOff", &ImgPadResample::SetCenterImageOff)
    .function("Update", &ImgPadResample::Update)
    ;
    
}

#endif