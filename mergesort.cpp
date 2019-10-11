#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <omp.h>
#include <chrono>

#ifdef __cplusplus
extern "C" {
#endif
  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (int* arr, size_t n);
#ifdef __cplusplus
}
#endif

using namespace std;
 //basic merge function
void merge(int* arr, int l, int m, int r)
{
    int i, j, k;
    int index1 = m - l + 1;
    int index2 =  r - m;
    int* array1 = new int[index1]; //temporary array
    int* array2 = new int[index2]; //temporary array

    for (i = 0; i < index1; i++)
        array1[i] = arr[l + i];//populating the temporary arrays 
    for (j = 0; j < index2; j++)
       array2[j] = arr[m + 1+ j];

    i = 0; // initial index of first temp sub array
    j = 0; // initial  index of second temp sub array
    k = l; // Initial index of merged array
    while (i < index1 && j < index2)
    {
        if (array1[i] <= array2[j])
        {
            arr[k] = array1[i];
            i++;
        }
        else
        {
            arr[k] = array2[j];
            j++;
        }
        k++;
    }

    
    while (i < index1)  //copy the remaining elements
    {
        arr[k] = array1[i];
        i++;
        k++;
    }

    
    while (j < index2)
    {
        arr[k] = array2[j];
        j++;
        k++;
    }
 //releasing memory
    delete[] array1;
    delete[] array2;
}

void mergeSort(int* arr, int l, int r, int numThreads)
{
   //setting number of threads
    omp_set_num_threads(numThreads);

    int n= r+1;

    for(int x = 1; x < n;x *= 2)  
    {
        #pragma omp parallel for schedule(static, 1) //setting type of schedule
        for(int index3=0;index3 < n;index3 += (2*x))
        {
            int start = index3;
            int end = index3 + ((2*x)-1);
            int mid = index3 + (x-1);
           
            if(mid >= n)
            {
                mid = (index3+n-1)/2;
                end = n-1;
            }
            else if(end >= n)
            {
                end = n-1;
            }
            merge(arr,start,mid,end);
        }
    }
}





int main (int argc, char* argv[]) {

  //forces openmp to create the threads beforehand
#pragma omp parallel
  {
    int fd = open (argv[0], O_RDONLY);
    if (fd != -1) {
      close (fd);
    }
    else {
      std::cerr<<"something is amiss"<<std::endl;
    }
  }

  if (argc < 3) { std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int n = atoi(argv[1]);
  int numOfThreads = atoi(argv[2]);
  int * arr = new int [n];

  generateMergeSortData (arr, n); //generating data


  //insert sorting code here.
  // start timing
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

  mergeSort(arr, 0, n-1, numOfThreads); /calling function

  // end time
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapased_seconds = end-start;
  //priting the time taken in the error stream
  cerr<<elapased_seconds.count()<<std::endl;
  checkMergeSortResult (arr, n);

  delete[] arr;

  return 0;
}

