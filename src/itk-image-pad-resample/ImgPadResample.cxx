
#include "ImgPadResample.h"

ImgPadResample::ImgPadResample(){
  m_InterpolationType = 0;
  m_OutputImage = 0;
  m_FitSpacingToOutputSize = true;
  m_IsoSpacing = false;
  m_CenterImage = true;
  m_OutputSpacing.clear();
  m_OutputSize.clear();
}

ImgPadResample::~ImgPadResample(){
  
}

template<typename ImageType>
typename ImageType::Pointer ImgPadResample::GetITKImage(val const & image){

  // image = {
  //   imageType: {
  //     dimension: 2,
  //     componentType: 'uint16_t',
  //     pixelType: 1,
  //     components: 1
  //   },
  //   name: 'Image',
  //   origin: [ 0, 0 ],
  //   spacing: [ 0.148489, 0.148489 ],
  //   direction: { rows: 2, columns: 2, data: [ 1, 0, 0, 1 ] },
  //   size: [ 1136, 852 ],
  //   data: Uint16Array []
  // }

  typename ImageType::Pointer itk_image = ImageType::New();

  const int dimension = itk_image->GetImageDimension();

  typename ImageType::SizeType size;
  size.Fill(1);
  
  for(unsigned i = 0; i < dimension; ++i) {
    size[i] = image["size"][i].as<int>();
  }

  typename ImageType::RegionType region;
  region.SetSize(size);

  typename ImageType::PointType origin;

  for(unsigned i = 0; i < dimension; ++i) {
    origin[i] = image["origin"][i].as<double>();
  }
  
  typename ImageType::SpacingType spacing;
  spacing.Fill(1);

  for(unsigned i = 0; i < dimension; ++i) {
    spacing[i] = image["spacing"][i].as<double>();
  }

  typename ImageType::DirectionType direction;

  for(unsigned i = 0; i < dimension*dimension; ++i) {
    direction.GetVnlMatrix().data_block()[i] = image["direction"]["data"][i].as<double>();
  }
  
  unsigned components = image["imageType"]["components"].as<unsigned>();
  itk_image->SetNumberOfComponentsPerPixel(components);
  itk_image->SetRegions(region);
  itk_image->SetOrigin(origin);
  itk_image->SetSpacing(spacing);
  itk_image->SetDirection(direction);
  itk_image->Allocate();

  typename ImageType::InternalPixelType* buffer = itk_image->GetBufferPointer();

  unsigned numberofpixels = image["data"]["length"].as<unsigned>();

  for(unsigned i = 0; i < numberofpixels; ++i) {
    buffer[i] = image["data"][i].as<float>();
  }

  return itk_image;
}

void ImgPadResample::Update(){

  val image = this->GetImage();
  val spacing = image["spacing"].as<val>();
  val size = image["size"].as<val>();

  if(m_OutputSpacing.size() == 0){
    this->SetOutputSpacing(spacing);
  }

  if(m_OutputSize.size() == 0){
    this->SetOutputSize(size);
  }

  if(m_FitSpacingToOutputSize){
    val output_spacing = val::array();
    for(unsigned i = 0; i < spacing["length"].as<unsigned>(); i++){
      double sp = spacing[i].as<double>();
      int si = size[i].as<int>();
      double osp = sp*si/m_OutputSize[i];
      output_spacing.call<void>("push", osp);
    }
    this->SetOutputSpacing(output_spacing);
  }
  if(m_IsoSpacing){
    val output_spacing = val::array();
    double maxsp = *(max_element(begin(m_OutputSpacing), end(m_OutputSpacing)));
    for(unsigned i = 0; i < m_OutputSpacing.size(); i++){
      output_spacing.call<void>("push", maxsp);
    }
    this->SetOutputSpacing(output_spacing);
  }
  if(m_CenterImage){
    
    vector<double> current_length;
    vector<double> target_length;
    for(unsigned i = 0; i < spacing["length"].as<unsigned>(); i++){
      current_length.push_back(spacing[i].as<double>()*size[i].as<int>());
    }
    for(unsigned i = 0; i < m_OutputSpacing.size(); i++){
      target_length.push_back(m_OutputSpacing[i]*m_OutputSize[i]);
    }
    
    val output_pad = val::array();
    for(unsigned i = 0; i < target_length.size(); i++){
      double o_p = (target_length[i] - current_length[i])/2.0;
      o_p = o_p > 0? round(o_p/m_OutputSpacing[i]): 0;
      output_pad.call<void>("push", o_p);
    }
    this->SetOutputPad(output_pad);
  }

  string componentType = image["imageType"]["componentType"].as<string>();

  if(componentType.compare("int8_t") == 0){
    this->UpdateTyped<char>(componentType);
  }else if(componentType.compare("uint8_t") == 0){
    this->UpdateTyped<unsigned char>(componentType);
  }else if(componentType.compare("int16_t") == 0){
    this->UpdateTyped<short>(componentType);
  }else if(componentType.compare("uint16_t") == 0){
    this->UpdateTyped<unsigned short>(componentType);
  }else if(componentType.compare("int32_t") == 0){
    this->UpdateTyped<int>(componentType);
  }else if(componentType.compare("uint32_t") == 0){
    this->UpdateTyped<unsigned int>(componentType);
  }else if(componentType.compare("int64_t") == 0){
    this->UpdateTyped<long>(componentType);
  }else if(componentType.compare("uint64_t") == 0){
    this->UpdateTyped<unsigned long>(componentType);
  }else if(componentType.compare("float") == 0){
    this->UpdateTyped<float>(componentType);
  }else{
    this->UpdateTyped<double>(componentType);
  }
}

template<typename PixelType>
void ImgPadResample::UpdateTyped(string componentType){

  val image = this->GetImage();

  if(image["imageType"]["dimension"].as<unsigned>() == 1){
    typedef itk::VectorImage< PixelType, 1 > ImageType;
    
    typename ImageType::Pointer input_image = this->GetITKImage<ImageType>(image);
    typename ImageType::PixelType zero(input_image->GetNumberOfComponentsPerPixel());
    zero.Fill(0);
    typename ImageType::Pointer output_image = this->ResampleImage<ImageType>(input_image, zero);
    this->SetOutputITKImage<ImageType>(output_image, componentType);

  }else if(image["imageType"]["dimension"].as<unsigned>() == 2){
    typedef itk::VectorImage< PixelType, 2 > ImageType;

    typename ImageType::Pointer input_image = this->GetITKImage<ImageType>(image);
    typename ImageType::PixelType zero(input_image->GetNumberOfComponentsPerPixel());
    zero.Fill(0);
    typename ImageType::Pointer output_image = this->ResampleImage<ImageType>(input_image, zero);
    this->SetOutputITKImage<ImageType>(output_image, componentType);
  
  }else if(image["imageType"]["dimension"].as<unsigned>() == 3){
    typedef itk::VectorImage< PixelType, 3 > ImageType;

    typename ImageType::Pointer input_image = this->GetITKImage<ImageType>(image);
    typename ImageType::PixelType zero(input_image->GetNumberOfComponentsPerPixel());
    zero.Fill(0);
    typename ImageType::Pointer output_image = this->ResampleImage<ImageType>(input_image, zero);
    this->SetOutputITKImage<ImageType>(output_image, componentType);
  
  }else if(image["imageType"]["dimension"].as<unsigned>() == 4){
    typedef itk::VectorImage< PixelType, 4 > ImageType;

    typename ImageType::Pointer input_image = this->GetITKImage<ImageType>(image);
    typename ImageType::PixelType zero(input_image->GetNumberOfComponentsPerPixel());
    zero.Fill(0);
    typename ImageType::Pointer output_image = this->ResampleImage<ImageType>(input_image, zero);
    this->SetOutputITKImage<ImageType>(output_image, componentType);
  
  }
}

template<typename ImageType>
typename ImageType::Pointer 
ImgPadResample::ResampleImage(typename ImageType::Pointer image, typename ImageType::PixelType zero){

  typename itk::InterpolateImageFunction<ImageType, double>::Pointer interpolator;

  if(this->GetInterpolationType() == 1){
    using InterpolatorType = itk::LinearInterpolateImageFunction<ImageType, double>;
    interpolator = InterpolatorType::New();
  }else{
    using InterpolatorType = itk::NearestNeighborInterpolateImageFunction<ImageType, double>;
    interpolator = InterpolatorType::New();
  }

  interpolator->SetInputImage(image);  

  const unsigned dimension = image->GetImageDimension();
  
  typename ImageType::SizeType input_size = image->GetLargestPossibleRegion().GetSize();
  typename ImageType::SizeType output_size;
  output_size.Fill(1);

  vector<int> o_size = this->GetOutputSize();
  for(unsigned i = 0; i < o_size.size() && i < dimension; ++i) {
    output_size[i] = o_size[i];
  }

  typename ImageType::SpacingType input_spacing = image->GetSpacing();
  typename ImageType::SpacingType output_spacing;
  output_spacing.Fill(1);

  vector<double> o_spacing = this->GetOutputSpacing();
  for(unsigned i = 0; i < o_spacing.size() && i < dimension; ++i) {
    output_spacing[i] = o_spacing[i];
  }

  typename ImageType::SizeType output_pad_size;
  output_pad_size.Fill(0);

  vector<int> o_pad = this->GetOutputPad();
  for(unsigned i = 0; i < o_pad.size() && i < dimension; ++i) {
    output_pad_size[i] = o_pad[i];
  }
  
  typename ImageType::RegionType region;
  region.SetSize(output_size);

  typename ImageType::Pointer output_image = ImageType::New();
  output_image->SetNumberOfComponentsPerPixel(image->GetNumberOfComponentsPerPixel());
  output_image->SetRegions(region);
  output_image->SetOrigin(image->GetOrigin());
  output_image->SetSpacing(output_spacing);
  output_image->SetDirection(image->GetDirection());
  output_image->Allocate();
  output_image->FillBuffer(zero);
  
  itk::ImageRegionIteratorWithIndex<ImageType> outit(output_image, output_image->GetLargestPossibleRegion());
  outit.GoToBegin();

  while(!outit.IsAtEnd()){

    typename ImageType::IndexType output_index = outit.GetIndex();
    for(unsigned i = 0; i < dimension; i++){
      output_index[i] -= output_pad_size[i];
    }

    typename ImageType::PointType out_point;
    output_image->TransformIndexToPhysicalPoint(output_index, out_point);

    if(interpolator->IsInsideBuffer(out_point)){
      outit.Set(interpolator->Evaluate(out_point));
    }
    ++outit;
  }

  return output_image;

  // using InterpolatorType = itk::NearestNeighborInterpolateImageFunction<ImageType, double>;
  // typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
  // interpolator->SetInputImage(image);

  // using TransformType = itk::IdentityTransform<double, ImageType::ImageDimension>;
  // typename TransformType::Pointer transform = TransformType::New();
  // transform->SetIdentity();
  
  // using ResampleFilterType = itk::ResampleImageFilter<ImageType, ImageType>;
  // typename ResampleFilterType::Pointer resample_filter = ResampleFilterType::New();
  

  // typename ImageType::SizeType input_size = image->GetLargestPossibleRegion().GetSize();
  // typename ImageType::SizeType output_size;
  // output_size.Fill(1);

  // val o_size = this->GetOutputSize();
  // for(unsigned i = 0; i < o_size["length"].as<unsigned>(); ++i) {
  //   output_size[i] = o_size[i].as<int>();
  // }

  // typename ImageType::SpacingType input_spacing = image->GetSpacing();
  // typename ImageType::SpacingType output_spacing;

  // for(unsigned i = 0; i < o_size["length"].as<unsigned>(); ++i) {
  //   output_spacing[i] = input_spacing[i]*(((double)input_size[i])/((double)output_size[i]));
  // }
  
  // resample_filter->SetInput(image);

  

  // resample_filter->SetInterpolator(interpolator);
  // resample_filter->SetTransform(transform);
  
  // resample_filter->SetOutputDirection(image->GetDirection());
  // resample_filter->SetOutputOrigin(image->GetOrigin());
  // resample_filter->SetOutputSpacing(output_spacing);
  // resample_filter->SetSize(output_size);
  

  // resample_filter->Update();

  // typename ImageType::RegionType region;
  // region.SetSize(output_size);

  // typename ImageType::Pointer output_image = ImageType::New();
  // output_image->SetNumberOfComponentsPerPixel(image->GetNumberOfComponentsPerPixel());
  // output_image->SetRegions(region);
  // output_image->SetOrigin(image->GetOrigin());
  // output_image->SetSpacing(output_spacing);
  // output_image->SetDirection(image->GetDirection());
  // output_image->Allocate();

  // resample_filter->SetReferenceImage(output_image);

  // return resample_filter->GetOutput();
}

template <typename ImageType>
void ImgPadResample::SetOutputITKImage(typename ImageType::Pointer itk_image, string componentType)
{
  // image = {
  //   imageType: {
  //     dimension: 2,
  //     componentType: 'uint16_t',
  //     pixelType: 1,
  //     components: 1
  //   },
  //   name: 'Image',
  //   origin: [ 0, 0 ],
  //   spacing: [ 0.148489, 0.148489 ],
  //   direction: { rows: 2, columns: 2, data: [ 1, 0, 0, 1 ] },
  //   size: [ 1136, 852 ],
  //   data: Uint16Array []
  // }

  val origin = val::array();
  val spacing = val::array();
  val size = val::array();
  val direction = val::array();

  typename ImageType::SpacingType i_spacing = itk_image->GetSpacing();
  typename ImageType::PointType i_origin = itk_image->GetOrigin();
  typename ImageType::SizeType i_size = itk_image->GetLargestPossibleRegion().GetSize();
  typename ImageType::DirectionType::InternalMatrixType i_direction = itk_image->GetDirection().GetVnlMatrix();

  int i_dimension = itk_image->GetImageDimension();

  for(unsigned i = 0; i < i_dimension; i++){
    origin.call<void>("push", i_origin[i]);
    spacing.call<void>("push", i_spacing[i]);
    size.call<void>("push", i_size[i]);
  }

  for(unsigned i = 0; i < i_direction.size(); i++){
    direction.call<void>("push", i_direction.data_block()[i]);
  }

  val imageType = val::object();
  imageType.set("dimension", i_dimension);
  imageType.set("components", (int)itk_image->GetNumberOfComponentsPerPixel());
  imageType.set("componentType", val(componentType));
  imageType.set("pixelType", (int)this->GetPixelType());

  val matrixType = val::object();
  matrixType.set("rows", i_direction.rows());
  matrixType.set("columns", i_direction.columns());
  matrixType.set("data", direction);

  val image = val::object();
  image.set("imageType", imageType);
  image.set("name", val("Image"));
  image.set("origin", origin);
  image.set("spacing", spacing);
  image.set("direction", matrixType);
  image.set("size", size);
  image.set("data", val(typed_memory_view((int)itk_image->GetPixelContainer()->Size(),itk_image->GetBufferPointer())));

  this->SetOutput(image);
}

// template<typename ImageType>
// void ImgPadResample::SetOutputBuffer(typename ImageType::Pointer image){
  // m_OutputBuffer = make_shared<val>(val(typed_memory_view(image->GetPixelContainer()->Size(),image->GetBufferPointer())));
// }