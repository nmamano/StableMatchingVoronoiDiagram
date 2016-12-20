/*
Copyright 2011 Erik Gorset. All rights reserved.
Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:
   1. Redistributions of source code must retain the above copyright notice, this list of
      conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, this list
      of conditions and the following disclaimer in the documentation and/or other materials
      provided with the distribution.
      THIS SOFTWARE IS PROVIDED BY Erik Gorset ``AS IS'' AND ANY EXPRESS OR IMPLIED
      WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
      FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Erik Gorset OR
      CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
      CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
      SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
      ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
      NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
      ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
      The views and conclusions contained in the software and documentation are those of the
      authors and should not be interpreted as representing official policies, either expressed
      or implied, of Erik Gorset.
*/

#ifndef RADIXSORT_H
#define RADIXSORT_H

#include <vector>
#include "matchingutils.h"

using namespace std;

void radix_sort(vector<int> &array);
void radix_sort(vector<int> &array, int offset, int end, int shift);
void insertion_sort(vector<int> &array, int offset, int end);

void radix_sort(vector<IdIntDist> &array);
void radix_sort(vector<IdIntDist> &array, int offset, int end, int shift);
void insertion_sort(vector<IdIntDist> &array, int offset, int end);

#endif // RADIXSORT_H
