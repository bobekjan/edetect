/** @file
 * @brief Definition of CpuConvolutionFilter class.
 *
 * @author Jan Bobek
 */

#include "edetect.hxx"
#include "IImage.hxx"
#include "cpu/CpuConvolutionFilter.hxx"

/*************************************************************************/
/* CpuConvolutionFilter                                                  */
/*************************************************************************/
void
CpuConvolutionFilter::convolve(
    IImage& dest,
    const IImage& src
    )
{
    for( unsigned int row = 0; row < src.rows(); ++row )
        for( unsigned int col = 0; col < src.columns(); ++col )
        {
            const unsigned int cstart =
                (col < mRadius ? mRadius - col : 0);
            const unsigned int rstart =
                (row < mRadius ? mRadius - row : 0);

            const unsigned int cend =
                (src.columns() <= col + mRadius
                 ? src.columns() - col + mRadius - 1
                 : 2 * mRadius);
            const unsigned int rend =
                (src.rows() <= row + mRadius
                 ? src.rows() - row + mRadius - 1
                 : 2 * mRadius);

            float* dstp =
                (float*)(dest.data() + row * dest.stride())
                + col;
            const unsigned char* rowp = src.data()
                + (row - mRadius + rstart) * src.stride()
                + (col - mRadius + cstart) * sizeof(float);
            const float* colp;

            unsigned int i, j;
            float x = 0.0f;

            for( i = 0; i < rstart; ++i )
            {
                colp = (float*)rowp;
                for( j = 0; j < cstart; ++j )
                    x += *colp * mKernel[i * (2 * mRadius + 1) + j];
                for(; j < cend; ++j, ++colp )
                    x += *colp * mKernel[i * (2 * mRadius + 1) + j];
                for(; j <= 2 * mRadius; ++j )
                    x += *colp * mKernel[i * (2 * mRadius + 1) + j];
            }
            for(; i < rend; ++i, rowp += src.stride() )
            {
                colp = (float*)rowp;
                for( j = 0; j < cstart; ++j )
                    x += *colp * mKernel[i * (2 * mRadius + 1) + j];
                for(; j < cend; ++j, ++colp )
                    x += *colp * mKernel[i * (2 * mRadius + 1) + j];
                for(; j <= 2 * mRadius; ++j )
                    x += *colp * mKernel[i * (2 * mRadius + 1) + j];
            }
            for(; i <= 2 * mRadius; ++i )
            {
                colp = (float*)rowp;
                for( j = 0; j < cstart; ++j )
                    x += *colp * mKernel[i * (2 * mRadius + 1) + j];
                for(; j < cend; ++j, ++colp )
                    x += *colp * mKernel[i * (2 * mRadius + 1) + j];
                for(; j <= 2 * mRadius; ++j )
                    x += *colp * mKernel[i * (2 * mRadius + 1) + j];
            }

            *dstp = x;
        }
}

/*************************************************************************/
/* CpuRowConvolutionFilter                                               */
/*************************************************************************/
void
CpuRowConvolutionFilter::convolve(
    IImage& dest,
    const IImage& src
    )
{
    for( unsigned int row = 0; row < src.rows(); ++row )
        for( unsigned int col = 0; col < src.columns(); ++col )
        {
            const unsigned int start =
                (col < mRadius ? mRadius - col : 0);
            const unsigned int end =
                (src.columns() <= col + mRadius
                 ? src.columns() - col + mRadius - 1
                 : 2 * mRadius);

            float* dstp =
                (float*)(dest.data() + row * dest.stride())
                + col;
            const float* srcp =
                (const float*)(src.data() + row * src.stride())
                + (col - mRadius + start);

            unsigned int k;
            float x = 0.0f;

            for( k = 0; k < start; ++k )
                x += *srcp * mKernel[k];
            for(; k < end; ++k, ++srcp )
                x += *srcp * mKernel[k];
            for(; k <= 2 * mRadius; ++k )
                x += *srcp * mKernel[k];

            *dstp = x;
        }
}

/*************************************************************************/
/* CpuColumnConvolutionFilter                                            */
/*************************************************************************/
void
CpuColumnConvolutionFilter::convolve(
    IImage& dest,
    const IImage& src
    )
{
    for( unsigned int row = 0; row < src.rows(); ++row )
        for( unsigned int col = 0; col < src.columns(); ++col )
        {
            const unsigned int start =
                (row < mRadius ? mRadius - row : 0);
            const unsigned int end =
                (src.rows() <= row + mRadius
                 ? src.rows() - row + mRadius - 1
                 : 2 * mRadius);

            float* dstp =
                (float*)(dest.data() + row * dest.stride())
                + col;
            const unsigned char* srcp = src.data()
                + (row - mRadius + start) * src.stride()
                + col * sizeof(float);

            unsigned int i;
            float x = 0.0f;

            for( i = 0; i < start; ++i )
                x += *(float*)srcp * mKernel[i];
            for(; i < end; ++i, srcp += src.stride() )
                x += *(float*)srcp * mKernel[i];
            for(; i <= 2 * mRadius; ++i )
                x += *(float*)srcp * mKernel[i];

            *dstp = x;
        }
}
