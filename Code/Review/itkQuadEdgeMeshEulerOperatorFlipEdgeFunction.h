/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkQuadEdgeMeshEulerOperatorFlipEdgeFunction.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkQuadEdgeMeshEulerOperatorFlipEdgeFunction_h
#define __itkQuadEdgeMeshEulerOperatorFlipEdgeFunction_h

#include "itkQuadEdgeMeshFunctionBase.h"

namespace itk
{
/**
 * \class EulerOperatorFlipEdgeFunction
 * \ingroup QEMeshModifierFunctions
 *
 * \brief Flip an edge.
 *
 * The original FlipEdge operator required both faces of the input edge
 * to be triangles (and to be set). This version does not have such requirement.
 * Either or both faces can be polygonal, the org and dest of the edge
 * is then "rotated" around the big polygon that would exist if the two faces 
 * of the edge e were joined.
 * \image html EulerOperatorFlipEdgeWidth600.png "Swapping the h edge"
 */
template < class TMesh, class TQEType >
class ITK_EXPORT QuadEdgeMeshEulerOperatorFlipEdgeFunction :
   public QuadEdgeMeshFunctionBase< TMesh, TQEType* >
{
public:
  /** Standard class typedefs. */
  typedef QuadEdgeMeshEulerOperatorFlipEdgeFunction       Self;
  typedef QuadEdgeMeshFunctionBase< TMesh, TQEType* >     Superclass;
  typedef SmartPointer< Self >                            Pointer;
  typedef SmartPointer< const Self >                      ConstPointer;
     
  itkNewMacro( Self );
  /** Run-time type information (and related methods). */
  itkTypeMacro( QuadEdgeMeshEulerOperatorFlipEdgeFunction,
    QuadEdgeMeshFunctionBase );

  /** Type of QuadEdge with which to apply slicing. */
  typedef TQEType QEType;

  typedef typename Superclass::MeshType   MeshType;
  typedef typename Superclass::OutputType OutputType;

  enum EdgeStatusType
  {
    STANDARD_CONFIG = 0,
    EDGE_NULL,
    MESH_NULL,
    NON_INTERNAL_EDGE,
    NON_TRIANGULAR_RIGHT_FACE,
    NON_TRIANGULAR_LEFT_FACE,
    EXISTING_OPPOSITE_EDGE
  };

  /** Evaluate at the specified input position */
  virtual OutputType Evaluate( QEType* h );

  // itkGetConstMacro( EdgeStatus, EdgeStatusType );

protected:
  QuadEdgeMeshEulerOperatorFlipEdgeFunction();
  ~QuadEdgeMeshEulerOperatorFlipEdgeFunction(){}

  void PrintSelf(std::ostream& os, Indent indent) const;
  EdgeStatusType m_EdgeStatus;

  void CheckStatus( QEType* h );

  OutputType Process( QEType* h );

private:
  //purposely not implemented
  QuadEdgeMeshEulerOperatorFlipEdgeFunction(const Self& );
  //purposely not implemented
  void operator=(const Self& );

};

} // namespace itkQE

#include "itkQuadEdgeMeshEulerOperatorFlipEdgeFunction.txx"

#endif

// eof - itkQuadEdgeMeshEulerOperatorFlipEdgeFunction.h
