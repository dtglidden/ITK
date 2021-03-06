/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkSampleToSubsampleFilterTest1.cxx
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

#include <iostream>
#include "itkListSample.h"
#include "itkSampleToSubsampleFilter.h"

namespace itk {
namespace Statistics {
namespace itkSampleToSubsampleFilter1Namespace {

template<class TSample >
class SubsamplerTester : public SampleToSubsampleFilter< TSample >
{
public:
  /** Standard class typedefs. */
  typedef SubsamplerTester                    Self;
  typedef SampleToSubsampleFilter<TSample>    Superclass;
  typedef SmartPointer< Self >                Pointer;
  typedef SmartPointer< const Self >          ConstPointer;

  /** Standard macros */ 
  itkTypeMacro(SubsamplerTester,SampleToSubsampleFilter);
  itkNewMacro(Self);

protected:
  SubsamplerTester() {}
  virtual ~SubsamplerTester() {}
  void PrintSelf(std::ostream& os, Indent indent) const
    {
    this->Superclass::PrintSelf(os,indent);  
    os << "Superclass = " <<  this->Superclass::GetNameOfClass() << std::endl;
    }
  
  void GenerateData()
    {
    }

private:
  SubsamplerTester(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented


};

} // end of itkSampleToSubsampleFilter1Namespace
} // end of Standard namespace
} // end of itk namespace


int itkSampleToSubsampleFilterTest1(int, char* [] )
{

  const unsigned int MeasurementVectorSize = 17;

  typedef itk::FixedArray< 
    float, MeasurementVectorSize >  MeasurementVectorType;

  typedef itk::Statistics::ListSample< MeasurementVectorType > SampleType;

  typedef itk::Statistics::itkSampleToSubsampleFilter1Namespace::SubsamplerTester< SampleType > FilterType;


  SampleType::Pointer sample = SampleType::New();

  FilterType::Pointer filter = FilterType::New();

  // Test GetInput() before setting the input
  if( filter->GetInput() != NULL )
    {
    std::cerr << "GetInput() should have returned NULL" << std::endl;
    return EXIT_FAILURE;
    }

  // Test GetOutput() before creating the output
  if( filter->GetOutput() == NULL )
    {
    std::cerr << "GetOutput() should have returned NON-NULL" << std::endl;
    return EXIT_FAILURE;
    }

  filter->SetInput( sample );

  if( filter->GetInput() != sample.GetPointer() )
    {
    std::cerr << "GetInput() didn't matched SetInput()" << std::endl;
    return EXIT_FAILURE;
    }


  //
  // Exercise the Print() method
  //
  filter->Print( std::cout );


  filter->Update();


  // Exercise the GetNameOfClass() method in the 
  // SampleToSubsampleFilter:
  std::cout << "Classname  " << filter->GetNameOfClass() << std::endl;
  std::cout << "Superclass " << filter->FilterType::Superclass::GetNameOfClass() << std::endl;

  std::cout << "Test Passed !" << std::endl;
  return EXIT_SUCCESS;
}
