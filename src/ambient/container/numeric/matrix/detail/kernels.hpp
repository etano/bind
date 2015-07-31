/*
 * Copyright Institute for Theoretical Physics, ETH Zurich 2015.
 * Distributed under the Boost Software License, Version 1.0.
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef AMBIENT_CONTAINER_NUMERIC_MATRIX_DETAIL_KERNELS
#define AMBIENT_CONTAINER_NUMERIC_MATRIX_DETAIL_KERNELS

#include "ambient/container/numeric/matrix/detail/math.hpp"
#include "ambient/container/numeric/matrix/detail/utils.hpp"
#include "ambient/container/numeric/traits.hpp"
#include "ambient/container/numeric/bindings/mkl.hpp"
#include "ambient/container/numeric/bindings/plasma.hpp"

#include "utils/export.hpp"

namespace ambient { namespace numeric { namespace kernels {
    namespace detail {

        using ambient::numeric::matrix;
        using ambient::numeric::traits::real_type;
        using ambient::memory::cpu::data_bulk;
       
        template<typename T, typename IB>
        void geqrt(matrix<T>& a, volatile matrix<T>& t){
            matrix<T>& t_ = const_cast<matrix<T>&>(t);
            T* tau  = (T*)ambient::memory::malloc<data_bulk,sizeof(T)*IB::value>(); 
            T* work = (T*)ambient::memory::malloc<data_bulk,sizeof(T)*IB::value*PLASMA_IB>();
            plasma::lapack<T>::geqrt(a.num_rows(), a.num_cols(), PLASMA_IB,
                                     a.data(), a.num_rows(),
                                     t_.data(), t_.num_rows(),
                                     tau, work);
        }
       
        template<typename T, typename TR, typename IB>
        void ormqr(const size_t& k, const matrix<T>& a, const matrix<T>& t, matrix<T>& c){
            T* work = (T*)ambient::memory::malloc<data_bulk,sizeof(T)*IB::value*PLASMA_IB>();
            plasma::lapack<T>::ormqr(PlasmaLeft, TR::value, c.num_rows(), c.num_cols(), k, PLASMA_IB,
                                     a.data(), a.num_rows(),
                                     t.data(), t.num_rows(),
                                     c.data(), c.num_rows(),
                                     work, IB::value);
        }
       
        template<typename T, typename IB>
        void tsqrt(matrix<T>& a1, matrix<T>& a2, volatile matrix<T>& t){
            matrix<T>& t_ = const_cast<matrix<T>&>(t);
            T* tau  = (T*)ambient::memory::malloc<data_bulk,sizeof(T)*IB::value>();
            T* work = (T*)ambient::memory::malloc<data_bulk,sizeof(T)*IB::value*PLASMA_IB>();
            plasma::lapack<T>::tsqrt(a2.num_rows(), a2.num_cols(), PLASMA_IB,
                                     a1.data(), a1.num_rows(),
                                     a2.data(), a2.num_rows(),
                                     t_.data(), t_.num_rows(),
                                     tau, work);
        }
       
        template<typename T, typename TR, typename IB>
        void tsmqr(const size_t& k, matrix<T>& a1, matrix<T>& a2, const matrix<T>& v, const matrix<T>& t){
            T* work = (T*)ambient::memory::malloc<data_bulk,sizeof(T)*IB::value*PLASMA_IB>();
            plasma::lapack<T>::tsmqr(PlasmaLeft, TR::value,
                                     IB::value, a1.num_cols(), a2.num_rows(), a2.num_cols(), k, PLASMA_IB,
                                     a1.data(), a1.num_rows(),
                                     a2.data(), a2.num_rows(),
                                     (T*)v.data(), v.num_rows(), // warning: const v might be modified
                                     (T*)t.data(), t.num_rows(), // warning: const t might be modified
                                     work, PLASMA_IB);
        }
       
        template<typename T, typename IB>
        void gelqt(matrix<T>& a, volatile matrix<T>& t){
            matrix<T>& t_ = const_cast<matrix<T>&>(t);
            T* tau  = (T*)ambient::memory::malloc<data_bulk,sizeof(T)*IB::value>();
            T* work = (T*)ambient::memory::malloc<data_bulk,sizeof(T)*IB::value*PLASMA_IB>();
            plasma::lapack<T>::gelqt(a.num_rows(), a.num_cols(), PLASMA_IB,
                                     a.data(), a.num_rows(), 
                                     t_.data(), t_.num_rows(),
                                     tau, work);
        }
       
        template<typename T, typename TR, typename IB>
        void ormlq(const size_t& k, const matrix<T>& a, const matrix<T>& t, matrix<T>& c){
            T* work = (T*)ambient::memory::malloc<data_bulk,sizeof(T)*IB::value*PLASMA_IB>();
            plasma::lapack<T>::ormlq(PlasmaRight, TR::value,
                                     c.num_rows(), c.num_cols(), k, PLASMA_IB,
                                     a.data(), a.num_rows(),
                                     t.data(), t.num_rows(),
                                     c.data(), c.num_rows(),
                                     work, IB::value);
        }
       
        template<typename T, typename IB>
        void tslqt(matrix<T>& a1, matrix<T>& a2, volatile matrix<T>& t){
            matrix<T>& t_ = const_cast<matrix<T>&>(t);
            T* tau  = (T*)ambient::memory::malloc<data_bulk,sizeof(T)*IB::value>();
            T* work = (T*)ambient::memory::malloc<data_bulk,sizeof(T)*IB::value*PLASMA_IB>();
            plasma::lapack<T>::tslqt(a2.num_rows(), a2.num_cols(), PLASMA_IB,
                                     a1.data(), a1.num_rows(),
                                     a2.data(), a2.num_rows(),
                                     t_.data(), t_.num_rows(),
                                     tau, work);
        }
       
        template<typename T, typename TR, typename IB>
        void tsmlq(const size_t& k, matrix<T>& a1, matrix<T>& a2, const matrix<T>& v, const matrix<T>& t){
            T* work = (T*)ambient::memory::malloc<data_bulk,sizeof(T)*IB::value*PLASMA_IB>();
            plasma::lapack<T>::tsmlq(PlasmaRight, TR::value,
                                     a1.num_rows(), IB::value, a2.num_rows(), a2.num_cols(), k, PLASMA_IB,
                                     a1.data(), a1.num_rows(),
                                     a2.data(), a2.num_rows(),
                                     (T*)v.data(), v.num_rows(), // warning: const v might be modified
                                     (T*)t.data(), t.num_rows(), // warning: const t might be modified
                                     work, IB::value);
        }
       
        template<class ViewA, class ViewB, class ViewC, typename T>
        void gemm_fma(const matrix<T,typename ViewA::allocator_type>& a, 
                      const matrix<T,typename ViewB::allocator_type>& b, 
                            matrix<T,typename ViewC::allocator_type>& c){
            if(!a.ambient_before->valid() || !b.ambient_before->valid()) return;
            const T* ad = a.data();
            const T* bd = b.data();
            T* cd = c.data();
            int m = ViewA::rows(a);
            int k = ViewA::cols(a);
            int n = ViewB::cols(b);
            int lda = a.num_rows();
            int ldb = b.num_rows();
            int ldc = c.num_rows();
            static const T alfa(1.0); 
            static const T beta(1.0);
            mkl::blas<T>::gemm(ViewA::code(), ViewB::code(), &m, &n, &k, &alfa, ad, &lda, bd, &ldb, &beta, cd, &ldc);
        }
       
        template<class ViewA, class ViewB, class ViewC, typename T>
        void gemm(const matrix<T,typename ViewA::allocator_type>& a, 
                  const matrix<T,typename ViewB::allocator_type>& b, 
                  volatile matrix<T,typename ViewC::allocator_type>& c){
            matrix<T,typename ViewC::allocator_type>& c_ = const_cast<matrix<T,typename ViewC::allocator_type>&>(c);
            if(!a.ambient_before->valid() || !b.ambient_before->valid()){
                memset(c_.data(), 0, ambient::extent(c_)); 
                return;
            }
            const T* ad = a.data();
            const T* bd = b.data();
            T* cd = c_.data();
            int m = ViewA::rows(a);
            int k = ViewA::cols(a);
            int n = ViewB::cols(b);
            int lda = a.num_rows();
            int ldb = b.num_rows();
            int ldc = c_.num_rows();
            static const T alfa(1.0);
            static const T beta(0.0);
            mkl::blas<T>::gemm(ViewA::code(), ViewB::code(), &m, &n, &k, &alfa, ad, &lda, bd, &ldb, &beta, cd, &ldc);
        }
            
        template<class ViewB, typename T, typename D>
        void gemm_diagonal_lhs(const matrix<D>& a_diag, const matrix<T>& b, volatile matrix<T>& c){
            matrix<T>& c_ = const_cast<matrix<T>&>(c);
            if(std::is_same<ViewB, transpose_view<matrix<T> > >::value){
                size_t sizex = b.num_cols();
                int size  = a_diag.num_rows();
                static const int ONE = 1;
                const T* bd = b.data();
                T* cd = c_.data();
                memset(cd, 0, ambient::extent(c_)); 
                const D* alfa = a_diag.data();
                
                for(int k = 0 ; k < sizex; k++)
                    mkl::blas<T>::axpy(&size, &alfa[k], &bd[k*size], &ONE, &cd[k], &size);// C - check carefully for TE a_diag double, b complex
            }else{
                int sizey = a_diag.num_rows();
                int size = b.num_cols();
                const T* bd = b.data();
                T* cd = c_.data();
                memset(cd, 0, ambient::extent(c_)); 
                const D* alfa = a_diag.data();
               
                for(int k = 0 ; k < sizey; k++)
                    mkl::blas<T>::axpy(&size, &alfa[k], &bd[k], &sizey, &cd[k], &sizey);
            }
        }
            
        template<class ViewA, typename T, typename D>
        void gemm_diagonal_rhs(const matrix<T>& a, const matrix<D>& b_diag, volatile matrix<T>& c){
            matrix<T>& c_ = const_cast<matrix<T>&>(c);
            if(std::is_same<ViewA, transpose_view<matrix<T> > >::value){
                int sizey = b_diag.num_rows();
                int size = a.num_cols();
                static const int ONE = 1;
                const T* ad = a.data();
                T* cd = c_.data();
                memset(cd, 0, ambient::extent(c_)); 
                const D* alfa = b_diag.data();
                
                for(int k = 0 ; k < sizey; k++)
                    mkl::blas<T>::axpy(&size, &alfa[k], &ad[k], &sizey, &cd[k*size], &ONE);// C - check carefully for TE b_diag double, b complex
            }else{
                size_t sizex = b_diag.num_rows();
                int size = a.num_rows(); // for the case of complex
                static const int ONE = 1;
                const T* ad = a.data();
                T* cd = c_.data();
                memset(cd, 0, ambient::extent(c_)); 
                const D* alfa = b_diag.data();
         
                for(int k = 0 ; k < sizex; k++)
                    mkl::blas<T>::axpy(&size, &alfa[k], &ad[k*size], &ONE, &cd[k*size], &ONE);
            }
        }

        template<typename T>
        void gemm_diagonal(const matrix<T>& a, const matrix<T>& b, volatile matrix<T>& c){
            const T* ad = a.data();
            const T* bd = b.data();
            T* cd = c.data();
            size_t size = b.num_rows();
            for(int k = 0 ; k < size; k++) cd[k] = ad[k]*bd[k];
        }
       
        template<typename T>
        void copy_rt(const matrix<T>& a, volatile matrix<T>& t){
            matrix<T>& t_ = const_cast<matrix<T>&>(t);
            const T* ad  = a.data();
            T* td  = t_.data();
            memset(td, 0, ambient::extent(t_)); 
            size_t sda = a.num_cols();
            size_t lda = a.num_rows();
            size_t ldt = t_.num_rows();
       
            for(int j = 0; j < sda; ++j)
            for(int i = 0; i <= j && i < ldt; ++i)
            td[i+ldt*j] = ad[i+lda*j]; 
        }
       
        template<typename T>
        void copy_lt(const matrix<T>& a, volatile matrix<T>& t){
            matrix<T>& t_ = const_cast<matrix<T>&>(t);
            const T* ad  = a.data();
            T* td  = t_.data();
            memset(td, 0, ambient::extent(t_)); 
            size_t sdt = t_.num_cols();
            size_t lda = a.num_rows();
            size_t ldt = t_.num_rows();
       
            for(int j = 0; j < sdt; ++j)
            for(int i = j; i < lda; ++i)
            td[i+ldt*j] = ad[i+lda*j]; 
        }
       
        template<class A1, class A2, typename T>
        void copy_block(const matrix<T,A1>& src, const size_t& si, const size_t& sj,
                        matrix<T,A2>& dst, const size_t& di, const size_t& dj, 
                        const size_t& m, const size_t& n)
        {
            const T* sd = src.data();
            T* dd = dst.data();
            ambient::memptf<T, ambient::memcpy>(dd, dst.num_rows(), dim2(dj, di), 
                                                sd, src.num_rows(), dim2(sj, si), 
                                                dim2( n, m ));
        }
       
        template<typename T>
        void copy_block_s(const matrix<T>& src, const size_t& si, const size_t& sj,
                          matrix<T>& dst, const size_t& di, const size_t& dj, 
                          const matrix<T>& alfa, const size_t& ai, const size_t& aj,
                          const size_t& m, const size_t& n)
        {
            const T* sd = src.data();
            T* dd = dst.data();
            T factor = alfa(ai,aj);
            ambient::memptf<T, ambient::memscal>(dd, dst.num_rows(), dim2(dj, di), 
                                                 sd, src.num_rows(), dim2(sj, si), 
                                                 dim2( n, m ), factor);
        }
       
        template<class A1, class A2, typename T>
        void copy_block_unbound(const matrix<T,A1>& src, const size_t& si, const size_t& sj,
                                volatile matrix<T,A2>& dst, const size_t& di, const size_t& dj, 
                                const size_t& m, const size_t& n)
        {
            T* dd = dst.data();
            detail::copy_block<A1,A2,T>(src, si, sj, const_cast<matrix<T,A2>&>(dst), di, dj, m, n);
        }
       
        template<typename T>
        void copy_block_s_unbound(const matrix<T>& src, const size_t& si, const size_t& sj,
                                  volatile matrix<T>& dst, const size_t& di, const size_t& dj, 
                                  const matrix<T>& alfa, const size_t& ai, const size_t& aj,
                                  const size_t& m, const size_t& n)
        {
            T* dd = dst.data();
            detail::copy_block_s(src, si, sj, const_cast<matrix<T>&>(dst), di, dj, alfa, ai, aj, m, n);
        }
       
        template<class A1, class A2, class A3, typename T>
        void copy_block_sa(const matrix<T,A1>& src, const size_t& si, const size_t& sj,
                           matrix<T,A2>& dst, const size_t& di, const size_t& dj, 
                           const matrix<T,A3>& alfa, const size_t& ai, const size_t& aj,
                           const size_t& m, const size_t& n, const T& alfa_scale)
        {
            T factor = alfa_scale * alfa(ai,aj);
            ambient::memptf<T, ambient::memscala>(dst.data(), dst.num_rows(), dim2(dj, di), 
                                                  src.data(), src.num_rows(), dim2(sj, si), 
                                                  dim2( n, m ), factor);
        }
            
        template<typename T, class A>
        void trace(const matrix<T,A>& a, future<T>& trace){
            size_t m = a.num_rows();
            size_t n = a.num_cols();
            const T* ad = a.data();
            T ret = 0;
        
            size_t sizex = std::min(n,m);
            for(size_t jj = 0; jj < sizex; jj++) ret += ad[jj + jj*m];
            trace.set(ret);
        }
            
        template<typename T, class A>
        void scalar_norm(const matrix<T,A>& a, future<double>& norm){
            const T* ad = a.data();
            norm.set(ambient::dot(ad, ad, ambient::get_square_dim(a)));
        }
            
        template<typename T>
        void overlap(const matrix<T>& a, const matrix<T>& b, future<T>& overlap){
            const T* ad = a.data();
            const T* bd = b.data();
            overlap.set(ambient::dot(ad, bd, ambient::get_square_dim(a)));
        }
       
        template<typename T, class A>
        void add(matrix<T,A>& a, const matrix<T,A>& b){
            const T* bd = b.data();
            T* ar = a.data();
       
            int size = ambient::get_square_dim(a);
            #ifdef AMBIENT_CILK
            ar[0:size] += bd[0:size];
            #else
            #pragma vector always
            for(int k = 0; k < size; k++)
                ar[k] += bd[k];
            #endif
        }

        template<typename T>
        void sub(matrix<T>& a, const matrix<T>& b){
            const T* bd = b.data();
            T* ar = a.data();
       
            int size = ambient::get_square_dim(a);
            #ifdef AMBIENT_CILK
            ar[0:size] -= bd[0:size];
            #else
            #pragma vector always
            for(int k = 0; k < size; k++)
                ar[k] -= bd[k];
            #endif
        }
            
        template<typename T>
        void scale(matrix<T>& a, const future<T>& t){
            T* ar = a.data();
            T factor = t.get();
            int size = ambient::get_square_dim(a);
            #ifdef AMBIENT_CILK
            ar[0:size] *= factor;
            #else
            #pragma vector always
            for(int k = 0; k < size; k++)
                ar[k] *= factor;
            #endif
        }
            
        template<typename T>
        void scale_offset(matrix<T>& a, const size_t& ai, const size_t& aj, const matrix<T>& alfa, const size_t& alfai){
            int m = num_rows(a);
            T* ad = &a(0,aj);
            T factor = alfa(alfai,0);
            for(int k = ai; k < m; k++) ad[k] *= factor;
        }
            
        template<typename T>
        void scale_inverse(matrix<T>& a, const future<T>& t){
            T* ar = a.data();
            T factor = t.get();
            int size = ambient::get_square_dim(a);
            #ifdef AMBIENT_CILK
            ar[0:size] /= factor;
            #else
            #pragma vector always
            for(int k = 0; k < size; k++)
                ar[k] /= factor;
            #endif
        }
            
        template<typename T>
        void sqrt_diagonal(matrix<T>& a){
            size_t size = a.num_rows();
            T* ar = a.data();
            #ifdef AMBIENT_CILK
            ar[0:size] = std::sqrt(ar[0:size]);
            #else
            for(size_t i = 0; i < size; ++i) ar[i] = std::sqrt(ar[i]);
            #endif
        }
            
        template<typename T>
        void exp_diagonal(matrix<T>& a, const T& alfa){
            size_t size = a.num_rows();
            T* ar = a.data();
            for(size_t i = 0; i < size; ++i) ar[i] = std::exp(alfa*ar[i]);
        }
       
        template<typename T, class A>
        void transpose_out(const matrix<T,A>& a, volatile matrix<T,A>& t){
            const T* od = a.data();
            T* td = t.data();
            int m = a.num_rows();
            int n = a.num_cols();
       
            for(int i = 0; i < m; i++){
                for(int j = 0; j < n; j++) *td++ = od[j*m];
                od++;
            }
        }
       
        template<typename T, class A>
        void conj_inplace(matrix<T,A>& a){
            size_t size = a.num_rows()*a.num_cols();
            T* ar = a.data();
            for(int i = 0; i < size; ++i)
                ar[i] = mkl::helper_complex<T>::conj(ar[i]);
        }
       
        template<typename T, class A>
        void resize(volatile matrix<T,A>& r, const matrix<T,A>& a, const size_t& m, const size_t& n){
            matrix<T,A>& r_ = const_cast<matrix<T,A>&>(r);
            T* dd = r_.data(); 
            if(m*n != ambient::get_square_dim(r_)) memset(dd, 0, ambient::extent(r_)); 
            ambient::memptf<T, ambient::memcpy>(dd, r_.num_rows(), dim2(0,0),
                                                a.data(), a.num_rows(), dim2(0,0), dim2(n, m)); 
        }
            
        template<typename T>
        void init_identity(volatile matrix<T>& a){
            matrix<T>& a_ = const_cast<matrix<T>&>(a);
            size_t n = a_.num_cols();
            size_t m = a_.num_rows();
            T* ad = a_.data();
            memset(ad, 0, ambient::extent(a_)); 
       
            size_t sizex = std::min(m,n); // respecting borders
            for(size_t jj = 0; jj < sizex; ++jj) ad[jj + m*jj] = 1.;
        }
           
        inline void randomize(double& a){ 
            a = drand48();
        }
       
        inline void randomize(std::complex<double>& a){
            a.real(drand48());
            a.imag(drand48());
        }

        inline void randomize_diag(double& a){
            a = drand48();
        }

        inline void randomize_diag(std::complex<double>& a){
            a.real(drand48());
            a.imag(0.0);
        }
       
        template<typename T>
        void init_random(volatile matrix<T>& a){
            matrix<T>& a_ = const_cast<matrix<T>&>(a);
            size_t size = ambient::get_square_dim(a_);
            T* ad = a_.data();
            for(size_t i = 0; i < size; ++i) randomize(ad[i]);
        }

        template<typename T>
        void init_random_hermitian(volatile matrix<T>& a){
            matrix<T>& a_ = const_cast<matrix<T>&>(a);
            size_t lda = a_.num_rows();
            T* ad = a_.data();

            for(size_t i = 0; i < a_.num_rows(); ++i)
            for(size_t j = i+1; j < a_.num_cols(); ++j){
                randomize(ad[i+j*lda]);
                ad[j+i*lda] = mkl::helper_complex<T>::conj(ad[i+j*lda]);
            }
            for(size_t k = 0; k < a_.num_cols(); ++k) 
            randomize_diag(ad[k*lda+k]);
        }

        template<typename T, class A>
        void init_value(volatile matrix<T,A>& a, const T& value){
            matrix<T>& a_ = const_cast<matrix<T>&>(a);
            size_t size = ambient::get_square_dim(a_);
            T* ad = a_.data();
            for(size_t i = 0; i < size; ++i) ad[i] = value; // not a memset due to complex
        }
            
        template<typename T>
        void round_square(const matrix<T>& a, std::vector<T>*& ac){
            const T* ad = a.data();
            size_t sizey = a.num_rows();
            for(int i = 0; i < sizey; i++){
                double v = std::abs(ad[i]);
                if(v > 1e-10) ac->push_back(v*v);
            }
        }
       
        template<typename T>
        void cast_to_vector(std::vector<T>*& ac, const matrix<T>& a, const size_t& m, const size_t& n, const size_t& lda, const size_t& offset){
            const T* ad = a.data();
            for(int j = 0; j < n; ++j) std::memcpy((void*)&(*ac)[j*lda + offset],(void*)&ad[j*m], m*sizeof(T));  
        }
            
        template<typename T>
        void cast_from_vector(const std::vector<T>*& ac, volatile matrix<T>& a, const size_t& m, const size_t& n, const size_t& lda, const size_t& offset){
            T* ad = a.data();
            for(int j = 0; j < n; ++j) std::memcpy((void*)&ad[j*m],(void*)&(*ac)[offset + j*lda], m*sizeof(T));
        }
       
        template<typename T1, typename T2>
        void cast_from_vector_t(const std::vector<T1>*& ac, volatile matrix<T2>& a, const size_t& m, const size_t& n, const size_t& lda, const size_t& offset){
            T2* ad = a.data();
            const T1* sd = &(*ac)[offset];
            for(int j = 0; j < n; ++j) 
                for(int i = 0; i < m; ++i)
                    ad[j*m + i] = sd[j*lda + i];
        }
       
        template<typename T, typename D>
        void cast_double_complex(volatile matrix<T>& a, const matrix<D>& b){
            matrix<T>& a_ = const_cast<matrix<T>&>(a);
            T* ad = a_.data();
            const D* bd = b.data();
            size_t size = a_.num_rows();
            for(size_t i = 0; i < size; ++i)
                ad[i] = mkl::helper_cast<T,D>::cast(bd[i]);
        };
       
        template<typename T, class A>
        void touch(const matrix<T,A>& a){ }
       
        template<typename T, class A>
        void migrate(matrix<T,A>& a){ }
       
        template<typename T, class A>
        void hint(const matrix<T,A>& a){ }
       
        inline double distance(const std::complex<double>& a, const std::complex<double>& b){ 
            return fabs(std::norm(a) - std::norm(b));
        }
        inline double magnitude(const std::complex<double>& a, const std::complex<double>& b){
            return std::max(fabs(std::norm(a)), fabs(std::norm(b)));
        }
        inline double distance(double a, double b) { 
            return fabs(fabs(a) - fabs(b));    
        }
        inline double magnitude(double a, double b){ 
            return std::max(fabs(a), fabs(b)); 
        }
        template<typename T>
        void validation(const matrix<T>& a, const matrix<T>& b, future<bool>& ret){ // see paper for Reference Dongara 
            const T* ad = a.data(); 
            const T* bd = b.data(); 
            double epsilon = std::numeric_limits<double>::epsilon();
            int count = 0;
            size_t sizey = std::min(a.num_rows(), b.num_rows());
            size_t sizex = std::min(a.num_cols(), b.num_cols());
            
            std::cout.precision(16);
            std::cout.setf( std::ios::fixed, std:: ios::floatfield );
       
            for(size_t i = 0; i < sizey; ++i){
                for(size_t j = 0; j < sizex; ++j){
                    T av = ad[i+j*a.num_rows()];
                    T bv = bd[i+j*b.num_rows()];
                    double d = distance(av, bv);
                    double m = magnitude(av, bv);
                    if(d > epsilon*1024 && d/m > epsilon*1024){ // 16 is recommended, 256 because MKL isn't bitwise stable
                        std::cout << i << " " << j << " : " << av << " " << bv << ", eps: " << std::min(d, d/m) << "\n";
                        ret.set(false);
                        if(++count > 10) return;
                    }
       
                }
            }
        }
       
        template<typename T>
        void svd(const matrix<T>& a, volatile matrix<T>& u, volatile matrix<T>& vt, volatile matrix<typename real_type<T>::type>& s){
            int m = a.num_rows();
            int n = a.num_cols();
            int k = std::min(m,n);
            int info;
            int lwork = -1;
            T wkopt;
            const T* ad  = a.data();
            T* ud  = u.data();
            T* vtd = vt.data();
            typename real_type<T>::type* sd  = s.data();
            mkl::lapack<T>::gesvd( "S", "S", &m, &n, (T*)ad, &m, sd, ud, &m, vtd, &k, &wkopt, &lwork, &info ); // warning: const a is modified
        }
       
        template<typename T>
        void inverse(matrix<T> & a){
            int info;
            int m = a.num_rows();
            int n = a.num_cols();
            T* ad = a.data(); 
            int* ipivd = new int[n];
            mkl::lapack<T>::getrf(&m, &n, ad, &m, ipivd, &info);
            mkl::lapack<T>::getri(&n, ad, &n, ipivd, &info);
            delete [] ipivd;
        }
       
        template<typename T>
        void geev(const matrix<T>& a, volatile matrix<T>& lv, volatile matrix<T>& rv, volatile matrix<T>& s){
            int n = a.num_cols();
            int info;
            int lwork = -1;
            T wkopt;
            const T* ad  = a.data();
            T* lvd = lv.data();
            T* rvd = rv.data();
            T* sd  = s.data();
            mkl::lapack<T>::geev("N", "V", &n, (T*)ad, &n, sd, lvd, &n, rvd, &n, &wkopt, &lwork, &info); // warning: const a is modified
        }
       
        template<typename T>
        void heev(matrix<T>& a, matrix<typename real_type<T>::type>& w){
            int m = a.num_rows();
            int info, lwork = -1;
            T wkopt;
            T* ad = a.data();
            typename real_type<T>::type* wd = w.data();
            mkl::lapack<T>::heev("V","U",&m,ad,&m,wd,&wkopt,&lwork,&info);

            typename real_type<T>::type s;
            for(int i = 0; i < (int)(m/2); i++){
                s = wd[i]; wd[i] = wd[m-1-i]; wd[m-1-i] = s;
            }
            // reversing eigenvectors
            size_t len = m*sizeof(T);
            T* work = (T*)std::malloc(len);
            for (int i = 0; i < (int)(m/2); i++){
                std::memcpy(work, &ad[i*m], len);
                std::memcpy(&ad[i*m], &ad[(m-1-i)*m], len);
                std::memcpy(&ad[(m-1-i)*m], work, len);
            }
            std::free(work);
        }
    }

    AMBIENT_EXPORT_TEMPLATE(detail::geqrt, geqrt)
    AMBIENT_EXPORT_TEMPLATE(detail::ormqr, ormqr)
    AMBIENT_EXPORT_TEMPLATE(detail::tsqrt, tsqrt)
    AMBIENT_EXPORT_TEMPLATE(detail::tsmqr, tsmqr)
    AMBIENT_EXPORT_TEMPLATE(detail::gelqt, gelqt)
    AMBIENT_EXPORT_TEMPLATE(detail::ormlq, ormlq)
    AMBIENT_EXPORT_TEMPLATE(detail::tslqt, tslqt)
    AMBIENT_EXPORT_TEMPLATE(detail::tsmlq, tsmlq)
    AMBIENT_EXPORT_TEMPLATE(detail::gemm,  gemm)
    AMBIENT_EXPORT_TEMPLATE(detail::gemm_fma, gemm_fma)
    AMBIENT_EXPORT_TEMPLATE(detail::gemm_diagonal_lhs, gemm_diagonal_lhs)
    AMBIENT_EXPORT_TEMPLATE(detail::gemm_diagonal_rhs, gemm_diagonal_rhs)
    AMBIENT_EXPORT_TEMPLATE(detail::gemm_diagonal, gemm_diagonal)
    AMBIENT_EXPORT_TEMPLATE(detail::trace, trace)
    AMBIENT_EXPORT_TEMPLATE(detail::scalar_norm, scalar_norm)
    AMBIENT_EXPORT_TEMPLATE(detail::overlap, overlap)
    AMBIENT_EXPORT_TEMPLATE(detail::add, add)
    AMBIENT_EXPORT_TEMPLATE(detail::sub, sub)
    AMBIENT_EXPORT_TEMPLATE(detail::scale, scale)
    AMBIENT_EXPORT_TEMPLATE(detail::scale_offset, scale_offset)
    AMBIENT_EXPORT_TEMPLATE(detail::scale_inverse, scale_inverse)
    AMBIENT_EXPORT_TEMPLATE(detail::sqrt_diagonal, sqrt_diagonal)
    AMBIENT_EXPORT_TEMPLATE(detail::exp_diagonal, exp_diagonal)
    AMBIENT_EXPORT_TEMPLATE(detail::transpose_out,transpose_out)
    AMBIENT_EXPORT_TEMPLATE(detail::conj_inplace, conj_inplace)
    AMBIENT_EXPORT_TEMPLATE(detail::resize, resize)
    AMBIENT_EXPORT_TEMPLATE(detail::init_identity, init_identity)
    AMBIENT_EXPORT_TEMPLATE(detail::init_value, init_value)
    AMBIENT_EXPORT_TEMPLATE(detail::round_square, round_square)
    AMBIENT_EXPORT_TEMPLATE(detail::cast_to_vector, cast_to_vector)
    AMBIENT_EXPORT_TEMPLATE(detail::cast_from_vector, cast_from_vector)
    AMBIENT_EXPORT_TEMPLATE(detail::cast_from_vector_t, cast_from_vector_t)
    AMBIENT_EXPORT_TEMPLATE(detail::cast_double_complex, cast_double_complex)
    AMBIENT_EXPORT_TEMPLATE(detail::touch, touch)
    AMBIENT_EXPORT_TEMPLATE(detail::migrate, migrate)
    AMBIENT_EXPORT_TEMPLATE(detail::hint, hint)
    AMBIENT_EXPORT_TEMPLATE(detail::svd, svd)
    AMBIENT_EXPORT_TEMPLATE(detail::inverse, inverse)
    AMBIENT_EXPORT_TEMPLATE(detail::geev, geev)
    AMBIENT_EXPORT_TEMPLATE(detail::heev, heev)
    AMBIENT_EXPORT_TEMPLATE(detail::copy_rt, copy_rt)
    AMBIENT_EXPORT_TEMPLATE(detail::copy_lt, copy_lt)
    AMBIENT_EXPORT_TEMPLATE(detail::copy_block_unbound, copy_block_unbound)
    AMBIENT_EXPORT_TEMPLATE(detail::copy_block_s_unbound, copy_block_s_unbound)
    AMBIENT_EXPORT_TEMPLATE(detail::copy_block, copy_block)
    AMBIENT_EXPORT_TEMPLATE(detail::copy_block_s, copy_block_s)
    AMBIENT_EXPORT_TEMPLATE(detail::copy_block_sa, copy_block_sa)
    AMBIENT_EXPORT_TEMPLATE(detail::init_random, init_random)
    AMBIENT_EXPORT_TEMPLATE(detail::init_random_hermitian, init_random_hermitian)
    AMBIENT_EXPORT_TEMPLATE(detail::validation, validation)

} } }

#endif
