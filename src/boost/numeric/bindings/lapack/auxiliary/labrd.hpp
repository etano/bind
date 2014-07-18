//
// Copyright (c) 2002--2010
// Toon Knapen, Karl Meerbergen, Kresimir Fresl,
// Thomas Klimpel and Rutger ter Borg
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// THIS FILE IS AUTOMATICALLY GENERATED
// PLEASE DO NOT EDIT!
//

#ifndef BOOST_NUMERIC_BINDINGS_LAPACK_AUXILIARY_LABRD_HPP
#define BOOST_NUMERIC_BINDINGS_LAPACK_AUXILIARY_LABRD_HPP

#include <boost/assert.hpp>
#include <boost/numeric/bindings/begin.hpp>
#include <boost/numeric/bindings/is_column_major.hpp>
#include <boost/numeric/bindings/is_complex.hpp>
#include <boost/numeric/bindings/is_mutable.hpp>
#include <boost/numeric/bindings/is_real.hpp>
#include <boost/numeric/bindings/remove_imaginary.hpp>
#include <boost/numeric/bindings/size.hpp>
#include <boost/numeric/bindings/stride.hpp>
#include <boost/numeric/bindings/value_type.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/utility/enable_if.hpp>

//
// The LAPACK-backend for labrd is the netlib-compatible backend.
//
#include <boost/numeric/bindings/lapack/detail/lapack.h>
#include <boost/numeric/bindings/lapack/detail/lapack_option.hpp>

namespace boost {
namespace numeric {
namespace bindings {
namespace lapack {

//
// The detail namespace contains value-type-overloaded functions that
// dispatch to the appropriate back-end LAPACK-routine.
//
namespace detail {

//
// Overloaded function for dispatching to
// * netlib-compatible LAPACK backend (the default), and
// * float value-type.
//
inline std::ptrdiff_t labrd( const fortran_int_t m, const fortran_int_t n,
        const fortran_int_t nb, float* a, const fortran_int_t lda, float* d,
        float* e, float* tauq, float* taup, float* x, const fortran_int_t ldx,
        float* y, const fortran_int_t ldy ) {
    fortran_int_t info(0);
    LAPACK_SLABRD( &m, &n, &nb, a, &lda, d, e, tauq, taup, x, &ldx, y, &ldy );
    return info;
}

//
// Overloaded function for dispatching to
// * netlib-compatible LAPACK backend (the default), and
// * double value-type.
//
inline std::ptrdiff_t labrd( const fortran_int_t m, const fortran_int_t n,
        const fortran_int_t nb, double* a, const fortran_int_t lda, double* d,
        double* e, double* tauq, double* taup, double* x,
        const fortran_int_t ldx, double* y, const fortran_int_t ldy ) {
    fortran_int_t info(0);
    LAPACK_DLABRD( &m, &n, &nb, a, &lda, d, e, tauq, taup, x, &ldx, y, &ldy );
    return info;
}

//
// Overloaded function for dispatching to
// * netlib-compatible LAPACK backend (the default), and
// * complex<float> value-type.
//
inline std::ptrdiff_t labrd( const fortran_int_t m, const fortran_int_t n,
        const fortran_int_t nb, std::complex<float>* a,
        const fortran_int_t lda, float* d, float* e,
        std::complex<float>* tauq, std::complex<float>* taup,
        std::complex<float>* x, const fortran_int_t ldx,
        std::complex<float>* y, const fortran_int_t ldy ) {
    fortran_int_t info(0);
    LAPACK_CLABRD( &m, &n, &nb, a, &lda, d, e, tauq, taup, x, &ldx, y, &ldy );
    return info;
}

//
// Overloaded function for dispatching to
// * netlib-compatible LAPACK backend (the default), and
// * complex<double> value-type.
//
inline std::ptrdiff_t labrd( const fortran_int_t m, const fortran_int_t n,
        const fortran_int_t nb, std::complex<double>* a,
        const fortran_int_t lda, double* d, double* e,
        std::complex<double>* tauq, std::complex<double>* taup,
        std::complex<double>* x, const fortran_int_t ldx,
        std::complex<double>* y, const fortran_int_t ldy ) {
    fortran_int_t info(0);
    LAPACK_ZLABRD( &m, &n, &nb, a, &lda, d, e, tauq, taup, x, &ldx, y, &ldy );
    return info;
}

} // namespace detail

//
// Value-type based template class. Use this class if you need a type
// for dispatching to labrd.
//
template< typename Value, typename Enable = void >
struct labrd_impl {};

//
// This implementation is enabled if Value is a real type.
//
template< typename Value >
struct labrd_impl< Value, typename boost::enable_if< is_real< Value > >::type > {

    typedef Value value_type;
    typedef typename remove_imaginary< Value >::type real_type;

    //
    // Static member function, that
    // * Deduces the required arguments for dispatching to LAPACK, and
    // * Asserts that most arguments make sense.
    //
    template< typename MatrixA, typename VectorD, typename VectorE,
            typename VectorTAUQ, typename VectorTAUP, typename MatrixX,
            typename MatrixY >
    static std::ptrdiff_t invoke( MatrixA& a, VectorD& d, VectorE& e,
            VectorTAUQ& tauq, VectorTAUP& taup, MatrixX& x, MatrixY& y ) {
        namespace bindings = ::boost::numeric::bindings;
        BOOST_STATIC_ASSERT( (bindings::is_column_major< MatrixA >::value) );
        BOOST_STATIC_ASSERT( (bindings::is_column_major< MatrixX >::value) );
        BOOST_STATIC_ASSERT( (bindings::is_column_major< MatrixY >::value) );
        BOOST_STATIC_ASSERT( (boost::is_same< typename remove_const<
                typename bindings::value_type< MatrixA >::type >::type,
                typename remove_const< typename bindings::value_type<
                VectorD >::type >::type >::value) );
        BOOST_STATIC_ASSERT( (boost::is_same< typename remove_const<
                typename bindings::value_type< MatrixA >::type >::type,
                typename remove_const< typename bindings::value_type<
                VectorE >::type >::type >::value) );
        BOOST_STATIC_ASSERT( (boost::is_same< typename remove_const<
                typename bindings::value_type< MatrixA >::type >::type,
                typename remove_const< typename bindings::value_type<
                VectorTAUQ >::type >::type >::value) );
        BOOST_STATIC_ASSERT( (boost::is_same< typename remove_const<
                typename bindings::value_type< MatrixA >::type >::type,
                typename remove_const< typename bindings::value_type<
                VectorTAUP >::type >::type >::value) );
        BOOST_STATIC_ASSERT( (boost::is_same< typename remove_const<
                typename bindings::value_type< MatrixA >::type >::type,
                typename remove_const< typename bindings::value_type<
                MatrixX >::type >::type >::value) );
        BOOST_STATIC_ASSERT( (boost::is_same< typename remove_const<
                typename bindings::value_type< MatrixA >::type >::type,
                typename remove_const< typename bindings::value_type<
                MatrixY >::type >::type >::value) );
        BOOST_STATIC_ASSERT( (bindings::is_mutable< MatrixA >::value) );
        BOOST_STATIC_ASSERT( (bindings::is_mutable< VectorD >::value) );
        BOOST_STATIC_ASSERT( (bindings::is_mutable< VectorE >::value) );
        BOOST_STATIC_ASSERT( (bindings::is_mutable< VectorTAUQ >::value) );
        BOOST_STATIC_ASSERT( (bindings::is_mutable< VectorTAUP >::value) );
        BOOST_STATIC_ASSERT( (bindings::is_mutable< MatrixX >::value) );
        BOOST_STATIC_ASSERT( (bindings::is_mutable< MatrixY >::value) );
        BOOST_ASSERT( bindings::size(d) >= bindings::size_column(a) );
        BOOST_ASSERT( bindings::size(e) >= bindings::size_column(a) );
        BOOST_ASSERT( bindings::size(taup) >= bindings::size_column(a) );
        BOOST_ASSERT( bindings::size(tauq) >= bindings::size_column(a) );
        BOOST_ASSERT( bindings::size_minor(a) == 1 ||
                bindings::stride_minor(a) == 1 );
        BOOST_ASSERT( bindings::size_minor(x) == 1 ||
                bindings::stride_minor(x) == 1 );
        BOOST_ASSERT( bindings::size_minor(y) == 1 ||
                bindings::stride_minor(y) == 1 );
        BOOST_ASSERT( bindings::stride_major(a) >= std::max< std::ptrdiff_t >(1,
                bindings::size_row(a)) );
        BOOST_ASSERT( bindings::stride_major(x) >= bindings::size_row(a) );
        BOOST_ASSERT( bindings::stride_major(y) >= bindings::size_column(a) );
        return detail::labrd( bindings::size_row(a), bindings::size_column(a),
                bindings::size_column(a), bindings::begin_value(a),
                bindings::stride_major(a), bindings::begin_value(d),
                bindings::begin_value(e), bindings::begin_value(tauq),
                bindings::begin_value(taup), bindings::begin_value(x),
                bindings::stride_major(x), bindings::begin_value(y),
                bindings::stride_major(y) );
    }

};

//
// This implementation is enabled if Value is a complex type.
//
template< typename Value >
struct labrd_impl< Value, typename boost::enable_if< is_complex< Value > >::type > {

    typedef Value value_type;
    typedef typename remove_imaginary< Value >::type real_type;

    //
    // Static member function, that
    // * Deduces the required arguments for dispatching to LAPACK, and
    // * Asserts that most arguments make sense.
    //
    template< typename MatrixA, typename VectorD, typename VectorE,
            typename VectorTAUQ, typename VectorTAUP, typename MatrixX,
            typename MatrixY >
    static std::ptrdiff_t invoke( MatrixA& a, VectorD& d, VectorE& e,
            VectorTAUQ& tauq, VectorTAUP& taup, MatrixX& x, MatrixY& y ) {
        namespace bindings = ::boost::numeric::bindings;
        BOOST_STATIC_ASSERT( (bindings::is_column_major< MatrixA >::value) );
        BOOST_STATIC_ASSERT( (bindings::is_column_major< MatrixX >::value) );
        BOOST_STATIC_ASSERT( (bindings::is_column_major< MatrixY >::value) );
        BOOST_STATIC_ASSERT( (boost::is_same< typename remove_const<
                typename bindings::value_type< VectorD >::type >::type,
                typename remove_const< typename bindings::value_type<
                VectorE >::type >::type >::value) );
        BOOST_STATIC_ASSERT( (boost::is_same< typename remove_const<
                typename bindings::value_type< MatrixA >::type >::type,
                typename remove_const< typename bindings::value_type<
                VectorTAUQ >::type >::type >::value) );
        BOOST_STATIC_ASSERT( (boost::is_same< typename remove_const<
                typename bindings::value_type< MatrixA >::type >::type,
                typename remove_const< typename bindings::value_type<
                VectorTAUP >::type >::type >::value) );
        BOOST_STATIC_ASSERT( (boost::is_same< typename remove_const<
                typename bindings::value_type< MatrixA >::type >::type,
                typename remove_const< typename bindings::value_type<
                MatrixX >::type >::type >::value) );
        BOOST_STATIC_ASSERT( (boost::is_same< typename remove_const<
                typename bindings::value_type< MatrixA >::type >::type,
                typename remove_const< typename bindings::value_type<
                MatrixY >::type >::type >::value) );
        BOOST_STATIC_ASSERT( (bindings::is_mutable< MatrixA >::value) );
        BOOST_STATIC_ASSERT( (bindings::is_mutable< VectorD >::value) );
        BOOST_STATIC_ASSERT( (bindings::is_mutable< VectorE >::value) );
        BOOST_STATIC_ASSERT( (bindings::is_mutable< VectorTAUQ >::value) );
        BOOST_STATIC_ASSERT( (bindings::is_mutable< VectorTAUP >::value) );
        BOOST_STATIC_ASSERT( (bindings::is_mutable< MatrixX >::value) );
        BOOST_STATIC_ASSERT( (bindings::is_mutable< MatrixY >::value) );
        BOOST_ASSERT( bindings::size(d) >= bindings::size_column(a) );
        BOOST_ASSERT( bindings::size(e) >= bindings::size_column(a) );
        BOOST_ASSERT( bindings::size(taup) >= bindings::size_column(a) );
        BOOST_ASSERT( bindings::size(tauq) >= bindings::size_column(a) );
        BOOST_ASSERT( bindings::size_minor(a) == 1 ||
                bindings::stride_minor(a) == 1 );
        BOOST_ASSERT( bindings::size_minor(x) == 1 ||
                bindings::stride_minor(x) == 1 );
        BOOST_ASSERT( bindings::size_minor(y) == 1 ||
                bindings::stride_minor(y) == 1 );
        BOOST_ASSERT( bindings::stride_major(a) >= std::max< std::ptrdiff_t >(1,
                bindings::size_row(a)) );
        BOOST_ASSERT( bindings::stride_major(x) >= std::max< std::ptrdiff_t >(1,
                bindings::size_row(a)) );
        BOOST_ASSERT( bindings::stride_major(y) >= std::max< std::ptrdiff_t >(1,
                bindings::size_column(a)) );
        return detail::labrd( bindings::size_row(a), bindings::size_column(a),
                bindings::size_column(a), bindings::begin_value(a),
                bindings::stride_major(a), bindings::begin_value(d),
                bindings::begin_value(e), bindings::begin_value(tauq),
                bindings::begin_value(taup), bindings::begin_value(x),
                bindings::stride_major(x), bindings::begin_value(y),
                bindings::stride_major(y) );
    }

};


//
// Functions for direct use. These functions are overloaded for temporaries,
// so that wrapped types can still be passed and used for write-access. In
// addition, if applicable, they are overloaded for user-defined workspaces.
// Calls to these functions are passed to the labrd_impl classes. In the 
// documentation, most overloads are collapsed to avoid a large number of
// prototypes which are very similar.
//

//
// Overloaded function for labrd. Its overload differs for
//
template< typename MatrixA, typename VectorD, typename VectorE,
        typename VectorTAUQ, typename VectorTAUP, typename MatrixX,
        typename MatrixY >
inline std::ptrdiff_t labrd( MatrixA& a, VectorD& d, VectorE& e,
        VectorTAUQ& tauq, VectorTAUP& taup, MatrixX& x, MatrixY& y ) {
    return labrd_impl< typename bindings::value_type<
            MatrixA >::type >::invoke( a, d, e, tauq, taup, x, y );
}

} // namespace lapack
} // namespace bindings
} // namespace numeric
} // namespace boost

#endif
