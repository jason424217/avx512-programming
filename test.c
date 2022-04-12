#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <inttypes.h>

inline int64_t lower_bound(int64_t* data, int64_t size, int64_t searchkey)
{
  /* this binary search variant
     (a) does only one comparison in the inner loop
     (b) doesn't require an exact match; instead it returns the index of the first key >= the search key.
         That's good in a DB context where we might be doing a range search, and using binary search to
	 identify the first key in the range.
     (c) If the search key is bigger than all keys, it returns size.
  */
  int64_t left=0;
  int64_t right=size;
  int64_t mid;
  
  while(left<right) {
    mid = (left + right)/2;   /* ignore possibility of overflow of left+right */
    if (data[mid]>=searchkey)
      right=mid;
    else
      left=mid+1;
  }
  return right;
}

int64_t lower_bound_nb_arithmetic(int64_t* data, int64_t size, int64_t searchkey)
{
  /* this binary search variant
     (a) does no comparisons in the inner loop by using multiplication and addition to convert control dependencies
         to data dependencies
     (b) doesn't require an exact match; instead it returns the index of the first key >= the search key.
         That's good in a DB context where we might be doing a range search, and using binary search to
	 identify the first key in the range.
     (c) If the search key is bigger than all keys, it returns size.
  */
  int64_t left=0;
  int64_t right=size;
  int64_t mid;
  //printf("left: %" PRId64 ", right: %" PRId64 "\n", left, right);
  while(left<right) {
    mid = (left + right)/2;   /* ignore possibility of overflow of left+right */

    /* YOUR CODE HERE */
    right = right * (data[mid]<searchkey) + mid * (data[mid]>=searchkey);
    left = left * (data[mid]>=searchkey) + (mid+1) * (data[mid]<searchkey);
    //printf("left: %" PRId64 ", right: %" PRId64 "\n", left, right);
  }
  return right;
}


int64_t lower_bound_nb_mask(int64_t* data, int64_t size, int64_t searchkey)
{
  /* this binary search variant
     (a) does no comparisons in the inner loop by using bit masking operations to convert control dependencies
         to data dependencies
     (b) doesn't require an exact match; instead it returns the index of the first key >= the search key.
         That's good in a DB context where we might be doing a range search, and using binary search to
	 identify the first key in the range.
     (c) If the search key is bigger than all keys, it returns size.
  */
  int64_t left=0;
  int64_t right=size;
  int64_t mid;
  printf("left: %" PRId64 ", right: %" PRId64 "\n", left, right);
  while(left<right) {
    mid = (left + right)/2;   /* ignore possibility of overflow of left+right */

    /* YOUR CODE HERE */
    right = right ^ ((right ^ mid) & (~((data[mid]>=searchkey)-1)));
    left = left ^ ((left ^ (mid+1)) & (~((data[mid]<searchkey)-1)));
    printf("left: %" PRId64 ", right: %" PRId64 "\n", left, right);
  }
  return right;
}


void lower_bound_nb_mask_8x(int64_t* data, int64_t size, int64_t* searchkey, int64_t* right)
{
  /* this binary search variant
     (a) does no comparisons in the inner loop by using bit masking operations instead
     (b) doesn't require an exact match; instead it returns the index of the first key >= the search key.
         That's good in a DB context where we might be doing a range search, and using binary search to
	 identify the first key in the range.
     (c) If the search key is bigger than all keys, it returns size.
     (d) does 8 searches at the same time in an interleaved fashion, so that an out-of-order processor
         can make progress even when some instructions are still waiting for their operands to be ready.

     Note that we're using the result array as the "right" elements in the search so no need for a return statement
  */
  int64_t left[8]={0,0,0,0,0,0,0,0};
  int64_t mid[8];
  right[0]=right[1]=right[2]=right[3]=right[4]=right[5]=right[6]=right[7]=size;

  while(left[0]<right[0] || left[1]<right[1] || left[2]<right[2] || left[3]<right[3] || 
  left[4]<right[4] || left[5]<right[5] || left[6]<right[6] || left[7]<right[7]){
    for(int i=0; i < 8; i++){
      mid[i] = (left[i] + right[i])/2;
      right[i] = right[i] ^ ((right[i] ^ mid[i]) & (~((data[mid[i]]>=searchkey[i])-1)));
      left[i] = left[i] ^ ((left[i] ^ (mid[i]+1)) & (~((data[mid[i]]<searchkey[i])-1)));
    }
  }
}

void main() {
    int64_t arr[] = {10, 20, 30, 50, 60, 80, 110, 130, 140, 170};
    int64_t n = sizeof(arr) / sizeof(arr[0]);
    int64_t x[8] = {8, 20, 29, 50, 51, 80, 110, 119};
    int64_t right[8]={0,0,0,0,0,0,0,0};
    lower_bound_nb_mask_8x(arr, n, x, right);
    for(int i = 0; i < 8; i++){
        printf("%" PRId64 "\t", right[i]);
    }
}