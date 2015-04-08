/** @file
 * @brief Definition of CudaDesaturateFilter class.
 *
 * @author Jan Bobek
 */

#include "edetect.hxx"
#include "cuda/CudaDesaturateFilter.hxx"
#include "cuda/CudaError.hxx"
#include "cuda/CudaImage.hxx"

/**
 * @brief CUDA kernel for desaturation using
 *   the Average method.
 *
 * @param[out] dst
 *   The destination image data.
 * @param[in] dstStride
 *   Size of the row stride in destination data.
 * @param[in] src
 *   The source image data.
 * @param[in] srcStride
 *   Size of the row stride in source data.
 * @param[in] rows
 *   Number of rows in the image.
 * @param[in] cols
 *   Number of columns in the image.
 */
__global__ void
desaturateAverage(
    unsigned char* dst,
    size_t dstStride,
    const unsigned char* src,
    size_t srcStride,
    size_t rows,
    size_t cols
    )
{
    const size_t col =
        blockIdx.x * blockDim.x + threadIdx.x;
    const size_t row =
        blockIdx.y * blockDim.y + threadIdx.y;

    if( row < rows && col < cols )
    {
        float* const dstp =
            (float*)(dst + row * dstStride) + col;
        const float3* const srcp =
            (const float3*)(src + row * srcStride) + col;

        *dstp = (srcp->x + srcp->y + srcp->z) / 3.0f;
    }
}

/**
 * @brief CUDA kernel for desaturation using
 *   the Lightness method.
 *
 * @param[out] dst
 *   The destination image data.
 * @param[in] dstStride
 *   Size of the row stride in destination data.
 * @param[in] src
 *   The source image data.
 * @param[in] srcStride
 *   Size of the row stride in source data.
 * @param[in] rows
 *   Number of rows in the image.
 * @param[in] cols
 *   Number of columns in the image.
 */
__global__ void
desaturateLightness(
    unsigned char* dst,
    size_t dstStride,
    const unsigned char* src,
    size_t srcStride,
    size_t rows,
    size_t cols
    )
{
    const size_t col =
        blockIdx.x * blockDim.x + threadIdx.x;
    const size_t row =
        blockIdx.y * blockDim.y + threadIdx.y;

    if( row < rows && col < cols )
    {
        float* const dstp =
            (float*)(dst + row * dstStride) + col;
        const float3* const srcp =
            (const float3*)(src + row * srcStride) + col;

        const float a = fminf( srcp->x, srcp->y );
        const float b = fmaxf( srcp->x, srcp->y );
        const float c = fminf( a, srcp->z );
        const float d = fmaxf( b, srcp->z );

        *dstp = 0.5f * (c + d);
    }
}

/**
 * @brief CUDA kernel for desaturation using
 *   the Luminosity method.
 *
 * @param[out] dst
 *   The destination image data.
 * @param[in] dstStride
 *   Size of the row stride in destination data.
 * @param[in] src
 *   The source image data.
 * @param[in] srcStride
 *   Size of the row stride in source data.
 * @param[in] rows
 *   Number of rows in the image.
 * @param[in] cols
 *   Number of columns in the image.
 */
__global__ void
desaturateLuminosity(
    unsigned char* dst,
    size_t dstStride,
    const unsigned char* src,
    size_t srcStride,
    size_t rows,
    size_t cols
    )
{
    const size_t col =
        blockIdx.x * blockDim.x + threadIdx.x;
    const size_t row =
        blockIdx.y * blockDim.y + threadIdx.y;

    if( row < rows && col < cols )
    {
        float* const dstp =
            (float*)(dst + row * dstStride) + col;
        const float3* const srcp =
            (const float3*)(src + row * srcStride) + col;

        *dstp =
            /* z:RED y:GREEN x:BLUE */
            0.2126f * srcp->z
            + 0.7152f * srcp->y
            + 0.0722f * srcp->x;
    }
}

/*************************************************************************/
/* CudaDesaturateFilter                                                  */
/*************************************************************************/
void
CudaDesaturateFilter::filter(
    CudaImage& image
    )
{
    switch( image.format() )
    {
    case Image::FMT_GRAY_UINT8:
    case Image::FMT_GRAY_FLOAT32:
        fputs( "CudaDesaturateFilter: Image already in grayscale\n", stderr );
        return;

    case Image::FMT_RGB_FLOAT32:
        break;

    default:
    case Image::FMT_RGB_UINT8:
        throw std::runtime_error(
            "CudaDesaturateFilter: Unsupported image format" );
    }

    // 32 = warp size, 8 * 32 = 256 threads
    const dim3 threadsPerBlock(32, 8);
    const dim3 numBlocks(
        (image.columns() + threadsPerBlock.x - 1) / threadsPerBlock.x,
        (image.rows() + threadsPerBlock.y - 1) / threadsPerBlock.y );

    CudaImage newImage;
    newImage.reset( image.rows(), image.columns(),
                    Image::FMT_GRAY_FLOAT32 );

    switch( mMethod )
    {
    case METHOD_AVERAGE:
        desaturateAverage<<< numBlocks, threadsPerBlock >>>(
            newImage.data(), newImage.stride(),
            image.data(), image.stride(),
            image.rows(), image.columns() );
        break;

    case METHOD_LIGHTNESS:
        desaturateLightness<<< numBlocks, threadsPerBlock >>>(
            newImage.data(), newImage.stride(),
            image.data(), image.stride(),
            image.rows(), image.columns() );
        break;

    case METHOD_LUMINOSITY:
        desaturateLuminosity<<< numBlocks, threadsPerBlock >>>(
            newImage.data(), newImage.stride(),
            image.data(), image.stride(),
            image.rows(), image.columns() );
        break;
    }

    cudaCheckLastError( "Desaturation kernel launch failed" );
    cudaMsgCheckError( cudaDeviceSynchronize(), "Desaturation kernel run failed" );

    image.swap( newImage );
}

void
CudaDesaturateFilter::setParam(
    const char* name,
    const void* value
    )
{
    if( !strcmp( name, "method" ) )
    {
        const char* strval = (const char*)value;

        if( !strcmp( strval, "average" ) )
            mMethod = METHOD_AVERAGE;
        else if( !strcmp( strval, "lightness" ) )
            mMethod = METHOD_LIGHTNESS;
        else if( !strcmp( strval, "luminosity" ) )
            mMethod = METHOD_LUMINOSITY;
        else
            throw std::invalid_argument(
                "CudaDesaturateFilter: Method not implemented" );
    }
    else
        IImageFilter::setParam( name, value );
}