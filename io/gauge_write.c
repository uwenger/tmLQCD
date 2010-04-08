/***********************************************************************
* Copyright (C) 2002,2003,2004,2005,2006,2007,2008 Carsten Urbach
*
* This file is part of tmLQCD.
*
* tmLQCD is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* tmLQCD is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with tmLQCD.  If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/

#include "gauge.ih"

int write_gauge_field(char * filename, const int prec, paramsXlfInfo const *xlfInfo)
{
  WRITER * writer = NULL;
  uint64_t bytes;

  DML_Checksum     checksum;
  paramsIldgFormat *ildg;

  bytes = (uint64_t)L * L * L * T_global * sizeof(su3) * prec / 16;

  /* the 0 is for not appending */
  construct_writer(&writer, filename, 0);

  write_xlf_info(writer, xlfInfo);

  ildg = construct_paramsIldgFormat(prec);
  write_ildg_format(writer, ildg);
  free(ildg);

  write_header(writer, 1, 1, "ildg-binary-data", bytes);
  write_binary_gauge_data(writer, prec, &checksum);
  write_checksum(writer, &checksum, NULL);

  if (g_cart_id == 0)
  {
    fprintf(stdout, "Checksum A: %#x \nChecksum B: %#x\n", checksum.suma, checksum.sumb);
    fflush(stdout);
  }
#ifdef MPI
    MPI_Barrier(MPI_COMM_WORLD);
#endif /* MPI */

  destruct_writer(writer);
  return 0;
}