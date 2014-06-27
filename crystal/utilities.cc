/******************************
   This file is part of PyLaDa.

   Copyright (C) 2013 National Renewable Energy Lab
  
   PyLaDa is a high throughput computational platform for Physics. It aims to make it easier to submit
   large numbers of jobs on supercomputers. It provides a python interface to physical input, such as
   crystal structures, as well as to a number of DFT (VASP, CRYSTAL) and atomic potential programs. It
   is able to organise and launch computational jobs on PBS and SLURM.
  
   PyLaDa is free software: you can redistribute it and/or modify it under the terms of the GNU General
   Public License as published by the Free Software Foundation, either version 3 of the License, or (at
   your option) any later version.
  
   PyLaDa is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
   Public License for more details.
  
   You should have received a copy of the GNU General Public License along with PyLaDa.  If not, see
   <http://www.gnu.org/licenses/>.
******************************/

math::rVector3d into_cell( math::rVector3d const &_vec, 
                           math::rMatrix3d const &_cell, 
                           math::rMatrix3d const &_inv)
{
  math::rVector3d result( _inv * _vec );
  result(0) -= std::floor(result(0)+types::roundoff);
  result(1) -= std::floor(result(1)+types::roundoff);
  result(2) -= std::floor(result(2)+types::roundoff);
  return _cell * result;
}

math::rVector3d zero_centered( math::rVector3d const &_vec, 
                               math::rMatrix3d const &_cell, 
                               math::rMatrix3d const &_inv)
{
  math::rVector3d result( _inv * _vec );
  result(0) -= std::floor(5e-1+result(0)+types::roundoff);
  result(1) -= std::floor(5e-1+result(1)+types::roundoff);
  result(2) -= std::floor(5e-1+result(2)+types::roundoff);
  // numerical stability check.
  if( math::eq(result(0), 5e-1) ) result(0) = -5e-1;
  else if( math::lt(result(0), -5e-1)) result(0) += 1e0;
  if( math::eq(result(1), 5e-1) ) result(1) = -5e-1;
  else if( math::lt(result(1), -5e-1)) result(1) += 1e0;
  if( math::eq(result(2), 5e-1) ) result(2) = -5e-1;
  else if( math::lt(result(2), -5e-1)) result(2) += 1e0;
  return _cell * result;
}

math::rVector3d into_voronoi( math::rVector3d const &_vec, 
                              math::rMatrix3d const &_cell, 
                              math::rMatrix3d const &_inv)
{
  math::rVector3d result( _inv * _vec );
  result(0) -= std::floor(result(0)+types::roundoff);
  result(1) -= std::floor(result(1)+types::roundoff);
  result(2) -= std::floor(result(2)+types::roundoff);
  // numerical stability check.
  if( math::eq(result(0), -1e0) or math::eq(result(0), 1e0) ) result(0) = 0e0;
  if( math::eq(result(1), -1e0) or math::eq(result(1), 1e0) ) result(1) = 0e0;
  if( math::eq(result(2), -1e0) or math::eq(result(2), 1e0) ) result(2) = 0e0;
  math::rVector3d const orig(result);
  types::t_real min_norm = (_cell*orig).squaredNorm();
  for(int i(-1); i < 2; ++i)
    for(int j(-1); j < 2; ++j)
      for(int k(-1); k < 2; ++k)
      {
        math::rVector3d const translated = orig + math::rVector3d(i,j,k);
        types::t_real const d( (_cell*translated).squaredNorm() );
        if( math::gt(min_norm, d) )
        {
          min_norm = d;
          result = translated;
        }
      }
  return _cell * result;
}

types::t_real third_order(math::rMatrix3d const & _matrix, types::t_int _n)
{
  typedef math::rVector3d rVector3d;
  typedef types::t_real t_real;
  t_real result = 0e0;
  t_real const ninv = 1e0 / t_real(_n);
  t_real const maxdist = (_matrix * 10e0 * rVector3d(1,1,1)).squaredNorm();

  //  printf ("third order called, n = %d\n", _n);
  
  for(size_t i(0); i < _n; ++i)
    {
      for(size_t j(0); j < _n; ++j)
	{
	  for(size_t k(0); k < _n; ++k)
	    {
	      t_real min_dist = maxdist;
	      for(int l(-1); l < 2; ++l)
		for(int m(-1); m < 2; ++m)
		  for(int n(-1); n < 2; ++n)
		    {
		      t_real const q = (_matrix * rVector3d( i*ninv+l-0.5,
							     j*ninv+m-0.5,
							     k*ninv+n-0.5  ) ).squaredNorm();
		      if( q < min_dist ) min_dist = q;
		    }
	      result += min_dist;
	    }
	}
    }
  return result / (_matrix.determinant() * t_real(_n*_n*_n));
}
