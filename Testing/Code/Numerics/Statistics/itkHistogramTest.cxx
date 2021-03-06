/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkHistogramTest.cxx
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
#include "itkHistogram.h"

#include "itkDenseFrequencyContainer.h"

int itkHistogramTest(int, char* [] ) 
{
  std::cout << "Histogram Test \n \n"; 
  bool pass = true;
  std::string whereFail = "" ;
  
  typedef unsigned int MeasurementType ;

  // creats a histogram with 3 components measurement vectors
  typedef itk::Statistics::Histogram< MeasurementType, 3, itk::Statistics::DenseFrequencyContainer > HistogramType ;
  HistogramType::Pointer histogram = HistogramType::New() ;

  const int NUM_BINS=64;
  const int LOWERBOUND=0;
  const int UPPERBOUND=1024;
  // initializes a 64 x 64 x 64 histogram with equal size interval
  HistogramType::SizeType size ;
  size.Fill(NUM_BINS) ;
  unsigned long totalSize = size[0] * size[1] * size[2] ;
  HistogramType::MeasurementVectorType lowerBound ;
  HistogramType::MeasurementVectorType upperBound ;
  lowerBound.Fill(LOWERBOUND) ;
  upperBound.Fill(UPPERBOUND) ;
  histogram->Initialize(size, lowerBound, upperBound ) ;
  histogram->SetToZero();
  double interval = 
    (upperBound[0] - lowerBound[0]) /
    static_cast< HistogramType::MeasurementType >(size[0]) ;

  // tests begin
  HistogramType::MeasurementVectorType measurements ;
  HistogramType::IndexType index ;
  HistogramType::IndexType ind;
  {
  const int fillValues[3]={LOWERBOUND,512,UPPERBOUND};
  const int knownFillLocation[3]={0,32,63};
  for ( unsigned int testiter=0; testiter<3; testiter++)
    {
    measurements.Fill(fillValues[testiter]) ;
    index.Fill(knownFillLocation[testiter]) ;
    if(histogram->GetIndex(measurements,ind))
      {
      if(index != ind)
        {
        pass = false ;
        whereFail = "GetIndex(MeasurementVectorType&)";
        }
      }
    else
      {
      pass = false ;
      whereFail = "GetIndex(MeasurementVectorType&)";
      }
    }
  }

  HistogramType::InstanceIdentifier id =
    histogram->GetInstanceIdentifier(index);
  if (index != histogram->GetIndex(id))
    {
    pass = false ;
    whereFail = "GetIndex(InstanceIdentifier&)" ;
    }

  index.Fill(100) ;

  if (!histogram->IsIndexOutOfBounds(index))
    {
    pass = false ;
    whereFail = "IsIndexOutOfBound(IndexType)" ;
    }

  if (totalSize != histogram->Size())
    {
    pass = false ;
    whereFail = "Size()" ;
    }

  if (size != histogram->GetSize())
    {
    pass = false ;
    whereFail = "GetSize()" ;
    }

  if ((lowerBound[0] + interval * 31) != histogram->GetBinMin(0,31))
    {
    pass = false ;
    whereFail = "GetBinMin(Dimension, nthBin)" ;
    }

  if ((lowerBound[0] + interval * 32) != histogram->GetBinMax(0,31))
    {
    pass = false ;
    whereFail = "GetBinMax(Dimension, nthBin)" ;
    }

  for (id = 0 ; 
       id < static_cast< HistogramType::InstanceIdentifier >(totalSize) ;
       id++)
    {
    histogram->SetFrequency(id, 1) ;
    histogram->IncreaseFrequency(id, 1) ;
    if (histogram->GetFrequency(id) != 2)
      {
      pass = false ;
      whereFail = 
        "SetFrequency(InstanceIdentifier, 1) + IncreaseFrequency(InstanceIdentifier, 1) + GetFrequency(InstanceIdentifier)" ;
      }
    }

  if (histogram->Quantile(0, 0.5) != 512.0)
    {
    pass = false ;
    whereFail = "Quantile(Dimension, percent)" ;
    }

  if( !pass )
    {
    std::cout << "Test failed in " << whereFail << "." << std::endl;
    return EXIT_FAILURE;
    }


  // Histogram with SparseFrequencyContainer
  typedef itk::Statistics::Histogram< MeasurementType, 3, 
    itk::Statistics::SparseFrequencyContainer > SparseHistogramType ;
  SparseHistogramType::Pointer sparseHistogram = SparseHistogramType::New() ;

  // initializes a 64 x 64 x 64 histogram with equal size interval
  sparseHistogram->Initialize(size, lowerBound, upperBound ) ;
  sparseHistogram->SetToZero();
  interval = (upperBound[0] - lowerBound[0]) / 
    static_cast< SparseHistogramType::MeasurementType >(size[0]) ;

  measurements.Fill(512) ;
  index.Fill(32);
  sparseHistogram->GetIndex(measurements,ind);

  if (index != ind)
    {
    pass = false ;
    whereFail = "Sparse Histogram: GetIndex(MeasurementVectorType&)" ;
    }
  
  id = sparseHistogram->GetInstanceIdentifier(index);
  if (index != sparseHistogram->GetIndex(id))
    {
    pass = false ;
    whereFail = "Sparse Histogram: GetIndex(InstanceIdentifier&)" ;
    }

  index.Fill(100) ;
  
  if (!sparseHistogram->IsIndexOutOfBounds(index))
    {
    pass = false ;
    whereFail = "Sparse Histogram: IsIndexOutOfBound(IndexType)" ;
    }

  if (totalSize != sparseHistogram->Size())
    {
    pass = false ;
    whereFail = "Sparse Histogram: Size()" ;
    }

  if (size != sparseHistogram->GetSize())
    {
    pass = false ;
    whereFail = "Sparse Histogram: GetSize()" ;
    }

  if ((lowerBound[0] + interval * 31) != sparseHistogram->GetBinMin(0,31))
    {
    pass = false ;
    whereFail = "Sparse Histogram: GetBinMin(Dimension, nthBin)" ;
    }

  if ((lowerBound[0] + interval * 32) != sparseHistogram->GetBinMax(0,31))
    {
    pass = false ;
    whereFail = "Sparse Histogram: GetBinMax(Dimension, nthBin)" ;
    }


  for (id = 0 ; 
       id < static_cast< SparseHistogramType::InstanceIdentifier >(totalSize) ;
       id++)
    {
    bool result = sparseHistogram->SetFrequency(id, 1) ;
    if( !result )
      {
      pass = false ;
      whereFail = 
        "SetFrequency(InstanceIdentifier, 1) " ;
      break;

      }

    result = sparseHistogram->IncreaseFrequency(id, 1) ;
    if( !result )
      {
      pass = false ;
      whereFail = 
        "IncreaseFrequency(InstanceIdentifier, 1) " ;
      break;
      }

    if (sparseHistogram->GetFrequency(id) != 2)
      {
      pass = false ;
      whereFail = 
        "SetFrequency(InstanceIdentifier, 1) + IncreaseFrequency(InstanceIdentifier, 1) + GetFrequency(InstanceIdentifier)" ;
      break;
      }
    }

  if (pass && (sparseHistogram->Quantile(0, 0.5) != 512.0))
    {
    pass = false ;
    whereFail = "Sparse Histogram: Quantile(Dimension, percent)" ;
    }

  if( !pass )
    {
    std::cout << "Test failed in " << whereFail << "." << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;


}



