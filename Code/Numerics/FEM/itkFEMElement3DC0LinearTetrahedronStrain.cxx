/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkFEMElement3DC0LinearTetrahedronStrain.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// disable debug warnings in MS compiler
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif

#include "itkFEMElement3DC0LinearTetrahedronStrain.h"
#include "vnl/vnl_math.h"

namespace itk {
namespace fem {


Element3DC0LinearTetrahedronStrain
::Element3DC0LinearTetrahedronStrain() : Superclass()
{
}

Element3DC0LinearTetrahedronStrain
::Element3DC0LinearTetrahedronStrain(
      NodeIDType ns_[],
      Material::ConstPointer m_) : Superclass()
{
  // Set the geometrical points
  for (int k=0; k<4; k++)
    { 
    this->SetNode( k, ns_[k] ); 
    }

  /*
   * Initialize the pointer to material object and check that
   * we were given the pointer to the right class.
   * If the material class was incorrect an exception is thrown.
   */
  if( (m_mat=dynamic_cast<const MaterialLinearElasticity*>(&*m_)) == 0 )
    {
    throw FEMExceptionWrongClass(__FILE__,__LINE__,"Element3DC0LinearTetrahedronStrain::Element3DC0LinearTetrahedronStrain()");
    }
}

FEM_CLASS_REGISTER(Element3DC0LinearTetrahedronStrain)

}} // end namespace itk::fem
