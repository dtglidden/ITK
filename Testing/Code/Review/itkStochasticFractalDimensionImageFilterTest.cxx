/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkStochasticFractalDimensionImageFilterTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkBinaryThresholdImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkStochasticFractalDimensionImageFilter.h"
#include "itkTimeProbe.h"
#include "itkSimpleFilterWatcher.h"

namespace StochasticFractalDimensionImageFilterTest
{

template <unsigned int NDimension>
class Helper
{
public:

  static int Run( int argc, char *argv[] )
    {
    typedef float                             PixelType;
    typedef itk::Image<PixelType, NDimension> ImageType;

    typedef itk::ImageFileReader<ImageType> ReaderType;
    typename ReaderType::Pointer imageReader = ReaderType::New();
    imageReader->SetFileName( argv[2] );
    imageReader->Update();

    typedef itk::StochasticFractalDimensionImageFilter<ImageType> FractalFilterType;
    typename FractalFilterType::Pointer fractalFilter = FractalFilterType::New();
    fractalFilter->SetInput( imageReader->GetOutput() );

    itk::SimpleFilterWatcher watcher( fractalFilter, "FractalDimensionFilter");
  
    typename FractalFilterType::RadiusType radius;
    typename FractalFilterType::RadiusType radius2;
    
    radius.Fill( 5 );
    fractalFilter->SetNeighborhoodRadius( radius );
    radius2 = fractalFilter->GetNeighborhoodRadius();
    if( radius2[0] != 5 )
      {
      std::cerr << "Error in Set/GetNeighborhoodRadius()" << std::endl;
      return EXIT_FAILURE;
      }

    radius.Fill( 2 );
    fractalFilter->SetNeighborhoodRadius( radius );
    radius2 = fractalFilter->GetNeighborhoodRadius();
    if( radius2[0] != 2 )
      {
      std::cerr << "Error in Set/GetNeighborhoodRadius()" << std::endl;
      return EXIT_FAILURE;
      }

    if( argc > 4 )
      {
      radius.Fill( atoi( argv[4] ) );
      fractalFilter->SetNeighborhoodRadius( radius );
      }

    if( argc > 5 )
      {
      PixelType maskLabel = 1.0;
      if( argc > 6 )
        {
        maskLabel = static_cast<PixelType>( atof( argv[6] ) );
        }

      typename ReaderType::Pointer labelImageReader = ReaderType::New();
      labelImageReader->SetFileName( argv[5] );
      labelImageReader->Update();

      typedef typename FractalFilterType::MaskImageType MaskImageType;

      typedef itk::BinaryThresholdImageFilter<ImageType, MaskImageType> ThresholderType;

      typename ThresholderType::Pointer thresholder = ThresholderType::New();
      thresholder->SetInput( labelImageReader->GetOutput() );
      thresholder->SetInsideValue( 1 );
      thresholder->SetOutsideValue( 0 );
      thresholder->SetLowerThreshold( maskLabel );
      thresholder->SetUpperThreshold( maskLabel );
      thresholder->Update();

      fractalFilter->SetMaskImage( thresholder->GetOutput() );
      }

    try
      {
      itk::TimeProbe timer;

      timer.Start();
      std::cout << "/" << std::flush;
      fractalFilter->Update();
      std::cout << "/" << std::flush;
      timer.Stop();

      std::cout << "   (elapsed time: " << timer.GetMeanTime() << ")" << std::endl;
      }
    catch( ... )
      {
      std::cerr << "Exception caught." << std::endl;
      return EXIT_FAILURE;
      }

    typedef itk::ImageFileWriter<ImageType> WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetInput( fractalFilter->GetOutput() );
    writer->SetFileName( argv[3] );
    writer->UseCompressionOn();
    writer->Update();

    return EXIT_SUCCESS;
  }

};

}

int itkStochasticFractalDimensionImageFilterTest( int argc, char *argv[] )
{
  if ( argc < 3 )
    {
    std::cout << "Usage: " << argv[0] << " imageDimension "
      << "inputImage outputImage [radius] [labelImage] [label]" << std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int imageDimension = atoi( argv[1] );

  switch( imageDimension )
    {
    case 2:
      {
      StochasticFractalDimensionImageFilterTest::Helper<2>::Run( argc, argv );
      break;
      }
    case 3:
      {
      StochasticFractalDimensionImageFilterTest::Helper<3>::Run( argc, argv );
      break;
      }
    default:
      std::cerr << "Image Dimension " << imageDimension;
      std::cerr << " is not supported by this test " << std::endl;
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
