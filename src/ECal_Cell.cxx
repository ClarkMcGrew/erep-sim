#include "ECal_Cell.hxx"

cell::cell() {
}
cell::cell(const cell & rhs)
   : id(const_cast<cell &>( rhs ).id)
   , z(const_cast<cell &>( rhs ).z)
   , y(const_cast<cell &>( rhs ).y)
   , x(const_cast<cell &>( rhs ).x)
   , l(const_cast<cell &>( rhs ).l)
   , adc1(const_cast<cell &>( rhs ).adc1)
   , tdc1(const_cast<cell &>( rhs ).tdc1)
   , adc2(const_cast<cell &>( rhs ).adc2)
   , tdc2(const_cast<cell &>( rhs ).tdc2)
   , mod(const_cast<cell &>( rhs ).mod)
   , lay(const_cast<cell &>( rhs ).lay)
   , cel(const_cast<cell &>( rhs ).cel)
   , pe_time1(const_cast<cell &>( rhs ).pe_time1)
   , hindex1(const_cast<cell &>( rhs ).hindex1)
   , pe_time2(const_cast<cell &>( rhs ).pe_time2)
   , hindex2(const_cast<cell &>( rhs ).hindex2)
{
   // This is NOT a copy constructor. This is actually a move constructor (for stl container's sake).
   // Use at your own risk!
   (void)rhs; // avoid warning about unused parameter
   cell &modrhs = const_cast<cell &>( rhs );
   modrhs.pe_time1.clear();
   modrhs.hindex1.clear();
   modrhs.pe_time2.clear();
   modrhs.hindex2.clear();
}
cell::~cell() {
}

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
