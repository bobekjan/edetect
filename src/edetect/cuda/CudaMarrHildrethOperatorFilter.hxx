/** @file
 * @brief Declaration of CudaMarrHildrethOperatorFilter class.
 *
 * @author Jan Bobek
 */

#ifndef CUDA__CUDA_MARR_HILDRETH_OPERATOR_FILTER_HXX__INCL__
#define CUDA__CUDA_MARR_HILDRETH_OPERATOR_FILTER_HXX__INCL__

#include "IMarrHildrethOperatorFilter.hxx"
#include "cuda/CudaConvolutionFilter.hxx"
#include "cuda/CudaZeroCrossFilter.hxx"

/**
 * @brief Applies Marr-Hildreth operator to the image.
 *
 * @author Jan Bobek
 */
class CudaMarrHildrethOperatorFilter
: public IMarrHildrethOperatorFilter<
    CudaConvolutionFilter,
    CudaZeroCrossFilter >
{
protected:
    /// @copydoc IMarrHildrethOperatorFilter< CF, ZCF >::mergeEdges(IImage&, const IImage&)
    void mergeEdges( IImage& dest, const IImage& src );
};

#endif /* !CUDA__CUDA_MARR_HILDRETH_OPERATOR_FILTER_HXX__INCL__ */
