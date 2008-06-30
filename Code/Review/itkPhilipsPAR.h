/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkPhilipsPAR.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

/**
 * \file   The code for this file reader was written based on
 *         examination of Philips REC/PAR image files acquired at the
 *         Center for NMR Research at the Penn State Milton S. Hershey
 *         Medical Center.
 *
 *
 * \author Don C. Bigler
 *         The Pennsylvania State University 2005
 *
 * This implementation was contributed as a paper to the Insight Journal
 * http://insight-journal.org/midas/handle.php?handle=1926/1381
 *
 */

#ifndef __itkPhilipsPAR_h
#define __itkPhilipsPAR_h

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "itkVectorContainer.h"
#include "vnl/vnl_vector_fixed.h"

#define RESEARCH_IMAGE_EXPORT_TOOL_V3  3
#define RESEARCH_IMAGE_EXPORT_TOOL_V4  4
#define RESEARCH_IMAGE_EXPORT_TOOL_V4_1  41

#define PAR_DEFAULT_STRING_LENGTH      32
#define PAR_DEFAULT_TRIGGER_TIMES_SIZE    128
#define PAR_DEFAULT_ECHO_TIMES_SIZE      128
#define PAR_DEFAULT_REP_TIMES_SIZE      128
#define PAR_DEFAULT_IMAGE_TYPES_SIZE    8
#define PAR_DEFAULT_SCAN_SEQUENCE_SIZE    8
#define PAR_RESCALE_VALUES_SIZE        3
#define PAR_DIFFUSION_VALUES_SIZE      3

#define PAR_SLICE_ORIENTATION_TRANSVERSAL  1
#define PAR_SLICE_ORIENTATION_SAGITTAL    2
#define PAR_SLICE_ORIENTATION_CORONAL    3

namespace itk
{

/**
 * \struct par_parameter
 */
struct par_parameter  //par_parameter
{
  int problemreading; // Marked 1 if problem occurred reading in PAR file
  int ResToolsVersion; // V3, V4, or V4.1 PAR/REC version
  char patient_name[PAR_DEFAULT_STRING_LENGTH]; // Patient name
  char exam_name[PAR_DEFAULT_STRING_LENGTH]; // Examination name
  char protocol_name[PAR_DEFAULT_STRING_LENGTH]; // Protocol name
  char exam_date[PAR_DEFAULT_STRING_LENGTH]; // Examination date/time
  char exam_time[PAR_DEFAULT_STRING_LENGTH]; // Examination date/time
  char series_type[PAR_DEFAULT_STRING_LENGTH]; // Series Type
  int scno; // Acquisition nr
  int recno; // Reconstruction nr
  int scan_duration; // Scan Duration [sec]
  int cardiac_phases; // Max. number of cardiac phases
  float trigger_times[PAR_DEFAULT_TRIGGER_TIMES_SIZE]; // trigger_time (float)
  int echoes; // Max. number of echoes
  float echo_times[PAR_DEFAULT_ECHO_TIMES_SIZE]; // Echo times read from PAR file
  int slice; // Max. number of slices/locations
  int dyn; // Max. number of dynamics
  int mixes; // Max. number of mixes
  char patient_position[PAR_DEFAULT_STRING_LENGTH]; // Patient position
  char prep_direction[PAR_DEFAULT_STRING_LENGTH]; // Preparation direction
  short int bit; // Image pixel size [8 or 16 bits]
  char technique[PAR_DEFAULT_STRING_LENGTH]; // Technique
  char scan_mode[PAR_DEFAULT_STRING_LENGTH]; // Scan mode
  int num_averages; // Number of averages
  int scan_resolution[2]; // Scan resolution  (x, y)
  int scan_percent; // Scan percentage
  int dim[3]; // Recon resolution (x, y) + slices (z)
  float repetition_time[PAR_DEFAULT_REP_TIMES_SIZE]; // Repetition time [msec]
  int sliceorient; // slice orientation ( TRA/SAG/COR ) (integer)
  float slth; // Slice thickness [mm]
  float gap; // Slice gap [mm]
  float fov[3]; // FOV (ap,fh,rl) [mm]
  float water_fat_shift; // Water Fat shift [pixels]
  float angAP; // Angulation midslice(ap,fh,rl)[degr]
  float angFH; // Angulation midslice(ap,fh,rl)[degr]
  float angRL; // Angulation midslice(ap,fh,rl)[degr]
  float offAP; // Off Centre midslice(ap,fh,rl) [mm]
  float offFH; // Off Centre midslice(ap,fh,rl) [mm]
  float offRL; // Off Centre midslice(ap,fh,rl) [mm]
  int flow_comp; // Flow compensation <0=no 1=yes> ?
  int presaturation; // Presaturation     <0=no 1=yes> ?
  int cardiac_freq; // Cardiac frequency
  int min_rr_int; // Min. RR interval
  int max_rr_int; // Max. RR interval
  float phase_encode_vel[3]; // Phase encoding velocity [cm/sec]
  int mtc; // MTC               <0=no 1=yes> ?
  int spir; // SPIR              <0=no 1=yes> ?
  int epi; // EPI factor        <0,1=no EPI>
  int turbo; // TURBO factor      <0=no turbo>
  int dynamic_scan; // Dynamic scan      <0=no 1=yes> ?
  int diffusion; // Diffusion         <0=no 1=yes> ?
  float diff_echo; // Diffusion echo time [msec]
  int max_num_diff_vals; // Max. number of diffusion values
  int max_num_grad_orient; // Max. number of gradient orients
  float inversion_delay; // Inversion delay [msec]
  float vox[3]; // pixel spacing (x,y) (in mm)
  int slicessorted; // 1-slices sorted, 0-slices not sorted
  int image_blocks; // The total number of image blocks stored in the REC file
  int num_image_types; // The number of image types in the REC file
  int image_types[PAR_DEFAULT_IMAGE_TYPES_SIZE]; // The different image types 
                                                 // detected in the REC
  int num_scanning_sequences; // The number of scanning sequences in the REC file
  int scanning_sequences[PAR_DEFAULT_SCAN_SEQUENCE_SIZE]; // The different 
                                                          // scanning sequences 
                                                          // detected in the REC
  int num_slice_repetitions; // If num_scanning_sequences > 1 then 
                  // num_image_types may not equal the total number of slice 
                  // repetitions for a single acquisition.  This value is the 
                  // total number of slice repetitions for a single acquisition 
                  // and is valid only when slicessorted == 0.

};

// Reads the PAR file parameters in "parFile" and stores the PAR parameters in 
// pPar.
// Returns false if an error is encountered during reading, otherwise true is 
// returned.
ITK_EXPORT bool ReadPAR(std::string parFile, struct par_parameter* pPar);

// Returns a vector of paired values, the first contains the slice index and the 
// second is the image type for the PAR file "parFile".
typedef std::pair< int, int >                 PARSliceIndexImageType;
typedef std::vector< PARSliceIndexImageType > PARSliceIndexImageTypeVector;
ITK_EXPORT PARSliceIndexImageTypeVector GetRECSliceIndexImageTypes(
  std::string parFile);

// Returns a vector of paired values, the first contains the slice index and the 
// second is the scan sequence for the PAR file "parFile".
typedef std::pair< int, int >                    PARSliceIndexScanSequence;
typedef std::vector< PARSliceIndexScanSequence > PARSliceIndexScanSequenceVector;
ITK_EXPORT PARSliceIndexScanSequenceVector GetRECSliceIndexScanningSequence(
  std::string parFile);

// Returns a vector of paired values, the first contains the image type and the 
// second is the scan sequence for that image type for the PAR file "parFile".
typedef std::pair< int, int >                   PARImageTypeScanSequence;
typedef std::vector< PARImageTypeScanSequence > PARImageTypeScanSequenceVector;
ITK_EXPORT PARImageTypeScanSequenceVector GetImageTypesScanningSequence(
  std::string parFile);

// Stores rescale values in the VectorContainer "rescaleValues" for each image 
// type of the specified scan sequence number "scan_sequence" (from 
// scanning_sequences) for the PAR file "parFile".
// Returns false if an error is encountered during reading, otherwise true is 
// returned.
typedef vnl_vector_fixed< double, PAR_RESCALE_VALUES_SIZE > 
PARRescaleValues;
typedef VectorContainer< unsigned int, PARRescaleValues > 
PARRescaleValuesContainer;
ITK_EXPORT bool GetRECRescaleValues(std::string parFile,
  PARRescaleValuesContainer *rescaleValues, int scan_sequence);

// Stores the diffusion gradient values in the VectorContainer "gradientValues" 
// and the diffusion b values in the VectorContainer "bValues" for each gradient 
// direction in the PAR file "parFile".
// Returns false if an error is encountered during reading, otherwise true is 
// returned.
typedef vnl_vector_fixed< double, PAR_DIFFUSION_VALUES_SIZE > 
PARDiffusionValues;
typedef VectorContainer< unsigned int, PARDiffusionValues >
PARDiffusionValuesContainer;
typedef VectorContainer< unsigned int, double > 
PARBValuesContainer;
ITK_EXPORT bool GetDiffusionGradientOrientationAndBValues(std::string parFile,
  PARDiffusionValuesContainer *gradientValues, PARBValuesContainer *bValues);

} // end namespace itk

#endif                           /* __itkPhilipsPAR_h */