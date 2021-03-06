/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkMembershipSampleTest4.cxx
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
#include "itkMembershipSample.h"
#include <vector>
#include "itkListSample.h"

// MembershipSample test using std::vector type measurement vector
int itkMembershipSampleTest4(int, char* [] )
{

  const unsigned int MeasurementVectorSize = 3;

  const unsigned int numberOfClasses1 = 2;

  typedef std::vector< float >  MeasurementVectorType;

  typedef itk::Statistics::ListSample< MeasurementVectorType > SampleType;

  typedef itk::Statistics::MembershipSample< SampleType >   MembershipSampleType;

  SampleType::Pointer sample = SampleType::New();

  sample->SetMeasurementVectorSize( MeasurementVectorSize );

  MembershipSampleType::Pointer membershipSample = MembershipSampleType::New();

  membershipSample->SetSample( sample );

  if( membershipSample->GetSample() != sample )
    {
    std::cerr << "SetSample() / GetSample() failed " << std::endl;
    return EXIT_FAILURE;
    }

  membershipSample->SetNumberOfClasses( numberOfClasses1 );
  if( membershipSample->GetNumberOfClasses() != numberOfClasses1 )
    {
    std::cerr << "SetNumberOfClasses() / GetNumberOfClasses() 1 failed " << std::endl;
    return EXIT_FAILURE;
    }

  // Exercise the Print() method
  membershipSample->Print( std::cout );

  // Add measurmement vectors to the list sample
  unsigned int sampleSize = 10;
  MeasurementVectorType mv;
  itk::Statistics::MeasurementVectorTraits::SetLength( mv, MeasurementVectorSize );

  std::cout << "Sample length = " << sample->GetMeasurementVectorSize() << std::endl;
  std::cout << "Vector length = " << itk::Statistics::MeasurementVectorTraits::GetLength( mv ) << std::endl;

  for ( unsigned int i = 0; i < sampleSize; i++ )
    {
    for (unsigned int j = 0; j < MeasurementVectorSize; j++ )
      {
      mv[j] = rand() / (RAND_MAX+1.0);
      }
    sample->PushBack(mv);
    }

  // Add instances to the membership sample
  SampleType::ConstIterator   begin = sample->Begin();
  SampleType::ConstIterator   end   = sample->End();

  SampleType::ConstIterator iter= begin;

  MembershipSampleType::ClassLabelType classLabel = 0;

  unsigned int sampleCounter = 0;
  
  while( iter != end ) 
    {
    if( sampleCounter >= 5 ) 
      {
      classLabel = 1;
      }
    SampleType::InstanceIdentifier id = iter.GetInstanceIdentifier();  
    membershipSample->AddInstance( classLabel, id );
    ++iter;
    ++sampleCounter;
    }

  // iterator tests
  //
  
  std::cerr << "Iterators..." << std::endl;
  sampleCounter = 0;
    {
    typedef MembershipSampleType::Iterator IteratorType;
    
    IteratorType s_iter = membershipSample->Begin();
    
    IteratorType bs_iter(s_iter);
    if (bs_iter != s_iter)
      {
      std::cerr << "Iterator::Copy Constructor failed" << std::endl;
      return EXIT_FAILURE; 
      }
    
    IteratorType assignment_iter( bs_iter );
    assignment_iter = s_iter;
    if (assignment_iter != s_iter)
      {
      std::cerr << "Iterator::assignment operator failed" << std::endl;
      return EXIT_FAILURE; 
      }

    MembershipSampleType::InstanceIdentifier id = 0;
    while (s_iter != membershipSample->End())
      {
      if (membershipSample->GetMeasurementVector(id) != 
          s_iter.GetMeasurementVector())
        {
        std::cerr << "Iterator::GetMeasurementVector (forward) failed" 
                  << std::endl;
        return EXIT_FAILURE;
        }
      if (id != s_iter.GetInstanceIdentifier())
        {
        std::cerr << "Iterator::GetInstanceIdentifier (forward) failed" 
                  << std::endl;
        return EXIT_FAILURE;
        }
      if (s_iter.GetFrequency() != 1)
        {
        std::cerr << "Iterator::GetFrequency (forward) failed" << std::endl;
        return EXIT_FAILURE;
        }
      if (sample->GetFrequency(id) != 1)
        {
        std::cerr << "GetFrequency (forward) failed" << std::endl;
        return EXIT_FAILURE;
        }
      if( sampleCounter < 5 ) 
        {
        if (s_iter.GetClassLabel() != 0)
          {
          std::cerr << "GetClassLabel failed:" << s_iter.GetClassLabel() << std::endl;
          return EXIT_FAILURE;
          }
        }
      else
        {
        if (s_iter.GetClassLabel() != 1)
          {
          std::cerr << "GetClassLabel failed:" << s_iter.GetClassLabel() << std::endl;
          return EXIT_FAILURE;
          }
        } 
        
      ++id;
      ++s_iter;
      ++sampleCounter;
      }
    
    if (s_iter != membershipSample->End())
      {
      std::cerr << "Iterator::End (forward) failed" << std::endl;
      return EXIT_FAILURE; 
      }
    }
 
  // ConstIterator test
  std::cerr << "Const Iterators..." << std::endl;
    {
    // forward iterator
    typedef MembershipSampleType::ConstIterator  ConstIteratorType;
    ConstIteratorType s_iter = membershipSample->Begin();
    
    // copy constructor
    ConstIteratorType bs_iter(s_iter);
    if (bs_iter != s_iter)
      {
      std::cerr << "Iterator::Copy Constructor (from const) failed" 
                << std::endl;
      return EXIT_FAILURE; 
      }

    // assignment operator
    ConstIteratorType assignment_iter( bs_iter );
    assignment_iter = s_iter;
    if (assignment_iter != s_iter)
      {
      std::cerr << "Const Iterator::operator= () failed" 
                << std::endl;
      return EXIT_FAILURE; 
      }

    // copy from non-const iterator
    MembershipSampleType::Iterator nonconst_iter = membershipSample->Begin();
    MembershipSampleType::ConstIterator s2_iter(nonconst_iter);
    if (s2_iter != s_iter)
      {
      std::cerr << "Iterator::Copy Constructor (from non-const) failed" 
                << std::endl;
      return EXIT_FAILURE; 
      }
    // assignment from non-const iterator
    s2_iter = nonconst_iter;
    if (s2_iter != s_iter)
      {
      std::cerr << "Iterator::assignment (from non-const) failed" << std::endl;
      return EXIT_FAILURE; 
      }
    
    MembershipSampleType::InstanceIdentifier id = 0;
    while (s_iter != membershipSample->End())
      {
      if (membershipSample->GetMeasurementVector(id) != 
          s_iter.GetMeasurementVector())
        {
        std::cerr << "Iterator::GetMeasurementVector (forward) failed" 
                  << std::endl;
        return EXIT_FAILURE;
        }
      if (id != s_iter.GetInstanceIdentifier())
        {
        std::cerr << "Iterator::GetInstanceIdentifier (forward) failed" 
                  << std::endl;
        return EXIT_FAILURE;
        }
      if (s_iter.GetFrequency() != 1)
        {
        std::cerr << "Iterator::GetFrequency (forward) failed" << std::endl;
        return EXIT_FAILURE;
        }
      ++id;
      ++s_iter;
      }
    
    if (s_iter != membershipSample->End())
      {
      std::cerr << "Iterator::End (forward) failed" << std::endl;
      return EXIT_FAILURE; 
      }
    
    } 

  std::cout << "Test Passed !" << std::endl;
 
  return EXIT_SUCCESS;
}
