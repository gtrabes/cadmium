/**
 * Copyright (c) 2020, Guillermo G. Trabes
 * Carleton University, Universidad Nacional de San Luis
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CADMIUM_PARALLEL_HELPERS_HPP
#define CADMIUM_PARALLEL_HELPERS_HPP

#include <vector>
#include <functional>
#include <chrono>
#include <omp.h>
#include <thread>

namespace cadmium {
    namespace parallel {

	    template< class InputIt, class UnaryFunction >
            void cpu_omp_parallel_for_each_v1(InputIt first, InputIt last, UnaryFunction& f, unsigned int thread_number = std::thread::hardware_concurrency()) {
			//#pragma omp parallel firstprivate(f, first, last) num_threads(thread_number) proc_bind(close)
			//{
			const size_t n = std::distance(first, last);

			//thread_number=6;

			#pragma omp parallel firstprivate(f, first) num_threads(thread_number) proc_bind(close)
    			{
    				int tid = omp_get_thread_num();
    				size_t P = thread_number;

    				if(tid != P-1) {
    					for(size_t i = (n/P) * tid; i < (n/P) * (tid+1) ; i++) {
    						f(*(i+first));
    					}
    				} else {
    					for(size_t i = (n/P) * tid; i < n ; i++) {
    						f(*(i+first));
    					}
    				}

/*			
				#pragma omp for schedule(static)
				//{
					for(size_t i = 0; i < n ; i++) {
						f(*(i+first));
					}
				//}
*/
    			}
    		}

    		void create_omp_threads(size_t thread_number = std::thread::hardware_concurrency()) {
				#pragma omp parallel num_threads(thread_number) proc_bind(close)
    			{
    			}
    		}

    		void destroy_omp_threads() {
    			//}
    		}

    		void begin_omp_sequential_section() {
    			//#pragma omp master
    		    //{
    		}

    		void end_omp_sequential_section() {
    			//}
    		}

    		void omp_thread_synchronization() {
				#pragma omp barrier
    		}

    		template< class InputIt, class UnaryFunction >
    		void cpu_omp_parallel_for_each_v2(InputIt first, InputIt last, UnaryFunction& f, size_t thread_number = std::thread::hardware_concurrency()) {

    			end_omp_sequential_section();
    			omp_thread_synchronization();

    			const size_t n = std::distance(first, last);
    		    int tid = omp_get_thread_num();
    		    size_t P = thread_number;

    		    if(tid != P-1) {
    		    	for(size_t i = (n/P) * tid; i < (n/P) * (tid+1) ; i++) {
    		    		f(*(i+first));
    		    	}
    		    } else {
    		    	for(size_t i = (n/P) * tid; i < n ; i++) {
    		    		f(*(i+first));
    		    	}
    		    }

    		    omp_thread_synchronization();
    		    begin_omp_sequential_section();

    		}

    		template<typename ITERATOR, typename FUNC>
    		void parallel_for_each_iterator(ITERATOR first, ITERATOR last, FUNC& f, unsigned int thread_number){
    			/* set number of threads */
    			//omp_set_num_threads(thread_number);
    			size_t n = std::distance(first, last);

    			#pragma omp parallel for num_threads(thread_number) firstprivate(f, first)
    			for(int i = 0; i < n; i++){
    				f(*(i+first));
    			}

    		//		#pragma omp parallel for firstprivate(f) shared(first,last)
    		//    	for (ITERATOR it = first; it != last; it++) {
    		//    		f(*it);
    		//    	}

    		//		#pragma omp parallel for firstprivate(f) shared(first)
    		//		for(size_t i = 0; i < n; i++){
    		//			auto& elem = *(first + i);
    					// do whatever you want with elem
    		//			f(elem);
    		//		}
    		}




    		template< class InputIt, class UnaryFunction >
    		void cpu_parallel_for_each(InputIt first, InputIt last, UnaryFunction& f, size_t thread_number = std::thread::hardware_concurrency()) {
			#ifdef CPU_PARALLEL
    			cpu_omp_parallel_for_each_v1(first, last, f, thread_number);
			#else
				#ifdef CPU_PARALLEL_V2
    				parallel_for_each_iterator(first, last, f, thread_number);
				#endif
			#endif
    		}

    }
}

#endif //CADMIUM_PARALLEL_HELPERS_HPP
