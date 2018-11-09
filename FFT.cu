//#include "Field.hpp"

//#include "cuda_runtime.h"
//#include "device_launch_parameters.h"

//we are computing FFT of the size 2**N 
#define N 1024
#define LOG_N 10

/*using namespace arithmetic;

std::initializer_list<uint_default_t> p = { 0x2370fb049d410fbe, 0x4e761a9886e50241, 0x7d023f4018000001, 0x7e80600000000001 };
static const bignum<MAX_BITSIZE> modulus(p);

using field = Field<MAX_BITSIZE, modulus>;

__constant__ field dev_roots_of_unity[LOG_N];

__global__ void FFTKernel(field* input_buf, field* output_buf);

int main()
{
	//first find suitable Cuda device
	//TBD: or split between several CUDA devices if possible
	int device_count;
	cudaError_t cudaStatus = cudaGetDeviceCount(&device_count);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaGetDeviceCount failed!");
		return 1;
	}
	if (device_count == 0)
	{
		fprintf(stderr, "No suitable CUDA devices were found!");
		return 1;
	}

	cudaDeviceProp prop;
	cudaStatus = cudaGetDeviceProperties(&prop, 0);

	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaGetDeviceCount failed!");
		return 1;
	}

	//TODO: check if there are enough constant memory and other additional checks
	//set appropriate device
	// Choose which GPU to run on, change this on a multi-GPU system.
	cudaStatus = cudaSetDevice(0);
	if (cudaStatus != cudaSuccess)
	{
		fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
		goto Error;
	}

	field input[N];
	field output[N];

	//we are goint to calculate roots of unity in corresponding powers
	field roots_of_unity[LOG_N];
	//TODO: compute them in advance before the kernel starts

	for (size_t i = 0; i < N; i++)
		input[i] = field::random();

	field* dev_input = nullptr;
	field* dev_output = nullptr;

	// Allocate GPU buffers for three vectors (one input, one output)    .
	cudaStatus = cudaMalloc((void**)&dev_input, N * sizeof(field));
	if (cudaStatus != cudaSuccess)
	{
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&dev_output, N * sizeof(field));
	if (cudaStatus != cudaSuccess)
	{
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	// Copy input vector from host memory to GPU buffers.
	cudaStatus = cudaMemcpy(dev_input, input, N * sizeof(field), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess)
	{
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	//copy precomputed roots of unity to constant memory
	cudaMemcpyToSymbol(dev_roots_of_unity, roots_of_unity, LOG_N * sizeof(field));

	// Launch a kernel on the GPU with one thread for each element.
	FFTKernel << <1, size >> > (dev_input, dev_output);

	// Check for any errors launching the kernel
	cudaStatus = cudaGetLastError();
	if (cudaStatus != cudaSuccess)
	{
		fprintf(stderr, "FFTKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
		goto Error;
	}

	// cudaDeviceSynchronize waits for the kernel to finish, and returns
	// any errors encountered during the launch.
	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess)
	{
		fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
		goto Error;
	}

	// Copy output vector from GPU buffer to host memory.
	cudaStatus = cudaMemcpy(output, dev_output, N * sizeof(field), cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess)
	{
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

Error:
	cudaFree(dev_input);
	cudaFree(dev_output);

	return 0;
}

//NB: we have precomputed the powers of roots of unity and put them into constant memory
__global__ void FFTKernel(const field* input_buf, field* output_buf)
{
	field temp_buf[N];

	const field* in_buf = input_buf;
	field* out_buf = (LOG_N % 2 ? output_buf : temp_buf);

	int thread_idx = threadIdx.x + blockIdx.x * blockDim.x;

	for (size_t i = 0; i < LOG_N; i++)
	{
		int idx = thread_idx;
		while (idx < N)
		{
			idx += blockDim.x * gridDim.x;
		}
	}
}*/



