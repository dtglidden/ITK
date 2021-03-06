/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkDistanceMetricTest.cxx
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

#include "itkDistanceMetric.h"
#include "itkObjectFactory.h"

namespace itk {
namespace Statistics {
namespace DistanceMetricTest {

template <class TMeasurementVector>
class MyDistanceMetric : public DistanceMetric< TMeasurementVector >
{
public:
  /** Standard class typedef. */
  typedef MyDistanceMetric                     Self;
  typedef DistanceMetric< TMeasurementVector > Superclass;
  typedef SmartPointer< Self >                 Pointer;
  typedef SmartPointer<const Self>             ConstPointer;

  /** Standard macros */
  itkTypeMacro(MyDistanceMetric, DistanceMetric);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
 
  /** Evaluate membership score */
  double Evaluate(const TMeasurementVector & ) const
    {
    double score;
    score = 1;
    return score;
    } 

  double Evaluate(const TMeasurementVector &, const TMeasurementVector & ) const
    {
    double score;
    score = 1;
    return score;
    } 
};

}
}
}
int itkDistanceMetricTest(int, char* [] )
{

  typedef unsigned int MeasurementVectorSizeType;

  const MeasurementVectorSizeType MeasurementVectorSize = 17;

  typedef itk::FixedArray< 
    float, MeasurementVectorSize >  MeasurementVectorType;


  typedef itk::Statistics::DistanceMetricTest::MyDistanceMetric< 
    MeasurementVectorType >   DistanceMetricType;

  typedef DistanceMetricType::MeasurementVectorSizeType MeasurementVectorSizeType;

  DistanceMetricType::Pointer distance = DistanceMetricType::New();

  std::cout << distance->GetNameOfClass() << std::endl;
  std::cout << distance->DistanceMetricType::Superclass::GetNameOfClass() << std::endl;

  distance->Print(std::cout);

  //try changing the measurment vector size, it should throw an exception
  try
    {
    MeasurementVectorSizeType newSize = 20;
    distance->SetMeasurementVectorSize( newSize ); 

    std::cerr << "Changing measurement vector size is not allowed for a fixed array vector\n"
              << "an exception should have been thrown" << std::endl;
    return EXIT_FAILURE;
    }
  catch( itk::ExceptionObject & excpt )
    {
    std::cerr << "Exception thrown: " << excpt << std::endl;
    }

  //try re-setting the measurment vector size to the same value, no exceptins should be
  //thrown 
  try
    {
    MeasurementVectorSizeType sameSize = 17;
    distance->SetMeasurementVectorSize( sameSize ); 
    }
  catch( itk::ExceptionObject & excpt )
    {
    std::cerr << "Exception thrown: " << excpt << std::endl;
    return EXIT_FAILURE;
    }


  //try setting an origin vector with a different size it should throw an exception
  try
    {
    DistanceMetricType::OriginType origin;
    MeasurementVectorSizeType newSize = 25;
    origin.SetSize( newSize );
    distance->SetOrigin( origin ); 

    std::cerr << "Attempting to set an origin vector with a different size,"
              << "should result in an exception" << std::endl;
    return EXIT_FAILURE;
    }
  catch( itk::ExceptionObject & excpt )
    {
    std::cerr << "Exception thrown: " << excpt << std::endl;
    }

  return EXIT_SUCCESS;
}
