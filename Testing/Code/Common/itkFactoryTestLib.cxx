/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkFactoryTestLib.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkFactoryTestLib.h"
#include "itkImage.h"
#include "itkRGBPixel.h"
#include "itkVersion.h"

static unsigned long itkTotalMemoryUsed = 0;

template<class TElementIdentifier, typename TElement>
class TestImportImageContainer : public itk::ImportImageContainer< TElementIdentifier, TElement >
{
public:
  /** Standard class typedefs.   */
  typedef TestImportImageContainer                             Self;
  typedef itk::ImportImageContainer< TElementIdentifier, TElement > Superclass;
  typedef itk::SmartPointer<Self>               Pointer;
  typedef itk::SmartPointer<const Self>         ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(TestImportImageContainer);

  /** Run-time type information (and related methods). */
  itkTypeMacro(TestImportImageContainer, ImportImageContainer);

  typedef typename Superclass::ElementIdentifier  ElementIdentifier;
  typedef typename Superclass::Element            Element;

  typedef std::allocator<TElement>                Allocator;

  // Methods from itkObject
  virtual ~TestImportImageContainer() 
    {
    itkTotalMemoryUsed -= m_TotalSize;
    m_TotalSize = 0;
    DeallocateManagedMemory();
    }
  TestImportImageContainer()
     : m_TotalSize(0)
     ,m_MemoryAllocatedByAllocator(false)
    {
    }
protected:
  TElement* AllocateElements(ElementIdentifier size) const
    { 
    std::cout << "TestImportImageContainer: Allocating "
              << size << " elements of type "
              << typeid(TElement).name() << " totaling "
              << sizeof(TElement) * size << " bytes" << std::endl;

    TElement* data;
    try
      {
      // allocate normally only requires 1 argument.
      // MSVC 6.0 makes it require 2, we set the second to be
      // a null pointer which means no allocation hint
      // Sun cc compiler needs a cast to assign a void pointer to another pointer
      data = static_cast<TElement*>(m_Allocator.allocate(size,0));
      if (data)
        {
        new (data) Element[size];
        }
      }
    catch(...)
      {
      data = 0;
      }
    if(!data)
      {
      // We cannot construct an error string here because we may be out
      // of memory.  Do not use the exception macro.
      throw itk::MemoryAllocationError(__FILE__, __LINE__,
                                       "Failed to allocate memory for image.",
                                       ITK_LOCATION);
      }

    m_TotalSize = size * sizeof(TElement);
    itkTotalMemoryUsed += m_TotalSize;
    
    m_MemoryAllocatedByAllocator = true;
    
    std::cout << "TestImportImageContainer: Total memory used is "
              << itkTotalMemoryUsed << " bytes" << std::endl;

    return data;
    }

  void DeallocateManagedMemory()
    {
    std::cout << "TestImportImageContainer: Deallocating "
              << this->Capacity() << " elements of type "
              << typeid(TElement).name() << " totaling "
              << sizeof(TElement) * this->Capacity() << " bytes" << std::endl;
    
    if (m_MemoryAllocatedByAllocator)
      {
      TElement * ptr = this->GetImportPointer();
      const TElement * const end = ptr + this->Capacity();
      for (TElement * base = ptr; base<end; ++base)
        {
        m_Allocator.destroy(base);
        }
      m_Allocator.deallocate(ptr, this->Capacity() );
      m_MemoryAllocatedByAllocator = false;

      this->SetImportPointer(0);
      this->SetCapacity(0);
      this->SetSize(0);
      }
    else
      {
      Superclass::DeallocateManagedMemory();
      }
    
    std::cout << "TestImportImageContainer: Total memory used is "
              << itkTotalMemoryUsed << " bytes" << std::endl;
    }

private:
  TestImportImageContainer(const TestImportImageContainer&);
  void operator=(const TestImportImageContainer&);
  mutable TElementIdentifier m_TotalSize;

  mutable Allocator          m_Allocator;
  mutable bool               m_MemoryAllocatedByAllocator;
};

class ImportImageContainerFactory : public itk::ObjectFactoryBase
{
public:
  typedef ImportImageContainerFactory   Self;
  typedef itk::ObjectFactoryBase        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Class methods used to interface with the registered factories. */
  virtual const char* GetITKSourceVersion() const { return ITK_SOURCE_VERSION; }
  const char* GetDescription() const { return "A Factory for ImportImageContainer"; }

  /** Method for class instantiation. */
  itkFactorylessNewMacro(Self);
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(ImportImageContainerFactory, itk::ObjectFactoryBase);

  /** Register one factory of this type  */
  static void RegisterOneFactory(void)
  {
    ImportImageContainerFactory::Pointer factory = ImportImageContainerFactory::New();
    itk::ObjectFactoryBase::RegisterFactory(factory);
  }

private:
  ImportImageContainerFactory(const Self&);    //purposely not implemented
  void operator=(const Self&); //purposely not implemented

#define OverrideTypeMacro(t)       this->RegisterOverride(\
        typeid(itk::ImportImageContainer<unsigned long,t >).name(),\
        typeid(TestImportImageContainer<unsigned long,t >).name(),\
        "Test ImportImageContainerOverride",\
        true,\
        itk::CreateObjectFunction<TestImportImageContainer<unsigned long,t > >::New())


  ImportImageContainerFactory()
    {
      OverrideTypeMacro(short);
      OverrideTypeMacro(unsigned char);
      OverrideTypeMacro(float);
      OverrideTypeMacro(int);
      OverrideTypeMacro(double);
      OverrideTypeMacro(itk::RGBPixel<unsigned char>);
      OverrideTypeMacro(itk::RGBPixel<unsigned short>);
    }
};

/**
 * Routine that is called when the shared library is loaded by
 * itk::ObjectFactoryBase::LoadDynamicFactories().
 *
 * itkLoad() is C (not C++) function.
 */
static ImportImageContainerFactory::Pointer staticImportImageContainerFactory;
itk::ObjectFactoryBase* itkLoad()
{
    staticImportImageContainerFactory = ImportImageContainerFactory::New();
    return staticImportImageContainerFactory;
}
