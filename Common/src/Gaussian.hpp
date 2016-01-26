/******************************************************************************
/* @file One-Dimensional Gaussian distribution density function.
/* 
/* @author langenhagen
/* @version 150126
/******************************************************************************/
#pragma once

///////////////////////////////////////////////////////////////////////////////
//INCLUDES C/C++ standard library (and other external libraries)

#include <math.h>


///////////////////////////////////////////////////////////////////////////////
// NAMESPACE, CONSTANTS and TYPE DECLARATIONS/IMPLEMENTATIONS


namespace app {

    /// A templated (one-dimensional) gaussian distribution density function.
    template< typename T=double>
    class Gaussian {
        
    public: // vars

        T mean;        ///< The mean aka expected value.
        T variance;    ///< The variance aka sigma*sigma.

    public: // Constructor

        /** Main constructor.
         * @param m The mean aka expected value.
         * @param v The variance aka sigma*sigma.
         */
        Gaussian( const T m=0, const T v=1) : mean(m), variance(v)
        {}

    public: // static functions

        /** Gets the density in a specified gaussian at a given sample point.
         * @param mean The mean aka expected value of the gaussian to be used.
         * @param variance The variance aka sigma*sigma of the gaussian to be used.
         * @param sample The point on which to measure the probability density.
         * @return The probability density at the given point.
         */
        static inline T density( const T mean, const T variance, const T sample ) {
            const register T two_var = 2*variance;
            const register T denom =  (T)sqrt(3.14159265358979*two_var);
            return exp(-(sample-mean)*(sample-mean)/two_var) / denom;
        }
        
    public: // methods

        /** Gets the density at a given sample point.
         * @param sample The point on which to measure the probability density.
         * @return The probability density at the given point.
         */
        inline T density( const T sample) const {
            return  density( this->mean, this->variance, sample);
        }

    public: // operators

        /** Gets the density at a given sample point.
         * Is equal to a call to density() but more handy.
         * @param sample The point on which to measure the probability density.
         * @return The probability density at the given point.
         */
        inline T operator()(const T sample ) const {
            return  density( this->mean, this->variance, sample);
        }

    };

}