/** @file
 * @brief Declaration of CpuSobelOperatorFilter class.
 *
 * @author Jan Bobek
 */

#ifndef CPU__CPU_SOBEL_OPERATOR_FILTER_HXX__INCL__
#define CPU__CPU_SOBEL_OPERATOR_FILTER_HXX__INCL__

#include "ISobelOperatorFilter.hxx"
#include "cpu/CpuConvolution2dSeparableFilter.hxx"

/**
 * @brief Applies Sobel operator to the image.
 *
 * @author Jan Bobek
 */
class CpuSobelOperatorFilter
: public ISobelOperatorFilter<
    CpuConvolution2dSeparableFilter >
{
protected:
    /// @copydoc ISobelOperatorFilter< F >::computeGradient(IImage&, const IImage&)
    void computeGradient( IImage& vert, const IImage& horz );
};

#endif /* !CPU__CPU_SOBEL_OPERATOR_FILTER_HXX__INCL__ */
