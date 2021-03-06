/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkKernelTransform.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkKernelTransform_txx
#define __itkKernelTransform_txx
#include "itkKernelTransform.h"

namespace itk
{


/**
 *
 */
template <class TScalarType, unsigned int NDimensions>
KernelTransform<TScalarType, NDimensions>::
KernelTransform():Superclass(
  NDimensions,
  NDimensions ) 
// the second NDimensions is associated is provided as
// a tentative number for initializing the Jacobian.
// The matrix can be resized at run time so this number
// here is irrelevant. The correct size of the Jacobian
// will be NDimension X NDimension.NumberOfLandMarks.
{

  this->m_I.set_identity();
  this->m_SourceLandmarks = PointSetType::New();
  this->m_TargetLandmarks = PointSetType::New();
  this->m_Displacements   = VectorSetType::New();
  this->m_WMatrixComputed = false;

  this->m_Stiffness = 0.0;
}

/**
 *
 */
template <class TScalarType, unsigned int NDimensions>
KernelTransform<TScalarType, NDimensions>::
~KernelTransform()
{
}


/**
 *
 */
template <class TScalarType, unsigned int NDimensions>
void
KernelTransform<TScalarType, NDimensions>::
SetSourceLandmarks(PointSetType * landmarks)
{
  itkDebugMacro("setting SourceLandmarks to " << landmarks ); 
  if (this->m_SourceLandmarks != landmarks) 
    { 
    this->m_SourceLandmarks = landmarks; 
    this->UpdateParameters();
    this->Modified(); 
    } 
}


/**
 *
 */
template <class TScalarType, unsigned int NDimensions>
void
KernelTransform<TScalarType, NDimensions>::
SetTargetLandmarks(PointSetType * landmarks)
{
  itkDebugMacro("setting TargetLandmarks to " << landmarks ); 
  if (this->m_TargetLandmarks != landmarks) 
    { 
    this->m_TargetLandmarks = landmarks; 
    this->UpdateParameters();
    this->Modified(); 
    } 
}


/**
 * This method has been deprecated as of ITK 3.6.
 * Please use the method: void ComputeG( vector, gmatrix ) instead.
 */
#if !defined(ITK_LEGACY_REMOVE)
template <class TScalarType, unsigned int NDimensions>
const typename KernelTransform<TScalarType, NDimensions>::GMatrixType &
KernelTransform<TScalarType, NDimensions>::
ComputeG( const InputVectorType & ) const
{
  itkLegacyReplaceBodyMacro( itkKernelTransform::ComputeG_vector, 3.6,
                             itkKernelTransform::ComputeG_vector_gmatrix );
  return m_GMatrix;
}
#endif

/**
 *
 */
template <class TScalarType, unsigned int NDimensions>
void
KernelTransform<TScalarType, NDimensions>::
ComputeG( const InputVectorType &, GMatrixType & itkNotUsed( gmatrix ) ) const
{
  itkExceptionMacro( << "ComputeG(vector,gmatrix) must be reimplemented"
                     << " in subclasses of KernelTransform." );
}

/**
 *
 */
template <class TScalarType, unsigned int NDimensions>
const typename KernelTransform<TScalarType, NDimensions>::GMatrixType &
KernelTransform<TScalarType, NDimensions>::
ComputeReflexiveG( PointsIterator ) const
{
  m_GMatrix.fill( NumericTraits< TScalarType >::Zero );
  m_GMatrix.fill_diagonal( m_Stiffness );

  return m_GMatrix;
}


/**
 * Default implementation of the the method. This can be overloaded
 * in transforms whose kernel produce diagonal G matrices.
 */
template <class TScalarType, unsigned int NDimensions>
void
KernelTransform<TScalarType, NDimensions>::
ComputeDeformationContribution( const InputPointType  & thisPoint,
                                OutputPointType & result  ) const
{

  unsigned long numberOfLandmarks = this->m_SourceLandmarks
                                        ->GetNumberOfPoints();

  PointsIterator sp  = this->m_SourceLandmarks->GetPoints()->Begin();

  GMatrixType Gmatrix; 

  for(unsigned int lnd=0; lnd < numberOfLandmarks; lnd++ )
    {
    this->ComputeG( thisPoint - sp->Value(), Gmatrix );
    for(unsigned int dim=0; dim < NDimensions; dim++ )
      {
      for(unsigned int odim=0; odim < NDimensions; odim++ )
        {
        result[ odim ] += Gmatrix(dim, odim ) * m_DMatrix(dim,lnd);
        }
      }
    ++sp;
    }

}


/**
 *
 */
template <class TScalarType, unsigned int NDimensions>
void KernelTransform<TScalarType, NDimensions>
::ComputeD(void)
{
  unsigned long numberOfLandmarks = this->m_SourceLandmarks
                                        ->GetNumberOfPoints();
  
  PointsIterator sp  = this->m_SourceLandmarks->GetPoints()->Begin();
  PointsIterator tp  = this->m_TargetLandmarks->GetPoints()->Begin();
  PointsIterator end = this->m_SourceLandmarks->GetPoints()->End();

  this->m_Displacements->Reserve( numberOfLandmarks );
  typename VectorSetType::Iterator vt = this->m_Displacements->Begin();

  while( sp != end )
    {
    vt->Value() = tp->Value() - sp->Value();
    vt++;
    sp++;
    tp++;
    }
}

/**
 *
 */
template <class TScalarType, unsigned int NDimensions>
void KernelTransform<TScalarType, NDimensions>
::ComputeWMatrix(void)
{

  typedef vnl_svd<TScalarType>  SVDSolverType;

  this->ComputeL();
  this->ComputeY();
  SVDSolverType svd( this->m_LMatrix, 1e-8 );
  this->m_WMatrix = svd.solve( this->m_YMatrix );

  this->ReorganizeW();

}

/**
 *
 */
template <class TScalarType, unsigned int NDimensions>
void KernelTransform<TScalarType, NDimensions>::
ComputeL(void)
{
  unsigned long numberOfLandmarks = this->m_SourceLandmarks
                                        ->GetNumberOfPoints();
  vnl_matrix<TScalarType> O2(NDimensions*(NDimensions+1),
                             NDimensions*(NDimensions+1), 0);

  this->ComputeP();
  this->ComputeK();

  this->m_LMatrix.set_size( 
    NDimensions*(numberOfLandmarks+NDimensions+1),
    NDimensions*(numberOfLandmarks+NDimensions+1) );

  this->m_LMatrix.fill( 0.0 );

  this->m_LMatrix.update( this->m_KMatrix, 0, 0 );
  this->m_LMatrix.update( this->m_PMatrix, 0, this->m_KMatrix.columns() );
  this->m_LMatrix.update( this->m_PMatrix.transpose(),
                          this->m_KMatrix.rows(), 0);
  this->m_LMatrix.update( O2, this->m_KMatrix.rows(),
                          this->m_KMatrix.columns());

}


/**
 *
 */
template <class TScalarType, unsigned int NDimensions>
void KernelTransform<TScalarType, NDimensions>::
ComputeK(void)
{
  unsigned long numberOfLandmarks = this->m_SourceLandmarks
                                        ->GetNumberOfPoints();
  GMatrixType G;

  this->ComputeD();

  this->m_KMatrix.set_size( NDimensions * numberOfLandmarks,
                    NDimensions * numberOfLandmarks );

  this->m_KMatrix.fill( 0.0 );

  PointsIterator p1  = this->m_SourceLandmarks->GetPoints()->Begin();
  PointsIterator end = this->m_SourceLandmarks->GetPoints()->End();

  // K matrix is symmetric, so only evaluate the upper triangle and
  // store the values in bot the upper and lower triangle
  unsigned int i = 0;
  while( p1 != end )
    {
    PointsIterator p2 = p1; // start at the diagonal element
    unsigned int j = i;

    // Compute the block diagonal element, i.e. kernel for pi->pi
    G = this->ComputeReflexiveG(p1);
    this->m_KMatrix.update(G, i*NDimensions, i*NDimensions);
    p2++;
    j++;
    
    // Compute the upper (and copy into lower) triangular part of K
    while( p2 != end ) 
      {
      const InputVectorType s = p1.Value() - p2.Value();
      this->ComputeG(s, G);
      // write value in upper and lower triangle of matrix
      this->m_KMatrix.update(G, i*NDimensions, j*NDimensions);
      this->m_KMatrix.update(G, j*NDimensions, i*NDimensions);  
      p2++;
      j++;
      }
    p1++;
    i++;
    }
}

/**
 *
 */
template <class TScalarType, unsigned int NDimensions>
void KernelTransform<TScalarType, NDimensions>::
ComputeP()
{
  unsigned long numberOfLandmarks = this->m_SourceLandmarks
                                        ->GetNumberOfPoints();
  IMatrixType I;
  IMatrixType temp;
  InputPointType p;

  I.set_identity();
  this->m_PMatrix.set_size( NDimensions*numberOfLandmarks,
                    NDimensions*(NDimensions+1) );
  this->m_PMatrix.fill( 0.0 );
  for (unsigned int i = 0; i < numberOfLandmarks; i++)
    {
    this->m_SourceLandmarks->GetPoint(i, &p);
    for (unsigned int j = 0; j < NDimensions; j++)
      {
      temp = I * p[j];
      this->m_PMatrix.update(temp, i*NDimensions, j*NDimensions);
      }
    this->m_PMatrix.update(I, i*NDimensions, NDimensions*NDimensions);
    }
}

/**
 *
 */
template <class TScalarType, unsigned int NDimensions>
void KernelTransform<TScalarType, NDimensions>::
ComputeY(void)
{
  unsigned long numberOfLandmarks = this->m_SourceLandmarks
                                        ->GetNumberOfPoints();

  typename VectorSetType::ConstIterator displacement =
    this->m_Displacements->Begin();

  this->m_YMatrix.set_size( NDimensions*(numberOfLandmarks+NDimensions+1), 1);

  this->m_YMatrix.fill( 0.0 );
    
  for (unsigned int i = 0; i < numberOfLandmarks; i++)
    {
    for (unsigned int j = 0; j < NDimensions; j++)
      {
      this->m_YMatrix.put(i*NDimensions+j, 0, displacement.Value()[j]);
      }
    displacement++;
    }

  for (unsigned int i = 0; i < NDimensions*(NDimensions+1); i++) 
    {
    this->m_YMatrix.put(numberOfLandmarks*NDimensions+i, 0, 0);
    }
}


/**
 *
 */
template <class TScalarType, unsigned int NDimensions>
void
KernelTransform<TScalarType, NDimensions>
::ReorganizeW(void) 
{
  unsigned long numberOfLandmarks = this->m_SourceLandmarks
                                        ->GetNumberOfPoints();

  // The deformable (non-affine) part of the registration goes here
  this->m_DMatrix.set_size(NDimensions,numberOfLandmarks);
  unsigned int ci = 0;
  for(unsigned int lnd=0; lnd < numberOfLandmarks; lnd++ )
    {
    for(unsigned int dim=0; dim < NDimensions; dim++ )
      {
      this->m_DMatrix(dim,lnd) = this->m_WMatrix(ci++,0);
      }
    }

  // This matrix holds the rotational part of the Affine component
  for(unsigned int j=0; j < NDimensions; j++ )
    {
    for(unsigned int i=0; i < NDimensions; i++ )
      {
      this->m_AMatrix(i,j) = this->m_WMatrix(ci++,0);
      }
    }

  // This vector holds the translational part of the Affine component
  for(unsigned int k=0; k < NDimensions; k++ )
    {
    this->m_BVector(k) = this->m_WMatrix(ci++,0);
    }

  // release WMatrix memory by assigning a small one.
  this->m_WMatrix = WMatrixType(1,1);   

}

/**
 *
 */
template <class TScalarType, unsigned int NDimensions>
typename KernelTransform<TScalarType, NDimensions>::OutputPointType
KernelTransform<TScalarType, NDimensions>
::TransformPoint( const InputPointType& thisPoint ) const
{

  OutputPointType result;

  typedef typename OutputPointType::ValueType ValueType;

  result.Fill( NumericTraits< ValueType >::Zero );

  this->ComputeDeformationContribution( thisPoint, result );

  // Add the rotational part of the Affine component
  for(unsigned int j=0; j < NDimensions; j++ )
    {
    for(unsigned int i=0; i < NDimensions; i++ )
      {
      result[i] += this->m_AMatrix(i,j) * thisPoint[j];
      }
    }

  
 
  // This vector holds the translational part of the Affine component
  for(unsigned int k=0; k < NDimensions; k++ )
    {
    result[k] += this->m_BVector(k) + thisPoint[k];
    }

  return result;

}

// Compute the Jacobian in one position 
template <class TScalarType, unsigned int NDimensions>
const typename KernelTransform<TScalarType,NDimensions>::JacobianType & 
KernelTransform< TScalarType,NDimensions>::
GetJacobian( const InputPointType & ) const
{


  this->m_Jacobian.Fill( 0.0 );

  // FIXME: TODO
  // The Jacobian should be computable in terms of the matrices
  // used to Transform points...
  itkExceptionMacro(<< "GetJacobian must be implemented in subclasses"
                    << " of KernelTransform.");

  return this->m_Jacobian;

}


// Set the parameters
// NOTE that in this transformation both the Source and Target
// landmarks could be considered as parameters. It is assumed
// here that the Target landmarks are provided by the user and
// are not changed during the optimization process required for
// registration.
template <class TScalarType, unsigned int NDimensions>
void
KernelTransform<TScalarType, NDimensions>::
SetParameters( const ParametersType & parameters )
{
  typename PointsContainer::Pointer landmarks = PointsContainer::New();
  const unsigned int numberOfLandmarks =  parameters.Size() / NDimensions; 
  landmarks->Reserve( numberOfLandmarks );

  PointsIterator itr = landmarks->Begin();
  PointsIterator end = landmarks->End();

  InputPointType  landMark; 

  unsigned int pcounter = 0;
  while( itr != end )
    {
    for(unsigned int dim=0; dim<NDimensions; dim++)
      {
      landMark[ dim ] = parameters[ pcounter ];
      pcounter++;
      }  
    itr.Value() = landMark;
    itr++;
    }

  this->m_SourceLandmarks->SetPoints( landmarks );

  // Modified is always called since we just have a pointer to the
  // parameters and cannot know if the parameters have changed.
  this->Modified();

}

// Set the fixed parameters
// Since the API of the SetParameters() function sets the
// source landmarks, this function was added to support the
// setting of the target landmarks, and allowing the Transform
// I/O mechanism to be supported.
template <class TScalarType, unsigned int NDimensions>
void
KernelTransform<TScalarType, NDimensions>::
SetFixedParameters( const ParametersType & parameters )
{
  typename PointsContainer::Pointer landmarks = PointsContainer::New();
  const unsigned int numberOfLandmarks =  parameters.Size() / NDimensions; 

  landmarks->Reserve( numberOfLandmarks );

  PointsIterator itr = landmarks->Begin();
  PointsIterator end = landmarks->End();

  InputPointType  landMark; 

  unsigned int pcounter = 0;
  while( itr != end )
    {
    for(unsigned int dim=0; dim<NDimensions; dim++)
      {
      landMark[ dim ] = parameters[ pcounter ];
      pcounter++;
      }  
    itr.Value() = landMark;
    itr++;
    }

  this->m_TargetLandmarks->SetPoints( landmarks );  
}


// Update parameters array
// They are the components of all the landmarks in the source space
template <class TScalarType, unsigned int NDimensions>
void
KernelTransform<TScalarType, NDimensions>::
UpdateParameters( void ) const
{
  this->m_Parameters = ParametersType( this->m_SourceLandmarks
                                           ->GetNumberOfPoints() 
                                       * NDimensions );

  PointsIterator itr = this->m_SourceLandmarks->GetPoints()->Begin();
  PointsIterator end = this->m_SourceLandmarks->GetPoints()->End();

  unsigned int pcounter = 0;
  while( itr != end )
    {
    InputPointType  landmark = itr.Value();
    for(unsigned int dim=0; dim<NDimensions; dim++)
      {
      this->m_Parameters[ pcounter ] = landmark[ dim ];
      pcounter++;
      }  
    itr++;
    }
}

// Get the parameters
// They are the components of all the landmarks in the source space
template <class TScalarType, unsigned int NDimensions>
const typename KernelTransform<TScalarType, NDimensions>::ParametersType &
KernelTransform<TScalarType, NDimensions>::
GetParameters( void ) const
{
  this->UpdateParameters();
  return this->m_Parameters;

}


// Get the fixed parameters
// This returns the target landmark locations 
// This was added to support the Transform Reader/Writer mechanism 
template <class TScalarType, unsigned int NDimensions>
const typename KernelTransform<TScalarType, NDimensions>::ParametersType &
KernelTransform<TScalarType, NDimensions>::
GetFixedParameters( void ) const
{
  this->m_FixedParameters = ParametersType( this->m_TargetLandmarks
                                                ->GetNumberOfPoints() 
                                            * NDimensions );
  
  PointsIterator itr = this->m_TargetLandmarks->GetPoints()->Begin();
  PointsIterator end = this->m_TargetLandmarks->GetPoints()->End();

  unsigned int pcounter = 0;
  while( itr != end )
    {
    InputPointType  landmark = itr.Value();
    for(unsigned int dim=0; dim<NDimensions; dim++)
      {
      this->m_FixedParameters[ pcounter ] = landmark[ dim ];
      pcounter++;
      }  
    itr++;
    }

  return this->m_FixedParameters;

}

template <class TScalarType, unsigned int NDimensions>
void
KernelTransform<TScalarType, NDimensions>::
PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  if( this->m_SourceLandmarks )
    {
    os << indent << "SourceLandmarks: " << std::endl;
    this->m_SourceLandmarks->Print(os,indent.GetNextIndent());
    }
  if( this->m_TargetLandmarks )
    {
    os << indent << "TargetLandmarks: " << std::endl;
    this->m_TargetLandmarks->Print(os,indent.GetNextIndent());
    }
  if( this->m_Displacements )
    {
    os << indent << "Displacements: " << std::endl;
    this->m_Displacements->Print(os,indent.GetNextIndent());
    }
  os << indent << "Stiffness: " << this->m_Stiffness << std::endl;
}
} // namespace itk

#endif
