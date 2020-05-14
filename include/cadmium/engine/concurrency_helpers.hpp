/**
 * Copyright (c) 2019, Juan Lanuza
 * Carleton University, Universidad de Buenos Aires
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

#ifndef CADMIUM_CONCURRENCY_HELPERS_HPP
#define CADMIUM_CONCURRENCY_HELPERS_HPP

#include <vector>
#include <functional>
#include <future>
#include <chrono>
#include <omp.h>

#include <boost/thread/executors/basic_thread_pool.hpp>

namespace cadmium {
    namespace concurrency {
        /*
         * for_each that runs using a thread_pool (assumed without running tasks),
         * and waits por all tasks to finish until it returns
         */

/*
        template<typename ITERATOR, typename FUNC>
        void concurrent_for_each(boost::basic_thread_pool& threadpool, int thread_number, ITERATOR first, ITERATOR last, FUNC& f) {
          std::vector<std::future<void> > task_statuses;

          for (ITERATOR it = first; it != last; it++) {
              std::packaged_task<void()> task(std::bind<void>(f, *it));
              task_statuses.push_back(task.get_future());

              threadpool.submit(std::move(task));
          }
          auto future_ready = [](auto& f) -> bool { return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready; };
          while(! std::all_of(task_statuses.begin(), task_statuses.end(), future_ready) ){
              // if there are tasks in the threadpool queue, the main thread executes one
              threadpool.schedule_one_or_yield();
          }
          //when concurrent_for_each end threadpool queue is empty
        }
*/


    	template<typename ITERATOR, typename FUNC>
    	void concurrent_for_each(boost::basic_thread_pool& threadpool, int thread_number, ITERATOR first, ITERATOR last, FUNC& f) {
//    		int size = obj.size();
    		const size_t n = std::distance(first, last);
    		/* set number of threads */
    		omp_set_num_threads(thread_number);

/*
			#pragma omp parallel for private(f) shared(obj)
    			for (int i = 0; i < size; i++) {
    				f(obj[i]);
    			}
*/


//    		#pragma omp parallel for firstprivate(f) shared(first,last)
//    		for (ITERATOR it = first; it != last; it++) {
//    			f(*it);
//    		}

			#pragma omp parallel for firstprivate(f)
    			for(size_t i = 0; i < n; i++) {
    				auto& elem = *(first + i);
    				// do whatever you want with elem
    				f(elem);
    			}
    	}


    	template<class T, class Function>
    	void parallel_for_each(int thread_number, std::vector<T> & obj, Function f){
    		/* set number of threads */
    		omp_set_num_threads(thread_number);
    		int size = obj.size();
//    		double result;
    		#pragma omp parallel for firstprivate(f) shared(obj)
    			for (int i = 0; i < size; i++){
    				f(obj[i]);
    			}
    	//	cout << "Aca parallel after loop" << result << '\n';
    	}


    	template< class InputIt, class UnaryFunction >
    	void omp_for_each(int thread_number, InputIt first, InputIt last, UnaryFunction f) {
    		#pragma omp parallel for num_threads(thread_number) schedule(static)
    		for(InputIt iter = first; iter < last; ++iter) {
    			f(*iter);
    		}
    	}


    }
}

#endif //CADMIUM_CONCURRENCY_HELPERS_HPP
