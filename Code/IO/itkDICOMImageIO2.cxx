
/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkDICOMImageIO2.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$  

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include "itkArray.h"
#include "itkDICOMImageIO2.h"
#include "itkExceptionObject.h"
#include "itkByteSwapper.h"
#include "itkMetaDataDictionary.h"
#include "itkMetaDataObject.h"

#include <iostream>
#include <list>

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "DICOMCallback.h"

namespace itk
{

/** Constructor */
DICOMImageIO2::DICOMImageIO2()
{
  this->SetNumberOfDimensions(2);
  m_PixelType  = UCHAR;
  m_ByteOrder = BigEndian;
  m_Parser = new dicom::DICOMParser();
  m_AppHelper = new dicom::DICOMAppHelper();
}


/** Destructor */
DICOMImageIO2::~DICOMImageIO2()
{
  delete m_Parser;
  delete m_AppHelper;
}

bool DICOMImageIO2::CanReadFile( const char* filename ) 
{ 
  bool open = m_Parser->OpenFile( filename);
  if (!open)
    {
    std::cerr << "Couldn't open file: " << filename << std::endl;
    return false;
    }
  bool magic = m_Parser->IsDICOMFile();
  return magic;
}

void DICOMImageIO2::ReadDataCallback( doublebyte,
                                      doublebyte,
                                      dicom::DICOMParser::VRTypes,
                                      unsigned char* val,
                                      quadbyte len)
{
  unsigned int imageBytes = this->GetImageSizeInBytes();
  if (len < 0)
    {
    len = 0;
    }
  if (len < static_cast<int>( imageBytes ) )
    {
    imageBytes = len;
    }
  memcpy( m_ImageDataBuffer, val, imageBytes );
}

void DICOMImageIO2::Read(void* buffer)
{
  m_Parser->ClearAllDICOMTagCallbacks();
  m_AppHelper->RegisterCallbacks(m_Parser);
  m_AppHelper->RegisterPixelDataCallback(m_Parser);
    
  bool open = m_Parser->OpenFile(m_FileName.c_str());
  if (!open)
    {
    std::cerr << "Couldn't open file: " << m_FileName << std::endl;
    return;
    }

  // Should ReadHeader() be Read() since more than just a header is read?
  m_Parser->ReadHeader();

  Array<float> imagePosition(3);
  imagePosition[0] = m_AppHelper->GetImagePositionPatient()[0];
  imagePosition[1] = m_AppHelper->GetImagePositionPatient()[1];
  imagePosition[2] = m_AppHelper->GetImagePositionPatient()[2];

  EncapsulateMetaData<Array<float> >(this->GetMetaDataDictionary(),
                                     "ITK_ImageOrigin",
                                     imagePosition);
  
  void* newData;
  dicom::DICOMParser::VRTypes newType;
  unsigned long imageDataLength = 0;


  m_AppHelper->GetImageData(newData, newType, imageDataLength);
  memcpy(buffer, newData, imageDataLength);

  // Clean up
  m_AppHelper->Clear();
}


/** 
 *  Read Information about the dicom file
 */
void DICOMImageIO2::ReadImageInformation()
{
  m_Parser->ClearAllDICOMTagCallbacks();
  m_AppHelper->RegisterCallbacks(m_Parser);

  bool open = m_Parser->OpenFile(m_FileName.c_str());
  if (!open)
    {
    std::cerr << "Couldn't open file: " << m_FileName << std::endl;
    return;
    }

  m_Parser->ReadHeader();

  float* spacing = m_AppHelper->GetPixelSpacing();
  float* origin  = m_AppHelper->GetImagePositionPatient();

  int* dims = m_AppHelper->GetDimensions();

  
  for (int i = 0; i < 2; i++)
    {
    this->SetOrigin(i, origin[i]);
    this->SetSpacing(i, spacing[i]);
    this->SetDimensions(i, dims[i]);
    }

  int numBits = m_AppHelper->GetBitsAllocated();
  bool sign = m_AppHelper->RescaledImageDataIsSigned();

  bool isFloat = m_AppHelper->RescaledImageDataIsFloat();
  int num_comp = m_AppHelper->GetNumberOfComponents();
      
  if (isFloat)  // Float
    {
    this->SetPixelType(ImageIOBase::FLOAT);
    this->SetComponentType(ImageIOBase::FLOAT);
    }
  else if (num_comp == 3) //RGB
    {
    if (numBits == 8)
      {
      this->SetComponentType(ImageIOBase::UCHAR);
      this->SetPixelType(ImageIOBase::UCHAR);
      }
    else
      {
      this->SetComponentType(ImageIOBase::USHORT);
      this->SetPixelType(ImageIOBase::USHORT);
      }
    }
  else // Everything else
    {
    if (numBits == 8)
      {
      if (sign)
        {
        this->SetPixelType(ImageIOBase::CHAR);
        this->SetComponentType(ImageIOBase::CHAR);
        }
      else
        {
        this->SetPixelType(ImageIOBase::UCHAR);
        this->SetComponentType(ImageIOBase::UCHAR);
        }
      }
    else if (numBits == 16)
      {
      if (sign)
        {
        this->SetPixelType(ImageIOBase::SHORT);
        this->SetComponentType(ImageIOBase::SHORT);
        }
      else
        {
        this->SetPixelType(ImageIOBase::USHORT);
        this->SetComponentType(ImageIOBase::USHORT);
        }
      }
    else
      {
      this->SetPixelType(ImageIOBase::USHORT);
      this->SetComponentType(ImageIOBase::USHORT);
      }
    }

  this->SetNumberOfComponents(num_comp);

  // Cleanup
  m_AppHelper->Clear();
}

/** Print Self Method */
void DICOMImageIO2::PrintSelf(std::ostream& os, Indent indent) const
{
  unsigned int i;
  Superclass::PrintSelf(os, indent);
  os << indent << "Spacing: ( ";
  for (i=0; i < m_NumberOfDimensions; i++)
    {
    os << m_Spacing[i] << " ";
    }
  os << " )\n";
  os << indent << "Origin: ( ";
  for (i=0; i < m_Origin.size(); i++)
    {
    os << m_Origin[i] << " ";
    }
  os << " )" << std::endl; 
}


void 
DICOMImageIO2
::GetPatientName(char* name)
{
  m_AppHelper->GetPatientName(name);
}


void 
DICOMImageIO2
::GetPatientID(char* id)
{
  m_AppHelper->GetPatientID(id);
}



void 
DICOMImageIO2
::GetPatientSex(char* sex)
{
  m_AppHelper->GetPatientSex(sex);
}



void 
DICOMImageIO2
::GetPatientAge(char* age)
{
  m_AppHelper->GetPatientAge(age);
}



void 
DICOMImageIO2
::GetStudyDate(char* date)
{
  m_AppHelper->GetStudyDate(date);
}



void 
DICOMImageIO2
::GetModality(char* modality)
{
  m_AppHelper->GetModality(modality);
}



void 
DICOMImageIO2
::GetManufacturer(char* manu)
{
  m_AppHelper->GetManufacturer(manu);
}



void 
DICOMImageIO2
::GetInstitution(char* ins)
{
  m_AppHelper->GetInstitution(ins);
}



void 
DICOMImageIO2
::GetModel(char* model)
{
  m_AppHelper->GetModel(model);
}



} // end namespace itk
