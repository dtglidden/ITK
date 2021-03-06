#include <iostream>
#include <string>
#include <math.h>
#include <itkWarpImageFilter.h>
#include <itkStreamingImageFilter.h>

#include "../IO/itkPipelineMonitorImageFilter.h"

typedef itk::Image<float,3> ImageType;
typedef itk::Image<itk::Vector<double,3> , 3 > DeformationFieldType;
typedef itk::WarpImageFilter<ImageType,
                             ImageType,
                             DeformationFieldType> WarpFilterType;

typedef itk::PipelineMonitorImageFilter<ImageType> MonitorFilter;
#define AllocateImageFromRegionAndSpacing(ImageType,rval,region,spacing) \
{ \
  rval = ImageType::New(); \
  rval->SetSpacing(spacing); \
  rval->SetRegions(region); \
  rval->Allocate(); \
}

namespace {

ImageType::Pointer
MakeCheckerboard()
{
  typedef itk::ImageRegionIterator<ImageType> IteratorType;
  ImageType::SizeType size = {{16,16,16}};
  ImageType::SpacingType spacing;
  spacing[0] = spacing[1] = spacing[2] = 1.0;
  ImageType::IndexType index = {{0,0,0}};
  ImageType::RegionType region;
  region.SetSize(size);
  region.SetIndex(index);
  ImageType::Pointer image;
  AllocateImageFromRegionAndSpacing(ImageType,image,region,spacing);
  image->FillBuffer(0.0);
  IteratorType it(image,image->GetLargestPossibleRegion());
  for(;it != it.End(); ++it)
    {
    ImageType::IndexType ind(it.GetIndex());
    // initially checkboard 4 pixels wide
    int x = ind[0] / 4;
    int y = ind[1] / 4;
    int z = ind[2] / 4;
    bool black(((x&1) + (y&1)) & 1);
    if(z & 1)
      {
      black = !black;
      }
    it.Set(black ? 255.0 : 0.0);
    }
  return image;
}

DeformationFieldType::Pointer MakeDeformationField(int dim)
{
  typedef itk::ImageRegionIterator<DeformationFieldType> IteratorType;
  DeformationFieldType::SizeType size = {{dim,dim,dim}};
  DeformationFieldType::SpacingType spacing;
  spacing[0] = spacing[1] = spacing[2] = 16.0/(double)dim;
  DeformationFieldType::IndexType index = {{0,0,0}};
  DeformationFieldType::RegionType region;
  region.SetSize(size);
  region.SetIndex(index);
  DeformationFieldType::Pointer image;
  AllocateImageFromRegionAndSpacing(DeformationFieldType,image,region,spacing);
  IteratorType it(image,image->GetLargestPossibleRegion());
  for(;it != it.End(); ++it)
    {
    DeformationFieldType::PixelType pix;
    for(unsigned i = 0; i < 3; i++)
      {
      pix[i] = 1.0;
      }
    it.Set(pix);
    }
  return image;
}

}

int
itkWarpImageFilterTest2(int, char * [])
{
  //  itk::MultiThreader::SetGlobalDefaultNumberOfThreads(1);
  // make test image
  ImageType::Pointer image = MakeCheckerboard();
  // make full-res deformation field
  DeformationFieldType::Pointer defField1 = MakeDeformationField(16);
  // make half-res deformation field
  DeformationFieldType::Pointer defField2 = MakeDeformationField(8);

  WarpFilterType::Pointer filter = WarpFilterType::New();
  // test with full res
  filter->SetDeformationField(defField1);
  filter->SetInput(image);
  filter->SetOutputParametersFromImage(image);
  filter->Update();
  ImageType::Pointer result1 = filter->GetOutput();
  // test with half res
  filter->SetDeformationField(defField2);
  filter->SetInput(image);
  filter->SetOutputParametersFromImage(image);
  filter->Update();
  ImageType::Pointer result2 = filter->GetOutput();
  itk::ImageRegionIterator<ImageType> 
    it1(result1,result1->GetLargestPossibleRegion()),
    it2(result2,result1->GetLargestPossibleRegion());
  for(it1.GoToBegin(),it2.GoToBegin();
      !it1.IsAtEnd() && !it2.IsAtEnd();
      ++it1, ++it2)
    {
    if(it1.Value() != it2.Value())
      {
      std::cout << "Pixels differ " << it1.Value() << " " 
                << it2.Value()
                << std::endl;
      return EXIT_FAILURE;
      }
    }
  if(it1.IsAtEnd() != it2.IsAtEnd())
    {
    std::cout << "Iterators don't agree on end of image" << std::endl;
    return EXIT_FAILURE;
    }
  //
  // try streaming
  MonitorFilter::Pointer monitor1 = MonitorFilter::New();
  monitor1->SetInput( image );

  WarpFilterType::Pointer filter2 = WarpFilterType::New();
  filter2->SetDeformationField(defField2);
  filter2->SetInput( monitor1->GetOutput() );
  filter2->SetOutputParametersFromImage(image);


  MonitorFilter::Pointer monitor2 = MonitorFilter::New();
  monitor2->SetInput(filter2->GetOutput());

  typedef itk::StreamingImageFilter<ImageType,ImageType> StreamerType;
  StreamerType::Pointer streamer = StreamerType::New();
  streamer->SetInput(monitor2->GetOutput());
  streamer->SetNumberOfStreamDivisions(4);
  streamer->Update();
  itk::ImageRegionIterator<ImageType> streamIt(streamer->GetOutput(),
                                               streamer->GetOutput()->GetBufferedRegion());
  for(streamIt.GoToBegin(),it2.GoToBegin();
      !streamIt.IsAtEnd() && !it2.IsAtEnd();
      ++streamIt, ++it2)
    {
    if(streamIt.Value() != it2.Value())
      {
      std::cout << "Pixels differ " << streamIt.Value() << " " 
                << it2.Value()
                << std::endl;
      return EXIT_FAILURE;
      }
    
    }
  if(streamIt.IsAtEnd() != it2.IsAtEnd())
    {
    std::cout << "Iterators don't agree on end of image" << std::endl;
    return EXIT_FAILURE;
    }

  // this verifies that the pipeline was executed as expected along
  // with correct region propagation and output information
  if (!monitor2->VerifyAllInputCanStream(4)) 
    {
    std::cout << "Filter failed to execute as expected!" << std::endl;
    std::cout << monitor2;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
