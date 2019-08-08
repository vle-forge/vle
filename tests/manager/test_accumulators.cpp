/*
 * Copyright (C) 2009-2015 INRA
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <vle/utils/unit-test.hpp>

#include <stdexcept>
#include <limits>
#include <fstream>
#include <iostream>

#include "details/accu_mono.hpp"

//Accumulators
void test_accumulators()
{
    namespace vm = vle::manager;
    {
        vm::AccuMono acc(vm::STANDARD);
        acc.insert(1);
        acc.insert(5);
        acc.insert(4);
        acc.insert(3.6);
        EnsuresApproximatelyEqual(acc.mean(),3.4,10e-4);
        EnsuresApproximatelyEqual(acc.moment2(),13.74,10e-4);
        EnsuresApproximatelyEqual(acc.stdDeviation(), 1.704895,10e-4);
        EnsuresApproximatelyEqual(acc.variance(), 2.90666,10e-4);
        acc.insert(8);
        acc.insert(3);
        acc.insert(2);
        EnsuresApproximatelyEqual(acc.mean(), 3.8,10e-4);
        EnsuresApproximatelyEqual(acc.moment2(), 18.85143,10e-4);
        EnsuresApproximatelyEqual(acc.stdDeviation(), 2.268627,10e-4);
        EnsuresApproximatelyEqual(acc.variance(), 5.146667,10e-4);
    }
    {
        vm::AccuMono acc(vm::QUANTILE);
        acc.insert(1);
        EnsuresApproximatelyEqual(acc.quantile(0.0), 1,10e-4);
        EnsuresApproximatelyEqual(acc.quantile(1.0), 1,10e-4);
        acc.insert(5);
        acc.insert(4);
        acc.insert(3.6);
        EnsuresApproximatelyEqual(acc.mean(),3.4,10e-4);
        EnsuresApproximatelyEqual(acc.squareSum(),54.96,10e-4);
        EnsuresApproximatelyEqual(acc.quantile(0.25), 2.95,10e-4);
        acc.insert(8);
        acc.insert(3);
        acc.insert(2);
        EnsuresApproximatelyEqual(acc.mean(),3.8,10e-4);
        EnsuresApproximatelyEqual(acc.squareSum(),131.96,10e-4);
        EnsuresApproximatelyEqual(acc.quantile(0.75), 4.5,10e-4);
    }
}

int main()
{
    test_accumulators();

    return unit_test::report_errors();
}

