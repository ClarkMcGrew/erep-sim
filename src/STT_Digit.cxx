#include "STT_Digit.hxx"

digit::digit() {
}
digit::digit(const digit & rhs)
   : det(const_cast<digit &>( rhs ).det)
   , x(const_cast<digit &>( rhs ).x)
   , y(const_cast<digit &>( rhs ).y)
   , z(const_cast<digit &>( rhs ).z)
   , t(const_cast<digit &>( rhs ).t)
   , de(const_cast<digit &>( rhs ).de)
   , hor(const_cast<digit &>( rhs ).hor)
   , hindex(const_cast<digit &>( rhs ).hindex)
{
   // This is NOT a copy constructor. This is actually a move constructor (for stl container's sake).
   // Use at your own risk!
   (void)rhs; // avoid warning about unused parameter
   digit &modrhs = const_cast<digit &>( rhs );
   modrhs.det.clear();
   modrhs.hindex.clear();
}
digit::~digit() {
}
