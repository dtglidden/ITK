/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkImageToListAdaptorTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
#include "itkImageToListAdaptor.h"
#include "itkRandomImageSource.h"
#include "itkImageRegionIterator.h"
#include "itkScalarToArrayCastImageFilter.h"
#include "itkFixedArray.h"

int itkImageToListAdaptorTest(int, char* [] ) 
{
  std::cout << "ImageToListAdaptor Test \n \n"; 
  bool pass = true;
  std::string whereFail = "" ;

  typedef itk::Image< float, 3 > FloatImage ;

  // Now generate a random image
  typedef itk::RandomImageSource<FloatImage> SourceType;
  SourceType::Pointer source = SourceType::New();
  FloatImage::SizeValueType size[3] = {17, 8, 20} ;
  FloatImage::SizeValueType totalSize = size[0] * size[1] * size[2] ;

  source->SetSize(size);
  float minValue = -100.0;
  float maxValue = 1000.0;

  source->SetMin( static_cast< FloatImage::PixelType >( minValue ) );
  source->SetMax( static_cast< FloatImage::PixelType >( maxValue ) );
  source->Update() ;

  // creat a new image with array pixel type from the source
  typedef itk::FixedArray< FloatImage::PixelType, 1 > ArrayPixelType ;
  typedef itk::Image< ArrayPixelType, 3 > ArrayPixelImageType ;
  typedef itk::ScalarToArrayCastImageFilter< FloatImage, ArrayPixelImageType >
    ImageCastFilterType ;
  ImageCastFilterType::Pointer castFilter = ImageCastFilterType::New() ;
  castFilter->SetInput(source->GetOutput()) ;
  castFilter->Update() ;

  // creates a sample
  typedef  itk::Statistics::ImageToListAdaptor< ArrayPixelImageType>
    ImageToListAdaptorType ;

  ImageToListAdaptorType::Pointer sample = ImageToListAdaptorType::New() ;
  sample->SetImage(castFilter->GetOutput()) ;

  // tests begin

  if (totalSize != sample->Size())
    {
      pass = false ;
      whereFail = "Size()" ;
    }

  ArrayPixelImageType::IndexType index ;
  index.Fill(2) ;// index {2, 2, 2} = instance identifier (offset from image) 
  ArrayPixelImageType::PixelType pixel = sample->GetImage()->GetPixel(index) ;
  ImageToListAdaptorType::InstanceIdentifier id = 
    static_cast< FloatImage::OffsetValueType >(sample->GetImage()
                                               ->ComputeOffset(index)) ;

  if (pixel[0] != sample->GetMeasurementVector(id)[0])
    {
      pass = false ;
      whereFail = "GetMeasurementVector()" ;
    }

  // iterator test
  typedef itk::ImageRegionConstIterator< ArrayPixelImageType > ImageIterator ;
  ImageIterator i_iter(sample->GetImage(),
                       sample->GetImage()->GetLargestPossibleRegion()) ;

  ImageToListAdaptorType::Iterator s_iter = sample->Begin() ;

  while (!i_iter.IsAtEnd())
    {
      if (i_iter.Get()[0] != s_iter.GetMeasurementVector()[0])
        {
          pass = false ;
          whereFail = "Iterator: GetMeasurementVecto()" ;
        }
      ++i_iter ;
      ++s_iter ;
    }

  if (s_iter != sample->End())
    {
      pass = false ;
      whereFail = "Iterator: End()" ;
    }

  if( !pass )
    {
      std::cout << "Test failed in " << whereFail << "." << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;


}



